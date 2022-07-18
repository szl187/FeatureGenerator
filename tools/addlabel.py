import os
import pandas as pd
path = "F:\大二下+大三上\VSpy\datachuli\FeatureResultAnalysis\\"
#自动为文件名中写了分辨率的无表头csv文件添加分辨率标签并删除空的列
#运行后进行合并时应删除原文件
for files in os.listdir(path):
    for fname in os.listdir(path):
        if 'csv' in fname and 'label' not in fname:
            fname_path = path + fname
            df = pd.read_csv(fname_path,header=None,names=["sd","id","b","t","pnb","tsb","db","riu",'apb','bb'])
            df.dropna(axis=1,how='all',inplace=True)
            #change
            resl = None
            if '360' in fname:
                resl = 360
            if '480' in fname:
                resl = 480
            if '720' in fname:
                resl = 720
            if '1080' in fname:
                resl = 1080
            df["label"] = resl
            newname = 'label'+fname
            df.to_csv(newname,header=None)
