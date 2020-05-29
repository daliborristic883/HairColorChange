#include "caffe/HairProcess.h"
#include "caffe/HairLib.h"
#include "caffe/face.h"
#include "caffe/fastguidedfilter.h"
#include "caffe/cvblob.h"
#include <algorithm>
#define CROP_WIDTH  200
#define CROP_HEIGHT 260
#define TRAIN_IMAGE_WIDTH CROP_WIDTH - 8
#define TRAIN_IMAGE_HEIGHT CROP_HEIGHT - 12
#define CROP_X 4
#define CROP_Y 6
#define min_f(a, b, c)  (fminf(a, fminf(b, c)))
#define max_f(a, b, c)  (fmaxf(a, fmaxf(b, c)))
#define MASK_THRESHOLD 148
Classifier classifier;
string LUT_file;
using namespace cvb;
Classifier::Classifier() {

}
Classifier::~Classifier() {

}
void Classifier::init(const string& model_file, const string& trained_file)
{

	Caffe::set_mode(Caffe::CPU);

	/* Load the network. */
	net_.reset(new Net<float>(model_file, TEST));
	net_->CopyTrainedLayersFrom(trained_file);

	// 	CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
	// 	CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

	Blob<float>* input_layer = net_->input_blobs()[0];
	num_channels_ = input_layer->channels();
	// 	CHECK(num_channels_ == 3 || num_channels_ == 1)
	// 		<< "Input layer should have 1 or 3 channels.";
	input_geometry_ = cv::Size(input_layer->width(), input_layer->height());
}
void Classifier::Predict(const cv::Mat& img, cv::Mat label_colours, cv::Mat& output_image) {
	Blob<float>* input_layer = net_->input_blobs()[0];
	input_layer->Reshape(1, num_channels_,
		input_geometry_.height, input_geometry_.width);
	/* Forward dimension change to all layers. */
	net_->Reshape();

	std::vector<cv::Mat> input_channels;
	WrapInputLayer(&input_channels);

	Preprocess(img, &input_channels);


	//struct timeval time;
	//gettimeofday(&time, NULL); // Start Time
	//long totalTime = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); //Just for time measurement

	net_->Forward();

	//gettimeofday(&time, NULL);  //END-TIME
	//totalTime = (((time.tv_sec * 1000) + (time.tv_usec / 1000)) - totalTime);
	//std::cout << "Processing time = " << totalTime << " ms" << std::endl;

	//std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
	//std::cout << "Processing time = " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())/1000000.0 << " sec" <<std::endl; //Just for time measurement


	/* Copy the output layer to a std::vector */
	Blob<float>* output_layer = net_->output_blobs()[0];

	int width = output_layer->width();
	int height = output_layer->height();
	int channels = output_layer->channels();
	int num = output_layer->num();

	std::cout << "output_blob(n,c,h,w) = " << num << ", " << channels << ", "
		<< height << ", " << width << std::endl;

	// compute argmax
	cv::Mat class_each_row(channels, width*height, CV_32FC1, const_cast<float *>(output_layer->cpu_data()));
	class_each_row = class_each_row.t(); // transpose to make each row with all probabilities
	cv::Point maxId;    // point [x,y] values for index of max
	double maxValue;    // the holy max value itself
	cv::Mat prediction_map(height, width, CV_8UC1);
	for (int i = 0;i<class_each_row.rows;i++) {
		minMaxLoc(class_each_row.row(i), 0, &maxValue, 0, &maxId);
		prediction_map.at<uchar>(i) = maxId.x;
	}

	Visualization(prediction_map, label_colours, output_image);
}
void Classifier::Visualization(cv::Mat prediction_map, cv::Mat label_colours, cv::Mat& output_image) {

	cv::cvtColor(prediction_map.clone(), prediction_map, cv::COLOR_GRAY2BGR);
	//cv::Mat label_colours = cv::imread(LUT_file, 1);
	cv::cvtColor(label_colours, label_colours, cv::COLOR_RGB2BGR);
	cv::LUT(prediction_map, label_colours, output_image);
}
/* Wrap the input layer of the network in separate cv::Mat objects
* (one per channel). This way we save one memcpy operation and we
* don't need to rely on cudaMemcpy2D. The last preprocessing
* operation will write the separate channels directly to the input
* layer. */
void Classifier::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
	Blob<float>* input_layer = net_->input_blobs()[0];

	int width = input_layer->width();
	int height = input_layer->height();
	float* input_data = input_layer->mutable_cpu_data();
	for (int i = 0; i < input_layer->channels(); ++i) {
		cv::Mat channel(height, width, CV_32FC1, input_data);
		input_channels->push_back(channel);
		input_data += width * height;
	}
}
void Classifier::Preprocess(const cv::Mat& img,
	std::vector<cv::Mat>* input_channels) {
	/* Convert the input image to the input image format of the network. */
	cv::Mat sample;
	if (img.channels() == 3 && num_channels_ == 1)
		cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
	else if (img.channels() == 4 && num_channels_ == 1)
		cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
	else if (img.channels() == 4 && num_channels_ == 3)
		cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
	else if (img.channels() == 1 && num_channels_ == 3)
		cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
	else
		sample = img;

	cv::Mat sample_resized;
	if (sample.size() != input_geometry_)
		cv::resize(sample, sample_resized, input_geometry_);
	else
		sample_resized = sample;

	cv::Mat sample_float;
	if (num_channels_ == 3)
		sample_resized.convertTo(sample_float, CV_32FC3);
	else
		sample_resized.convertTo(sample_float, CV_32FC1);

	/* This operation will write the separate BGR planes directly to the
	* input layer of the network because it is wrapped by the cv::Mat
	* objects in input_channels. */
	cv::split(sample_float, *input_channels);

	// 	CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
	// 		== net_->input_blobs()[0]->cpu_data())
	// 		<< "Input channels are not wrapping the input layer of the network.";
}

