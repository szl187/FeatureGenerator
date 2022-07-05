# 项目介绍
本项目针对视频服务QoE预测的特征提取进行设计。

可对pcap或pcapng文件进行初步流量分类并提取视频流量特征。
# 环境依赖
WinPcap环境

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

输入文件格式：

    .pcap或.pcapng格式文件

输入文件内容：

    参数一：文件相对路径

    参数二：特征标签


输入示例：


    ./FeatureGenerator.exe /moa_tx/moa_1_ppt.pcapng 480p

    可执行文件名              输入文件相对路径              特征标签

输出文件格式：

    utf-8编码无表头csv文件

输出文件名：

    输入文件名+res.csv