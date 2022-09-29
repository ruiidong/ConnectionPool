#include <iostream>
#include "Connection.h"

using namespace std;

int main()
{
    connection conn;
    conn.connect("127.0.0.1",3306,"lrd","lrd520LRD","school");
    char sql[256] = {0};
    sprintf(sql,"insert into user(name,age,sex) values('%s',%d,'%s')","wanghan",20,"M");
    conn.update(sql);

    return 0;
}