/*
    Author: Alberto G. Lagos (Kronen)
    Copyright (C) 2010  Alberto G. Lagos (Kronen)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "camshift.h"

CamShift::CamShift(CvSize size) {
    float *ranges = mRangesArray;
    mHistBins = 30;
    mRangesArray[0] = 0;
    mRangesArray[1] = 180;
    mVMin = 50;
    mVMax = 256;
    mSMin = 50;
    mFrames = 0;
    mHSVImg  = cvCreateImage(size, 8, 3);
    mHueImg  = cvCreateImage(size, 8, 1);
    mMask    = cvCreateImage(size, 8, 1);
    mProbImg = cvCreateImage(size, 8, 1);

    mHist = cvCreateHist(1, &mHistBins, CV_HIST_ARRAY, &ranges, 1);
}

CamShift::~CamShift() {
    cvReleaseImage(&mHSVImg);
    cvReleaseImage(&mHueImg);
    cvReleaseImage(&mMask);
    cvReleaseImage(&mProbImg);

    cvReleaseHist(&mHist);
}

void CamShift::startTracking(IplImage *cvImage, CvRect cvRect) {
    float maxVal = 0.f;

    // Get mHSVImg and mHueImg
    updateHueImage(cvImage);

    // Calc the histogram of the defined rect in mHueImg
    cvSetImageROI(mHueImg, cvRect);
    cvSetImageROI(mMask, cvRect);
    cvCalcHist(&mHueImg, mHist, 0, mMask);  // Format: cvCalcHist(image, histogram, accumulate, mask)

    // We get the MaxValue in the histogram and scale for that value
    cvGetMinMaxHistValue(mHist, 0, &maxVal, 0, 0);  // Format: cvGetMinMaxHistValue(hist, minVal, maxVal, minIndex, maxIndex)
    cvConvertScale(mHist->bins, mHist->bins, maxVal ? (255.0 / maxVal) : 0, 0); // Format: cvConvertScale(src, dst, scale, shift)
    cvResetImageROI(mHueImg);
    cvResetImageROI(mMask);

    // Store the previous face location
    mPrevFaceRect = cvRect;
}

CvBox2D CamShift::trackFace(IplImage *cvImage) {
    CvConnectedComp components;

    // Create a new hue image
    updateHueImage(cvImage);

    // Create a probability image based on the face histogram (precalculated on startTracking())
    cvCalcBackProject(&mHueImg, mProbImg, mHist);
    cvAnd(mProbImg, mMask, mProbImg, 0);    // cvAnd(src1, src2, dst, mask)
    CvSize size = cvGetSize(mProbImg);

    // Check for face out of scope
    if(mPrevFaceRect.x < 0) mPrevFaceRect.x = 0;
    if(mPrevFaceRect.x >= size.width) mPrevFaceRect.x = size.width - 1;
    if(mPrevFaceRect.y < 0) mPrevFaceRect.y = 0;
    if(mPrevFaceRect.y >= size.height) mPrevFaceRect.y = size.height - 1;
    if(mPrevFaceRect.x + mPrevFaceRect.width > size.width) mPrevFaceRect.width = size.width - mPrevFaceRect.x;
    if(mPrevFaceRect.y + mPrevFaceRect.height > size.height) mPrevFaceRect.height = size.height - mPrevFaceRect.y;

    // Use CamShift to find the center of the new face probability
    cvCamShift(mProbImg, mPrevFaceRect, cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
               &components, &mFaceBox);

    // Update face location and angle
    mPrevFaceRect = components.rect;
    mFaceBox.angle = -mFaceBox.angle;

    return mFaceBox;
}

void CamShift::updateHueImage(const IplImage *cvImage) {
    // Convert to HSV color model
    cvCvtColor(cvImage, mHSVImg, CV_BGR2HSV);

    // Mask out-of-range values
    cvInRangeS(mHSVImg, cvScalar(0, mSMin, MIN(mVMin, mVMax), 0), cvScalar(180, 256, MAX(mVMin, mVMax), 0), mMask);

    // Extract the hue channel
    cvSplit(mHSVImg, mHueImg, 0, 0, 0);
}

void CamShift::setVMin(int vMin) {
    if(mVMin != vMin) mVMin = vMin;
}

void CamShift::setSMin(int sMin) {
    if(mSMin != sMin) mSMin = sMin;
}

int CamShift::vMin() const {
    return mVMin;
}

int CamShift::sMin() const {
    return mSMin;
}
