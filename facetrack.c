/*
    FaceTrack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FaceTrack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FaceTrack.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <opencv2/core/core_c.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include "haarCascade.h"
#include "camshift.h"

IplImage  * frameCopy = 0;

void cleanup(camshift * cs, int code);
void nextFrame();
void camShifterLoop();
void haarCascadeLoop();

int loops = 50;
CvRect * faceRect = 0;
CvCapture * capture;
camshift cs;

int main( int argc, char** argv )
{
  capture = cvCaptureFromCAM(-1);

  initHaarCascade("/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml");

  cvNamedWindow( "video", 1 );

  nextFrame();
  createCamshift(&cs, frameCopy);

  setVmin(&cs, 60);
  setSmin(&cs, 50);

  while(1) {
    haarCascadeLoop();
    initTracking(&cs, frameCopy, faceRect);
    camShifterLoop();
  }

  cleanup(&cs, 0);
}

int camShifter() {
  CvBox2D fb;
  CvBox2D * faceBox = &fb;
  nextFrame();

  if(!track(&cs, frameCopy, faceBox)) return 0;

  cvEllipseBox(frameCopy, *faceBox,
      CV_RGB(255,0,0), 3, CV_AA, 0 );
  cvShowImage( "video", frameCopy );
  return 1;
}


void camShifterLoop() {
  int i;
  for (i = 0; i < loops; i++)
  {
    if (!camShifter()) break;
    char key = cvWaitKey(10);
    if( (char)27==key ) break;
    if( (char)43==key ) loops += 10;
    if( (char)45==key ) loops -= 10;

    printf("%d\n", i);
  }
}

int haarCascade() {
  nextFrame();
  faceRect = haarDetectFace(frameCopy);

  cvShowImage( "video", frameCopy );
  if( (char)27==cvWaitKey(10) ) cleanup(&cs, 0);

  if(faceRect) return -1;
  return 0;
}

void haarCascadeLoop() {
  while(haarCascade() == 0)
  {
  }
}

void cleanup(camshift * cs, int code)
{
  cvDestroyWindow( "video" );
  cvReleaseImage( &frameCopy );

  cvReleaseCapture(&capture);
  closeHaarCascade();
  releaseCamshift(cs);

  exit(code);
}

void nextFrame(camshift * cs)
{
  IplImage  * frame = cvQueryFrame( capture );

  if( !frame ) cleanup(cs, -1);

  if( !frameCopy )
    frameCopy = cvCreateImage( cvGetSize(frame), 8, 3 );
  cvCopy( frame, frameCopy, 0 );
  frameCopy->origin = frame->origin;

  if( 1 == frameCopy->origin ) // 1 means the image is inverted
  {
    cvFlip( frameCopy, 0, 0 );
    frameCopy->origin = 0;
  }
}


