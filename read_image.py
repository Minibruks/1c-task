import cv2 as cv
import numpy

src = cv.imread("/Users/pavel/Desktop/1c/graph.png", cv.IMREAD_COLOR)
height, width, channels = src.shape

res_str = str(height) + " " + str(width) + " " + str(channels) + "\n"


for elem in src:
	for elem1 in elem:
		l = elem1.tolist()
		l = [str(i) for i in l]
		s = " ".join(l)
		res_str += s + "\n"

print(res_str)
