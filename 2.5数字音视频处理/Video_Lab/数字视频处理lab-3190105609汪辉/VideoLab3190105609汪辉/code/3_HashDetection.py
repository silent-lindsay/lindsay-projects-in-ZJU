import numpy as np
import cv2
import os

COMPRESS = 48 # the size of compression
CHARACSIZE = 36 # the size of characteristic matrix
threshold = 370

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

count = 0 # count the number of frames in the video
collection = [] # collector of target frames

# print( len(cv2.imread("database/"+str(1) +".jpg")) ) 
# size of each frame is 240

while ( True ) :
    if ( not os.path.exists("database/"+str( count+2 ) +".jpg") ) :
        break
    count += 1

    img1 = cv2.imread("database/"+str( count ) +".jpg", -1)
    img1 = convertImage(img1) .astype(float) # compress the image and transform to matrix of float
    dct_img1 = cv2.dct( cv2.dct(img1) )[:CHARACSIZE,:CHARACSIZE] # by double DCT transforming

    img2 = cv2.imread("database/"+str( count+1 ) +".jpg", -1)
    img2 = convertImage(img2) .astype(float) # compress the image and transform to matrix of float
    dct_img2 = cv2.dct( cv2.dct(img2) )[:CHARACSIZE,:CHARACSIZE] # by double DCT transforming
    avg = ( np.mean(dct_img2) + np.mean(dct_img1) )/2

    img1_list = ['0' if i > avg else '1' for i in dct_img1.flatten()] # calculate the 0-1 array that represents the character of matrix of imag
    img2_list = ['0' if i > avg else '1' for i in dct_img2.flatten()] # calculate the 0-1 array that represents the character of matrix of imag

    difference = hammingDist(img1_list,img2_list)
    # print(difference)
    if difference > threshold :
        collection.append(count+1) # collect the switching shot
count += 1

shots = 0 # count the number of correct frames
for e in collection :
    if ret.count(e) > 0 :
        shots += 1

print("Compression size:",COMPRESS,"Character size:",CHARACSIZE,"Threshold:",threshold)
print("Accuracy: {:.2f}%".format(100*shots/len(collection)) )
print("Recall: {:.2f}%".format( 100*shots/len(ret)) )