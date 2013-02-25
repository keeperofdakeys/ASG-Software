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
struct camshiftStruct {
  int   nHistBins;
  float rangesArr[2];
  int vmin;
  int vmax;
  int smin;
  IplImage * HSVImg;
  IplImage * hueImg;
  IplImage * mask;
  IplImage * probImg;
  CvHistogram * hist;
  CvRect prevFaceRect;
  CvBox2D faceBox;
  int nFrames;
};
typedef struct camshiftStruct camshift;

void     createCamshift(camshift * cs, const IplImage * img);
void    releaseCamshift(camshift * cs);
void    initTracking(camshift * cs, IplImage * img, CvRect * rect);
int track(camshift * cs, IplImage *, CvBox2D *);

void setVmin(camshift * cs, int vmin);
void setSmin(camshift * cs, int smin);
