#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>
#include <atomic>
using namespace std;

//维护群组信息的操作接口方法
class GroupModel
{
public:
    //创建群组
    bool createGroup(Group &group);
    //加入群组
    bool addGroup(int userid, int groupid, string role);
    //查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    //根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其他成员发送消息
    vector<int> queryGroupsUser(int userid, int groupid);

private:
    //群组最大人员数量
    const int _maxNumGroup = 200;
};

#endif