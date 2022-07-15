#include "userModel.hpp"
#include "CommonConnectionPool.h"
#include <iostream>
using namespace std;

// User表的增加方法
bool UserModel::insert(User &user)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into User(name, password, state) values('%s', '%s', '%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    if (cp->update(sql))
    {
        //获取插入成功的用户数据生成的主键id
        user.setId(mysql_insert_id(cp->getConnection()));
        return true;
    }

    return false;
}

//根据用户号码查询用户消息
User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id = %d", id);
    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    MYSQL_RES *res = cp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPwd(row[2]);
            user.setState(row[3]);
            mysql_free_result(res);
            return user;
        }
        mysql_free_result(res);
    }

    return User();
}

//更新用户的状态消息
bool UserModel::updateState(User user)
{
    char sql[1024] = {0};
    sprintf(sql, "update User set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    if (cp->update(sql))
    {
        return true;
    }

    return false;
}

//操作用户的状态信息
void UserModel::resetState()
{
    char sql[1024] = {0};
    sprintf(sql, "update User set state = 'offline' where state = 'online'");
    ConnectionPool *sp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> cp = sp->getConnection();
    cp->update(sql);
}