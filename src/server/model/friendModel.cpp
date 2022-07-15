#include "friendModel.hpp"
#include "CommonConnectionPool.h"

//添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    //组装sql语句
    char sqlown[1024] = {0};
    sprintf(sqlown, "insert into Friend values(%d,%d)", userid, friendid);
    char sqlto[1024] = {0};
    sprintf(sqlto, "insert into Friend values(%d,%d)", friendid, userid);
    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    cp->update(sqlown);
    cp->update(sqlto);
}

//返回用户好友列表
vector<User> FriendModel::query(int userid)
{
    //组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,a.state from User a inner join Friend b on b.friendid = a.id where b.userid = %d", userid);

    vector<User> vec;
    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    MYSQL_RES *res = cp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setState(row[2]);
            vec.push_back(user);
        }
        mysql_free_result(res);
        return vec;
    }

    return vec;
}