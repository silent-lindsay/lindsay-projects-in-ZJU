import cv2
import os
vc = cv2.VideoCapture(r'movie.mp4')  # 读入视频文件，命名vc
n = 1  # 计数
 
if vc.isOpened():  # 判断是否正常打开
    rval, frame = vc.read()
else:
    rval = False
 
timeF = 5  # 视频帧计数间隔频率
 
i = 0
while rval:  # 循环读取视频帧
    rval, frame = vc.read()
    if (n % timeF == 0):  # 每隔timeF帧进行存储操作
        i += 1
        # print(i)
        cv2.imwrite(r'database/{}.jpg'.format(i), frame)  # 存储为图像
    n = n + 1
    cv2.waitKey(1)
vc.release()

print("finished!\n")

# suber = cv2.imread("database/"+str( count ) +".jpg", -1)
# subee = cv2.imread("database/"+str( count-1 ) +".jpg", -1)
# cv2.imshow("Tets",cv2.subtract(suber,subee))
# cv2.waitKey(0)
# cv2.destroyAllWindows()