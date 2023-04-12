
import cv2

img = cv2.imread('data/a.jpeg')
print(img.shape[0:2])
gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
moment = cv2.moments(gray_img)
X = int(moment ["m10"] / moment["m00"])
print('X:',X)
print('\n')
Y = int(moment ["m01"] / moment["m00"])
print('y:',Y)
print('\n')
#cent = cv2.circle(img, (X, Y), 15, (205, 114, 101), 1)
#image, center_coordinates, Radius of circle, color, thickness
cent = cv2.circle(img, (X, Y),2,(255, 255, 255), 10)
print('Central Point:',cent[0])
cv2.imshow("Center of the Image", img)

cv2.waitKey(0)

'''
img = cv2.imread('data/a.jpeg')
gray_image = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
# convert the grayscale image to binary image
ret,thresh = cv2.threshold(gray_image,127,255,0)
# calculate moments of binary image
M = cv2.moments(thresh)
# calculate x,y coordinate of center
cX = int(M["m10"] / M["m00"])
cY = int(M["m01"] / M["m00"])
# put text and highlight the center
cv2.circle(img, (cX, cY), 5, (255, 255, 255), -1)
cv2.putText(img, "centroid", (cX - 25, cY - 25),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
# display the image
cv2.imshow("Image", img)
cv2.waitKey(0)'''
