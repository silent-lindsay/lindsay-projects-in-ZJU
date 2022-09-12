import matplotlib.pyplot as plt
import numpy as np
import cv2
import os

def convertImage(image):
    image = cv2.resize(image,(32,32),interpolation=cv2.INTER_CUBIC)
    image = cv2.cvtColor(image,cv2.COLOR_RGB2GRAY)
    return image
def hammingDist(s1, s2):
    assert len(s1) == len(s2)
    return [ch1 != ch2 for ch1, ch2 in zip(s1, s2)].count(True)

COMPRESS = 32
MAXB = 250
MINB = 190
LEVEL = 5
with open("expection",'r') as f :
    target = f.readline().strip().split(",")
ret = [int(i) for i in target] # ret is list of all target frames
accuracies = {}
recalls = {}
accurecall = {}

thresholds = [i for i in range(MINB,MAXB+1,LEVEL)]

for threshold in thresholds :
    count = 0 # count the number of frames in the video
    collection = [] # collector of target frames

    while ( True ) :
        name = "database/"+str( count+2 ) +".jpg"
        if not os.path.exists(name) :
            break
        count += 1

        img1 = cv2.imread("database/"+str( count ) +".jpg", -1)
        img1 = convertImage(img1) .astype(float)
        dct_img1 = cv2.dct( cv2.dct(img1) )[:COMPRESS,:COMPRESS]

        img2 = cv2.imread("database/"+str( count+1 ) +".jpg", -1)
        img2 = convertImage(img2) .astype(float)
        dct_img2 = cv2.dct( cv2.dct(img2) )[:COMPRESS,:COMPRESS]
        avg = ( np.mean(dct_img2) + np.mean(dct_img1) )/2

        img1_list = ['0' if i > avg else '1' for i in dct_img1.flatten()]
        img2_list = ['0' if i > avg else '1' for i in dct_img2.flatten()]

        # difference = np.sum(np.abs(np.subtract(dct_img2,dct_img1)))
        difference = hammingDist(img1_list,img2_list)
        if difference > threshold :
            collection.append(count+1) # collect the switching frame
    count += 1

    shots = 0 
    for e in collection :
        if ret.count(e) > 0 :
            shots += 1
    accuracies[threshold] = float( "{:.2f}".format( 100*shots/len(collection) ) )
    recalls[threshold] = float( "{:.2f}".format( 100*shots/len(ret) ) )
    accurecall[threshold] = accuracies[threshold]+recalls[threshold]

plt.plot(accuracies.keys(),accuracies.values(),label = "Accuracy")
plt.plot(recalls.keys(),recalls.values(),label = "Recall")
plt.plot(accurecall.keys(),accurecall.values(),label = "Sum")
plt.legend()
plt.xlabel("Threshold")
plt.ylabel("%")
plt.title("Hash-Detection Curve")
plt.show()