void Segment_Init(char *sFilePath)
{
	std::string filepath(sFilePath);
	const string& model_file = filepath + "//hair_deep_matt.prototxt";
	const string& trained_file = filepath + "//hair_deep_matt.caffemodel";
	LUT_file = filepath + "//LutFile.png";
	classifier.init(model_file, trained_file);
}

void CorrectHairMask(Mat &inImage)
{	
	cv::Point cen(96, 97);
	Mat Binmask, orImg;
	cvtColor(inImage, orImg, COLOR_BGR2GRAY);
	threshold(orImg, Binmask, 128, 255, THRESH_BINARY);
		
	CvBlobs blobs;
	int max_len, min_len;
	int width = Binmask.cols;
	int height = Binmask.rows;
	int class_no = 1;
	unsigned int* pImgOut = (unsigned int *)malloc(width * height * sizeof(unsigned int));
	Mat pBinImg = Binmask.clone();
	int numpix = cvLabel(Binmask.data, width, height, pImgOut, blobs);
	if (blobs.size() < 2) {
		free(pImgOut);
		return;
	}
	cvb::CvBlobs::iterator it = blobs.begin();
	bool bDelete = false, tmp;
	float min_dist = FLT_MAX, ratio;
	memset(pBinImg.data, 0, width * height * sizeof(unsigned char));

	int adr = 0, adrr = 0;
	Point2f pt[4];	
	float dist;
	while (it != blobs.end()) {
		CvBlob *blob = (*it).second;
		 dist = sqrtf((cen.x - blob->centroid.x) * (cen.x - blob->centroid.x) + (cen.y - blob->centroid.y) *(cen.y - blob->centroid.y));
		 if (dist < min_dist) min_dist = dist;
		 ++it;
	}
	it = blobs.begin();
	while (it != blobs.end()) {
		CvBlob *blob = (*it).second;
		vector<Point2i> contour;
		bDelete = false;
		for (unsigned int y = blob->miny; y <= blob->maxy; y++) {
			for (unsigned int x = blob->minx; x <= blob->maxx; x++) {
				adr = y * width + x;
				if (Binmask.data[adr] == 255 && pImgOut[adr] == class_no) {
					Point2i pos;
					pos.x = x;
					pos.y = y;
					contour.push_back(pos);
				}
			}
		}
		float dists = sqrtf((cen.x - blob->centroid.x) * (cen.x - blob->centroid.x) + (cen.y - blob->centroid.y) *(cen.y - blob->centroid.y));
		if (min_dist < dists && dists > 100) {
			tmp = true;
			bDelete = bDelete || tmp;
		}
		if (bDelete) {
			cvReleaseBlob(blob);
			CvBlobs::iterator Tmp = it;
			++it;
			blobs.erase(Tmp);
		}
		else {
			for (int i = 0; i < contour.size(); i++) {
				pBinImg.data[contour.at(i).y * width + contour.at(i).x] 
					= orImg.data[contour.at(i).y * width + contour.at(i).x];
			}			
			++it;
		}
		class_no++;
	}
	cvReleaseBlobs(blobs);
	free(pImgOut);
	cvtColor(pBinImg, inImage, COLOR_GRAY2BGR);	
}

