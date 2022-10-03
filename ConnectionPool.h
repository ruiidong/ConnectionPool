#pragma once
#include <queue>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>

using namespace std;

class connection;
class connectionpool
{
public:
    connectionpool();
private:
    bool loadConfigFile();
    void produceConnTask();

    string ip_;
    unsigned short port_;
    string username_;
    string password_;
    string dbname_;
    int initSize_;                   //初始连接数
    int maxSize_;                     //最大连接数
    int maxIdleTime_;                //最大空闲时间
    int connectionTimeout_;         //超时连接事件

    queue<connection*> connQue_;
    atomic_int connCnt_;            //已创建连接的数量

    mutex queueMtx_;
    condition_variable cv_;
};