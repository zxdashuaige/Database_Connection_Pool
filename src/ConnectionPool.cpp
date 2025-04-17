#include "../include/ConnectionPool.h" 

ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::parseJsonFile()
{
    ifstream ifs("../config.json");
    Reader rd;
    Value root;
    rd.parse(ifs, root);
    if(root.isObject())
    {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();
        return true;
    }
    return false;
}
void ConnectionPool::addConnection()
{
    MysqlConn *conn = new MysqlConn();
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);
    conn->refreshAliveTime();
    if(conn)
    {
        m_connectionQ.push(conn);
    }
}
ConnectionPool::ConnectionPool()
{
    if(!parseJsonFile())
    {
        return;
    }
    for(int i = 0; i < m_minSize; ++i)
    {
        addConnection();
    }
    thread producer(&ConnectionPool::produceConnection, this);
    thread recycler(&ConnectionPool::recycleConnection, this);
    producer.detach();
    recycler.detach();
    m_isRunning.store(true);
}
void ConnectionPool::produceConnection()
{
    while(m_isRunning)
    {
        unique_lock<mutex> locker(m_mutexQ);
        while(static_cast<int>(m_connectionQ.size()) >= m_maxSize)
        {
            m_cond.wait(locker);
        }
        addConnection();
        m_cond.notify_all();
    }
}
void ConnectionPool::recycleConnection()
{
    while(m_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<mutex> locker(m_mutexQ);
        while(static_cast<int>(m_connectionQ.size()) > m_minSize)
        {
            MysqlConn * conn = m_connectionQ.front();
            if(conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connectionQ.pop();
            }
            else
            {
                break;
            }
        }
    }
}

shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while(m_connectionQ.empty())
    {
        if(std::cv_status::timeout == m_cond.wait_for(locker, std::chrono::milliseconds(m_timeout)))
        {
            if(m_connectionQ.empty()) 
            {
                continue;
            }
        }
    }
    std::function<void(MysqlConn *)> deleter = [this](MysqlConn *conn)
    {
          std::lock_guard<mutex> locker(m_mutexQ);
          conn->refreshAliveTime();
          m_connectionQ.push(conn);
    };
    shared_ptr<MysqlConn> connptr(m_connectionQ.front(), deleter);
    m_connectionQ.pop();
    m_cond.notify_all();
    return connptr;
}

ConnectionPool::~ConnectionPool()
{
    while(!m_connectionQ.empty())
    {
        MysqlConn *conn = m_connectionQ.front();
        m_connectionQ.pop();
        delete conn;
    }
    m_isRunning.store(false);
    m_cond.notify_all();
}

