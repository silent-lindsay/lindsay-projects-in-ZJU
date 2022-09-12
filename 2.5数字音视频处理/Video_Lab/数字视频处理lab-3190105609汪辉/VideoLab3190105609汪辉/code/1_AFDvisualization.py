import matplotlib.pyplot as plt
import numpy as np
import cv2
import os

with open("expection",'r') as f :
    target = f.readline().strip().split(",")
ret = [int(i) for i in target] # ret is list of all target frames

ThreUnit = 1e5
thresholds = [i for i in range(2,55,3)]
accuracies = {}
recalls = {}
accurecall = {}

for threshold in thresholds:
    collection = []
    count = 0 # count the number of frames in the video
    while ( True ) :
        name = "database/"+str( count+2 ) +".jpg" # count +2 since we need compare every two frames
        if ( not os.path.exists(name) ) :
            break
        count += 1
        img1 = cv2.imread("database/"+str( count ) +".jpg", 0)
        img2 = cv2.imread("database/"+str( count+1 ) +".jpg", 0)
        difference = abs( int(np.sum(img2)) - int(np.sum(img1)) )
        if difference > threshold*ThreUnit :
            collection.append(count+1) # collect the switching shot
            
    shots = 0 
    for e in collection :
        if ret.count(e) > 0 :
            shots += 1 # count the valid frames found
    accuracies[threshold] = float( "{:.2f}".format( 100*shots/len(collection) ) )
    recalls[threshold] = float( "{:.2f}".format( 100*shots/len(ret) ) )
    accurecall[threshold] = accuracies[threshold]+recalls[threshold]

plt.rcParams['font.sans-serif']=['SimHei'] # 用来正常显示中文标签

plt.plot(accuracies.keys(),accuracies.values(),label = "Accuracy")
plt.plot(recalls.keys(),recalls.values(),label = "Recall")
plt.plot(accurecall.keys(),accurecall.values(),label = "Sum")
plt.legend()
plt.xlabel("Threshold/1e5")
plt.ylabel("%")
plt.title("Absolute-Frame-Difference Curve")
plt.show()