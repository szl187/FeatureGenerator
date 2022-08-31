# 项目介绍

FeatureGenerator是一种特征工程工具，用于解决网络服务QoE评价及业务分类问题。详细说明见[FeatureGenerator论文](https://szl187.github.io/FeatureGenerator/doc/FeatureGenerator.pdf)
# 支持协议

IPv4

TCP UDP

RTP RTCP
# 安装

**1.环境依赖**

包含目录：$(WinPcapHome)\Include;

库目录：$(WinPcapHome)\Lib;

预处理器：WPCAP;

链接器命令行其他选项：wpcap.lib ws2_32.lib

WinPcap安装包获取路径：[WinPcap](https://www.winpcap.org/install/default.htm)

**2.安装**
[FeatureGenerator源码](https://github.com/szl187/FeatureGenerator)

# 使用说明

**1.输入参数说明**

输入格式：可执行程序名 输入类型参数 绝对路径 可选参数 参数值 
     
输入类型参数说明：

输入类型参数           说明                   备注 

  -file     pcap或pcapng文件的绝对路径  

   -folder         文件夹绝对路径         不读取子文件夹 

可选参数说明：

 可选参数   参数值           说明                      备注 

 -csv        无          输出csv版本              默认为txt输出 

 -v         整型   输出每个流前v个数据包的长度 

 -rawdata    无    输出每个包的时间戳及四元组 

 -i         数字   以i秒为单位截取流量并计算特征 

 -thpmin    数字   输出每秒包数大于参数值的流特征      默认为0 

 -nortcp     无        不输出rtcp流信息 
 
 -allflow   0或1  0：输出全体流特征 1：仅输出全体流特征
 
 -sort       无       输出特征按吞吐量排序
 
 -retraninfo 无       输出详细重传信息
 
 -lenmin    数字   输出平均长度大于参数值的流特征
	
**3.示例**

**读取文件并默认输出易读的txt特征文件**

./featuregenerator -file home/pcap/text.pcap

**读取文件夹中所有pcap或pcapng文件并输出csv特征文件**

./featuregenerator -folder home/pcap -csv

**读取文件并输出每个数据包的信息摘要**

./featuregenerator -file home/pcap/text.pcap -rawdata

**以20秒为单位提取特征（默认为10秒）**

./featuregenerator -file home/pcap/text.pcap -i 20

**读取文件并输出每秒包数大于10的非rtcp流信息**

./featuregenerator -file home/pcap/text.pcap -nortcp -thpmin 10


# 特征说明

**1.获取设定时间范围内一条流量的特征**

**TCP或UDP**

总包数，平均包长(byte)，平均应用层长度(byte)，平均间隔(s)，带宽(byte/s)，应用层带宽(byte/s),吞吐量(包数/s)，流首包长度

**TCP**

重传数，重传率，RTT，RTT最大最小值，RTT差

**2.获取设定时间范围内全体流量的总带宽(byte/s)**





# 关联工具
1.流量MoS打分工具 [getscore](https://github.com/szl187/FeatureGenerator/blob/main/doc/getScore.md)
