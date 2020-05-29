// Copyright (C) 2007 by Cristóbal Carnero Liñán
// grendel.ccl@gmail.com
//
// This file is part of cvBlob.
//
// cvBlob is free software: you can redistribute it and/or modify
// it under the terms of the Lesser GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cvBlob is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Lesser GNU General Public License for more details.
//
// You should have received a copy of the Lesser GNU General Public License
// along with cvBlob.  If not, see <http://www.gnu.org/licenses/>.
//

#include <cmath>
#include <iostream>
using namespace std;

//#include <opencv2/opencv.hpp>
#include "caffe/cvblob.h"

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif
using namespace cv;
namespace cvb
{
	CvLabel cvLargestBlob(const CvBlobs &blobs)
	{
		CvLabel label=0;
		unsigned int maxArea=0;

		for (CvBlobs::const_iterator it=blobs.begin();it!=blobs.end();++it)
		{
			CvBlob *blob=(*it).second;

			if (blob->area > maxArea)
			{
				label=blob->label;
				maxArea=blob->area;
			}
		}

		return label;
	}
	
	void cvFilter(CvBlobs &blobs, int cx, int cy)
	{
		int nWidth, nHeight;

		CvBlobs::iterator it = blobs.begin();
		bool bDelete = false;
		int nLeft, nTop, nRight, nBottom;
		while (it != blobs.end()) {
			CvBlob *blob = (*it).second;

			nWidth = blob->maxx - blob->minx;
			nHeight= blob->maxy - blob->miny;


			CvBlobs::iterator it_j = blobs.begin();
			bDelete = (blob->area < 0.2 * (nWidth*nHeight)) ? true : false;
			while (it_j != blobs.end()) {
				CvBlob *blob_j = (*it_j).second;
				++it_j;
				if((blob->maxx - blob->minx) * (blob->maxy - blob->miny) >= (blob_j->maxx - blob_j->minx) * (blob_j->maxy - blob_j->miny) ) continue;
				nLeft = max(blob->minx, blob_j->minx);
				nTop = max(blob->miny, blob_j->miny);
				nRight = min(blob->maxx, blob_j->maxx);
				nBottom = min(blob->maxy, blob_j->maxy);
				if(nLeft > nRight || nTop > nBottom) continue;
				if((blob->maxx - blob->minx) * (blob->maxy - blob->miny) * 0.01 < (nRight - nLeft) * (nBottom - nTop)){
					bDelete = true;
					break;
				}
			}

			//bDelete = bDelete || (nWidth > cx/2 || nHeight > cy/2 || nWidth > 5*nHeight || nHeight > 5*nWidth);
			if (bDelete) {
				cvReleaseBlob(blob);

				CvBlobs::iterator tmp = it;
				++it;
				blobs.erase(tmp);
			} else {
				++it;
			}
		}
	}

