#ifndef _FACE_H_
#define _FACE_H_
#include <vector>
#include <algorithm>
using namespace std;
using std::min;
using std::max;

#define MAX_PATH						260

#define MAX_FACE_PASS_CNT					3	//If the number of detected faces as of one person is less than this value abandon that.
#define CNT_DETECT_STEP						3
#define MAX_IMG_WIDTH						1600
#define MAX_IMG_HEIGHT						1600
#define MAX_ORGINIMG_WIDTH					5000
#define MAX_ORGINIMG_HEIGHT					5000
#define MAX_IMG_SIZE						(MAX_IMG_WIDTH * MAX_IMG_HEIGHT)
#define FIRST_SHRINK_RATE					1.25f
#define BORDER_WIDTH						8
#define BORDER_HEIGHT						8
#define SCAN_WIDTH							28
#define SCAN_HEIGHT							28
#define LIMIT_OF_SHRINK_RATE				83333.33f
#define SHRINK_COEFFICIENT					1.1892100572586060000f
#define WEIGHT_COEFFICIENT					0.8
#define FIRST_THRESHOLD						50.0
#define INTEGRAL_AVG_COEFF					0.00127551017794758f
#define STANDARD_CX							1600
#define NORMSIZE							640

#define STEP_CNT0_0	10
#define STEP_CNT0_1	3
#define STEP_CNT0_2	2

#define STEP_CNT1_0	16
#define STEP_CNT1_1	6
#define STEP_CNT1_2	4

#ifndef SafeMemFree
#define SafeMemFree(x)			{ if (x) { free(x); x = NULL;} }
#endif
#ifndef SafeMemDelete
#define SafeMemDelete(x)		{ if (x) {	delete x; x = NULL;	} }
#endif

typedef struct {
	int x;
	int y;
	int width;
	int height;
} CF_Rect;

typedef struct
{
	int			nFaces;							// face count
	CF_Rect		pFace[50];				// face structure
	int			nMaxFaceIdx;					// max face index
} SFaceInfo;

typedef struct{
	float		x;
	float		y;
	float		fScale;
	float		flt_20;
	float		flt_24;
}STU_DETECTED_INFO, *LPSTU_DETECTED_INFO;

typedef struct{
	int			nReserved;
	char		nPosX;
	char		nPosY;
	char		nCx;
	char		nCy;
	char		nType;
	char		nSiftCount;
	unsigned short		wp;
	signed char pbPattern[16];
	int			nPatternSize;
}STU_PATTERN0, *LPSTU_PATTERN0;

typedef struct{
	float fltFilterCoeff;
	float fltFilterLimit;
	unsigned short* pwCntStepPattern;
	float* pfltStepThreshold;
	int nStepCnt;
	LPSTU_PATTERN0 pstuPattern;
}STU_FILTER0, *LPSTU_FILTER0;

typedef struct{
	int nReserved;
	char byX;
	char byY;
	char byWidth;
	char byHeight;
	char by_8;
	char nSiftCount;
	char by_A;
	char by_B;
	float flt_C;
	float flt_10;
	unsigned char puby_14[16];
	signed char pby_24[16];
}STU_PATTERN2, *LPSTU_PATTERN2;

typedef struct{
	float flt_10;
	int nPatternSize;
	unsigned short* pwCntStepPattern;
	float* pfltFloorThreshold;
	float* pfltCeilThreshold;
	float* pflt_B0;
	int nCntStep;
	LPSTU_PATTERN2 pstuPattern;
}STU_FILTER2, *LPSTU_FILTER2;

typedef struct{
	unsigned char ubyPosX_4;
	unsigned char ubyPosY_5;
	unsigned char ubyCx;
	unsigned char ubyCy;
	unsigned short w_8;
	signed char pbyFilter_A[25];
	float flt_24;
	float flt_28;
	unsigned char puby_2C[32];
}STU_PATTERN1, *LPSTU_PATTERN1;

