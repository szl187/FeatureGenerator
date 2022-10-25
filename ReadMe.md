# 工具介绍

本工具获取UDP流量特征

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