void HairSegment(Mat inImage, cv::Mat label_colours, Mat& outImage)
{
	classifier.Predict(inImage, label_colours, outImage);
	CorrectHairMask(outImage);
}
int CropImage(Mat inimage, cv::Rect& crop_rt, Mat& outImage, Mat& disImage)
{
	unsigned char* DetectImage = (unsigned char*)calloc(inimage.rows*inimage.cols*inimage.step.buf[1], sizeof(unsigned char));
	memcpy(DetectImage, inimage.data, inimage.rows*inimage.cols*inimage.step.buf[1]);
	SFaceInfo FaceInfo;
	memset(&FaceInfo, 0, sizeof(SFaceInfo));
	FaceDetectFunction(DetectImage, inimage.cols, inimage.rows, inimage.step.buf[1] * 8, &FaceInfo);
	SafeMemFree(DetectImage);
	if (FaceInfo.nFaces == 0) return -1;
	float crop_ratio_w = 1.25f;
	float crop_ratio_h_t = 1.3f;
	float crop_ratio_h_d = 2.25f;
	crop_rt.x = FaceInfo.pFace[FaceInfo.nMaxFaceIdx].x - crop_ratio_w*FaceInfo.pFace[FaceInfo.nMaxFaceIdx].width;
	crop_rt.y = FaceInfo.pFace[FaceInfo.nMaxFaceIdx].y - crop_ratio_h_t*FaceInfo.pFace[FaceInfo.nMaxFaceIdx].height;
	crop_rt.width = (1 + crop_ratio_w * 2)*FaceInfo.pFace[FaceInfo.nMaxFaceIdx].width;
	crop_rt.height = (1 + crop_ratio_h_t + crop_ratio_h_d)*FaceInfo.pFace[FaceInfo.nMaxFaceIdx].height;

	Mat cropImage(crop_rt.height, crop_rt.width, CV_8UC3, cv::Scalar(255, 0, 0));
	Mat cropImage1(crop_rt.height, crop_rt.width, CV_8UC3, cv::Scalar(240, 240, 240));
	int ii, jj;
	for (ii = crop_rt.y; ii < crop_rt.y + crop_rt.height; ii++) {
		for (jj = crop_rt.x; jj < crop_rt.x + crop_rt.width; jj++) {
			if (ii < 0 || jj < 0) continue;
			if (ii >= inimage.rows || jj >= inimage.cols) continue;
			int idx = ((ii - crop_rt.y) * crop_rt.width + (jj - crop_rt.x)) * 3;
			int iidx = (ii * inimage.cols + jj) * 3;
			cropImage.data[idx] = inimage.data[iidx];
			cropImage.data[idx + 1] = inimage.data[iidx + 1];
			cropImage.data[idx + 2] = inimage.data[iidx + 2];
			cropImage1.data[idx] = inimage.data[iidx];
			cropImage1.data[idx + 1] = inimage.data[iidx + 1];
			cropImage1.data[idx + 2] = inimage.data[iidx + 2];


		}
	}
	Mat norImage, tempImage;
	cv::resize(cropImage, norImage, cv::Size(CROP_WIDTH, CROP_HEIGHT));
	cv::resize(cropImage1, tempImage, cv::Size(CROP_WIDTH, CROP_HEIGHT));
	cv::Rect rtt(CROP_X, CROP_Y, TRAIN_IMAGE_WIDTH, TRAIN_IMAGE_HEIGHT);
	outImage = norImage(rtt);
	Mat tempImage1 = tempImage(rtt);
	cv::resize(tempImage1, disImage, cv::Size(TRAIN_IMAGE_WIDTH, TRAIN_IMAGE_HEIGHT));
	return 0;
}
void HairInit(char* strDataPath)
{
	InitFaceDetector(strDataPath);
	Segment_Init(strDataPath);
}
void ReleaseDetect()
{
	ReleaseFaceDetector();
}

