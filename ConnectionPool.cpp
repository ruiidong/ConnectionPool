#include "ConnectionPool.h"
#include "public.h"
#include "Connection.h"

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
        connection * conn= new connection();     
        conn->connect(ip_,port_,username_,password_,dbname_);
        connQue_.push(conn);
        connCnt_++;
    }
}