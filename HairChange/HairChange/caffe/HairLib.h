#pragma once
#include <opencv2/core/core.hpp>

#ifdef USE_HAIRDLL
#define HAIRDLL __declspec(dllexport)
#else
#define HAIRDLL 
#endif

void HAIRDLL HairSegment(cv::Mat inImage, cv::Mat label_colours, cv::Mat& outImage);
int HAIRDLL CropImage(cv::Mat inimage, cv::Rect& crop_rt, cv::Mat& outImage, cv::Mat& disImage);
void HAIRDLL ReleaseDetect();
void HAIRDLL HairInit(char *strDataPath);
void HAIRDLL HairColorTransform(cv::Mat originImg, cv::Mat MaskImg, cv::Mat& processImg, int RValue, int GValue, int BValue);
cv::Mat HAIRDLL MaskImageTransfor(cv::Mat inImage, cv::Mat inMaskImg, cv::Rect rt);
