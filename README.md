# LinpopLanIM

#### 介绍
这是一款以Qt5为框架的局域网即时通讯软件，功能极其简单，实现了注册、登录、头像、单聊、群聊、好友系统、图片系统、点对点文件传输和群文件功能，是bit cs大三上破防小学期《计算机专业基础实习》课程的组队大作业，答辩结束后修复了部分bug并加上整活部分后上传到此仓库。不对软件质量做任何保证。目前大重构已经完成，客户端不会随机崩溃了。

#### 软件架构
一个客户端，一个服务端，全部以Qt5为框架，用MinGW编译，代码跨平台，运用到的技术有Json序列化和反序列化、TCP网络编程、数据库（服务端使用Sqlite）和多线程。

#### 使用说明
1.  下载客户端和服务端的发行版（目前只有Windows版，其他版本需自行在对应平台上编译）。
2.  客户端解压后直接运行，服务端只需要在局域网中的任意一台电脑上运行即可。
3.  客户端在设置中设置好服务端电脑的IP和开服的端口即可注册/登录。
4.  如果想在Qt Creator中运行客户端，请以先管理员身份运行Qt Creator，然后再打开客户端项目并编译运行，否则会启动失败，提示权限不够。
5.  如果想自行编译，要注意编译出来的程序是不带OpenSSL动态链接库的，Windows平台下需要先把仓库根目录的两个文件复制到"Qt安装根目录/Qt5.xx.xx/Tools/mingwxxx_64/bin"，再构建Release版本，然后使用打包工具对构建好的exe打包，最后把这两个文件复制到打包后的程序根目录才能体验完整功能。Linux平台请自行研究。
