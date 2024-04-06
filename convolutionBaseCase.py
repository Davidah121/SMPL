import cv2
import numpy as np
import skimage.exposure

img = cv2.imread("Bikesgray.jpg", 0)
xKernel = np.array([[1, 0, -1],
                      [2, 0, -2],
                      [1, 0, -1]])


yKernel = np.array([[1, 2, 1],
                      [0, 0, 0],
                      [-1, -2, -1]])

fltXImg = cv2.filter2D(src=img, ddepth=-1, kernel=xKernel)
fltYImg = cv2.filter2D(src=img, ddepth=-1, kernel=yKernel)

magImg = np.hypot(fltXImg, fltYImg)
magImg = skimage.exposure.rescale_intensity(magImg, in_range='image', out_range=(0,255)).astype(np.uint8)

cv2.imshow('X Gradient', fltXImg)
cv2.imshow('Y Gradient', fltYImg)
cv2.imshow('Magnitude', magImg)
cv2.waitKey(0)

cv2.destroyAllWindows()