# 工具介绍

接受FeatureGenerator输出的CSV特征文件，并基于网络层特征的稳定性进行打分

# 安装

**1.依赖项**

python3

pandas

**2.源码**

[getscore.py](https://github.com/szl187/FeatureGenerator/tools)

# 使用说明

**1.输入文件**

total.csv ： 对某pcap文件按长时间跨度进行特征计算的特征文件

dis.csv   ： 对同一pcap文件按短时间跨度进行特征计算的特征文件

**2.输出文件**

输出csv文件，内容为在total.csv每条流量之后附加该流的得分
