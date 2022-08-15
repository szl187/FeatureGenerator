# 项目介绍
本项目用于从pcap或pcapng文件中提取流量特征信息。
# 环境依赖
一、WinPcap环境

包含目录：$(WinPcapHome)\Include;

库目录：$(WinPcapHome)\Lib;

预处理器：WPCAP;

链接器命令行其他选项：wpcap.lib ws2_32.lib

WinPcap安装包获取路径：https://www.winpcap.org/install/default.htm


# 目录结构描述

├── readme.md // 帮助文档

├── 程序说明.doc //代码介绍

├── flow.h 

├── flow.cpp

├── flow_process.h

├── flow_process.cpp

├── main.cpp

├── …… //视频流量文件夹

# 使用说明



程序使用方法：

可执行程序名 输入类型参数 绝对路径 可选参数 参数值 
     
输入类型参数说明：
| 输入类型参数 | 说明 | 备注 |
| --- | --- | --- |
|  -file | pcap或pcapng文件的绝对路径 |  |
| -folder  | 文件夹绝对路径 | 不读取子文件夹 |

可选参数说明：
| 可选参数 | 参数值 | 说明 | 备注 |
|----|----|----|----|
| -csv | 无 | 输出csv版本 | 默认为txt输出 |
| -v | 数字 | 输出每个流前v个数据包的长度 | txt版本输出有效 |
| -rawdata | 无 | 输出每个包的时间戳及四元组 |  |
| -i | 数字 | 以i秒为单位截取流量并计算特征 |  |
	
    
    

输出文件格式：

    输出1： 输入文件名+QoEFeat.txt
   
    输出2： 输入文件名+RawData.csv
