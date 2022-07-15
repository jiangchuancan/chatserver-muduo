#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"

#include <functional>
#include <string>
#include <assert.h>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

//初始化聊天服务器对象
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg),
      _numConnected(0),
      _kMaxConnections(1000),
      _numThreadPoll(8),
      _numEventloop(4),
      _idleSeconds(5)
{
    //注册连接回调函数
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    //注册信息回调函数
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    //设置EventLoop线程数量
    _server.setThreadNum(_numEventloop);

    loop->runEvery(1.0, std::bind(&ChatServer::onTimer, this));
    connectionBuckets_.resize(_idleSeconds);
    dumpConnectionBuckets();
}

//启动服务
void ChatServer::start()
{
    _threadPool.start(_numThreadPoll);
    _server.start();
}

//连接的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    //客户端断开连接
    if (!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }

    if (conn->connected())
    {
        ++_numConnected;
        if (_numConnected > _kMaxConnections)
        {
            conn->shutdown();
            conn->forceCloseWithDelay(3.0); // > round trip of the whole Internet.
        }

        EntryPtr entry(new Entry(conn));
        connectionBuckets_.back().insert(entry);
        dumpConnectionBuckets();
        WeakEntryPtr weakEntry(entry);
        conn->setContext(weakEntry);
    }
    else
    {
        --_numConnected;
        assert(!conn->getContext().empty());
        WeakEntryPtr weakEntry(boost::any_cast<WeakEntryPtr>(conn->getContext()));
        LOG_DEBUG << "Entry use_count = " << weakEntry.use_count();
    }
}

//读写事件回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    //数据的反序列化
    json js = json::parse(buf);
    //达到的目的：完全解耦网络模块的代码和业务模块的代码
    //通过js["msgid"]获取=》业务handler=》conn js time
    // note:js["msgid"]是字符串，通过get<int>()强转为整形
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    //回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn, js, time);

    assert(!conn->getContext().empty());
    WeakEntryPtr weakEntry(boost::any_cast<WeakEntryPtr>(conn->getContext()));
    EntryPtr entry(weakEntry.lock());
    if (entry)
    {
        connectionBuckets_.back().insert(entry);
        dumpConnectionBuckets();
    }
}

void ChatServer::onTimer()
{
  connectionBuckets_.push_back(Bucket());
  dumpConnectionBuckets();
}

void ChatServer::dumpConnectionBuckets() const
{
  LOG_INFO << "size = " << connectionBuckets_.size();
  int idx = 0;
  for (WeakConnectionList::const_iterator bucketI = connectionBuckets_.begin();
      bucketI != connectionBuckets_.end();
      ++bucketI, ++idx)
  {
    const Bucket& bucket = *bucketI;
    printf("[%d] len = %zd : ", idx, bucket.size());
    for (const auto& it : bucket)
    {
      bool connectionDead = it->weakConn_.expired();
      printf("%p(%ld)%s, ", get_pointer(it), it.use_count(),
          connectionDead ? " DEAD" : "");
    }
    puts("");
  }
}
