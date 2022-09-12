import numpy as np
import cv2
import os

COMPRESS = 16 # the size of compression
threshold = 80

def convertImage(image): # compress size to 
    image = cv2.resize(image,(COMPRESS,COMPRESS),interpolation=cv2.INTER_CUBIC)
    image = cv2.cvtColor(image,cv2.COLOR_RGB2GRAY)
    return image

def hammingDist(s1, s2): # calculate the Hamming Distance
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
    ham_img1 = convertImage(img1) .astype(float)# compress the image and transform to matrix of float

    img2 = cv2.imread("database/"+str( count+1 ) +".jpg", -1)
    ham_img2 = convertImage(img2) .astype(float)# compress the image and transform to matrix of float
    avg = ( np.mean(ham_img2) + np.mean(ham_img1) )/2

    img1_list = ['0' if i > avg else '1' for i in ham_img1.flatten()]# calculate the 0-1 array that represents matrix of imag
    img2_list = ['0' if i > avg else '1' for i in ham_img2.flatten()]# calculate the 0-1 array that represents matrix of imag

    difference = hammingDist(img1_list,img2_list)
    if difference > threshold :
        collection.append(count+1) # collect the switching shot
count += 1

shots = 0  # count the number of correct frames
for e in collection :
    if ret.count(e) > 0 :
        shots += 1

print("Compression size: {}*{}".format(COMPRESS,COMPRESS),"Threshold:",threshold)
print("Accuracy: {:.2f}%".format(100*shots/len(collection)) )
print("Recall: {:.2f}%".format( 100*shots/len(ret)) )