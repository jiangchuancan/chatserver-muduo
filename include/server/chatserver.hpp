#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Thread.h>
#include <muduo/base/ThreadPool.h>
#include <atomic>
#include <unordered_set>
#include <boost/circular_buffer.hpp>
#include <muduo/base/Logging.h>
using namespace muduo;
using namespace muduo::net;
using namespace std;

class ChatServer
{
public:
    //初始化聊天服务器对象
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);

    //启动服务
    void start();

private:
    //连接的回调函数
    void onConnection(const TcpConnectionPtr &);

    //读写事件回调函数
    void onMessage(const TcpConnectionPtr &,
                   Buffer *,
                   Timestamp);

    TcpServer _server;
    EventLoop *_loop;
    atomic_int _numConnected;   //该服务器已经连接的数量
    const int _kMaxConnections; //最大连接数量
    ThreadPool _threadPool;     //线程池对象
    const int _numThreadPoll;   //线程池数量
    const int _numEventloop;    // EventLoop对象数量

    void onTimer();

    void dumpConnectionBuckets() const;

    typedef std::weak_ptr<muduo::net::TcpConnection> WeakTcpConnectionPtr;

    struct Entry : public muduo::copyable
    {
        explicit Entry(const WeakTcpConnectionPtr &weakConn)
            : weakConn_(weakConn)
        {
        }

        ~Entry()
        {
            muduo::net::TcpConnectionPtr conn = weakConn_.lock();
            if (conn)
            {
                conn->shutdown();
            }
        }

        WeakTcpConnectionPtr weakConn_;
    };
    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakConnectionList;
    WeakConnectionList connectionBuckets_;
    const int _idleSeconds;
};

#endif