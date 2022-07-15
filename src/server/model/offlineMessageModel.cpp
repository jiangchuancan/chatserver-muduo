#include "offlineMessageModel.hpp"
#include "CommonConnectionPool.h"

//存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    //组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into OfflineMessage values(%d,'%s')", userid, msg.c_str());

    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    cp->update(sql);
}

//删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    //组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userid = %d", userid); //一次性删完关于userid的信息

    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    cp->update(sql);
}

//查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    //组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from OfflineMessage where userid = %d", userid);

    vector<string> vec;
    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    MYSQL_RES *res = cp->query(sql);
    if (res != nullptr)
    {
        //把用户所有的离线消息放入vector中
        MYSQL_ROW row;
        while (row = mysql_fetch_row(res))
        {
            vec.push_back(row[0]);
        }

        mysql_free_result(res);
        return vec;
    }

    return vec;
}