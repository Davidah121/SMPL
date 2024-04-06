import numpy as np
import cv2 as cv
from matplotlib import pyplot as plt

img = cv.imread('Bikesgray.jpg', cv.IMREAD_GRAYSCALE)
assert img is not None, "file could not be read, check with os.path.exists()"

laplacianf = cv.Laplacian(img,cv.CV_64F)
sobelxf = cv.Sobel(img,cv.CV_64F,1,0,ksize=3)
sobelyf = cv.Sobel(img,cv.CV_64F,0,1,ksize=3)

laplacian = np.uint8(np.absolute(laplacianf))
sobelx = np.uint8(np.absolute(sobelxf))
sobely = np.uint8(np.absolute(sobelyf))

plt.subplot(2,2,1),plt.imshow(img,cmap = 'gray')
plt.title('Original'), plt.xticks([]), plt.yticks([])
plt.subplot(2,2,2),plt.imshow(laplacian,cmap = 'gray')
plt.title('Laplacian'), plt.xticks([]), plt.yticks([])
plt.subplot(2,2,3),plt.imshow(sobelx,cmap = 'gray')
plt.title('Sobel X'), plt.xticks([]), plt.yticks([])
plt.subplot(2,2,4),plt.imshow(sobely,cmap = 'gray')
plt.title('Sobel Y'), plt.xticks([]), plt.yticks([])
plt.show()