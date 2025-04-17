#include <iostream>
#include "../include/DatabaseConnection.h"
#include "../include/ConnectionPool.h"

using std::shared_ptr;
using std::cout;
using std::endl;

void test1()
{
    MysqlConn conn;
    bool flag_conn = conn.connect("root", "root", "db", "127.0.0.1", 3306);
    if(flag_conn)
    {
        cout << "connect success !" << endl;
    }
    else 
    {
        cout << "connect fail !" << endl;
    }
    string sql = "insert into person values (7, 27, 'man', 'Jerry')";
    bool flag = conn.update(sql);
    if(flag)
    {
        cout << "update success!" << endl;
    } 
    else 
    {
        cout << "update fail!" << endl;
    }
    sql = "select * from person";
    bool flag_query = conn.query(sql);
    if(flag_query)
    {
        cout << "query success!" << endl;
    }
    else
    {
        cout << "query fail !" << endl;
    }
    while(conn.next())
    {
        cout << conn.value(0) << ", " << conn.value(1) << ", " << conn.value(2) << ", " << conn.value(3) << endl; 
    }
}

void func1(int begin, int end)
{
    for(int i = begin; i < end; ++i)
    {
    
        MysqlConn conn; 
        bool flag_conn = conn.connect("root", "root", "db", "127.0.0.1", 3306);
        /*    
        if(flag_conn)
        {
            cout << i << ",connection success!" << endl;
        }
        else
        {
            cout << "connection fail!" << endl;
        }
        */
        char sql[1024] = {0};
        sprintf(sql, "insert into person values (%d, 27, 'man', 'Tom')", i);
        bool flag_update = conn.update(sql);
        /*
        if(flag_update)
        {
            cout << i  << ",update success!" << endl;
        }
        else
        {
            cout << "update fail!" << endl;
        }
        */
    }
}

void func2(ConnectionPool *pool, int begin, int end)
{
    for(int i = begin; i < end; ++i)
    {
        bool flag_update = false;
        //while(!flag_update)
        //{
            shared_ptr<MysqlConn> conn = pool->getConnection();
            /*
            bool flag_conn = conn->connect("root", "root", "db", "127.0.0.1", 3306);
            if(flag_conn)
            {
                cout << i << ",connection success!" << endl;
            }
            else
            {
                cout << "connection fail!" << endl;
            }
            */
            char sql[1024] = {0};
            sprintf(sql, "insert into person values (%d, 27, 'man', 'Jam')", i);
            flag_update = conn->update(sql);
            /*   
            if(flag_update)
            {
                cout << i  << ",update success!" << endl;
            }
            else
            {
                cout << "update fail!" << endl;
            }
            */
        //}
    }
}

void test2()
{
    steady_clock::time_point begin = steady_clock::now();
    func1(0,5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "dont't use Connection pool and single thread, time is " << length.count() / 1000000 << "ms" << endl;
    
}
void test3()
{
    ConnectionPool *pool = ConnectionPool::getConnectionPool();
    steady_clock::time_point begin = steady_clock::now();
    func2(pool,5000,10000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "use Connection pool and single thread, time is " << length.count() / 1000000 << "ms" << endl;
}
void test4()
{
    MysqlConn conn; 
    bool flag_conn = conn.connect("root", "root", "db", "127.0.0.1", 3306);
    steady_clock::time_point begin = steady_clock::now();
    thread t1(func1, 10000, 11000);
    thread t2(func1, 11000, 12000);
    thread t3(func1, 12000, 13000);
    thread t4(func1, 13000, 14000);
    thread t5(func1, 14000, 15000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "dont't use Connection pool and multiply thread, time is " << length.count() / 1000000 << "ms" << endl;
}

void test5()
{
    ConnectionPool *pool = ConnectionPool::getConnectionPool();
    steady_clock::time_point begin = steady_clock::now();
    thread t1(func2, pool, 15000, 16000);
    thread t2(func2, pool, 16000, 17000);
    thread t3(func2, pool, 17000, 18000);
    thread t4(func2, pool, 18000, 19000);
    thread t5(func2, pool, 19000, 20000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "use Connection pool and multiply thread, time is " << length.count() / 1000000 << "ms" << endl;
}
int main(int argc, char *argv[])
{
    //test1();
    test2();
    test3();
    test4();
    test5(); 
    return 0;
}

