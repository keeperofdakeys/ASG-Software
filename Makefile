CC=clang

All:
	$(CC) *.c -I opencv2 -l opencv_core -l opencv_highgui -l opencv_video -l opencv_objdetect -l opencv_imgproc -o facetrack
