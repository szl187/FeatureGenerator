import csv
import os
#change##################
path = "F:\大二下+大三上\VSpy\datachuli\FeatureResultAnalysis\\"#从此路径获取csv文件。（应仅存放需要合并的无表头csv）
excel_name = 'res.csv'#输出文件名（带表头）
csv_head = [
    "sd","id","b","t","pnb","tsb","db","riu",'apb','bb','label'
]#表头
#######################
for files in os.listdir(path):
    datas = []
    # 读取csv文件
    for fname in os.listdir(path):
        if 'csv' in fname:
            fname_path = path + fname
            with open(fname_path, "r") as csvfile:
                reader = csv.reader(csvfile)
                # 去掉表头（第一行）
                for line in reader:
                    datas.append(line)


with open(excel_name, 'w') as csvfile2:
    writer = csv.writer(csvfile2)
    # 写表头
    writer.writerow(csv_head)
    writer.writerows(datas)
with open(excel_name, 'rt') as csvfile3:
    lines=''
    for line in csvfile3:
        if line != '\n':
            lines+=line
with open(excel_name, 'w') as csvfile2:
    csvfile2.write(lines)
#需要手动删掉空的一列和最后一列