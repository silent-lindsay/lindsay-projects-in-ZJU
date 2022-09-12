import numpy as np
import cv2
import os

count = 0 # count the number of frames in the video
collection = []
threshold = 1e6

while ( True ) :
    name = "database/"+str( count+2 ) +".jpg"
    if ( not os.path.exists(name) ) :
        break
    count += 1

    img1 = cv2.imread("database/"+str( count ) +".jpg", 0)
    img2 = cv2.imread("database/"+str( count+1 ) +".jpg", 0)
    difference = np.sum( np.abs( cv2.subtract(img2,img1)) ) 
    # difference = np.sum(img2-img1)
    if difference > threshold :
        collection.append(count+1) # collect the switching shot
count += 1
# print("totally there are",count,"frames")
# print("and the switches of shot:",collection,"\ntotally:",len(collection))

with open("expection",'r') as f :
    target = f.readline().strip().split(",")
ret = [int(i) for i in target] # ret is list of all target frames

shots = 0 
for e in collection :
    if ret.count(e) > 0 :
        shots += 1

print("Threshold:",threshold)
print("Accuracy: {:.2f}%".format(100*shots/len(collection)) )
print("Recall: {:.2f}%".format( 100*shots/len(ret)) )