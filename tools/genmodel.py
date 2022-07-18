import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
from sklearn.preprocessing import LabelEncoder
from sklearn.metrics import confusion_matrix
from joblib import dump,load
import m2cgen as m2c


#need change#########################################
#输入文件名
df = pd.read_csv("webrtcres.csv")
#输出模型名
output_joblibname = "models/webrtc.joblib"
#输出代码文件名
output_cppcodename = "models/webrtc.cpp"
#标签类型：res或frame
labeltype = "res"
#特征列["sd","id","b","t","pnb","tsb","db","riu","bb","apb"]
featurelist=["sd","id","b","t","pnb","tsb","db","riu"]
#训练集比例
train_rate=0.5
#######################################################

#生成标签名的数字代号
if labeltype == "res":
    encoder0 = LabelEncoder().fit(df['label'])
    df['l'] = encoder0.transform(df['label'])
elif labeltype == "frame":
    df['l'] = df["label"]
    df.loc[df['l']<=30,'l'] = 0
    df.loc[df['l']>30,'l'] = 1

#bb列可能存在超出float的值，需要删除
print(df[df["bb"]>3E+38].index)
df = df.drop(df[df["bb"]>3E+38].index)

#混洗df，输出标签与数字代号的映射
from sklearn.utils import shuffle
df = shuffle(df)
map_dict = dict(zip(df['l'].tolist(),df['label'].tolist()))
print(map_dict)
print(df.head(5))

#选取特征；列
#X = df[["sd","id","b","t","pnb","tsb","db","riu","bb","apb"]]#,"bb","apb"
X = df[featurelist]

y = df["l"]
x_train, x_test, y_train, y_test = train_test_split(X,y,test_size = train_rate)

clf1 = RandomForestClassifier(max_depth=11,n_estimators=10)
clf1.fit(x_train, y_train)
y_pred1 = clf1.predict(x_test)
print(sorted(zip(map(lambda x:round(x,4),clf1.feature_importances_),featurelist)))
rf1 = pd.DataFrame(list(zip(y_pred1,y_test)),columns=['predicted','actual'])
rf1['correct'] = rf1.apply(lambda r: 1 if r['predicted']==r['actual'] else 0,axis=1)
print("随机森林准确率：",rf1['correct'].sum()/rf1['correct'].count())
print(classification_report(y_test,y_pred1))

#from sklearn.neighbors import  KNeighborsClassifier
#knn = KNeighborsClassifier(n_neighbors=5)
#knn.fit(x_train,y_train)
#y_pred2 = knn.predict(x_test)
#print("KNN准确率：",knn.score(x_test,y_test))
#print(classification_report(y_test,y_pred2))

#from sklearn.naive_bayes import MultinomialNB
#mlt = MultinomialNB()
#mlt.fit(x_train,y_train)
#y_pred3 = mlt.predict(x_test)
#print("朴素贝叶斯准确率：",mlt.score(x_test,y_test))
#print(classification_report(y_test,y_pred3))

#from sklearn import svm
#clf4 = svm.SVC(kernel='linear')
#clf4.fit(x_train,y_train)
#y_pred4 = clf4.predict(x_test)
#print("SVM准确率：",clf4.score(x_test,y_test))
#print("精准率和召回率",classification_report(y_test,y_pred4))
#import warnings
#warnings.filterwarnings("ignore")
#from sklearn.linear_model import LogisticRegression
#clf5 = LogisticRegression(penalty='l2',tol=0.01)
#clf5.fit(x_train,y_train)
#y_pred5 = clf5.predict(x_test)
#print("线性回归准确率：",clf5.score(x_test,y_test))
#print(classification_report(y_test,y_pred5))

from sklearn import tree
clf6 = tree.DecisionTreeClassifier()
model = clf6.fit(x_train,y_train)
dump(clf6,output_joblibname)
y_pred6 = clf6.predict(x_test)
with open(output_cppcodename,"w") as f:
    f.write(m2c.export_to_c(model))

print(sorted(zip(map(lambda x:round(x,4),clf6.feature_importances_),featurelist)))
print("决策树准确率：",clf6.score(x_test,y_test))
print(classification_report(y_test,y_pred6))
print(confusion_matrix(y_test,y_pred6))