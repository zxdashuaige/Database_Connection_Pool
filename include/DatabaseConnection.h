#ifndef __MYSQLCONNECTION_H__ 
#define __MYSQLCONNECTION_H__

#include <string>
#include <mysql/mysql.h>
#include <chrono>
#include <iostream>

using namespace std::chrono;
using std::string;
using std::cout;
using std::endl;

class MysqlConn
{
public:
    //Init Database Connection
    MysqlConn();
    //release Database Connection 
    ~MysqlConn();
    // Connect Database
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306);
    // update datbase
    bool update(string sql);
    // query database
    bool query(string sql);
    // traverse database
    bool next();
    // get value of result set
    string value(int index);
    //affairs operate
    bool transaction();
    //submit affairs
    bool commit();
    //rollback affairs
    bool rollback();
    //begin time point
    void refreshAliveTime();
    //comput alive time 
    long long getAliveTime();

private:
    void freeResult();
    MYSQL *m_conn = nullptr;
    MYSQL_RES *m_result = nullptr;
    MYSQL_ROW m_row = nullptr;
    steady_clock::time_point m_alivetime;
};
#endif
