import matplotlib.pyplot as plt
import numpy as np
import cv2
import os

COMPRESS = 32 # the size of compression
CHARACSIZE = 8 # the size of characteristic matrix
# args for iterating thresholds
MAXB = 32 
MINB = 4
LEVEL = 2

def convertImage(image): # compress size to 
    image = cv2.resize(image,(COMPRESS,COMPRESS),interpolation=cv2.INTER_CUBIC)
    image = cv2.cvtColor(image,cv2.COLOR_RGB2GRAY)
    return image
def hammingDist(s1, s2): # compute the number of different bits
    assert len(s1) == len(s2)
    return [ch1 != ch2 for ch1, ch2 in zip(s1, s2)].count(True)

with open("expection",'r') as f : # load the correct frames in advance
    target = f.readline().strip().split(",")
ret = [int(i) for i in target] # ret is list of all target frames
accuracies = {} # record data of accuracies
recalls = {} # record data of recalls
accurecall = {} # record data of sum

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
        img1 = convertImage(img1) .astype(float) # compress the image and transform to matrix of float
        dct_img1 = cv2.dct( cv2.dct(img1) )[:CHARACSIZE,:CHARACSIZE]# by double DCT transforming and character extracting

        img2 = cv2.imread("database/"+str( count+1 ) +".jpg", -1)
        img2 = convertImage(img2) .astype(float) # compress the image and transform to matrix of float
        dct_img2 = cv2.dct( cv2.dct(img2) )[:CHARACSIZE,:CHARACSIZE]# by double DCT transforming and character extracting
        avg = ( np.mean(dct_img2) + np.mean(dct_img1) )/2

        img1_list = ['0' if i > avg else '1' for i in dct_img1.flatten()]# calculate the 0-1 array that represents the character of matrix of imag
        img2_list = ['0' if i > avg else '1' for i in dct_img2.flatten()]# calculate the 0-1 array that represents the character of matrix of imag

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