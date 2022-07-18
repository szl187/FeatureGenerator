# 项目介绍
本项目针对视频服务QoE预测的特征提取进行设计。

可对pcap或pcapng文件进行初步流量分类并提取视频流量特征。
# 环境依赖
一、WinPcap环境

包含目录：$(WinPcapHome)\Include;

库目录：$(WinPcapHome)\Lib;

预处理器：WPCAP;

链接器命令行其他选项：wpcap.lib ws2_32.lib

WinPcap安装包获取路径：https://www.winpcap.org/install/default.htm

**二、Windows环境

使用io.h库及time.h库，需要在windows环境下运行**

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

输入文件格式：

    .pcap或.pcapng格式文件

~~输入文件内容~~ 

   ~~参数一：文件相对路径~~

   ~~参数二：特征标签~~

    
     可执行程序名 输入类型参数 绝对路径 可选参数 参数值 
     

| 输入类型参数 | 说明 | 备注 |
| --- | --- | --- |
|  -file | pcap或pcapng文件的绝对路径 |  |
| -folder  | 文件夹绝对路径 | 不读取子文件夹，文件数<2时无法读取 |

| 可选参数 | 参数值 | 说明 | 备注 |
|----|----|----|----|
| -autocl | 无 | 依模型进行流量分类 | 未实现 |
| -cl | 分类标签 | 人工打流量分类标签 | 未实现 |
| -autoel | 无 | 依模型进行QoE预测 | 未实现 |
| -el | QoE标签 | 人工打QoE标签 | OK |
| -rawdata | 无 | 产生额外输出文件记录数据包原始信息 | OK |
| -sd | 无 | 使用带延迟参数的bulk特征（默认不使用） | OK |
| -lite | 无 | 不输出似乎没啥用的特征 | OK |
	
    
    

输出文件格式：

    utf-8编码无表头csv文件
~~输入文件名+res.csv~~
    输出1： 输入文件名+QoEFeat.csv
   
    输出2： 输入文件名+QoSFeat.csv （未实现）
   
    输出3： 输入文件名+RawData.csv