CC=gcc

All:
	$(CC) *.c -I opencv2 -l opencv_core -l opencv_highgui -l opencv_video -l opencv_objdetect -o facetrack
