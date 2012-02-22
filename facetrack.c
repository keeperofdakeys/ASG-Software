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
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

IplImage  * frameCopy = 0;

void cleanup(camshift * cs, int code);
void nextFrame();
void camShifterLoop();
void haarCascadeLoop();

int loops = 50;
struct timeval now_time;
camshift cs;
CvRect * faceRect;
CvCapture * capture;
double face_centre_x;
double face_centre_y;

int main( int argc, char** argv )
{
  capture = cvCaptureFromCAM(1);

  initHaarCascade("/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml");

  cvNamedWindow( "video", 1 );

  nextFrame();
  createCamshift(&cs, frameCopy);

  setVmin(&cs, 60);
  setSmin(&cs, 50);
  int gun_port = open("/dev/ttyACM0",O_WRONLY,O_ASYNC);

  while(1) {
    haarCascadeLoop();
    initTracking(&cs, frameCopy, faceRect);
    camShifterLoop(gun_port);
  }
  close(gun_port);
  cleanup(&cs, 0);
}

int camShifter() {
  CvBox2D fb;
  CvBox2D * faceBox = &fb;
  nextFrame();

  if(!track(&cs, frameCopy, faceBox)) return 0;
  
  face_centre_x = faceBox->center.x;
  face_centre_y = faceBox->center.y;

  cvEllipseBox(frameCopy, *faceBox,
      CV_RGB(255,0,0), 3, CV_AA, 0 );
  cvShowImage( "video", frameCopy );
  return 1;
}


void camShifterLoop(int comm_dev) {
  int i;
  unsigned long int curr_time = now_time.tv_sec*100000 + now_time.tv_usec;
  suseconds_t last_move = curr_time;
  suseconds_t last_fire = curr_time;
  suseconds_t last_ready_fire = curr_time;
  int move_sleep = 75 * 1000; // micro seconds
  int fire_sleep = 500 * 1000;
  int ready_fire_sleep = 200 * 1000;
  int ready_fire = 0;
  double last_x = 0;
  double last_y = 0;

  //for (i = 0; i < loops; i++)
  for(;;)
  {
    curr_time = now_time.tv_sec*100000 + now_time.tv_usec;
    gettimeofday(&now_time,NULL);
    if (!camShifter()) break;
    char key = cvWaitKey(10);

    int window_centre = 320;
    double diff = face_centre_x - window_centre;
    double allowance = 30.0;
    if( last_move < curr_time && !ready_fire ){
      if( diff > allowance ){
        write(comm_dev,">>",2);
        last_move = curr_time + move_sleep;
      }else if( diff < -allowance ){
        write(comm_dev,"<<",2);
        last_move = curr_time + move_sleep;
      }else if( last_fire < curr_time ){
        write(comm_dev,"s",1);
        last_move = curr_time + move_sleep;
        last_ready_fire = curr_time + ready_fire_sleep;
        last_fire = curr_time + fire_sleep;
        last_x = face_centre_x;
        last_y = face_centre_y;
        ready_fire = 1;
      }
    }else if( ready_fire && last_ready_fire < curr_time ){
      if( (int) last_x != (int) face_centre_x || (int) last_y != (int) face_centre_y ){
        write(comm_dev,"f",1);
        break;
      }
      ready_fire = 0;
    }
    //printf("%d\n", i);
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