void func_Edge_Merge(unsigned short* pwEdge, unsigned short* pwBinImg, int nWidth, int nEdge)
{
	int	i;
	for (i = 0; i < nEdge; i++)
		pwEdge[i] = pwBinImg[0];

	for (i = 0; i < nWidth; i++)
		pwEdge[nEdge + i] = pwBinImg[i];

	for (i = 0; i < nEdge; i++)
		pwEdge[nEdge + nWidth + i] = pwBinImg[nWidth - 1];
}

void DripEnhance(unsigned char* pbyBinImg, int nWidth, int nHeight, int nEdge, int nlEE)
{
	unsigned short	*_pwEdgeArray[5120];
	unsigned short    *diffArray[5120];
	unsigned short	_wEdgeValue[8192];
	unsigned char*	_pbyEdgeSrc;
	unsigned char*	_pbyCurBinImg;
	unsigned char*	pbyEsi;
	unsigned char*  pbyEdi;
	unsigned char*  pbyEax;
	unsigned char*  pbyEbx;
	unsigned char	_byGrayColor;
	int		_nEdge, _nBytesEdge;
	int		_nCount, _nPixelCount;
	int		_nSquare, _nMultiEdge;
	int     nSum, nAvg;
	int		i, j, k, m;
	int		nGC = 12, nLeftGC = 4096;
	float	flAmpl = 1;
	float	flEE = (float)(nlEE* 0.2);

	if (nEdge == 0)
		return;
	int nTwoEdge = nEdge;

	unsigned short*	pProcImage;
	int		dwScanBytes, dwScrapBytes;
	int		x, y;

	dwScanBytes = (nWidth + 3) >> 2 << 2;
	dwScrapBytes = dwScanBytes - nWidth;
	pProcImage = (unsigned short*)calloc(sizeof(unsigned short), dwScanBytes*nHeight);

	for (y = 0; y < nHeight; y++) {
		unsigned short* tmpProcImage = &(pProcImage[y*dwScanBytes]);
		unsigned char* tmpImage = &(pbyBinImg[y*nWidth]);
		for (x = 0; x < nWidth; x++) {
			*tmpProcImage = (unsigned short)((*tmpImage) << 4);
			tmpProcImage++;			tmpImage++;
		}
	}

	unsigned char* pTmpProcImage = (unsigned char*)pProcImage;

	_byGrayColor = (nGC - 1) / 8 + 1;
	_nMultiEdge = 2 * nEdge + 1;
	_nSquare = _nMultiEdge * _nMultiEdge;
	_nEdge = 2 * nEdge + dwScanBytes;
	_nPixelCount = (nHeight - 1) * dwScanBytes;
	_nBytesEdge = (_byGrayColor * _nEdge);

	for (j = 0; j < _nMultiEdge; j++)
	{
		_pwEdgeArray[j] = (unsigned short*)malloc(_nBytesEdge);
		if (_pwEdgeArray[j] == 0) {
			SafeMemFree(pProcImage);
			return;
		}
		memset(_pwEdgeArray[j], 0, _nBytesEdge);
	}
	for (j = 0; j < nHeight; j++) {
		diffArray[j] = (unsigned short*)new unsigned short*[dwScanBytes + 1];
		if (diffArray[j] == 0) {
			SafeMemFree(pProcImage);
			return;
		}
		memset(diffArray[j], 0, dwScanBytes + 1);
	}
	_pbyEdgeSrc = (unsigned char*)malloc(_nBytesEdge);
	if (_pbyEdgeSrc == 0) {
		SafeMemFree(pProcImage);
		return;
	}
	memset(_pbyEdgeSrc, 0, _nBytesEdge);

	for (j = 0; j < nEdge; j++)
		func_Edge_Merge(_pwEdgeArray[j], (unsigned short*)pTmpProcImage, dwScanBytes, nEdge);

	unsigned short* _pbyBinImg = (unsigned short*)pTmpProcImage;
	for (j = 0; j < nEdge + 1; j++)
	{
		func_Edge_Merge(_pwEdgeArray[nEdge + j], _pbyBinImg, dwScanBytes, nEdge);
		_pbyBinImg += dwScanBytes;
	}

	if (_nEdge > 0)
		memset(_wEdgeValue, 0, _nEdge * 2);

	for (i = 0; i < _nEdge; i++)
	{
		for (int j = 0; j < _nMultiEdge; j++)
		{
			_pbyBinImg = _pwEdgeArray[j];
			_wEdgeValue[i] += _pbyBinImg[i];
		}
	}

	nSum = 0;
	for (i = 0; i < _nMultiEdge; i++)
		nSum += _wEdgeValue[i];

	pbyEbx = (unsigned char*)_pbyEdgeSrc;
	nAvg = nSum / _nSquare;
	m = *(int*)_wEdgeValue;
	m &= 0x0FFFF;
	nSum -= m;

	*(unsigned short*)pbyEbx = (unsigned short)nAvg;

	if (dwScanBytes > 1)
	{
		pbyEax = _pbyEdgeSrc;
		pbyEbx = (unsigned char*)(_wEdgeValue + 1);
		pbyEsi = (unsigned char*)(_wEdgeValue + _nMultiEdge);
		pbyEdi = pbyEax + 2;

		for (_nCount = 0; _nCount < dwScanBytes - 1; _nCount++)
		{
			nSum += *(unsigned short*)pbyEsi;
			nAvg = nSum / _nSquare;
			*(unsigned short*)pbyEdi = (unsigned short)nAvg;
			nSum -= *(unsigned short*)pbyEbx;
			pbyEsi += 2;
			pbyEbx += 2;
			pbyEdi += 2;
		}
		_nCount = dwScanBytes - 1;
		pbyEbx = _pbyEdgeSrc;
	}

	memcpy(pTmpProcImage, _pbyEdgeSrc, _byGrayColor * dwScanBytes);

	k = 0;
	if ((nHeight - 1) > 0)
	{
		_nCount = 2 * dwScanBytes;

		do
		{
			int		nTemp;
			div_t	temp = div(k, _nMultiEdge);
			unsigned short*	pwEsi;
			unsigned char*	_pbyBinCount;

			for (nTemp = 0; nTemp < _nEdge; nTemp++)
				_wEdgeValue[nTemp] -= _pwEdgeArray[temp.rem][nTemp];

			pbyEbx = _pbyEdgeSrc;
			m = nHeight - nEdge - 1;
			pwEsi = _pwEdgeArray[temp.rem];

			if (k < m)
			{
				i = k + nEdge + 1;
				i *= dwScanBytes;
			}
			else
				i = _nPixelCount;

			_pbyCurBinImg = pTmpProcImage + i * 2;
			func_Edge_Merge(pwEsi, (unsigned short*)_pbyCurBinImg, dwScanBytes, nEdge);

			for (nTemp = 0; nTemp < _nEdge; nTemp++)
				_wEdgeValue[nTemp] += pwEsi[nTemp];

			if (_nMultiEdge > 0)
			{
				nSum = 0;
				for (i = 0; i < _nMultiEdge; i++)
					nSum += _wEdgeValue[i];
			}

			nAvg = nSum / _nSquare;
			*(unsigned short*)pbyEbx = nAvg;
			m = *(unsigned short*)_wEdgeValue;
			m &= 0x0FFFF;
			nSum -= m;

			if (dwScanBytes > 1)
			{
				pbyEdi = _pbyEdgeSrc + 2;
				pbyEbx = (unsigned char*)(_wEdgeValue + 1);
				pbyEsi = (unsigned char*)(_wEdgeValue + _nMultiEdge);

				for (nTemp = 0; nTemp < (dwScanBytes - 1); nTemp++)
				{
					nSum += *(unsigned short*)pbyEsi;
					nAvg = nSum / _nSquare;
					*(unsigned short*)pbyEdi = (unsigned short)nAvg;
					nSum -= *(unsigned short*)pbyEbx;
					pbyEsi += 2;
					pbyEbx += 2;
					pbyEdi += 2;
				}
				pbyEbx = _pbyEdgeSrc;
			}

			if (dwScanBytes > 0)
			{
				pbyEsi = pbyEbx;
				j = dwScanBytes;
				pbyEdi = pTmpProcImage + _nCount;

				for (j = 0; j < dwScanBytes; j++)
				{
					unsigned short nValue = *(unsigned short*)pbyEdi;
					int nDiff = *(unsigned short*)pbyEdi - *(unsigned short*)pbyEsi;
					double dblst = nTwoEdge * flEE;

					if (nTwoEdge > 1)
					{
						dblst *= 0.5f;
						dblst *= nDiff;
						dblst += nValue;
					}
					else
					{
						dblst *= nDiff;
						dblst += nValue;
					}

					m = (int)dblst;
					i = nLeftGC - 1;

					if (m >= i)
					{
						m = nLeftGC - 1;
						*(unsigned short*)pbyEsi = (unsigned short)m;
					}
					else
					{
						i = m < 0 ? 1 : 0;
						i--;
						i &= m;
						*(unsigned short*)pbyEsi = (unsigned short)i;
					}

					nValue = *(unsigned short*)pbyEsi;
					*(unsigned short*)pbyEsi = (unsigned short)(flAmpl * nValue);

					pbyEdi += 2; pbyEsi += 2;
				}
				pbyEbx = _pbyEdgeSrc;
			}

			_pbyBinCount = pTmpProcImage + _nCount;
			memcpy(_pbyBinCount, pbyEbx, _byGrayColor * dwScanBytes);

			_nCount += 2 * dwScanBytes;
			m = nHeight - 1;
			k++;
		} while (k < m);
	}


	free(pbyEbx);
	m = _nMultiEdge - 1;
	if (m < 0) {
		free(pProcImage);
		return;
	}

	m++;
	pbyEbx = (unsigned char*)_pwEdgeArray;

	for (i = 0; i < m; i++)
	{
		free(_pwEdgeArray[i]);
	}

	for (i = 0; i < nHeight; i++) {
		delete diffArray[i];
	}

	for (y = 0; y < nHeight; y++) {
		unsigned short* tmpProcImage = &(pProcImage[y*dwScanBytes]);
		unsigned char* tmpImage = &(pbyBinImg[y*nWidth]);
		for (x = 0; x < nWidth; x++) {
			*tmpImage = (unsigned char)((*tmpProcImage) >> 4);
			if (*tmpImage < 0)		*tmpImage = 0;
			if (*tmpImage > 0xFF)	*tmpImage = 0xFF;

			tmpProcImage++;
			tmpImage++;
		}
	}
	SafeMemFree(pProcImage);
}
Mat MaskImageTransfor(Mat inImage, Mat inMaskImg, cv::Rect rt)
{
	Mat MidImage(CROP_HEIGHT, CROP_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
	for (int i = 0; i < inMaskImg.rows; i++) {
		for (int j = 0; j < inMaskImg.cols; j++) {
			int idx = (i * inMaskImg.cols + j) * 3;
			int idxx = ((i + CROP_Y)*CROP_WIDTH + (j + CROP_X)) * 3;
			MidImage.data[idxx] = inMaskImg.data[idx];
			MidImage.data[idxx + 1] = inMaskImg.data[idx + 1];
			MidImage.data[idxx + 2] = inMaskImg.data[idx + 2];
		}
	}
	Mat resizeImg;
	cv::resize(MidImage, resizeImg, cv::Size(rt.width, rt.height));
	Mat outMaskImg(inImage.rows, inImage.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	for (int i = rt.y; i < rt.y + rt.height; i++) {
		for (int j = rt.x; j < rt.x + rt.width; j++) {
			if (i < 0 || j < 0) continue;
			if (i >= inImage.rows || j >= inImage.cols) continue;
			int idx = (i * inImage.cols + j) * 3;
			int ii = ((i - rt.y)*resizeImg.cols + (j - rt.x)) * 3;
			outMaskImg.data[idx] = resizeImg.data[ii];
			outMaskImg.data[idx + 1] = resizeImg.data[ii + 1];
			outMaskImg.data[idx + 2] = resizeImg.data[ii + 2];
		}
	}
	return outMaskImg;
}
void rgb2hsv(const int &src_r, const int &src_g, const int &src_b, int &dst_h, int &dst_s, int &dst_v)
{
	float r = src_r / 255.0f;
	float g = src_g / 255.0f;
	float b = src_b / 255.0f;

	float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

	float max = max_f(r, g, b);
	float min = min_f(r, g, b);

	v = max;

	if (max == 0.0f) {
		s = 0;
		h = 0;
	}
	else if (max - min == 0.0f) {
		s = 0;
		h = 0;
	}
	else {
		s = (max - min) / max;

		if (max == r) {
			h = 60 * ((g - b) / (max - min)) + 0;
		}
		else if (max == g) {
			h = 60 * ((b - r) / (max - min)) + 120;
		}
		else {
			h = 60 * ((r - g) / (max - min)) + 240;
		}
	}

	if (h < 0) h += 360.0f;

	dst_h = (int)(h / 2);   // dst_h : 0-180
	dst_s = (int)(s * 255); // dst_s : 0-255
	dst_v = (int)(v * 255); // dst_v : 0-255
}
void HairColorTransform(Mat originImg, Mat MaskImg, Mat& processImg, int RValue, int GValue, int BValue)
{
	if (originImg.empty() || MaskImg.empty()) return;
	if (originImg.cols != MaskImg.cols || originImg.rows != originImg.rows) return;
	int width = originImg.cols;
	int height = originImg.rows;
	Mat HSLImg, TransHSLImg;
	int HValue, SValue, LValue;
	rgb2hsv(RValue, GValue, BValue, HValue, SValue, LValue);
	float ro = 0;
	unsigned char *pImage = (unsigned char *)malloc(width * height * sizeof(unsigned char));
	int adr = 0;
	cvtColor(originImg, HSLImg, COLOR_RGB2HSV);
	float minvalue = FLT_MAX, maxvalue = FLT_MIN;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int idx = (i * width + j) * 3;			
			pImage[adr] = HSLImg.data[idx + 2];
			adr++;
		}
	}
	DripEnhance(pImage, width, height, 4, 1);
	float mean = 0;
	int num = 1;
	adr = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int idx = (i * width + j) * 3;
			int val = HSLImg.data[idx];
			if (MaskImg.data[idx] >= MASK_THRESHOLD) {
				pImage[adr] = pImage[adr];
				mean += pImage[adr];
				HSLImg.data[idx] = HValue * (1 + MaskImg.data[idx] / 255.0f) * 0.5;
				HSLImg.data[idx + 1] = SValue * (1 + MaskImg.data[idx] / 255.0f) * 0.5;
				num++;
			}
			adr++;
		}
	}
	mean /= num;
	float ratio = LValue / (mean + 1);
	adr = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int idx = (i * width + j) * 3;
			if (MaskImg.data[idx] >= MASK_THRESHOLD) {
				HSLImg.data[idx + 2] = MIN(255, pImage[adr] * ratio);
			}
			adr++;
		}
	}
	SafeMemFree(pImage);

	Mat dstImg;
	cvtColor(HSLImg, dstImg, COLOR_HSV2RGB);

	int r = 8;
	double eps = 1e-4;
	eps *= 255 * 255;
	processImg = fastGuidedFilter(originImg, dstImg, r, eps, 4);
}
