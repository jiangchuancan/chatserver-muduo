#pragma once
#include <mysql/mysql.h>
#include <string>
#include <ctime>
using namespace std;

/*
对数据库的操作
*/
class Connection
{
public:
    //初始化数据库
    Connection();

    //释放数据库资源
    ~Connection();

    //连接数据库
    bool connect(
        string ip,
        unsigned short port,
        string user,
        string password,
        string dbname);

    //更新操作
    bool update(string sql);

    //查询操作
    MYSQL_RES *query(string sql);

    //刷新连接的起始空闲时间点
    void refreshAliveTime()
    {
        _alivetime = clock();
    }

    //返回存活的时间
    clock_t getAliveeTime()
    {
        return clock() - _alivetime;
    }

    //获取连接
    MYSQL *getConnection();

private:
    MYSQL *_conn;       // msyql对象
    clock_t _alivetime; //记录进入起始空闲状态后的起始存活时间
};