	void cvFilterByArea(CvBlobs &blobs, unsigned int minArea, unsigned int maxArea)
	{
		CvBlobs::iterator it=blobs.begin();
		while(it!=blobs.end())
		{
			CvBlob *blob=(*it).second;
			if ((blob->area<minArea)||(blob->area>maxArea))
			{
				cvReleaseBlob(blob);

				CvBlobs::iterator tmp=it;
				++it;
				blobs.erase(tmp);
			}
			else {
				++it;
			}
		}
	}

int movesE[4][3][4] = { { {-1, -1, 3, CV_CHAINCODE_UP_LEFT   }, { 0, -1, 0, CV_CHAINCODE_UP   }, { 1, -1, 0, CV_CHAINCODE_UP_RIGHT   } },
{ { 1, -1, 0, CV_CHAINCODE_UP_RIGHT  }, { 1,  0, 1, CV_CHAINCODE_RIGHT}, { 1,  1, 1, CV_CHAINCODE_DOWN_RIGHT } },
{ { 1,  1, 1, CV_CHAINCODE_DOWN_RIGHT}, { 0,  1, 2, CV_CHAINCODE_DOWN }, {-1,  1, 2, CV_CHAINCODE_DOWN_LEFT  } },
{ {-1,  1, 2, CV_CHAINCODE_DOWN_LEFT }, {-1,  0, 3, CV_CHAINCODE_LEFT }, {-1, -1, 3, CV_CHAINCODE_UP_LEFT    } }
};

int movesI[4][3][4] = { { { 1, -1, 3, CV_CHAINCODE_UP_RIGHT   }, { 0, -1, 0, CV_CHAINCODE_UP   }, {-1, -1, 0, CV_CHAINCODE_UP_LEFT    } },
{ {-1, -1, 0, CV_CHAINCODE_UP_LEFT    }, {-1,  0, 1, CV_CHAINCODE_LEFT }, {-1,  1, 1, CV_CHAINCODE_DOWN_LEFT  } },
{ {-1,  1, 1, CV_CHAINCODE_DOWN_LEFT  }, { 0,  1, 2, CV_CHAINCODE_DOWN }, { 1,  1, 2, CV_CHAINCODE_DOWN_RIGHT } },
{ { 1,  1, 2, CV_CHAINCODE_DOWN_RIGHT }, { 1,  0, 3, CV_CHAINCODE_RIGHT}, { 1, -1, 3, CV_CHAINCODE_UP_RIGHT   } }
};
#if 1
 unsigned int cvLabel(unsigned char* pImgIn, int cx, int cy, unsigned int *pImgOut, CvBlobs &blobs)
 {
 	unsigned int numPixels=0;
 
 	memset(pImgOut, 0, cx*cy*sizeof(unsigned int));
 
 	CvLabel label=0;
 	cvReleaseBlobs(blobs);
 
 	unsigned int stepIn = cx;
 	unsigned int stepOut = cx;
 	unsigned int imgIn_width = cx;
 	unsigned int imgIn_height = cy;
 	unsigned int imgIn_offset = 0;
 	unsigned int imgOut_width = cx;
 	unsigned int imgOut_height = cy;
 	unsigned int imgOut_offset = 0;
 	/*if(img->roi){
 		imgIn_width = img->roi->width;
 		imgIn_height = img->roi->height;
 		imgIn_offset = img->roi->xOffset + (img->roi->yOffset * stepIn);
 	}
 	if(imgOut->roi){
 		imgOut_width = imgOut->roi->width;
 		imgOut_height = imgOut->roi->height;
 		imgOut_offset = imgOut->roi->xOffset + (imgOut->roi->yOffset * stepOut);
 	}*/
	unsigned char* imgDataIn = (unsigned char*)pImgIn + imgIn_offset;
 	CvLabel *imgDataOut = (CvLabel *)pImgOut + imgOut_offset;
 
 	#define imageIn(X, Y) imgDataIn[(X) + (Y)*stepIn]
 	#define imageOut(X, Y) imgDataOut[(X) + (Y)*stepOut]
 
 	CvLabel lastLabel = 0;
 	CvBlob *lastBlob = NULL;
 
 	for (unsigned int y = 0; y < imgIn_height; y++) {
 		for (unsigned int x = 0; x < imgIn_width; x++) {
 			if (imageIn(x, y)) {
 				bool labeled = (bool)imageOut(x, y);
 				if ((!imageOut(x, y)) && ((y == 0) || (!imageIn(x, y-1)))) {
 					labeled = true;
 					// Label contour.
 					label++;
  					//ASSERT(label!=CV_BLOB_MAX_LABEL);
 					imageOut(x, y) = label;
 					numPixels++;
 					// XXX This is not necessary at all. I only do this for consistency.
 	 				if (y>0)
 	 					imageOut(x, y-1) = CV_BLOB_MAX_LABEL;
 
 					CvBlob *blob = new CvBlob;
 					blob->active = 0;
 					blob->label = label;
 					blob->area = 1;
 					blob->nbPeople = 1;
 					blob->minx = x; blob->maxx = x;
 					blob->miny = y; blob->maxy = y;
 					blob->m10=x; blob->m01=y;
 					blob->m11=x*y;
 					blob->m20=x*x; blob->m02=y*y;
 					blob->internalContours.clear();
 					blobs.insert(CvLabelBlob(label,blob));
 
 					lastLabel = label;
 					lastBlob = blob;
 
 					blob->contour.startingPoint = Point(x, y);
 
 					unsigned char direction=1;
 					unsigned int xx = x;
 					unsigned int yy = y;
 					bool contourEnd = false;			
 					do {
 						for (unsigned int numAttempts=0; numAttempts<3; numAttempts++){
 							bool found = false;
 
 							for (unsigned char i=0; i<3; i++){
 								unsigned int nx = xx+movesE[direction][i][0];
 								unsigned int ny = yy+movesE[direction][i][1];
 								if ((nx<imgIn_width)&&(nx>=0)&&(ny<imgIn_height)&&(ny>=0)){
 									if (imageIn(nx, ny)){
 										found = true;
 										blob->contour.chainCode.push_back(movesE[direction][i][3]);
 										xx=nx;
 										yy=ny;
 										direction=movesE[direction][i][2];
 										break;
 									}else{
 		 									imageOut(nx, ny) = CV_BLOB_MAX_LABEL;
 									}
 								}
 							}
 							if (!found)
 							direction=(direction+1)%4;
 							else{
 							if (imageOut(xx, yy) != label)
 							{
 								imageOut(xx, yy) = label;
 								numPixels++;
 
 								if (xx<blob->minx) blob->minx = xx;
 								else if (xx>blob->maxx) blob->maxx = xx;
 								if (yy<blob->miny) blob->miny = yy;
 								else if (yy>blob->maxy) blob->maxy = yy;
 
 								blob->area++;
 								blob->m10+=xx; blob->m01+=yy;
 								blob->m11+=xx*yy;
 								blob->m20+=xx*xx; blob->m02+=yy*yy;
 							} 
 							break;
 							}
 		  
 							if (contourEnd = ((xx==x) && (yy==y) && (direction==1)))
 							break;
 						}
 					}while (!contourEnd);
 				}
 
 				if ((y+1<imgIn_height)&&(!imageIn(x, y+1))&&(!imageOut(x, y+1))){
 					labeled = true;
 
 					// Label internal contour
 					CvLabel l;
 					CvBlob *blob = NULL;
 
 					if (!imageOut(x, y)){
 						l = imageOut(x-1, y);
 
 						imageOut(x, y) = l;
 						numPixels++;
 
 								if (l==lastLabel)
 									blob = lastBlob;
 								else
 								{
 									blob = blobs.find(l)->second;
 									lastLabel = l;
 									lastBlob = blob;
 								}
 						blob->area++;
 						blob->m10+=x; blob->m01+=y;
 						blob->m11+=x*y;
 						blob->m20+=x*x; blob->m02+=y*y;
 					}else{
 						l = imageOut(x, y);
 
 						if (l==lastLabel)
 							blob = lastBlob;
 						else{
 							blob = blobs.find(l)->second;
 							lastLabel = l;
 							lastBlob = blob;
 						}
 					}
 
 				// XXX This is not necessary (I believe). I only do this for consistency.
 					imageOut(x, y+1) = CV_BLOB_MAX_LABEL;
 
 					CvContourChainCode *contour = new CvContourChainCode;
 					contour->startingPoint = Point(x, y);
 
 					unsigned char direction = 3;
 					unsigned int xx = x;
 					unsigned int yy = y;
 
 					do{
 						for (unsigned int numAttempts=0; numAttempts<3; numAttempts++){
 							bool found = false;
 
 							for (unsigned char i=0; i<3; i++){
 								int nx = xx+movesI[direction][i][0];
 								int ny = yy+movesI[direction][i][1];
 								if (imageIn(nx, ny)){
 									found = true;
 
 									contour->chainCode.push_back(movesI[direction][i][3]);
 
 									xx=nx;
 									yy=ny;
 
 									direction=movesI[direction][i][2];
 									break;
 								}else{
 									imageOut(nx, ny) = CV_BLOB_MAX_LABEL;
 								}
 							}
 
 							if (!found)
 							direction=(direction+1)%4;
 							else{
 							if (!imageOut(xx, yy)){
 								imageOut(xx, yy) = l;
 								numPixels++;
 
 								blob->area++;
 								blob->m10+=xx; blob->m01+=yy;
 								blob->m11+=xx*yy;
 								blob->m20+=xx*xx; blob->m02+=yy*yy;
 							}
 							break;
 							}
 						}
 					}while (!(xx==x && yy==y));
 
 					blob->internalContours.push_back(contour);
 			} else if (!imageOut(x, y))
 				if (!labeled){
 					// Internal pixel
 					CvLabel l = imageOut(x-1, y);
 
 					imageOut(x, y) = l;
 					numPixels++;
 
 					CvBlob *blob = NULL;
 						if (l==lastLabel)
 							blob = lastBlob;
 						else{
 							blob = blobs.find(l)->second;
 							lastLabel = l;
 							lastBlob = blob;
 						}
 					blob->area++;
 					blob->m10+=x; blob->m01+=y;
 					blob->m11+=x*y;
 					blob->m20+=x*x; blob->m02+=y*y;
 				}
 			}
 		}
 	}
 
 	for (CvBlobs::iterator it=blobs.begin(); it!=blobs.end(); ++it){
 		cvCentroid((*it).second);
		/*
 		(*it).second->u11 = (*it).second->m11 - ((*it).second->m10*(*it).second->m01)/(*it).second->m00;
 		(*it).second->u20 = (*it).second->m20 - ((*it).second->m10*(*it).second->m10)/(*it).second->m00;
 		(*it).second->u02 = (*it).second->m02 - ((*it).second->m01*(*it).second->m01)/(*it).second->m00;
 
 		double m00_2 = (*it).second->m00 * (*it).second->m00;
 
 		(*it).second->n11 = (*it).second->u11 / m00_2;
 		(*it).second->n20 = (*it).second->u20 / m00_2;
 		(*it).second->n02 = (*it).second->u02 / m00_2;
 
 		(*it).second->p1 = (*it).second->n20 + (*it).second->n02;
 
 		double nn = (*it).second->n20 - (*it).second->n02;
 		(*it).second->p2 = nn*nn + 4.*((*it).second->n11*(*it).second->n11);
		*/
 	}
 	return numPixels;
 }
#else
unsigned int cvLabel(BYTE* pImgIn, int cx, int cy, unsigned int *pImgOut, CvBlobs &blobs)
{
	unsigned int numPixels = 0;

	memset(pImgOut, 0, cx*cy*sizeof(unsigned int));

	CvLabel label = 0;
	cvReleaseBlobs(blobs);

	unsigned int stepIn = cx;
	unsigned int stepOut = cx;
	unsigned int imgIn_width = cx;
	unsigned int imgIn_height = cy;
	unsigned int imgIn_offset = 0;
	unsigned int imgOut_width = cx;
	unsigned int imgOut_height = cy;
	unsigned int imgOut_offset = 0;
	/*if(img->roi){
	imgIn_width = img->roi->width;
	imgIn_height = img->roi->height;
	imgIn_offset = img->roi->xOffset + (img->roi->yOffset * stepIn);
	}
	if(imgOut->roi){
	imgOut_width = imgOut->roi->width;
	imgOut_height = imgOut->roi->height;
	imgOut_offset = imgOut->roi->xOffset + (imgOut->roi->yOffset * stepOut);
	}*/
	BYTE* imgDataIn = (BYTE*)pImgIn + imgIn_offset;
	CvLabel *imgDataOut = (CvLabel *)pImgOut + imgOut_offset;

#define imageIn(X, Y) imgDataIn[(X) + (Y)*stepIn]
#define imageOut(X, Y) imgDataOut[(X) + (Y)*stepOut]

	CvLabel lastLabel = 0;
	CvBlob *lastBlob = NULL;

	for (unsigned int y = 0; y < imgIn_height; y++) {
		for (unsigned int x = 0; x < imgIn_width; x++) {
			if (imageIn(x, y)) {
				bool labeled = (bool)imageOut(x, y);
				if ((!imageOut(x, y)) && ((y == 0) || (!imageIn(x, y - 1)))) {
					labeled = true;
					// Label contour.
					label++;
					//ASSERT(label!=CV_BLOB_MAX_LABEL);
					imageOut(x, y) = label;
					numPixels++;
					// XXX This is not necessary at all. I only do this for consistency.
					if (y>0)
						imageOut(x, y - 1) = CV_BLOB_MAX_LABEL;

					CvBlob *blob = new CvBlob;
					blob->active = 0;
					blob->label = label;
					blob->area = 1;
					blob->nbPeople = 1;
					blob->minx = x; blob->maxx = x;
					blob->miny = y; blob->maxy = y;
					blob->m10 = x; blob->m01 = y;
					blob->m11 = x*y;
					blob->m20 = x*x; blob->m02 = y*y;
					blob->internalContours.clear();
					blobs.insert(CvLabelBlob(label, blob));

					lastLabel = label;
					lastBlob = blob;

					blob->contour.startingPoint = cvPoint(x, y);

					unsigned char direction = 1;
					unsigned int xx = x;
					unsigned int yy = y;
					bool contourEnd = false;
					do {
						for (unsigned int numAttempts = 0; numAttempts<3; numAttempts++) {
							bool found = false;

							for (unsigned char i = 0; i<3; i++) {
								unsigned int nx = xx + movesE[direction][i][0];
								unsigned int ny = yy + movesE[direction][i][1];
								if ((nx<imgIn_width) && (nx >= 0) && (ny<imgIn_height) && (ny >= 0)) {
									if (imageIn(nx, ny)) {
										found = true;
										blob->contour.chainCode.push_back(movesE[direction][i][3]);
										xx = nx;
										yy = ny;
										direction = movesE[direction][i][2];
										break;
									}
									else {
										imageOut(nx, ny) = CV_BLOB_MAX_LABEL;
									}
								}
							}
							if (!found)
								direction = (direction + 1) % 4;
							else {
								if (imageOut(xx, yy) != label)
								{
									imageOut(xx, yy) = label;
									numPixels++;

									if (xx<blob->minx) blob->minx = xx;
									else if (xx>blob->maxx) blob->maxx = xx;
									if (yy<blob->miny) blob->miny = yy;
									else if (yy>blob->maxy) blob->maxy = yy;

									blob->area++;
									blob->m10 += xx; blob->m01 += yy;
									blob->m11 += xx*yy;
									blob->m20 += xx*xx; blob->m02 += yy*yy;
								}
								break;
							}

							if (contourEnd = ((xx == x) && (yy == y) && (direction == 1)))
								break;
						}
					} while (!contourEnd);
				}

				if ((y + 1<imgIn_height) && (!imageIn(x, y + 1)) && (!imageOut(x, y + 1))) {
					labeled = true;

					// Label internal contour
					CvLabel l;
					CvBlob *blob = NULL;

					if (!imageOut(x, y)) {
						l = imageOut(x - 1, y);

						imageOut(x, y) = l;
						numPixels++;

						if (l == lastLabel)
							blob = lastBlob;
						else
						{
							blob = blobs.find(l)->second;
							lastLabel = l;
							lastBlob = blob;
						}
						blob->area++;
						blob->m10 += x; blob->m01 += y;
						blob->m11 += x*y;
						blob->m20 += x*x; blob->m02 += y*y;
					}
					else {
						l = imageOut(x, y);

						if (l == lastLabel)
							blob = lastBlob;
						else {
							blob = blobs.find(l)->second;
							lastLabel = l;
							lastBlob = blob;
						}
					}

					// XXX This is not necessary (I believe). I only do this for consistency.
					imageOut(x, y + 1) = CV_BLOB_MAX_LABEL;

					CvContourChainCode *contour = new CvContourChainCode;
					contour->startingPoint = cvPoint(x, y);

					unsigned char direction = 3;
					unsigned int xx = x;
					unsigned int yy = y;

					do {
						for (unsigned int numAttempts = 0; numAttempts<3; numAttempts++) {
							bool found = false;

							for (unsigned char i = 0; i<3; i++) {
								int nx = xx + movesI[direction][i][0];
								int ny = yy + movesI[direction][i][1];
								if (imageIn(nx, ny)) {
									found = true;

									contour->chainCode.push_back(movesI[direction][i][3]);

									xx = nx;
									yy = ny;

									direction = movesI[direction][i][2];
									break;
								}
								else {
									imageOut(nx, ny) = CV_BLOB_MAX_LABEL;
								}
							}

							if (!found)
								direction = (direction + 1) % 4;
							else {
								if (!imageOut(xx, yy)) {
									imageOut(xx, yy) = l;
									numPixels++;

									blob->area++;
									blob->m10 += xx; blob->m01 += yy;
									blob->m11 += xx*yy;
									blob->m20 += xx*xx; blob->m02 += yy*yy;
								}
								break;
							}
						}
					} while (!(xx == x && yy == y));

					blob->internalContours.push_back(contour);
				}
				else if (!imageOut(x, y))
					if (!labeled) {
						// Internal pixel
						CvLabel l = imageOut(x - 1, y);

						imageOut(x, y) = l;
						numPixels++;

						CvBlob *blob = NULL;
						if (l == lastLabel)
							blob = lastBlob;
						else {
							blob = blobs.find(l)->second;
							lastLabel = l;
							lastBlob = blob;
						}
						blob->area++;
						blob->m10 += x; blob->m01 += y;
						blob->m11 += x*y;
						blob->m20 += x*x; blob->m02 += y*y;
					}
			}
		}
	}

	for (CvBlobs::iterator it = blobs.begin(); it != blobs.end(); ++it) {
		cvCentroid((*it).second);

		(*it).second->u11 = (*it).second->m11 - ((*it).second->m10*(*it).second->m01) / (*it).second->m00;
		(*it).second->u20 = (*it).second->m20 - ((*it).second->m10*(*it).second->m10) / (*it).second->m00;
		(*it).second->u02 = (*it).second->m02 - ((*it).second->m01*(*it).second->m01) / (*it).second->m00;

		double m00_2 = (*it).second->m00 * (*it).second->m00;

		(*it).second->n11 = (*it).second->u11 / m00_2;
		(*it).second->n20 = (*it).second->u20 / m00_2;
		(*it).second->n02 = (*it).second->u02 / m00_2;

		(*it).second->p1 = (*it).second->n20 + (*it).second->n02;

		double nn = (*it).second->n20 - (*it).second->n02;
		(*it).second->p2 = nn*nn + 4.*((*it).second->n11*(*it).second->n11);
	}
	return numPixels;
}
#endif
}


