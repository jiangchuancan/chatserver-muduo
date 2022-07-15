#include "Connection.h"
#include <muduo/base/Logging.h>
#include <iostream>
using namespace std;

//初始化数据库
Connection::Connection()
{
    _conn = mysql_init(nullptr);
}

//释放数据库资源
Connection::~Connection()
{
    if (_conn != nullptr)
    {
        _conn == nullptr;
    }
}

//连接数据库
bool Connection::connect(
    string ip,
    unsigned short port,
    string user,
    string password,
    string dbname)
{
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
    if (p != nullptr)
    {
        mysql_query(_conn, "set names gbk");
        return true;
    }
    else
    {
        LOG_INFO << "connect error";
        return false;
    }
}

//更新操作
bool Connection::update(string sql)
{
    //查询成功返回0，否则返回非0
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << "update error";
        return false;
    }

    return true;
}

//查询操作
MYSQL_RES *Connection::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << "query error";
        return nullptr;
    }

    return mysql_use_result(_conn);
}

//获取连接
MYSQL *Connection::getConnection()
{
    return _conn;
}
