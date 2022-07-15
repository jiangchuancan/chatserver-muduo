#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// User表的数据操作类
class UserModel
{
public:
    // User表的增加方法
    bool insert(User &user);

    //User表根据用户号码查询用户消息
    User query(int id);

    //User表更新用户的状态消息
    bool updateState(User user);

    //操作用户的状态信息
    void resetState();
};

#endif