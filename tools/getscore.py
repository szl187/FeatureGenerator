import pandas as pd
import numpy as np
#featurelist=["ipsrc","portsrc","startts","endts",'pktlen','itvl','bw','thp']
featurelist=["ipsrc","portsrc","startts","endts",'pktlen','itvl','bw','thp']
df1=pd.read_csv("score/total.csv")[featurelist]#读取整条流量的特征
df2=pd.read_csv("score/dis.csv")[featurelist]#读取切割后的特征
scorelist=[]
for tup in zip(df1["ipsrc"],df1["portsrc"],df1["startts"],df1["endts"],df1["pktlen"],df1['itvl'],df1['bw'],df1['thp']):
    df = df2[(df2['ipsrc'].str.match(str(tup[0]))) & (df2['portsrc']==tup[1]) & (df2['startts']>=tup[2])& (df2['endts']<=tup[3])]
    df=df[['pktlen','itvl','bw','thp']]
    i,s1,s2,s3,s4=0,0,0,0,0
    for tup1 in zip(df["pktlen"],df['itvl'],df['bw'],df['thp']):
        i += 1
        s1 += abs(tup1[0] - tup[4])
        s2 += abs(tup1[1] - tup[5])
        s3 += abs(tup1[2] - tup[6])
        s4 += abs(tup1[3] - tup[7])
    score =4 - (s1/tup[4]+s2/tup[5]+s3/tup[6]+s4/tup[7])
    scorelist.append(score)
df3=pd.read_csv("score/total.csv")
df3["mosscore"] = scorelist
df3.to_csv("score/total_scored.csv")
print(scorelist)

