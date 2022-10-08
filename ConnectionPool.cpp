#include "ConnectionPool.h"
#include "public.h"
#include "Connection.h"
#include <thread>

connectionpool* connectionpool::getInstance()
{
    static connectionpool connpool;
    return &connpool;
}

bool connectionpool::loadConfigFile()
{
    FILE *fp = fopen("../mysql.cnf", "r");
    if (fp == nullptr)
    {
        LOG("打开配置文件失败");
        return false;
    }

    while (!feof(fp))
    {
        char buf[1024] = {0};
        fgets(buf, 1024, fp);
        string str = buf;
        int idx = str.find("=", 0);
        if (idx == -1)
        {
            continue;
        }
        int endidx = str.find("\n", idx);
        string key = str.substr(0, idx);
        string value = str.substr(idx + 1, endidx - idx - 1);
        if (key == "ip")
        {
            ip_ = value;
        }
        else if (key == "port")
        {
            port_ = atoi(value.c_str());
        }
        else if (key == "username")
        {
            username_ = value;
        }
        else if (key == "password")
        {
            password_ = value;
        }
        else if (key == "dbname")
        {
            dbname_ = value;
        }
        else if (key == "initSize")
        {
            initSize_ = atoi(value.c_str());
        }
        else if (key == "maxSize")
        {
            maxSize_ = atoi(value.c_str());
        }
        else if (key == "maxIdleTime")
        {
            maxIdleTime_ = atoi(value.c_str());
        }
        else if (key == "connectionTimeout")
        {
            connectionTimeout_ = atoi(value.c_str());
        }
    }

    return true;
}

connectionpool::connectionpool()
{
    if (!loadConfigFile())
    {
        return;
    }

    for (int i = 0; i < initSize_; i++)
    {
        connection *conn = new connection();
        conn->connect(ip_, port_, username_, password_, dbname_);
        connQue_.push(conn);
        conn->refreshTime();
        connCnt_++;
    }

    thread produceConn(std::bind(&connectionpool::produceConnTask, this));
    produceConn.detach();

    thread scanConn(std::bind(&connectionpool::scanConnTask, this));
    scanConn.detach();
}

void connectionpool::produceConnTask()
{
    for (;;)
    {
        unique_lock<mutex> lock(queueMtx_);
        while (!connQue_.empty())
        {
            cv_.wait(lock);
        }

        if (connCnt_ < maxSize_)
        {
            connection *conn = new connection();
            conn->connect(ip_, port_, username_, password_, dbname_);
            connQue_.push(conn);
            conn->refreshTime();
            connCnt_++;
        }

        cv_.notify_all();
    }
}

shared_ptr<connection> connectionpool::getConnection()
{
    unique_lock<mutex> lock(queueMtx_);
    while (connQue_.empty())
    {
        if (cv_status::timeout == cv_.wait_for(lock, std::chrono::milliseconds(connectionTimeout_)))
        {
            if (connQue_.empty())
            {
                LOG("获取链接超时...获取连接失败");
                return nullptr;
            }
        }
    }
    shared_ptr<connection> sp(connQue_.front(), [&](connection *conn)
                              {
          unique_lock<mutex> lock(queueMtx_);
          connQue_.push(conn); 
          conn->refreshTime(); });
    connQue_.pop();
    cv_.notify_all();
    return sp;
}

void connectionpool::scanConnTask()
{
    for (;;)
    {
        this_thread::sleep_for(std::chrono::seconds(maxIdleTime_));

        unique_lock<mutex> lock(queueMtx_);
        while(connCnt_ > initSize_)
        {
            connection* conn = connQue_.front();
            if(conn->getTime() >= (maxIdleTime_*1000))
            {
                connQue_.pop();
                delete conn;
                connCnt_--;
            }
            else
            {
                break;
            }
        }
    }
}