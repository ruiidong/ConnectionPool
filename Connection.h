#include <string>
#include <mysql/mysql.h>
#include <iostream>
#include "public.h"
using namespace std;

class connection
{
public:
    // 初始化数据库连接
    connection();
    // 释放数据库连接资源
    ~connection();
    // 连接数据库
    bool connect(string ip, unsigned short port, string username, string password, string dbname);
    // 更新操作 insert、delete、update
    bool update(string sql);
    // 查询操作 select
    MYSQL_RES *query(string sql);

    void refreshTime();      //设置入队时间
    clock_t getTime() const; //获得空闲时间

private:
    // 表示和MySQL Server的一条连接
    MYSQL *_conn;
    clock_t time_; //入队时间，以毫秒为单位
};