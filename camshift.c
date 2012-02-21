/*    This file is part of FaceTrack.

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
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <stdio.h>
#include "camshift.h"
void updateHueImage(camshift * cs, const IplImage * img)
{
  cvCvtColor( img, cs->HSVImg, CV_BGR2HSV );

  cvInRangeS( cs->HSVImg, cvScalar(0, cs->smin, MIN(cs->vmin,cs->vmax), 0),
      cvScalar(180, 256, MAX(cs->vmin,cs->vmax) ,0), cs->mask );

  cvSplit( cs->HSVImg, cs->hueImg, 0, 0, 0 );
}


void createCamshift(camshift * cs, const IplImage * img)
{
  float * pRanges = cs->rangesArr;
  cs->nHistBins = 30;
  cs->rangesArr[0] = 0;
  cs->rangesArr[1] = 180;
  cs->vmin = 65;
  cs->vmax = 256;
  cs->smin = 55;
  cs->nFrames = 0;
  cs->HSVImg  = cvCreateImage( cvGetSize(img), 8, 3 );
  cs->hueImg  = cvCreateImage( cvGetSize(img), 8, 1 );
  cs->mask    = cvCreateImage( cvGetSize(img), 8, 1 );
  cs->probImg = cvCreateImage( cvGetSize(img), 8, 1 );

  cs->hist = cvCreateHist( 1, &cs->nHistBins, CV_HIST_ARRAY, &pRanges, 1 );
}

void releaseCamshift(camshift * cs)
{
  cvReleaseImage( &cs->HSVImg );
  cvReleaseImage( &cs->hueImg );
  cvReleaseImage( &cs->mask );
  cvReleaseImage( &cs->probImg );

  cvReleaseHist( &cs->hist );
}


void initTracking(camshift * cs, IplImage * img, CvRect * faceRect)
{
  float maxVal = 0.f;

  updateHueImage(cs, img);

  cvSetImageROI( cs->hueImg, *faceRect );
  cvSetImageROI( cs->mask,   *faceRect );
  cvCalcHist( &cs->hueImg, cs->hist, 0, cs->mask );
  cvGetMinMaxHistValue( cs->hist, 0, &maxVal, 0, 0 );
  cvConvertScale( cs->hist->bins, cs->hist->bins,
      maxVal? 255.0/maxVal : 0, 0 );
  cvResetImageROI( cs->hueImg );
  cvResetImageROI( cs->mask );

  cs->prevFaceRect = *faceRect;
}

int track(camshift * cs, IplImage * img, CvBox2D * fBox)
{
  CvConnectedComp components;

  updateHueImage(cs, img);

  cvCalcBackProject( &cs->hueImg, cs->probImg, cs->hist );
  cvAnd( cs->probImg, cs->mask, cs->probImg, 0 );

  CvSize size = cvGetSize(cs->probImg);
//  printf("%d %d %d %d\n", cs->prevFaceRect.x, cs->prevFaceRect.y, cs->prevFaceRect.width, cs->prevFaceRect.height);
  if (cs->prevFaceRect.x <= 0) {
    return 0;
  }
  if (cs->prevFaceRect.x > size.width) {
    return 0;
  }
  if (cs->prevFaceRect.y <= 0) {
    return 0;
  }
  if (cs->prevFaceRect.y > size.height) {
    return 0;
  }

  if (cs->prevFaceRect.x + cs->prevFaceRect.width > size.width) {
    return 0;
  }
  if (cs->prevFaceRect.y + cs->prevFaceRect.height > size.height) {
    return 0;
  }
  if (cs->prevFaceRect.width <= 0) {
    return 0;
  }
  if (cs->prevFaceRect.height <= 0) {
    return 0;
  }


  cvCamShift( cs->probImg, cs->prevFaceRect,
      cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
      &components, &cs->faceBox );

  cs->prevFaceRect = components.rect;
  cs->faceBox.angle = -cs->faceBox.angle;

  *fBox = cs->faceBox;
  return 1;
}

void setVmin(camshift * cs, int _vmin)
{ cs->vmin = _vmin; }


void setSmin(camshift * cs, int _smin)
{ cs->smin = _smin; }