typedef struct{
	unsigned short*	pwCntStepPattern;
	int		nStepCnt;
	float*	pfltStepThreshold;
	LPSTU_PATTERN1 pstuPattern;
}STU_FILTER1, *LPSTU_FILTER1;

typedef struct{
	float	x;
	int		nIsPassed;
	float	y;
}EVAL_RESULTS, *LPEVAL_RESULTS;

typedef struct{
	int		piPassedNo[3];
	int		nCntPassed;
}STU_DETECTOR_STATE, *LPSTU_DETECTOR_STATE;

typedef struct{
	float x;
	float y;
}FLT_POINT, *LPFLT_POINT;

typedef vector<LPSTU_DETECTED_INFO> ARR_DETECTED_INFO, *LPARR_DETECTED_INFO;
typedef struct _NRECT NRECT, *LPNRECT;
struct _NRECT {
	union {
		int x;
		int l;
	};
	union {
		int y;
		int t;
	};
	union {
		int w;
		int r;
	};
	union {
		int h;
		int b;
	};
	LPNRECT pNext;
};
int		EvalSecond(LPSTU_DETECTOR_STATE pstuDetectorState, int* pnIntegralImg, int* pnSqrIntegralImg, int nX, int nY, LPARR_DETECTED_INFO pArrDetectedInfo);
int		PackResult(LPEVAL_RESULTS pEvalResult, float* pFilter, int nX, int nY, LPARR_DETECTED_INFO parrDetectedInfo);
int		Eval1thHaarFeature2(LPSTU_FILTER0 pstuFilter0, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult);
int		EvalSecondFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult);
int		MakeIntegralImg();
int		ShrinkImage();
int		ZoomOutTwice();
bool	CreateWorkImage();
int		WeightedAverage(LPNRECT pOutRect, ARR_DETECTED_INFO pDetectedRectArr, int* pnSortIndex, int nOffset, int nNum);
void	PreProc(unsigned char* pbyImg, int nImgWidth, int nImgHeight);
int		IsLagerThanScanRect();
void	ChannelDetector(int nTop, int nHeight);
int		EvalFirst(LPSTU_DETECTOR_STATE pstuDetectState, int* pnIntegralImg, int* pnSqrIntegralImg);
void	Choose2thStepData(LPSTU_DETECTOR_STATE pstuSecondStep, LPSTU_DETECTOR_STATE pstuFirstStep);
int		ClassifyDetectedRects(ARR_DETECTED_INFO pstuDetectedFaceArr, float fltAreaCoeff, int nScanWidth, int nScanHeight, int** ppnSortNo, int** ppnCntSameClass);
void	CorrectIntegralImg(int nY);
int		EvalFirstFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, float fltInitX);
int		Eval1thHaarFeature1(LPSTU_FILTER0 pstuFilter0, int* pnIntegralImg, float fltInitX, LPEVAL_RESULTS pstuResult);
int		Eval2ndHaarFeature(LPSTU_FILTER1 pstuFilter1, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult);
int		Eval3rdHaarFeature(LPSTU_FILTER2 pTrainResult, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult);
int		ExtractFeatureHaar(int nIntegralWidth, LPSTU_PATTERN0 pstuPattern, int* pnIntegralImg);
bool	IsAvaiableData(float* pflt2, float* pflt1, float pfltThreshold);
int		InitFaceDetector(char *sFilePath);
void	ReleaseFaceDetector();
int		LoadFirstStepData(char *sFilePath);
int		LoadSecondStepData(char *sFilePath);
float	Extract3thHaarFeature(LPSTU_PATTERN2 pstuPattern, int* pnIntegralImg, int* pnSqrIntegralImg, int nIntegralImgWidth, int nPatternEdgeLen, float fltAreaAvgCoeff, float fltPatternAvgCoeff);
int		DetectFace(unsigned char* pbyImg, int nImgWidth, int nImgHeight, LPNRECT *ppOutRect, double fltScale);
int		FaceDetectFunction(unsigned char* i_pImage, int cx, int cy, int bc, SFaceInfo *o_pFaceInfo);

#endif
