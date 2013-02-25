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

#include <opencv/cv.h>
#include <stdio.h>
#include "haarCascade.h"


CvHaarClassifierCascade * cascade = 0;
CvMemStorage * storage = 0;
CvSeq * faceRectSeq;

void initHaarCascade(const char * haarCascadePath)
{
        storage = cvCreateMemStorage(0);

        cascade = (CvHaarClassifierCascade *)cvLoad( haarCascadePath, 0, 0, 0 );
        if( !cascade )
        {
                fprintf(stderr, "XML Classifier not found\n");
                exit(-1);
        }
}


void closeHaarCascade()
{
        if(cascade) cvReleaseHaarClassifierCascade(&cascade);
        if(storage) cvReleaseMemStorage(&storage);
}

CvRect * haarDetectFace(IplImage * img)
{
        CvRect* rect = 0;

        int minFaceSize = img->width / 10; // change these depending on size of face in the frame
        int maxFaceSize = img->width / 3;
        faceRectSeq = cvHaarDetectObjects
                (img, cascade, storage,
                 1.1,
                 6,
                 CV_HAAR_DO_CANNY_PRUNING, // other options here, should explore
                 cvSize(minFaceSize, minFaceSize), cvSize(maxFaceSize, maxFaceSize));

        if( faceRectSeq && faceRectSeq->total )
                rect = (CvRect*)cvGetSeqElem(faceRectSeq, 0);

        return rect;
}

