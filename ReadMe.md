# 项目介绍
本项目针对视频服务QoE预测的特征提取进行设计。

可对pcap或pcapng文件进行初步流量分类并提取视频流量特征。
# 支持协议

IPv4

TCP UDP

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
**1.输入**

1.输入文件格式：

    .pcap或.pcapng格式文件

     
2.输入格式： 

可执行程序名 输入类型参数 绝对路径 
     
3.输入类型参数说明：

输入类型参数 说明 备注

-file pcap或pcapng文件的绝对路径

-folder 文件夹绝对路径 不读取子文件夹 
    


**2.输出**

1.输出文件格式

    utf-8编码无表头csv文件

2.输出文件内容（按列）
源ip地址，目的ip地址，平均包字节数，平均间隔，带宽（byte/s），平均每秒包数，平均每段包数，平均段字节数，平均段持续时间，平均段请求间隔，是否发生停滞
