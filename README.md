本项目运行执行指令：
g++ test.cpp ../src/ConnectionPool.cpp ../src/DatabaseConnection.cpp -lpthread -ljsoncpp -lmysqlclient -o test
生成test文件，再利用指令：
./test
执行该项目，运行结果为：数据库会生成20000条数据。
在mysql中执行：
select count(*) from 表名;
可以知道生成数据的数量。
