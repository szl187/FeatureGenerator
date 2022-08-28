# 工具介绍

接受FeatureGenerator输出的csv格式特征文件，并基于网络层特征对流量质量进行打分

# 安装
**1.依赖项**

python 3

pandas 

**2.源码文件**
[getscore.py](https://github.com/szl187/FeatureGenerator)

# 使用说明
**1.输入文件**
进行流量特征评分需要输入两个文件：total.csv及dis.csv。它们是对同一pcap文件以不同时间跨度进行分割与特征计算的结果。其中total.csv的分割长度应>dis.csv的。

**2.输出文件**
输出新的csv文件，内容为在total.csv后附加每条流量的得分。

