# 工具介绍

本工具获取UDP流量特征
# 支持协议

IPv4

TCP UDP

RTP RTCP
# 安装

**1.Debain**

安装GCC： sudo apt-get install build-essential

安装flex： sudo apt-get install flex

安装bison： sudo apt-get install bison

安装libpcap版本 > 1.9.1：

[libpcap](https://www.tcpdump.org/release)

tar -zxvf libpcap-*

sudo ./configure

sudo make

sudo make install


**2.Cent OS**

安装GCC： yum -y install build-essential

安装flex： yum -y install flex

安装bison：yum -y installl bison

安装libpcap版本 > 1.9.1：

[libpcap](https://www.tcpdump.org/release)

tar -zxvf libpcap-*

sudo ./configure

sudo make

sudo make install

# 使用说明

**1.输入参数说明**

输入格式：可执行程序名 输入类型参数 绝对路径 可选参数 参数值 
     
输入类型参数说明：

输入类型参数 说明 备注

-file pcap或pcapng文件的绝对路径

-folder 文件夹绝对路径 不读取子文件夹 
   
可选参数             说明                  类型

   -i           设置单位时间长度          double(>0)
   
   -thpmin      设置输出的最低每秒包数    double(>0)
   
   -lenmin      设置输出的最小平均包长    int(>=0)
   
   -lenmax      设置输出的最大平均包长    int(>=0)
   
   -cntmin      设置输出的最小包数        int(>=0)
   
   -duramin     设置输出的持续时间占单位时间最小比例 double(0~1)
   

