#ifndef __CONNECTIONPOOL_H__
#define __CONNECTIONPOOL_H__

#include "DatabaseConnection.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <json/json.h>
#include <fstream>
#include <memory>
#include <thread>
#include <functional>
#include <atomic>

using std::atomic;
using std::thread;
using std::shared_ptr;
using namespace Json;
using std::mutex;
using std::queue;
using std::condition_variable;
using std::ifstream;
using std::unique_lock;

class ConnectionPool
{
public:
    static ConnectionPool *getConnectionPool();
    ConnectionPool(const ConnectionPool &rhs) = delete;
    ConnectionPool &operator=(const ConnectionPool &rhs) = delete;
    shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();
private:
    ConnectionPool();
    bool parseJsonFile();
    void produceConnection();
    void recycleConnection();
    void addConnection();
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short m_port;
    int m_maxSize;
    int m_minSize;
    int m_timeout;
    int m_maxIdleTime;
    mutex m_mutexQ;
    condition_variable m_cond;
    queue<MysqlConn *> m_connectionQ;
    
    atomic<bool> m_isRunning{false};
    
};

#endif
