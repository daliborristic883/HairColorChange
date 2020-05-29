
#include "caffe/face.h"
#include <math.h>
#include <stdlib.h>




#define TRUE							1
#define FALSE							0

#define SQR(a)							((a) * (a))
#define IMIN(a, b)						((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))
#define IMAX(a, b)						((a) ^ (((a)^(b)) & (((a) > (b)) - 1)))

char	g_pbyOrgImg[MAX_IMG_SIZE];
int		g_nOrgWidth;
int		g_nOrgHeight;
int		g_nOrgImgSize;

char	g_pbyShrinkedImg[MAX_IMG_SIZE];
int		g_pnIntegralImg[MAX_IMG_SIZE * 4];
int		g_pnSqrIntegralImg[MAX_IMG_SIZE * 4];
int		g_nShrinkedWidth;
int		g_nShrinkedHeight;
int		g_nIntegralImgWidth;
int		g_nIntegralImgHeight;

float	g_fltShrinkRate;
float	g_fltMaxShrinkRate;

int		g_nZoomOutCnt;
int		g_nChannelCnt;

const float fltMin = (float)-3.4028235e38;
const float fltMax = 4.2949673e9f;
const float g_fConst16384 = 16384.0f;
const float g_fltZoomScale = 0.0000152587890625f;

STU_FILTER0 g_stuFilter0[3];
STU_FILTER1 g_stuFilter1[3];
STU_FILTER2 g_stuFilter2[3];
STU_FILTER0 g_IstuFilter0[3];
STU_FILTER1 g_IstuFilter1[3];
STU_FILTER2 g_IstuFilter2[3];

float g_pfltStepThreshold0_0_0[STEP_CNT0_0] = { -0.7421879768371582000f, -0.7734379768371582000f, -0.7617189884185791000f, -0.6074219942092895500f, -0.3808589875698089600f, -0.1777340024709701500f, 0.0146484002470970150f, 0.2158199995756149300f, 0.4169920086860656700f, 0.5356450080871582000f };
float g_pfltStepThreshold0_0_12[STEP_CNT0_0] = { -0.8046879768371582000f, -0.8945310115814209000f, -0.7304689884185791000f, -0.5253909826278686500f, -0.2285159975290298500f, 0.0996093973517417910f, 0.4345700144767761200f, 0.7373049855232238800f, 1.0493199825286865000f, 1.2973599433898926000f };
float g_pfltStepThreshold0_1_0[STEP_CNT0_1] = { 1.34131002426147f, 2.01623201370239f, 2.54932761192322f };
float g_pfltStepThreshold0_1_12[STEP_CNT0_1] = { 2.11377000808716f, 2.84326004981995f, 3.43701004981995f };
float g_pfltFloorThreshold0_2_0[STEP_CNT0_2] = { 3.1821300983429f, 3.76514005661011f };
float g_pfltCeilThreshold0_2[STEP_CNT0_2] = { 3.4028200183756568600e+38f, 3.4028200183756568600e+38f };
float g_pfltFloorThreshold0_2_12[STEP_CNT0_2] = { 4.00049018859863f, 4.58936023712158f };

unsigned short g_pwCntStepPattern0_0[STEP_CNT0_0] = { 0x2, 0x4, 0x6, 0x9, 0xE, 0x17, 0x25, 0x3B, 0x5F, 0x97 };
unsigned short g_pwCntStepPattern0_1[STEP_CNT0_1] = { 40, 62, 98 };
unsigned short g_pwCntStepPattern0_2[STEP_CNT0_2] = { 0x28, 0x3C };

float g_pfltStepThreshold1_0_0[STEP_CNT1_0] = { -2.1328101158142090000f, -1.9335900545120239000f, -1.6523400545120239000f, -1.3085900545120239000f,	-0.9589840173721313500f, -0.6621090173721313500f, -0.3105469942092895500f, 0.0283202994614839550f, 0.3837890028953552200f, 0.7197269797325134300f, 1.0107400417327881000f, 1.2783199548721313000f,	1.5048799514770508000f, 1.6923799514770508000f, 1.8154300451278687000f, 1.8232400417327881000f };
float g_pfltStepThreshold1_0_12[STEP_CNT1_0] = { -1.7890599966049194000f, -1.5273400545120239000f, -1.2695300579071045000f, -0.9121090173721313500f,	-0.5371090173721313500f, -0.0957031026482582090f, 0.3408200144767761200f, 0.7470700144767761200f, 1.2001999616622925000f, 1.5947300195693970000f, 1.9736299514770508000f, 2.3740200996398926000f, 2.7114300727844238000f, 3.0258800983428955000f, 3.2924799919128418000f, 3.5688500404357910000f };
float g_pfltStepThreshold1_1_0[STEP_CNT1_1] = { 2.66308999061584f, 3.33495998382568f, 3.94042992591858f, 4.46387004852295f, 4.93065977096558f, 5.30762004852295f };
float g_pfltStepThreshold1_1_12[STEP_CNT1_1] = { 4.36182022094727f, 5.02002000808716f, 5.58349990844727f, 6.11670017242432f, 6.59912014007568f, 7.03075981140137f };
float g_pfltFloorThreshold1_2_0[STEP_CNT1_2] = { 6.09668016433716f, 6.84472990036011f, 7.54442977905273f, 8.15966987609863f };
float g_pfltCeilThreshold1_2_0[STEP_CNT1_2] = { 20.8388996124268f, 15.5635004043579f, 12.5298004150391f, 10.8275995254517f };
float g_pfltFloorThreshold1_2_12[STEP_CNT1_2] = { 7.71045017242432f, 8.34716987609863f, 8.95557022094727f, 9.41748046875f };
float g_pfltCeilThreshold1_2_12[STEP_CNT1_2] = { 17.5443992614746f, 13.6625995635986f, 11.6781997680664f, 10.8002996444702f };

unsigned short g_pwCntStepPattern1_0[STEP_CNT1_0] = { 0x4, 0x5, 0x7, 0x9, 0xC, 0xF, 0x13, 0x19, 0x1F, 0x28, 0x34, 0x42, 0x55, 0x6D, 0x8C, 0xB4 };
unsigned short g_pwCntStepPattern1_1[STEP_CNT1_1] = { 0x14, 0x1B, 0x25, 0x33, 0x46, 0x5F };
unsigned short g_pwCntStepPattern1_2[STEP_CNT1_2] = { 0x19, 0x2A, 0x45, 0x73 };

float g_pflt_2step_3_0[8] = { 0.0f, 0.0f, 0.0f, 30.00000f, 35.00000f, 11.25000f, 0.9900000f, -1.000000f };
float g_pflt_2step_3_1[8] = { 0.0f, 35.0f, 0.0f, 30.00000f, 35.00000f, 15.00000f, 2.50999999046326f, -1.000000f };
float g_pflt_2step_3_2[8] = { 0.0f, -35.0f, 0.0f, 30.00000f, 35.00000f, 11.25000f, 0.9900000f, -1.000000f };

unsigned int fltAry256_1[256] = {
	0x3901679F, 0x3914DCB5, 0x392B0EAE, 0x39445845,
	0x39612005, 0x3980EB78, 0x39937DDD, 0x39A88DCB,
	0x39C06981, 0x39DB681B, 0x39F9E9D5, 0x3A0E2CC3,
	0x3A219671, 0x3A37731E, 0x3A500A66, 0x3A6BAB45,
	0x3A85562F, 0x3A96B66E, 0x3AAA2A72, 0x3ABFEB46,
	0x3AD83728, 0x3AF351FA, 0x3B08C2D8, 0x3B199154,
	0x3B2C4014, 0x3B40FE5E, 0x3B57FF5F, 0x3B717A5A,
	0x3B86D57A, 0x3B9668AD, 0x3BA7994A, 0x3BBA8C56,
	0x3BCF697F, 0x3BE65B36, 0x3BFF8EC1, 0x3C0D9A35,
	0x3C1CBFC5, 0x3C2D5354, 0x3C3F71C6, 0x3C5339AC,
	0x3C68CB49, 0x3C80244D, 0x3C8CEAAF, 0x3C9ACB8C,
	0x3CA9DA84, 0x3CBA2C1C, 0x3CCBD5B5, 0x3CDEED8B,
	0x3CF38AAF, 0x3D04E281, 0x3D10DA94, 0x3D1DBA41,
	0x3D2B8E8D, 0x3D3A64CD, 0x3D4A4A9D, 0x3D5B4DD5,
	0x3D6D7C7E, 0x3D807263, 0x3D8ACA79, 0x3D95CDAA,
	0x3DA18317, 0x3DADF1DA, 0x3DBB20F4, 0x3DC91747,
	0x3DD7DB8D, 0x3DE77449, 0x3DF7E7C0, 0x3E049DF5,
	0x3E0DBB34, 0x3E174E39, 0x3E21596A, 0x3E2BDEEA,
	0x3E36E098, 0x3E426005, 0x3E4E5E6E, 0x3E5ADCB5,
	0x3E67DB5C, 0x3E755A7C, 0x3E81ACE0, 0x3E88EC31,
	0x3E906A8E, 0x3E982718, 0x3EA020AC, 0x3EA855E2,
	0x3EB0C50F, 0x3EB96C3C, 0x3EC2492D, 0x3ECB5958,
	0x3ED499EC, 0x3EDE07C9, 0x3EE79F87, 0x3EF15D73,
	0x3EFB3D90, 0x3F029DCD, 0x3F07A983, 0x3F0CBF84,
	0x3F11DD4A, 0x3F17002D, 0x3F1C256D, 0x3F214A2C,
	0x3F266B77, 0x3F2B8642, 0x3F309771, 0x3F359BD8,
	0x3F3A903C, 0x3F3F715B, 0x3F443BEE, 0x3F48ECA8,
	0x3F4D8041, 0x3F51F375, 0x3F564309, 0x3F5A6BCF,
	0x3F5E6AAA, 0x3F623C90, 0x3F65DE93, 0x3F694DDD,
	0x3F6C87BB, 0x3F6F899C, 0x3F725117, 0x3F74DBEC,
	0x3F772808, 0x3F793387, 0x3F7AFCBB, 0x3F7C8227,
	0x3F7DC286, 0x3F7EBCCD, 0x3F7F7028, 0x3F7FDC03,
	0x3F800000, 0x3F7FDC03, 0x3F7F7028, 0x3F7EBCCD,
	0x3F7DC286, 0x3F7C8227, 0x3F7AFCBB, 0x3F793387,
	0x3F772808, 0x3F74DBEC, 0x3F725117, 0x3F6F899C,
	0x3F6C87BB, 0x3F694DDD, 0x3F65DE93, 0x3F623C90,
	0x3F5E6AAA, 0x3F5A6BCF, 0x3F564309, 0x3F51F375,
	0x3F4D8041, 0x3F48ECA8, 0x3F443BEE, 0x3F3F715B,
	0x3F3A903C, 0x3F359BD8, 0x3F309771, 0x3F2B8642,
	0x3F266B77, 0x3F214A2C, 0x3F1C256D, 0x3F17002D,
	0x3F11DD4A, 0x3F0CBF84, 0x3F07A983, 0x3F029DCD,
	0x3EFB3D90, 0x3EF15D73, 0x3EE79F87, 0x3EDE07C9,
	0x3ED499EC, 0x3ECB5958, 0x3EC2492D, 0x3EB96C3C,
	0x3EB0C50F, 0x3EA855E2, 0x3EA020AC, 0x3E982718,
	0x3E906A8E, 0x3E88EC31, 0x3E81ACE0, 0x3E755A7C,
	0x3E67DB5C, 0x3E5ADCB5, 0x3E4E5E6E, 0x3E426005,
	0x3E36E098, 0x3E2BDEEA, 0x3E21596A, 0x3E174E39,
	0x3E0DBB34, 0x3E049DF5, 0x3DF7E7C0, 0x3DE77449,
	0x3DD7DB8D, 0x3DC91747, 0x3DBB20F4, 0x3DADF1DA,
	0x3DA18317, 0x3D95CDAA, 0x3D8ACA79, 0x3D807263,
	0x3D6D7C7E, 0x3D5B4DD5, 0x3D4A4A9D, 0x3D3A64CD,
	0x3D2B8E8D, 0x3D1DBA41, 0x3D10DA94, 0x3D04E281,
	0x3CF38AAF, 0x3CDEED8B, 0x3CCBD5B5, 0x3CBA2C1C,
	0x3CA9DA84, 0x3C9ACB8C, 0x3C8CEAAF, 0x3C80244D,
	0x3C68CB49, 0x3C5339AC, 0x3C3F71C6, 0x3C2D5354,
	0x3C1CBFC5, 0x3C0D9A35, 0x3BFF8EC1, 0x3BE65B36,
	0x3BCF697F, 0x3BBA8C56, 0x3BA7994A, 0x3B9668AD,
	0x3B86D57A, 0x3B717A5A, 0x3B57FF5F, 0x3B40FE5E,
	0x3B2C4014, 0x3B199154, 0x3B08C2D8, 0x3AF351FA,
	0x3AD83728, 0x3ABFEB46, 0x3AAA2A72, 0x3A96B66E,
	0x3A85562F, 0x3A6BAB45, 0x3A500A66, 0x3A37731E,
	0x3A219671, 0x3A0E2CC3, 0x39F9E9D5, 0x39DB681B,
	0x39C06981, 0x39A88DCB, 0x39937DDD, 0x3980EB78,
	0x39612005, 0x39445845, 0x392B0EAE, 0x3914DCB5
};
unsigned int fltAry256_2[256] = {
	0x00000000, 0x3CC90AB0, 0x3D48FB2F, 0x3D96A905,
	0x3DC8BD36, 0x3DFAB273, 0x3E164083, 0x3E2F10A2,
	0x3E47C5C2, 0x3E605C13, 0x3E78CFCC, 0x3E888E93,
	0x3E94A031, 0x3EA09AE5, 0x3EAC7CD4, 0x3EB8442A,
	0x3EC3EF15, 0x3ECF7BCA, 0x3EDAE880, 0x3EE63375,
	0x3EF15AEA, 0x3EFC5D27, 0x3F039C3D, 0x3F08F59B,
	0x3F0E39DA, 0x3F13682A, 0x3F187FC0, 0x3F1D7FD1,
	0x3F226799, 0x3F273656, 0x3F2BEB4A, 0x3F3085BB,
	0x3F3504F3, 0x3F396842, 0x3F3DAEF9, 0x3F41D870,
	0x3F45E403, 0x3F49D112, 0x3F4D9F02, 0x3F514D3D,
	0x3F54DB31, 0x3F584853, 0x3F5B941A, 0x3F5EBE05,
	0x3F61C598, 0x3F64AA59, 0x3F676BD8, 0x3F6A09A7,
	0x3F6C835E, 0x3F6ED89E, 0x3F710908, 0x3F731447,
	0x3F74FA0B, 0x3F76BA07, 0x3F7853F8, 0x3F79C79D,
	0x3F7B14BE, 0x3F7C3B28, 0x3F7D3AAC, 0x3F7E1324,
	0x3F7EC46D, 0x3F7F4E6D, 0x3F7FB10F, 0x3F7FEC43,
	0x3F800000, 0x3F7FEC43, 0x3F7FB10F, 0x3F7F4E6D,
	0x3F7EC46D, 0x3F7E1324, 0x3F7D3AAC, 0x3F7C3B28,
	0x3F7B14BE, 0x3F79C79D, 0x3F7853F8, 0x3F76BA07,
	0x3F74FA0B, 0x3F731447, 0x3F710908, 0x3F6ED89E,
	0x3F6C835E, 0x3F6A09A7, 0x3F676BD8, 0x3F64AA59,
	0x3F61C598, 0x3F5EBE05, 0x3F5B941A, 0x3F584853,
	0x3F54DB31, 0x3F514D3D, 0x3F4D9F02, 0x3F49D112,
	0x3F45E403, 0x3F41D870, 0x3F3DAEF9, 0x3F396842,
	0x3F3504F3, 0x3F3085BB, 0x3F2BEB4A, 0x3F273656,
	0x3F226799, 0x3F1D7FD1, 0x3F187FC0, 0x3F13682A,
	0x3F0E39DA, 0x3F08F59B, 0x3F039C3D, 0x3EFC5D27,
	0x3EF15AEA, 0x3EE63375, 0x3EDAE880, 0x3ECF7BCA,
	0x3EC3EF15, 0x3EB8442A, 0x3EAC7CD4, 0x3EA09AE5,
	0x3E94A031, 0x3E888E93, 0x3E78CFCC, 0x3E605C13,
	0x3E47C5C2, 0x3E2F10A2, 0x3E164083, 0x3DFAB273,
	0x3DC8BD36, 0x3D96A905, 0x3D48FB2F, 0x3CC90AB0,
	0x250D400C, 0xBCC90AB0, 0xBD48FB2F, 0xBD96A905,
	0xBDC8BD36, 0xBDFAB273, 0xBE164083, 0xBE2F10A2,
	0xBE47C5C2, 0xBE605C13, 0xBE78CFCC, 0xBE888E93,
	0xBE94A031, 0xBEA09AE5, 0xBEAC7CD4, 0xBEB8442A,
	0xBEC3EF15, 0xBECF7BCA, 0xBEDAE880, 0xBEE63375,
	0xBEF15AEA, 0xBEFC5D27, 0xBF039C3D, 0xBF08F59B,
	0xBF0E39DA, 0xBF13682A, 0xBF187FC0, 0xBF1D7FD1,
	0xBF226799, 0xBF273656, 0xBF2BEB4A, 0xBF3085BB,
	0xBF3504F3, 0xBF396842, 0xBF3DAEF9, 0xBF41D870,
	0xBF45E403, 0xBF49D112, 0xBF4D9F02, 0xBF514D3D,
	0xBF54DB31, 0xBF584853, 0xBF5B941A, 0xBF5EBE05,
	0xBF61C598, 0xBF64AA59, 0xBF676BD8, 0xBF6A09A7,
	0xBF6C835E, 0xBF6ED89E, 0xBF710908, 0xBF731447,
	0xBF74FA0B, 0xBF76BA07, 0xBF7853F8, 0xBF79C79D,
	0xBF7B14BE, 0xBF7C3B28, 0xBF7D3AAC, 0xBF7E1324,
	0xBF7EC46D, 0xBF7F4E6D, 0xBF7FB10F, 0xBF7FEC43,
	0xBF800000, 0xBF7FEC43, 0xBF7FB10F, 0xBF7F4E6D,
	0xBF7EC46D, 0xBF7E1324, 0xBF7D3AAC, 0xBF7C3B28,
	0xBF7B14BE, 0xBF79C79D, 0xBF7853F8, 0xBF76BA07,
	0xBF74FA0B, 0xBF731447, 0xBF710908, 0xBF6ED89E,
	0xBF6C835E, 0xBF6A09A7, 0xBF676BD8, 0xBF64AA59,
	0xBF61C598, 0xBF5EBE05, 0xBF5B941A, 0xBF584853,
	0xBF54DB31, 0xBF514D3D, 0xBF4D9F02, 0xBF49D112,
	0xBF45E403, 0xBF41D870, 0xBF3DAEF9, 0xBF396842,
	0xBF3504F3, 0xBF3085BB, 0xBF2BEB4A, 0xBF273656,
	0xBF226799, 0xBF1D7FD1, 0xBF187FC0, 0xBF13682A,
	0xBF0E39DA, 0xBF08F59B, 0xBF039C3D, 0xBEFC5D27,
	0xBEF15AEA, 0xBEE63375, 0xBEDAE880, 0xBECF7BCA,
	0xBEC3EF15, 0xBEB8442A, 0xBEAC7CD4, 0xBEA09AE5,
	0xBE94A031, 0xBE888E93, 0xBE78CFCC, 0xBE605C13,
	0xBE47C5C2, 0xBE2F10A2, 0xBE164083, 0xBDFAB273,
	0xBDC8BD36, 0xBD96A905, 0xBD48FB2F, 0xBCC90AB0
};

float*		g_fltAry256_1 = (float*)fltAry256_1;
float*		g_fltAry256_2 = (float*)fltAry256_2;


int Eval1thHaarFeature2(LPSTU_FILTER0 pstuFilter0, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{
	int cy = SCAN_HEIGHT * g_nIntegralImgWidth;
	unsigned short* pwStepPattern;
	float fltY;
	int nVal;
	int nSum;
	float fltSqrIntegralVal;
	float fltIntegralVal;
	int i, j;
	LPSTU_PATTERN0 pPatternAddr = pstuFilter0->pstuPattern;

	fltIntegralVal = (float)(pnIntegralImg[SCAN_WIDTH + cy] - pnIntegralImg[SCAN_WIDTH] - pnIntegralImg[cy] + pnIntegralImg[0]);
	if(fltIntegralVal < 0)
		fltIntegralVal += fltMax;
	
	fltSqrIntegralVal = (float)(pnSqrIntegralImg[SCAN_WIDTH + cy] - pnSqrIntegralImg[SCAN_WIDTH] - pnSqrIntegralImg[cy] + pnSqrIntegralImg[0]);
	if(fltSqrIntegralVal < 0)
		fltSqrIntegralVal += fltMax;
	fltIntegralVal = pstuFilter0->fltFilterCoeff * fltIntegralVal;
	fltIntegralVal = pstuFilter0->fltFilterCoeff * fltSqrIntegralVal - fltIntegralVal * fltIntegralVal;
	pstuResult->nIsPassed = 0;
	if(pstuFilter0->fltFilterLimit > fltIntegralVal){
		pstuResult->nIsPassed = -1;
		return -1;
	}
	
	fltSqrIntegralVal = g_fConst16384 / sqrt(fltIntegralVal);
	fltSqrIntegralVal = (fltSqrIntegralVal >= 0) ? fltSqrIntegralVal + 0.5f : fltSqrIntegralVal - 0.5f;
	nVal = (int)fltSqrIntegralVal;
	pwStepPattern = pstuFilter0->pwCntStepPattern;
	fltY = pstuResult->y;
	for(i = 0; i < pstuFilter0->nStepCnt; i++){
		nSum = 0;
		for(j = 0; j < pwStepPattern[i]; j++){
			cy = ExtractFeatureHaar(g_nIntegralImgWidth, pPatternAddr, pnIntegralImg) * nVal;
			cy += pPatternAddr->nPatternSize;
			cy /= 16777216;
			if(cy > 15)
				cy = 15;
			else if(cy < 0)
				cy = 0;
			cy = pPatternAddr->pbPattern[cy];
			nSum += (cy << pPatternAddr->nSiftCount);
			pPatternAddr++;
		}
		fltSqrIntegralVal = g_fltZoomScale * nSum + pstuResult->x;
		pstuResult->x = fltSqrIntegralVal;
		if(pstuFilter0->pfltStepThreshold[i] > fltSqrIntegralVal){
			pstuResult->nIsPassed = -1;
			break;
		}
		fltY += fltSqrIntegralVal - pstuFilter0->pfltStepThreshold[i];
	}
	pstuResult->x = fltSqrIntegralVal;
	pstuResult->y = fltY;
	return pstuResult->nIsPassed;
}

int EvalSecondFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{
	int rst = 0;
	if(Eval1thHaarFeature2(&g_IstuFilter0[index], pnIntegralImg, pnSqrIntegralImg, pstuResult) == 0)
		if(Eval2ndHaarFeature(&g_IstuFilter1[index], pnIntegralImg, pnSqrIntegralImg, pstuResult) == 0)
			Eval3rdHaarFeature(&g_IstuFilter2[index], pnIntegralImg, pnSqrIntegralImg, pstuResult);
	return rst;
}

int PackResult(LPEVAL_RESULTS pEvalResult, float* pFilter, int nX, int nY, LPARR_DETECTED_INFO parrDetectedInfo)
{
	LPSTU_DETECTED_INFO pDetectedInfo;
	if(pEvalResult->nIsPassed != -1){//add region
		if(fltMin >= pEvalResult->y)
			return -1;
		pDetectedInfo = new STU_DETECTED_INFO;
		pDetectedInfo->x = (float)(nX - BORDER_WIDTH) * g_fltShrinkRate;
		pDetectedInfo->y = (nY - BORDER_HEIGHT) * g_fltShrinkRate;
		pDetectedInfo->fScale = g_fltShrinkRate;
		pDetectedInfo->flt_20 = pFilter[2];
		pDetectedInfo->flt_24 = pEvalResult->y;
		parrDetectedInfo->push_back(pDetectedInfo);
	}
	return 0;
}

int EvalSecond(LPSTU_DETECTOR_STATE pstuDetectorState, int* pnIntegralImg, int* pnSqrIntegralImg, int nX, int nY, LPARR_DETECTED_INFO pArrDetectedInfo)
{
	int i;
	int iPassedNo;

	EVAL_RESULTS stuResult;
	for(i = 0; i < pstuDetectorState->nCntPassed; i++){
		iPassedNo = pstuDetectorState->piPassedNo[i];
		memset(&stuResult, 0, sizeof(EVAL_RESULTS));
		stuResult.nIsPassed = -1;
		EvalSecondFeatures(iPassedNo, pnIntegralImg, pnSqrIntegralImg, &stuResult);
		PackResult(&stuResult, g_IstuFilter2[iPassedNo].pflt_B0, nX, nY, pArrDetectedInfo);
	}
	return 0;
}

int MakeIntegralImg()
{
	int nIntegral;
	int nSqrIntegral;
	int* pnIntegralImgAddr, *pnSqrIntegralImgAddr, *pnIntegralImgAddr1, *pnSqrIntegralImgAddr1;
	int i, j;
	unsigned char* pubyShrinkedImgAddr;

	g_nIntegralImgWidth = g_nShrinkedWidth + 1;
	g_nIntegralImgHeight = g_nShrinkedHeight + 1;
	if(g_nIntegralImgHeight > 0x3D)
		g_nIntegralImgHeight = 0x3D;
	pnIntegralImgAddr = g_pnIntegralImg;
	pnSqrIntegralImgAddr = g_pnSqrIntegralImg;
	pubyShrinkedImgAddr = (unsigned char*)g_pbyShrinkedImg;
	memset(pnIntegralImgAddr, 0, sizeof(int) * g_nIntegralImgWidth);
	memset(pnSqrIntegralImgAddr, 0, sizeof(int) * g_nIntegralImgWidth);
	pnIntegralImgAddr1 = pnIntegralImgAddr;
	pnSqrIntegralImgAddr1 = pnSqrIntegralImgAddr;
	pnIntegralImgAddr += g_nIntegralImgWidth;
	pnSqrIntegralImgAddr += g_nIntegralImgWidth;
	for(i = 0; i < g_nShrinkedHeight; i++){
		*pnIntegralImgAddr = 0;
		*pnSqrIntegralImgAddr = 0;
		pnIntegralImgAddr++;
		pnSqrIntegralImgAddr++;
		pnIntegralImgAddr1++;
		pnSqrIntegralImgAddr1++;
		nIntegral = 0;
		nSqrIntegral = 0;
		for(j = 0; j < g_nShrinkedWidth; j++){
			nSqrIntegral += (*pubyShrinkedImgAddr) * (*pubyShrinkedImgAddr);
			nIntegral += (*pubyShrinkedImgAddr);
			*pnIntegralImgAddr = (*pnIntegralImgAddr1) + nIntegral;
			*pnSqrIntegralImgAddr = (*pnSqrIntegralImgAddr1) + nSqrIntegral;
			pubyShrinkedImgAddr++;
			pnIntegralImgAddr++;
			pnSqrIntegralImgAddr++;
			pnIntegralImgAddr1++;
			pnSqrIntegralImgAddr1++;
		}
	}
	g_nChannelCnt = 0;
	return 0;
}

int ShrinkImage()
{
	float fltShrinkRate;
	int nNewWidth;
	int nNewHeight;
	int iZoomedShrinkRate = 0;
	int nZoomedShrinkRate;
	int nZoomOutTimes = 1;
	int nOffset;
	int nWeight1;
	int nWeight2;
	int nDiffWidth;
	int nCnt;
	int nBase;
	int i, j;
	int nVal;
	unsigned short pwImg[MAX_IMG_WIDTH];
	unsigned char* pubyImgAddr;
	unsigned char* pbyImgAddr;

	nCnt = g_nZoomOutCnt > 0 ? g_nZoomOutCnt : - g_nZoomOutCnt;
	nBase = 2;
	while(nCnt){
		if(nCnt & 1)
			nZoomOutTimes *= nBase;
		nCnt >>= 1;
		nBase *= nBase;
	}
	if(g_nZoomOutCnt > 0)
		fltShrinkRate =  g_fltShrinkRate / nZoomOutTimes;
	else
		fltShrinkRate = g_fltShrinkRate;

	nZoomedShrinkRate = (int)(65536.0f * fltShrinkRate);
	nNewWidth = ((g_nOrgWidth << 16) - 1) / nZoomedShrinkRate;
	nNewHeight = ((g_nOrgHeight << 16) - 1) / nZoomedShrinkRate;

	g_nShrinkedWidth = nNewWidth + BORDER_WIDTH * 2;
	g_nShrinkedHeight = nNewHeight + BORDER_HEIGHT * 2;
	pbyImgAddr = (unsigned char*)g_pbyShrinkedImg + BORDER_HEIGHT * g_nShrinkedWidth + BORDER_WIDTH;//eax
	for(i = 0; i < nNewHeight; i++){
		pubyImgAddr = (unsigned char*)((iZoomedShrinkRate >> 16) * g_nOrgWidth + g_pbyOrgImg);
		nWeight2 = iZoomedShrinkRate & 0xFFFF;
		nWeight1 = 65536 - nWeight2;
		iZoomedShrinkRate += nZoomedShrinkRate;
		for(j = 0; j < g_nOrgWidth; j++){
			nVal = pubyImgAddr[j] * nWeight1;
			nVal += nWeight2 * pubyImgAddr[g_nOrgWidth + j];
			pwImg[j] = nVal >> 10;
		}
		nVal = 0;
		for(j = 0; j < nNewWidth; j++){
			nWeight1 = nVal >> 16;
			nWeight2 = nVal & 0xFFFF;
			pbyImgAddr[j] = (pwImg[nWeight1 + 1] * nWeight2 + (65536 - nWeight2) * pwImg[nWeight1]) >> 22;
			nVal += nZoomedShrinkRate;
		}
		pbyImgAddr += g_nShrinkedWidth;
	}
	if(nNewWidth > 1 && nNewHeight > 1){
		nZoomedShrinkRate = BORDER_WIDTH;
		nCnt = (int)(65536.0 * WEIGHT_COEFFICIENT);
		iZoomedShrinkRate = (int)((1.0 - WEIGHT_COEFFICIENT) * 128.0 * 65536.0);
		nOffset = (BORDER_HEIGHT + nNewHeight - 1) * g_nShrinkedWidth;
		nDiffWidth = - g_nShrinkedWidth;
		nWeight2 = BORDER_HEIGHT * g_nShrinkedWidth;
		for(i = 0; i < BORDER_HEIGHT; i++){
			pubyImgAddr = (unsigned char*)&g_pbyShrinkedImg[nWeight2 + nZoomedShrinkRate];
			pbyImgAddr = (unsigned char*)&g_pbyShrinkedImg[nWeight2 - g_nShrinkedWidth + nZoomedShrinkRate];
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr++;
			for(j = 0; j < nNewWidth - 2; j++){
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr++;
				pubyImgAddr++;
			}
			*pbyImgAddr = (pubyImgAddr[1] * nCnt + iZoomedShrinkRate) >> 16;
			pubyImgAddr = (unsigned char*)g_pbyShrinkedImg + nOffset + nZoomedShrinkRate;
			pbyImgAddr = (unsigned char*)pubyImgAddr + g_nShrinkedWidth;
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr++;
			for(j = 0; j < nNewWidth - 2; j++){
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr++;
				pubyImgAddr++;
			}
			
			*pbyImgAddr = (pubyImgAddr[1] * nCnt + iZoomedShrinkRate) >> 16;
			nOffset += g_nShrinkedWidth;
			nWeight2 += nDiffWidth;
		}
		for(i = 0; i < nZoomedShrinkRate; i++){
			pubyImgAddr = (unsigned char*)&g_pbyShrinkedImg[nZoomedShrinkRate - i];
			pbyImgAddr = (unsigned char*)&g_pbyShrinkedImg[nZoomedShrinkRate - i - 1];
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr += g_nShrinkedWidth;
			for(j = 0; j < g_nShrinkedHeight - 2; j++){
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[g_nShrinkedWidth * 2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr+= g_nShrinkedWidth;
				pubyImgAddr+= g_nShrinkedWidth;
			}
			*pbyImgAddr = (pubyImgAddr[g_nShrinkedWidth] * nCnt + iZoomedShrinkRate) >> 16;
			pubyImgAddr = (unsigned char*)g_pbyShrinkedImg + nZoomedShrinkRate + i + nNewWidth - 1;
			pbyImgAddr = (unsigned char*)pubyImgAddr + 1;
			*pbyImgAddr = ((*pubyImgAddr) * nCnt + iZoomedShrinkRate) >> 16;
			pbyImgAddr += g_nShrinkedWidth;
			for(j = 0; j < g_nShrinkedHeight - 2; j++){
				*pbyImgAddr = ((pubyImgAddr[0] + pubyImgAddr[g_nShrinkedWidth * 2]) * nCnt + iZoomedShrinkRate * 2) >> 17;
				pbyImgAddr+= g_nShrinkedWidth;
				pubyImgAddr+= g_nShrinkedWidth;
			}
			*pbyImgAddr = (pubyImgAddr[g_nShrinkedWidth] * nCnt + iZoomedShrinkRate) >> 16;
		}
#ifdef DEBUG_MODE
		WriteDebugInfo("g:\\debugNew.dat", g_pbyShrinkedImg, g_nShrinkedWidth * g_nShrinkedHeight);
#endif
	}else{
		pbyImgAddr = (unsigned char*)&g_pbyShrinkedImg[g_nShrinkedWidth - 1];
		for(i = 0; i < g_nShrinkedHeight; i++){
			if(i < BORDER_HEIGHT || i < g_nShrinkedWidth - BORDER_HEIGHT){
				if(g_nShrinkedWidth > 0)
					memset(g_pbyShrinkedImg, 0x80, g_nShrinkedWidth);
			}else{
				for(j = 0; j < BORDER_WIDTH; j++){
					g_pbyShrinkedImg[j] = (char)0x80;
					*pbyImgAddr = (char)0x80;
					pbyImgAddr--;
				}
			}
		}
	}
	return 0;
}

int ZoomOutTwice()
{
	int iImg = 0;
	int nBeforeWidth;
	int nVal;
	int i, j;

	nBeforeWidth = g_nOrgWidth;
	g_nOrgWidth = nBeforeWidth / 2;
	g_nOrgHeight = g_nOrgHeight / 2;
	if(g_nZoomOutCnt >= 0){
		for(i = 0; i < g_nOrgHeight; i++){
			for(j = 0; j < g_nOrgWidth; j++){
				nVal = (unsigned char)g_pbyOrgImg[iImg + j * 2 + nBeforeWidth + 1];
				nVal += (unsigned char)g_pbyOrgImg[iImg + j * 2 + 1];
				nVal += (unsigned char)g_pbyOrgImg[iImg + j * 2 + nBeforeWidth];
				nVal += (unsigned char)g_pbyOrgImg[iImg + j * 2] + 2;
				g_pbyOrgImg[i * g_nOrgWidth + j] = nVal >> 2;
			}
			iImg += 2 * nBeforeWidth;
		}
	}else{
		for(i = 0; i < g_nOrgHeight; i++){
			for(j = 0; j < g_nOrgWidth; j++){
				g_pbyOrgImg[i * g_nOrgWidth + j] = g_pbyOrgImg[j * 2];
			}
		}
	}
	g_nZoomOutCnt++;
	return 0;
}

bool CreateWorkImage()
{
	double dbl_exp, dbl_val;
	g_fltShrinkRate = SHRINK_COEFFICIENT * g_fltShrinkRate;
	if(g_fltShrinkRate > g_fltMaxShrinkRate){
		return false;
	}
	dbl_exp = g_nZoomOutCnt + 1;
	dbl_val = pow(2.0, dbl_exp);
	while(dbl_val < g_fltShrinkRate){
		ZoomOutTwice();
		dbl_exp = g_nZoomOutCnt + 1;
		dbl_val = pow(2.0, dbl_exp);
	}
 	ShrinkImage();
 	MakeIntegralImg();
	return true;				   
}

int WeightedAverage(LPNRECT pOutRect, ARR_DETECTED_INFO pDetectedRectArr, int* pnSortIndex, int nOffset, int nNum)
{
	float fltDist;
	int iMaxNo;
	float fMax_24;
	float fScaleSize;
	float fScalPos;
	float fTemp;
	float fltLogScale;
	float fltLogScaleForMaxNo;
	float fltCoeffSize;
	FLT_POINT pos;
	FLT_POINT posCenter;
	FLT_POINT posDiff;
	FLT_POINT posMax;

	int i, iSortIndex;
	int nDetectNum = (int)pDetectedRectArr.size();

	if(nNum + nOffset > nDetectNum){
		return -1;
	}
	fMax_24 = fltMin;
	iMaxNo = 0;
	for(i = 0; i < nNum; i++){
		iSortIndex = pnSortIndex[nOffset + i];
		if(fMax_24 < pDetectedRectArr[iSortIndex]->flt_24){
			fMax_24 = pDetectedRectArr[iSortIndex]->flt_24;
			iMaxNo = iSortIndex;
		}
	}
	                        
	posMax.x = pDetectedRectArr[iMaxNo]->fScale * SCAN_WIDTH * 0.5f + pDetectedRectArr[iMaxNo]->x;
	posMax.y = pDetectedRectArr[iMaxNo]->fScale * SCAN_HEIGHT * 0.5f + pDetectedRectArr[iMaxNo]->y;
	fltLogScaleForMaxNo = log(pDetectedRectArr[iMaxNo]->fScale);
	fScaleSize = pDetectedRectArr[iMaxNo]->flt_20;
	posCenter.x = 0;
	posCenter.y = 0;
	fltCoeffSize = 0;
	fScalPos = 0;
	for(i = 0; i < nNum; i++){
		iSortIndex = pnSortIndex[nOffset + i];
		fTemp = pDetectedRectArr[iSortIndex]->flt_20 - fScaleSize;
		while(fTemp > 180.0){
			fTemp -= 360.0;
		}
		while(fTemp < -180.0){
			fTemp += 360.0;
		}
		if(fTemp <= 0)
			fTemp = -fTemp;
		fltLogScale = log(pDetectedRectArr[iSortIndex]->fScale);
		pos.x = pDetectedRectArr[iSortIndex]->fScale * SCAN_WIDTH * 0.5f + pDetectedRectArr[iSortIndex]->x;
		pos.y = pDetectedRectArr[iSortIndex]->fScale * SCAN_HEIGHT * 0.5f + pDetectedRectArr[iSortIndex]->y;
		
		if(FIRST_THRESHOLD >= fTemp){
			fTemp = fltLogScale - fltLogScaleForMaxNo;
			if(fTemp <= 0)
				fTemp = -fTemp;
			
			if(0.349999994039536 >= fTemp){
				posDiff.y = posMax.y - pos.y;
				posDiff.x = posMax.x - pos.x;
				fltDist = sqrt(posDiff.x * posDiff.x + posDiff.y * posDiff.y);
				if(0.6 * SCAN_WIDTH * pDetectedRectArr[iSortIndex]->fScale >= fltDist){
					fTemp = pDetectedRectArr[iSortIndex]->flt_24;
					fScalPos += fTemp;
					posCenter.x += pos.x * fTemp;
					posCenter.y += pos.y * fTemp;
					fltCoeffSize += fTemp * fltLogScale;
				}
			}
		}
	}
	fScalPos = fScalPos > 0 ? 1 / fScalPos : 0;
	fScaleSize = exp(fScalPos * fltCoeffSize);
	pOutRect->l = (int)(posCenter.x * fScalPos - 0.5 * SCAN_WIDTH * fScaleSize);
	pOutRect->t = (int)(posCenter.y * fScalPos - 0.5 * SCAN_HEIGHT * fScaleSize);
	pOutRect->r = (int)(pOutRect->l + fScaleSize * SCAN_WIDTH);
	pOutRect->b = (int)(pOutRect->t + fScaleSize * SCAN_HEIGHT);
	return 0;
}

void PreProc(unsigned char* pbyImg, int nImgWidth, int nImgHeight)
{
	float fltMaxShrinkHorzRate, fltMaxShrinkVertRate;
	int nHalfScanW, nRealScanW, nRealScanH, nHalfScanH;
	g_nOrgWidth = nImgWidth;
	g_nOrgHeight = nImgHeight;
	g_nOrgImgSize = nImgWidth * nImgHeight;

	g_fltShrinkRate = FIRST_SHRINK_RATE;
	memcpy(g_pbyOrgImg, pbyImg, g_nOrgImgSize);
	nRealScanW = SCAN_WIDTH - BORDER_WIDTH * 2 + 1;
	nRealScanH = SCAN_HEIGHT - BORDER_HEIGHT * 2 + 1;
	nHalfScanW = SCAN_WIDTH >> 1;
	nHalfScanH = SCAN_HEIGHT >> 1;
	fltMaxShrinkHorzRate = (float)(nRealScanW > nHalfScanW ? nRealScanW : nHalfScanW);
	fltMaxShrinkVertRate = (float)(nRealScanH > nHalfScanH ? nRealScanH : nHalfScanH);
	fltMaxShrinkHorzRate = (float)g_nOrgWidth / fltMaxShrinkHorzRate;
	fltMaxShrinkVertRate = (float)g_nOrgHeight / fltMaxShrinkVertRate;

	g_fltMaxShrinkRate = fltMaxShrinkHorzRate < fltMaxShrinkVertRate ? fltMaxShrinkHorzRate : fltMaxShrinkVertRate;
	if(LIMIT_OF_SHRINK_RATE <= g_fltMaxShrinkRate)
		g_fltMaxShrinkRate = LIMIT_OF_SHRINK_RATE;
	g_nZoomOutCnt = 0;
	ShrinkImage();
	MakeIntegralImg();
	return;
}

int IsLagerThanScanRect()
{
	if(g_nShrinkedWidth < SCAN_WIDTH)
		return 0;
	if(g_nShrinkedHeight < SCAN_HEIGHT)
		return 0;
	return 1;
}

void ChannelDetector(int nTop, int nHeight)
{
	int nY;
	if(60 >= nHeight){
		nY = nTop - g_nChannelCnt + nHeight;
		if(nY > 60){
			nY = nTop  + 60;
			if(nY > g_nShrinkedHeight){
				nY = g_nShrinkedHeight - g_nChannelCnt - 60;
			}else{
				nY = nTop - g_nChannelCnt;
			}
			if(nY > 0){
				CorrectIntegralImg(nY);
			}
		}
	}
}

int EvalFirst(LPSTU_DETECTOR_STATE pstuDetectState, int* pnIntegralImg, int* pnSqrIntegralImg)
{
	int nRB;//Right Bottom Position
	int nLB;//Left Bottom Position
	float flt_val;
	float fltIntegralAvg;
	float fltSqrIntegralAvg;
	int nSqrIntegral, nIntegral;

	nLB = g_nIntegralImgWidth * SCAN_HEIGHT;
	nRB = nLB + SCAN_WIDTH;
	nIntegral = pnIntegralImg[nRB];
	nIntegral -= pnIntegralImg[SCAN_WIDTH];
	nIntegral -= pnIntegralImg[nLB];
	nIntegral += pnIntegralImg[0];
	fltIntegralAvg = (float)nIntegral;
	if(fltIntegralAvg < 0)
		fltIntegralAvg += fltMax;
	
	fltIntegralAvg = fltIntegralAvg * INTEGRAL_AVG_COEFF;
	nSqrIntegral = pnSqrIntegralImg[nRB];
	nSqrIntegral -= pnSqrIntegralImg[SCAN_WIDTH];
	nSqrIntegral -= pnSqrIntegralImg[nLB];
	nSqrIntegral += pnSqrIntegralImg[0];
	fltSqrIntegralAvg = (float)nSqrIntegral;
	if(fltSqrIntegralAvg < 0)
		fltSqrIntegralAvg += fltMax;
	fltSqrIntegralAvg = fltSqrIntegralAvg * INTEGRAL_AVG_COEFF;
	
	fltIntegralAvg = pow(fltIntegralAvg, 2);
	fltSqrIntegralAvg = fltSqrIntegralAvg - fltIntegralAvg;
	if(fltSqrIntegralAvg <= FIRST_THRESHOLD)
		return 0;
	
	flt_val = 1 / sqrt(fltSqrIntegralAvg);
	pstuDetectState->nCntPassed = 0;
	for(int i = 0; i < CNT_DETECT_STEP; i ++)
	{
		if(EvalFirstFeatures(i, pnIntegralImg, pnSqrIntegralImg, flt_val) >= 0)
		{
			pstuDetectState->piPassedNo[pstuDetectState->nCntPassed] = i;
			pstuDetectState->nCntPassed++;
		}
	}
	return pstuDetectState->nCntPassed;
}

void Choose2thStepData(LPSTU_DETECTOR_STATE pstuSecondStep, LPSTU_DETECTOR_STATE pstuFirstStep)
{
	int nCntFirstPassed;
	int i, j;
	float* pflt_ESI;
	int iPassedDataNo;
	
	nCntFirstPassed = pstuFirstStep->nCntPassed;
	pstuSecondStep->nCntPassed = 0;
	for(i = 0; i < CNT_DETECT_STEP; i ++){
		pflt_ESI = g_IstuFilter2[i].pflt_B0;
		for(j = 0; j < nCntFirstPassed; j ++){
			iPassedDataNo = pstuFirstStep->piPassedNo[j];
			if(IsAvaiableData(g_IstuFilter2[iPassedDataNo].pflt_B0, pflt_ESI, 0.1f)){
				pstuSecondStep->piPassedNo[pstuSecondStep->nCntPassed] = i;
				pstuSecondStep->nCntPassed ++;
				break;
			}
		}
	}
}

int ClassifyDetectedRects(ARR_DETECTED_INFO pstuDetectedFaceArr, float fltAreaCoeff, int nScanWidth, int nScanHeight, int** ppnSortNo, int** ppnCntSameClass)
{
	int nCntCandidate = 0;
	float fltAreaA;
	float fltAreaB;
	float flt_tmp;
	int j, k, i = 0, nCntNeighbourRect;
	int *pnCntSameClass, *pnSortNo;
	float fltAX1, fltAY1, fltAX2, fltAY2;
	float fltBX1, fltBY1, fltBX2, fltBY2;
	int nDetectCnt = (int)pstuDetectedFaceArr.size();

	if(nDetectCnt <= 0)
		return 0;
	
	pnSortNo = new int[nDetectCnt];
	pnCntSameClass = new int[nDetectCnt];
	for(j = 0; j < nDetectCnt; j++){
		pnSortNo[j] = j;
		pnCntSameClass[j] = 1;
	}

	*ppnSortNo = pnSortNo;
	*ppnCntSameClass = pnCntSameClass;

	if(nDetectCnt == 1)
		return 1;
	
	while(i < nDetectCnt){
		fltAreaB = fltMin;
		nCntNeighbourRect = 0;
		for(j = i; j < nDetectCnt; j++){
			flt_tmp = pstuDetectedFaceArr[pnSortNo[j]]->flt_24;
			if(fltAreaB < flt_tmp){
				fltAreaB = flt_tmp;
				nCntNeighbourRect = j;
			}
		}
		k = pnSortNo[nCntNeighbourRect];
		fltAX1 = pstuDetectedFaceArr[k]->x;
		fltAY1 = pstuDetectedFaceArr[k]->y;
		fltAX2 = pstuDetectedFaceArr[k]->fScale * SCAN_WIDTH + fltAX1;
		fltAY2 = pstuDetectedFaceArr[k]->fScale * SCAN_HEIGHT + fltAY1;
		pnSortNo[nCntNeighbourRect] = pnSortNo[i];
		pnSortNo[i] = k;
		i++;
		nCntNeighbourRect = 1;
		fltAreaA = (fltAY2 - fltAY1) * (fltAX2 - fltAX1);
		for(j = i; j < nDetectCnt; j++){
			k = pnSortNo[j];
			fltBX1 = pstuDetectedFaceArr[k]->x;
			fltBY1 = pstuDetectedFaceArr[k]->y;
			fltBX2 = pstuDetectedFaceArr[k]->fScale * SCAN_WIDTH + fltBX1;
			fltBY2 = pstuDetectedFaceArr[k]->fScale * SCAN_HEIGHT + fltBY1;
			fltAreaB = (fltBY2 - fltBY1) * (fltBX2 - fltBX1);
			fltBX1 = fltAX1 > fltBX1 ? fltAX1 : fltBX1;
			fltBY1 = fltAY1 > fltBY1 ? fltAY1 : fltBY1;
			fltBX2 = fltAX2 < fltBX2 ? fltAX2 : fltBX2;
			fltBY2 = fltAY2 < fltBY2 ? fltAY2 : fltBY2;
			if(fltBX1 > fltBX2)
				fltBX2 = fltBX1;
			if(fltBY1 > fltBY2)
				fltBY2 = fltBY1;
			flt_tmp = fltAreaA;
			if(fltAreaB <= fltAreaA){
				flt_tmp = fltAreaB;
			}
			fltAreaB = (fltBY2 - fltBY1) * (fltBX2 - fltBX1);
			if(flt_tmp * fltAreaCoeff < fltAreaB){
				pnSortNo[j] = pnSortNo[i];
				pnSortNo[i] = k;
				i ++;
				nCntNeighbourRect ++;
			}
		}
		pnCntSameClass[nCntCandidate] = nCntNeighbourRect;
		nCntCandidate ++;
	}
	return nCntCandidate;
}

void CorrectIntegralImg(int nY)
{
	int	iIntegral;
	int	iSqrIntegral;
	int n, nHeight, cx, cy;
	int *pnIntegralImgAddr, *pnSqrIntegralImgAddr, *pnIntegralImgAddr_1, *pnSqrIntegralImgAddr_1;
	int i, j;
	unsigned char *pbyShrinkedImgAddr;
	cy = 60;
	if(cy + g_nChannelCnt + nY > g_nShrinkedHeight){
		cy = g_nShrinkedHeight - g_nChannelCnt - nY;
	}
	
	pnIntegralImgAddr = g_pnIntegralImg;
	pnSqrIntegralImgAddr = g_pnSqrIntegralImg;
	pbyShrinkedImgAddr = (unsigned char*)g_pbyShrinkedImg + (g_nChannelCnt + cy) * g_nShrinkedWidth;
	nHeight = cy + 1;
	cx = g_nShrinkedWidth + 1;
	n = cx * nY;
	for(nHeight -= nY; nHeight > 0; nHeight--){
		memcpy(pnIntegralImgAddr, pnIntegralImgAddr + n, cx * 4);
		memcpy(pnSqrIntegralImgAddr, pnSqrIntegralImgAddr + n, cx * 4);
		pnIntegralImgAddr += cx;
		pnSqrIntegralImgAddr += cx;
	}
	for(i = 0; i < nY; i++){
		*(pnIntegralImgAddr++) = 0;
		*(pnSqrIntegralImgAddr++) = 0;
		iIntegral = 0;
		iSqrIntegral = 0;
		pnSqrIntegralImgAddr_1 = pnSqrIntegralImgAddr - cx;
		pnIntegralImgAddr_1 = pnIntegralImgAddr - cx;
		for(j = 0; j < g_nShrinkedWidth; j++){
			n = *(unsigned char*)(pbyShrinkedImgAddr++);
			iIntegral += n;
			iSqrIntegral += n * n;
			*(pnIntegralImgAddr++) = *(pnIntegralImgAddr_1++) + iIntegral;
			*(pnSqrIntegralImgAddr++) = *(pnSqrIntegralImgAddr_1++) + iSqrIntegral;
		}
	}
	g_nChannelCnt += nY;
}

int EvalFirstFeatures(int index, int* pnIntegralImg, int* pnSqrIntegralImg, float fltInitX)
{
	EVAL_RESULTS stuResult = {0, -1, 0};
	if(Eval1thHaarFeature1(&g_stuFilter0[index], pnIntegralImg, fltInitX, &stuResult) == 0)
		if(Eval2ndHaarFeature(&g_stuFilter1[index], pnIntegralImg, pnSqrIntegralImg, &stuResult) == 0)
			Eval3rdHaarFeature(&g_stuFilter2[index], pnIntegralImg, pnSqrIntegralImg, &stuResult);
	return stuResult.nIsPassed;
}

int Eval1thHaarFeature1(LPSTU_FILTER0 pstuFilter0, int* pnIntegralImg, float fltInitX, LPEVAL_RESULTS pstuResult)
{
	int nHaar;
	int n;
	unsigned short* pwStepPattern;
	int nStepCnt;
	float fltX, fltY;
	int nSum;
	int i, index = 0;

	fltX = fltInitX * g_fConst16384;
	if(fltX > 0)
		fltX += 0.5;
	else
		fltX -= 0.5;
	n = (int)fltX;
	nStepCnt = pstuFilter0->nStepCnt;
	pwStepPattern = pstuFilter0->pwCntStepPattern;
	fltX = pstuResult->x;
	fltY = pstuResult->y;
	pstuResult->nIsPassed = 0;
	for(i = 0; i < nStepCnt; i++){
		nSum = 0;
		for(int j = 0; j < pwStepPattern[i]; j++){
			nHaar = ExtractFeatureHaar(g_nIntegralImgWidth, &pstuFilter0->pstuPattern[index], pnIntegralImg);
			nHaar = nHaar * n + pstuFilter0->pstuPattern[index].nPatternSize;
			nHaar = nHaar >> 0x18;
			if(nHaar < 0){
				nHaar = 0;
			}else if(nHaar > 15){
				nHaar = 15;
			}
            nSum += ((int)pstuFilter0->pstuPattern[index].pbPattern[nHaar] << pstuFilter0->pstuPattern[index].nSiftCount);
            index++;
		}
		fltX += g_fltZoomScale * nSum;
		if(pstuFilter0->pfltStepThreshold[i] > fltX){
			pstuResult->nIsPassed = -1;
			break;
		}
		fltY += (fltX - pstuFilter0->pfltStepThreshold[i]);
	}
	pstuResult->x = fltX;
	pstuResult->y = fltY;
	return pstuResult->nIsPassed;
}

float Extract2ndHaarFeature(LPSTU_PATTERN1 pstuPattern, int* pnIntegralImg, int* pnSqrIntegralImg, int nIntegralWidth, float fltAvgCoeff)
{
	int nBR;
	int nBL;
	int nTR;
	int nOffX;
	int nIntegralVal;
	int* pnIntegralImgAddr;
	int* pnSqrIntegralImgAddr;
	float fltThreshold;
	float fltIntegralVal;
	float fltSqrIntegralVal;
	int i, j;
	int index = 0;
	int cx = pstuPattern->ubyCx;
	int cy = pstuPattern->ubyCy * nIntegralWidth;
	int nSum = 0;

	nOffX = pstuPattern->ubyPosY_5 * nIntegralWidth + pstuPattern->ubyPosX_4;
	pnIntegralImgAddr = pnIntegralImg + nOffX;
	pnSqrIntegralImgAddr = pnSqrIntegralImg + nOffX;
	nTR = cx * 4;//TopRight
	nBL = cy * 4;//BottomLeft
	nBR = nTR + nBL;//BottomRight
	nIntegralVal = pnIntegralImgAddr[nBR] - pnIntegralImgAddr[nTR] - pnIntegralImgAddr[nBL] + pnIntegralImgAddr[0];
	fltIntegralVal = (float)nIntegralVal;
	if(fltIntegralVal < 0)
		fltIntegralVal += fltMax;
	nIntegralVal = pstuPattern->ubyCx;
	fltThreshold = (float)(pstuPattern->ubyCy * nIntegralVal * 16);
	
	fltSqrIntegralVal = (float)(pnSqrIntegralImgAddr[nBR] - pnSqrIntegralImgAddr[nTR] - pnSqrIntegralImgAddr[nBL] + pnSqrIntegralImgAddr[0]);
	if(fltSqrIntegralVal < 0)
		fltSqrIntegralVal += fltMax;
	fltSqrIntegralVal *= fltThreshold;
	fltIntegralVal = fltIntegralVal * fltIntegralVal;
	fltSqrIntegralVal -= fltIntegralVal;
	fltThreshold = fltThreshold * fltThreshold * fltAvgCoeff;
	if(fltSqrIntegralVal <= fltThreshold){
		return 0.0;
	}

	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++){
			nSum += pnIntegralImgAddr[j * cx] * pstuPattern->pbyFilter_A[index++];
		}
		pnIntegralImgAddr += cy;
	}
	return nSum / sqrt(fltSqrIntegralVal);
}

int Eval2ndHaarFeature(LPSTU_FILTER1 pstuFilter1, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{
	float fltTmp, fltX, fltY;
	float* pfltStepThreshold;
	int nHaar, nStepCnt;
	int wCntStepPattern;
	unsigned short* pwCntStepPattern;
	int iNo, i, j;
	LPSTU_PATTERN1 pstuFilter;
	
	nStepCnt = pstuFilter1->nStepCnt;
	pfltStepThreshold = pstuFilter1->pfltStepThreshold;
	fltY = pstuResult->y;
	fltX = pstuResult->x;
	pwCntStepPattern = pstuFilter1->pwCntStepPattern;
	pstuFilter = pstuFilter1->pstuPattern;
	for(i = 0; i < nStepCnt; i++){
		wCntStepPattern = pwCntStepPattern[i];
		for(j = 0; j < wCntStepPattern; j++){
			fltTmp = Extract2ndHaarFeature(pstuFilter, pnIntegralImg, pnSqrIntegralImg, g_nIntegralImgWidth, 0.00999999977648258f);
			fltTmp = (fltTmp + pstuFilter->flt_28) * pstuFilter->flt_24;
			if(fltTmp > 0)
				fltTmp += 0.5;
			else
				fltTmp -= 0.5;
			nHaar = (int)fltTmp;
			iNo = (nHaar < pstuFilter->puby_2C[8]) ? 7 : 15;
			if(nHaar <pstuFilter->puby_2C[iNo - 3])
				iNo -= 4;
			if(nHaar <pstuFilter->puby_2C[iNo - 1])
				iNo -= 2;
			if(nHaar <pstuFilter->puby_2C[iNo])
				iNo --;

			iNo = (signed char)pstuFilter->puby_2C[16 + iNo];
			iNo <<= pstuFilter->puby_2C[0];
			fltTmp = iNo * g_fltZoomScale;
			fltX = fltX + fltTmp;
			pstuFilter++;
		}
		fltTmp = fltX - pfltStepThreshold[i];
		if(fltTmp <= 0) break;
		fltY += fltTmp;
	}
	pstuResult->x = fltX;
	pstuResult->y = fltY;
	if(nStepCnt > i){
		pstuResult->nIsPassed --;
		return -1;
	}
	return 0;
}

int Eval3rdHaarFeature(LPSTU_FILTER2 pTrainResult, int* pnIntegralImg, int* pnSqrIntegralImg, LPEVAL_RESULTS pstuResult)
{
	float fltX, fltY;
	int nResult;
	float* pfltCeilThreshold;
	float* pfltFloorThreshold;
	float fltPatternAvgCoeff;
	int nCntStep;
	int nPatternArea;
	int iNo;
	LPSTU_PATTERN2 pstuPattern;
	float flt_result;
	
	nCntStep = pTrainResult->nCntStep;
	pfltCeilThreshold = pTrainResult->pfltCeilThreshold;
	pfltFloorThreshold = pTrainResult->pfltFloorThreshold;
	nPatternArea = pTrainResult->nPatternSize * pTrainResult->nPatternSize;
	fltPatternAvgCoeff = (float)1 / nPatternArea;
	pstuPattern = pTrainResult->pstuPattern;
	fltX = pstuResult->x;
	fltY = pstuResult->y;
	pstuResult->nIsPassed = 0;
	for(int i = 0; i < nCntStep; i ++){
		for(int j = 0 ; j < pTrainResult->pwCntStepPattern[i]; j++){
			flt_result = Extract3thHaarFeature(pstuPattern, pnIntegralImg, pnSqrIntegralImg, g_nIntegralImgWidth, pTrainResult->nPatternSize, pTrainResult->flt_10, fltPatternAvgCoeff);
			flt_result = (flt_result + pstuPattern->flt_10) * pstuPattern->flt_C;
			nResult = (int)(flt_result < 0 ? flt_result - 0.5f : flt_result + 0.5f);
			iNo = (nResult < pstuPattern->puby_14[8]) ? 7 : 15;
			if(nResult < pstuPattern->puby_14[iNo - 3])
				iNo = iNo - 4;
			if(nResult < pstuPattern->puby_14[iNo - 1])
				iNo = iNo - 2;
			if(nResult < pstuPattern->puby_14[iNo])
				iNo = iNo - 1;
			iNo = pstuPattern->pby_24[iNo] << pstuPattern->puby_14[0];
			fltX += iNo * g_fltZoomScale;
			pstuPattern++;
		}
		if(pfltFloorThreshold[i] > fltX){
			pstuResult->nIsPassed = -1;
			break;
		}
		fltY += fltX - pfltFloorThreshold[i];
		if(pfltCeilThreshold[i] < fltX){
			pstuResult->nIsPassed = 1;
			break;
		}
	}
	pstuResult->x = fltX;
	pstuResult->y = fltY;
	return pstuResult->nIsPassed;
}

int ExtractFeatureHaar(int nIntegralWidth, LPSTU_PATTERN0 pstuPattern, int* pnIntegralImg)
{
	int *pnIntegralAddr;
	int result = 0;
	int nFeatureVal = 0, nOff3 = 0, nVal = 0, nCx = 0, nCy = 0, nOff4 = 0;
	
	nCx = pstuPattern->nCx;
	nCy = pstuPattern->nCy * nIntegralWidth;

	pnIntegralAddr = pnIntegralImg + (pstuPattern->nPosY * nIntegralWidth + pstuPattern->nPosX);
	switch(pstuPattern->nType){
	case 0:
		nFeatureVal = pnIntegralAddr[nCx*2];
		nFeatureVal -= pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + pstuPattern->nCy * nIntegralWidth;
		nFeatureVal += pnIntegralAddr[nCx] * 2;
		nFeatureVal -= pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 1:
		nOff3 = nCx * 3;
		nFeatureVal = (pnIntegralAddr[nCx*2] - pnIntegralAddr[nCx]) * 3;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 3;
		nFeatureVal += pnIntegralAddr[nOff3] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 2:
		nOff3 = nCx * 3;
		nOff4 = nCx * 4;
		nFeatureVal = (pnIntegralAddr[nOff3] - pnIntegralAddr[nCx]) * 2;
		nFeatureVal -= pnIntegralAddr[nOff4];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nVal = (pnIntegralAddr[nCx] - pnIntegralAddr[nOff3]) * 2;
		nVal += pnIntegralAddr[nOff4];
		nVal -= pnIntegralAddr[0];
		nFeatureVal += nVal;
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 3:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 2;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[0] - pnIntegralAddr[nCx];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 4:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 3;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 3;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 5:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 2;
		pnIntegralAddr = pnIntegralAddr + nCy * 2;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 2;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 6:
		nOff3 = nCx * 3;
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nOff3];
		pnIntegralAddr += nCx + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 9;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 9;
		pnIntegralAddr += nCy - nCx;
		nFeatureVal += pnIntegralAddr[nOff3] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 7:
		nFeatureVal = pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nVal = pnIntegralAddr[nCx] * 2;
		nVal -= pnIntegralAddr[nCx * 2];
		nVal -= pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		nFeatureVal += nVal * 2;
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 8:
		nFeatureVal = pnIntegralAddr[nCx * 2] * 3;
		nFeatureVal -= pnIntegralAddr[nCx] * 4;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 4;
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 9:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr = pnIntegralAddr + nCy;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 4;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx] * 4;
		nFeatureVal -= pnIntegralAddr[nCx*2] * 3;
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 10:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 4;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx] * 4;
		nFeatureVal += pnIntegralAddr[0] * 3;
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 11:
		nFeatureVal = pnIntegralAddr[nCx] * 4 - pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[0] * 3;
		nVal = pnIntegralAddr[nCy];
		pnIntegralAddr += nCy;
		nVal -= pnIntegralAddr[nCx];
		pnIntegralAddr += nCy;
		nFeatureVal += nVal * 4;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 12:
		nOff3 = nCx * 3;
		nFeatureVal = (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 6;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nOff3] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 13:
		nOff3 = nCx * 3;
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nOff3];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 6;
		nFeatureVal += pnIntegralAddr[nOff3];
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 14:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 15:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx * 2];
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[nCx] - pnIntegralAddr[0]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += (pnIntegralAddr[0] - pnIntegralAddr[nCx]) * 6;
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx * 2] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 16:
		nOff3 = nCx * 3;
		nFeatureVal = (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 2;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nVal = (pnIntegralAddr[nCx] - pnIntegralAddr[nCx * 2]) * 2;
		nVal += pnIntegralAddr[nOff3];
		nVal -= pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nFeatureVal += nVal * 2;
		nFeatureVal += (pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx]) * 2;
		nFeatureVal -= pnIntegralAddr[nOff3];
		nFeatureVal += pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 17:
		nFeatureVal = pnIntegralAddr[nCx * 2] - pnIntegralAddr[nCx] * 2;
		nFeatureVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nVal = pnIntegralAddr[nCx] * 2;
		nVal -= pnIntegralAddr[nCx * 2];
		nVal -= pnIntegralAddr[0];
		nFeatureVal += nVal * 2;
		pnIntegralAddr += nCy;
		nVal = pnIntegralAddr[nCx * 2];
		nVal -= pnIntegralAddr[nCx] * 2;
		nVal += pnIntegralAddr[0];
		pnIntegralAddr += nCy;
		nFeatureVal += nVal * 2;
		nFeatureVal += pnIntegralAddr[nCx] * 2;
		nFeatureVal -= pnIntegralAddr[nCx * 2];
		nFeatureVal -= pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	case 18:
		nFeatureVal = pnIntegralAddr[0] - pnIntegralAddr[nCx];
		pnIntegralAddr += nCy;
		nFeatureVal += pnIntegralAddr[nCx] - pnIntegralAddr[0];
		result = nFeatureVal * pstuPattern->wp;
		break;
	default:
		result = 0;
		break;
	};
	return result;
}

bool IsAvaiableData(float* pflt2, float* pflt1, float pfltThreshold)
{
	float flt1, flt2;
	int i;

	for(i = 0; i < 3; i++){
		flt1 = fabs(pflt1[i] - pflt2[i]);
		flt2 = pflt2[i + 3] + pfltThreshold;
		if(flt2 < flt1)
			return false;
	}
	return true;
}

float Extract3thHaarFeature(LPSTU_PATTERN2 pstuPattern, int* pnIntegralImg, int* pnSqrIntegralImg, int nIntegralImgWidth, int nPatternEdgeLen, float fltAreaAvgCoeff, float fltPatternAvgCoeff)
{
	float fltDiff;
	float fltY = 0;
	float fltX = 0;
	int nIntegralRectBefore, nIntegralRect;
	int* pnSqrIntegralImgAddr;
	float fltIntegralAvg;
	int nIntegralVal;
	float* pfltDataAddr1, *pfltDataAddr2;
	int nOffset, nOffX, nOffXY, nOffY, nStep;
	int *pnIImgAddrLT, *pnIImgAddrRB;
	int i, j;
	float fltSqrIntegralVal;
	float fltIntegralVal;
	float fltPatternArea, fltAreaAvg;
	char byIsEdgeLenEven;

	int iNo;
	float f_result = 0;
	
	nOffset = pstuPattern->byY * nIntegralImgWidth + pstuPattern->byX;
	pnIImgAddrLT = pnIntegralImg + nOffset;
	pnSqrIntegralImgAddr = pnSqrIntegralImg + nOffset;
	nOffX = pstuPattern->byWidth * nPatternEdgeLen;
	nOffY = pstuPattern->byHeight * nIntegralImgWidth * nPatternEdgeLen;
	nOffXY = nOffY + nOffX;
	nIntegralVal = nOffXY;
	nIntegralVal = pnIImgAddrLT[nOffXY];
	nIntegralVal -= pnIImgAddrLT[nOffX];
	nIntegralVal -= pnIImgAddrLT[nOffY];
	nIntegralVal += pnIImgAddrLT[0];
	fltIntegralVal = (float)nIntegralVal;
	if(fltIntegralVal < 0)
		fltIntegralVal += fltMax;
	fltPatternArea = (float)(pstuPattern->byHeight * pstuPattern->byWidth * nPatternEdgeLen * nPatternEdgeLen);
	nIntegralVal = pnSqrIntegralImgAddr[nOffXY];
	nIntegralVal -= pnSqrIntegralImgAddr[nOffX];
	nIntegralVal -= pnSqrIntegralImgAddr[nOffY];
	nIntegralVal += pnSqrIntegralImgAddr[0];
	fltSqrIntegralVal = (float)nIntegralVal;
	fltIntegralAvg = fltIntegralVal * fltPatternAvgCoeff;
	if(fltSqrIntegralVal < 0)
		fltPatternArea += fltMax;
	fltAreaAvg = fltPatternArea * fltPatternArea * fltAreaAvgCoeff;
	fltPatternArea = fltPatternArea * fltSqrIntegralVal - fltIntegralVal * fltIntegralVal;
	if(fltPatternArea <= fltAreaAvg){
		return 0;
	}
	nOffset = pstuPattern->byHeight * nIntegralImgWidth;
	pnIImgAddrRB = pnIImgAddrLT + nOffset;
	nOffXY = nOffset - pstuPattern->byWidth * nPatternEdgeLen;
	//
	byIsEdgeLenEven = (nPatternEdgeLen + 1) % 2;
	nOffset = (pstuPattern->by_8 / 2) * byIsEdgeLenEven;
	nOffX = nOffset - (nPatternEdgeLen / 2) * pstuPattern->by_8;
	nOffset = (pstuPattern->nSiftCount / 2) * byIsEdgeLenEven;
	nOffY = nOffset - (nPatternEdgeLen / 2) * pstuPattern->nSiftCount;
	pfltDataAddr1 = &g_fltAry256_1[pstuPattern->by_A];
	
	nStep = pstuPattern->by_8 & 0xFF;
	for(i = 0; i < nPatternEdgeLen; i ++)
	{
		pfltDataAddr2 = &g_fltAry256_1[pstuPattern->by_A];
		fltSqrIntegralVal = 0;
		fltIntegralVal = 0;
		nOffset = nOffX + nOffY;
		nIntegralRectBefore = *pnIImgAddrLT - *pnIImgAddrRB;
		for(j = 0; j < nPatternEdgeLen; j ++){
			pnIImgAddrLT += pstuPattern->byWidth;
			pnIImgAddrRB += pstuPattern->byWidth;
			nIntegralRect = *pnIImgAddrLT - *pnIImgAddrRB;
			fltDiff = (nIntegralRectBefore - nIntegralRect - fltIntegralAvg) * pfltDataAddr2[0];
			nIntegralRectBefore = nIntegralRect;
			iNo = nOffset & 0xFF;
			fltIntegralVal += fltDiff * g_fltAry256_2[iNo];
			iNo = (nOffset + 64) & 0xFF;
			fltSqrIntegralVal += fltDiff * g_fltAry256_2[iNo];
			nOffset += nStep;
			pfltDataAddr2 += pstuPattern->by_B;
		}
		nOffY += pstuPattern->nSiftCount;
		pnIImgAddrLT += nOffXY;
		pnIImgAddrRB += nOffXY;
		fltX += pfltDataAddr1[0] * fltSqrIntegralVal;
		fltY += pfltDataAddr1[0] * fltIntegralVal;
		pfltDataAddr1 += pstuPattern->by_B;
	}
	f_result = (fltY * fltY + fltX * fltX) / fltPatternArea;
	return f_result;
}

unsigned char* RGB2Gray(unsigned char* pRGBImg, int cx, int cy)
{
	unsigned char* pGrayBuf = (unsigned char *) malloc(cx * cy);
	if (pGrayBuf == NULL) return NULL;
	unsigned char r,g,b;
	int x, y, lVal, nAddr = 0;
	for (y = 0; y < cy; y++) {
		for (x = 0; x < cx; x++) {
			b = pRGBImg[3 * (y * cx + x) + 0];
			g = pRGBImg[3 * (y * cx + x) + 1];
			r = pRGBImg[3 * (y * cx + x) + 2];
			lVal = IMIN((b * 117 + g * 601 + r * 306) >> 10, 255);
			pGrayBuf[nAddr++] = (unsigned char)lVal;
		}
	}

	return pGrayBuf;
}

int DetectFace(unsigned char* pbyImg, int nImgWidth, int nImgHeight, LPNRECT *ppOutRect, double fltScale)
{
	ARR_DETECTED_INFO arrDetectedInfo;
	int* pSortedIndex = NULL;
	int* pCntSameClass = NULL;
	STU_DETECTOR_STATE stuSecondState;
	STU_DETECTOR_STATE stuFirstState;
	int nX1, nY1, nX2, nY2;
	int nScanRect1Right;
	int nScanRect1Bottom;
	int nScanRect2Left;
	int nScanRect2Top;
	int nScanRect2Right;
	int nScanRect2Bottom;
	int nOffset;
	int* pnIntegralImgAddr1, *pnSqrIntegralImgAddr1;
	int* pnIntegralImgAddr2, *pnSqrIntegralImgAddr2;
	bool isShrinked = true;
	LPNRECT pout_Rect, pIterator = NULL;
	LPNRECT pRect = *ppOutRect;
	int nDetectCnt = 0;
	int nFaceCnt;
	int i;

	if (nImgWidth > MAX_IMG_WIDTH || nImgHeight > MAX_IMG_HEIGHT) {

		for (i = 0; i < nDetectCnt; i++) {
			LPSTU_DETECTED_INFO ptemp = arrDetectedInfo.at(i);
			delete ptemp;
		}
		arrDetectedInfo.clear();
		return -1;
	}
	PreProc(pbyImg, nImgWidth, nImgHeight);
	while (isShrinked) {
		if (IsLagerThanScanRect() > 0) {
			nScanRect1Right = g_nShrinkedWidth - 2 - SCAN_WIDTH;
			nScanRect1Bottom = g_nShrinkedHeight - 2 - SCAN_HEIGHT;
			for (nY1 = 2; nY1 <= nScanRect1Bottom; nY1 += 5) {
				nScanRect2Top = nY1 - 2;
				ChannelDetector(nScanRect2Top, SCAN_HEIGHT + 4);
				nOffset = (nY1 - g_nChannelCnt) * g_nIntegralImgWidth + 2;
				pnIntegralImgAddr1 = g_pnIntegralImg + nOffset;
				pnSqrIntegralImgAddr1 = g_pnSqrIntegralImg + nOffset;
				for (nX1 = 2; nX1 <= nScanRect1Right; nX1 += 5) {
					nScanRect2Left = nX1 - 2;
					if (EvalFirst(&stuFirstState, pnIntegralImgAddr1, pnSqrIntegralImgAddr1) > 0) {
						Choose2thStepData(&stuSecondState, &stuFirstState);

						nScanRect2Bottom = nY1 + 2;
						nScanRect2Right = nX1 + 2;
						for (nY2 = nScanRect2Top; nY2 <= nScanRect2Bottom; nY2++) {
							nOffset = (nY2 - g_nChannelCnt) * g_nIntegralImgWidth + nScanRect2Left;
							pnIntegralImgAddr2 = g_pnIntegralImg + nOffset;
							pnSqrIntegralImgAddr2 = g_pnSqrIntegralImg + nOffset;
							for (nX2 = nScanRect2Left; nX2 <= nScanRect2Right; nX2++) {
								EvalSecond(&stuSecondState, pnIntegralImgAddr2, pnSqrIntegralImgAddr2, nX2, nY2, &arrDetectedInfo);

								pnIntegralImgAddr2++;
								pnSqrIntegralImgAddr2++;
							}
						}
					}
					pnIntegralImgAddr1 += 5;
					pnSqrIntegralImgAddr1 += 5;
				}
			}
		}
		isShrinked = CreateWorkImage();
	}
	nDetectCnt = (int)arrDetectedInfo.size();
	if (nDetectCnt <= 0)
		return -1;
	nFaceCnt = ClassifyDetectedRects(arrDetectedInfo, 0.3f, SCAN_WIDTH, SCAN_HEIGHT, &pSortedIndex, &pCntSameClass);

	nOffset = 0;
	for (i = 0; i < nFaceCnt; i++) {
		if (pCntSameClass[i] >= MAX_FACE_PASS_CNT) {
			pout_Rect = (LPNRECT)calloc(1, sizeof(NRECT));
			WeightedAverage(pout_Rect, arrDetectedInfo, pSortedIndex, nOffset, pCntSameClass[i]);
			pout_Rect->l = (int)(max(0, pout_Rect->l) / fltScale);
			pout_Rect->t = (int)(max(0, pout_Rect->t) / fltScale);
			pout_Rect->r = (int)(min(nImgWidth - 1, pout_Rect->r) / fltScale);
			pout_Rect->b = (int)(min(nImgHeight - 1, pout_Rect->b) / fltScale);
			pout_Rect->pNext = NULL;
			if (pIterator == NULL) {
				*ppOutRect = pout_Rect;
				pIterator = pout_Rect;
			}
			else {
				pIterator->pNext = pout_Rect;
				pIterator = pout_Rect;
			}
		}
		nOffset += pCntSameClass[i];
	}
	if (pSortedIndex)
		delete pSortedIndex;
	if (pCntSameClass)
		delete pCntSameClass;
	nDetectCnt = (int)arrDetectedInfo.size();

	for (i = 0; i < nDetectCnt; i++) {
		LPSTU_DETECTED_INFO ptemp = arrDetectedInfo.at(i);
		delete ptemp;
	}
	arrDetectedInfo.clear();

	return 0;
}

void FreeRectList(LPNRECT* pRectRoot) {
	LPNRECT pRect, pTmpRect;
	pRect = *pRectRoot;
	while (pRect) {
		pTmpRect = pRect->pNext;
		free(pRect);
		pRect = pTmpRect;
	}
	*pRectRoot = NULL;
}
int DetectFaces(unsigned char* pRGBImg, int nImgWidth, int nImgHeight, int nBitCount, LPNRECT *ppOutRect)
{
	int dstW, dstH;
	double fltScale = 1.0;
	unsigned char* GrayImg = NULL;
	unsigned char* pStretchImg = NULL;
	if (STANDARD_CX < nImgWidth ||STANDARD_CX < nImgHeight){
		return -1;
	}else{
		dstW = nImgWidth;
		dstH = nImgHeight;
		if (nBitCount == 24){
			GrayImg = RGB2Gray(pRGBImg, dstW, dstH);
		}else if (nBitCount == 8){
			GrayImg = (unsigned char*)malloc(dstW * dstH);
			memcpy(GrayImg, pRGBImg, dstW*dstH);
		}
	}
	if (GrayImg == NULL) return -1;
	if ( DetectFace((unsigned char*)GrayImg, dstW, dstH, ppOutRect, fltScale) == -1){
		if (GrayImg != NULL) free(GrayImg);
		FreeRectList(ppOutRect);
		return -1;
	}
	if (GrayImg != NULL) free(GrayImg);
	return 0;
}
void* read_file(char* path, int nSize)
{
	unsigned char* buf = (unsigned char*)malloc(nSize);
	if (buf == NULL) {
		return NULL;
	}

	FILE* fp = fopen(path, "rb");
	if (fp == NULL) {
		return NULL;
	}

	fread(buf, 1, nSize, fp);

	fclose(fp);

	return buf;
}
int LoadFirstStepData(char *sFilePath)
{
	char strPathDat[MAX_PATH];
	//0_0
	g_stuFilter0[0].fltFilterCoeff = 0.001275509f; g_stuFilter0[0].fltFilterLimit = 0.004999999f;
	g_stuFilter0[0].nStepCnt = 10;
	sprintf(strPathDat, "%s/1_Step_1_1.dat", sFilePath);
	g_stuFilter0[0].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 12800); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter0[0].pstuPattern == NULL) return -1;
	//g_stuFilter0[0].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_0_0), _T("dat")));
	g_stuFilter0[0].pwCntStepPattern = g_pwCntStepPattern0_0;
	g_stuFilter0[0].pfltStepThreshold = g_pfltStepThreshold0_0_0;
	//0_1
	g_stuFilter0[1].fltFilterCoeff = 0.001275509f; g_stuFilter0[1].fltFilterLimit = 0.004999999f;
	g_stuFilter0[1].nStepCnt = 0xA;
	sprintf(strPathDat, "%s/1_Step_1_2.dat", sFilePath);
	g_stuFilter0[1].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 12800); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter0[1].pstuPattern == NULL) return -1;
	//g_stuFilter0[1].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_0_1), _T("dat")));
	g_stuFilter0[1].pwCntStepPattern = g_pwCntStepPattern0_0;
	g_stuFilter0[1].pfltStepThreshold = g_pfltStepThreshold0_0_12;
	//0_2
	g_stuFilter0[2].fltFilterCoeff = 0.001275509f; g_stuFilter0[1].fltFilterLimit = 0.004999999f;
	g_stuFilter0[2].nStepCnt = 0xA;
	sprintf(strPathDat, "%s/1_Step_1_3.dat", sFilePath);
	g_stuFilter0[2].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 12800); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter0[2].pstuPattern == NULL) return -1;
	//g_stuFilter0[2].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_0_2), _T("dat")));
	g_stuFilter0[2].pwCntStepPattern = g_pwCntStepPattern0_0;
	g_stuFilter0[2].pfltStepThreshold = g_pfltStepThreshold0_0_12;
	//1_0
	g_stuFilter1[0].nStepCnt = 3;
	g_stuFilter1[0].pfltStepThreshold = g_pfltStepThreshold0_1_0;
	g_stuFilter1[0].pwCntStepPattern = g_pwCntStepPattern0_1;
	sprintf(strPathDat, "%s/1_Step_2_1.dat", sFilePath);
	g_stuFilter1[0].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 14400); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter1[0].pstuPattern == NULL) return -1;
	//g_stuFilter1[0].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_1_0), _T("dat")));
	//1_1
	g_stuFilter1[1].nStepCnt = 3;
	g_stuFilter1[1].pfltStepThreshold = g_pfltStepThreshold0_1_12;
	g_stuFilter1[1].pwCntStepPattern = g_pwCntStepPattern0_1;
	sprintf(strPathDat, "%s/1_Step_2_2.dat", sFilePath);
	g_stuFilter1[1].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 14400); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter1[1].pstuPattern == NULL) return -1;
	//g_stuFilter1[1].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_1_1), _T("dat")));
	//1_2
	g_stuFilter1[2].nStepCnt = 3;
	g_stuFilter1[2].pfltStepThreshold = g_pfltStepThreshold0_1_12;
	g_stuFilter1[2].pwCntStepPattern = g_pwCntStepPattern0_1;
	sprintf(strPathDat, "%s/1_Step_2_3.dat", sFilePath);
	g_stuFilter1[2].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 14400); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter1[2].pstuPattern == NULL) return -1;
	//g_stuFilter1[2].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_1_2), _T("dat")));
	//2_0
	g_stuFilter2[0].nPatternSize = 6;
	g_stuFilter2[0].nCntStep = 2;
	g_stuFilter2[0].pfltFloorThreshold = g_pfltFloorThreshold0_2_0;
	g_stuFilter2[0].pfltCeilThreshold = g_pfltCeilThreshold0_2;
	g_stuFilter2[0].pwCntStepPattern = g_pwCntStepPattern0_2;
	sprintf(strPathDat, "%s/1_Step_3_1.dat", sFilePath);
	g_stuFilter2[0].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 5200); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter2[0].pstuPattern == NULL) return -1;
	//g_stuFilter2[0].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_2_0), _T("dat")));
	//2_1
	g_stuFilter2[1].nPatternSize = 6;
	g_stuFilter2[1].nCntStep = 2;
	g_stuFilter2[1].pfltFloorThreshold = g_pfltFloorThreshold0_2_12;
	g_stuFilter2[1].pfltCeilThreshold = g_pfltCeilThreshold0_2;
	g_stuFilter2[1].pwCntStepPattern = g_pwCntStepPattern0_2;
	sprintf(strPathDat, "%s/1_Step_3_2.dat", sFilePath);
	g_stuFilter2[1].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 5200); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter2[1].pstuPattern == NULL) return -1;
	//g_stuFilter2[1].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_2_1), _T("dat")));
	//2_2
	g_stuFilter2[2].nPatternSize = 6;
	g_stuFilter2[2].nCntStep = 2;
	g_stuFilter2[2].pfltFloorThreshold = g_pfltFloorThreshold0_2_12;
	g_stuFilter2[2].pfltCeilThreshold = g_pfltCeilThreshold0_2;
	g_stuFilter2[2].pwCntStepPattern = g_pwCntStepPattern0_2;
	sprintf(strPathDat, "%s/1_Step_3_3.dat", sFilePath);
	g_stuFilter2[2].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 5200); //data\rcpr_38_pn_4_flt_4.mem
	if (g_stuFilter2[2].pstuPattern == NULL) return -1;
	//g_stuFilter2[2].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP0_2_2), _T("dat")));
	return 0;
}

int LoadSecondStepData(char *sFilePath)
{
	char strPathDat[MAX_PATH];
	//0_0
	g_IstuFilter0[0].fltFilterCoeff = 0.00127550994511694f; g_IstuFilter0[0].fltFilterLimit = 0.00499999988824129f;
	g_IstuFilter0[0].nStepCnt = 16;
	sprintf(strPathDat, "%s/2_Step_1_1.dat", sFilePath);
	g_IstuFilter0[0].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 25568); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter0[0].pstuPattern == NULL) return -1;
	//g_IstuFilter0[0].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_0_0), _T("dat")));
	g_IstuFilter0[0].pwCntStepPattern = g_pwCntStepPattern1_0;
	g_IstuFilter0[0].pfltStepThreshold = g_pfltStepThreshold1_0_0;
	//0_1
	g_IstuFilter0[1].fltFilterCoeff = 0.00127550994511694f; g_IstuFilter0[1].fltFilterLimit = 0.00499999988824129f;
	g_IstuFilter0[1].nStepCnt = 16;
	sprintf(strPathDat, "%s/2_Step_1_2.dat", sFilePath);
	g_IstuFilter0[1].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 25568); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter0[1].pstuPattern == NULL) return -1;
	//g_IstuFilter0[1].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_0_1), _T("dat")));
	g_IstuFilter0[1].pwCntStepPattern = g_pwCntStepPattern1_0;
	g_IstuFilter0[1].pfltStepThreshold = g_pfltStepThreshold1_0_12;
	//0_2
	g_IstuFilter0[2].fltFilterCoeff = 0.00127550994511694f; g_IstuFilter0[1].fltFilterLimit = 0.00499999988824129f;
	g_IstuFilter0[2].nStepCnt = 16;
	sprintf(strPathDat, "%s/2_Step_1_3.dat", sFilePath);
	g_IstuFilter0[2].pstuPattern = (LPSTU_PATTERN0)read_file(strPathDat, 25568); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter0[2].pstuPattern == NULL) return -1;
	//g_IstuFilter0[2].pstuPattern = (LPSTU_PATTERN0)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_0_2), _T("dat")));
	g_IstuFilter0[2].pwCntStepPattern = g_pwCntStepPattern1_0;
	g_IstuFilter0[2].pfltStepThreshold = g_pfltStepThreshold1_0_12;
	//1_0
	g_IstuFilter1[0].nStepCnt = 6;
	g_IstuFilter1[0].pfltStepThreshold = g_pfltStepThreshold1_1_0;
	g_IstuFilter1[0].pwCntStepPattern = g_pwCntStepPattern1_1;
	sprintf(strPathDat, "%s/2_Step_2_1.dat", sFilePath);
	g_IstuFilter1[0].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 21600); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter1[0].pstuPattern == NULL) return -1;
	//g_IstuFilter1[0].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_1_0), _T("dat")));
	//1_1
	g_IstuFilter1[1].nStepCnt = 6;
	g_IstuFilter1[1].pfltStepThreshold = g_pfltStepThreshold1_1_12;
	g_IstuFilter1[1].pwCntStepPattern = g_pwCntStepPattern1_1;
	sprintf(strPathDat, "%s/2_Step_2_2.dat", sFilePath);
	g_IstuFilter1[1].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 43200); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter1[1].pstuPattern == NULL) return -1;
	//g_IstuFilter1[1].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_1_1), _T("dat")));
	//1_2
	g_IstuFilter1[2].nStepCnt = 6;
	g_IstuFilter1[2].pfltStepThreshold = g_pfltStepThreshold1_1_12;
	g_IstuFilter1[2].pwCntStepPattern = g_pwCntStepPattern1_1;
	sprintf(strPathDat, "%s/2_Step_2_3.dat", sFilePath);
	g_IstuFilter1[2].pstuPattern = (LPSTU_PATTERN1)read_file(strPathDat, 64800); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter1[2].pstuPattern == NULL) return -1;
	//g_IstuFilter1[2].pstuPattern = (LPSTU_PATTERN1)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_1_2), _T("dat")));
	//2_0
	g_IstuFilter2[0].nPatternSize = 6;
	g_IstuFilter2[0].nCntStep = 4;
	g_IstuFilter2[0].pfltFloorThreshold = g_pfltFloorThreshold1_2_0;
	g_IstuFilter2[0].pfltCeilThreshold = g_pfltCeilThreshold1_2_0;
	g_IstuFilter2[0].pwCntStepPattern = g_pwCntStepPattern1_2;
	sprintf(strPathDat, "%s/2_Step_3_1.dat", sFilePath);
	g_IstuFilter2[0].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 13052); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter2[0].pstuPattern == NULL) return -1;
	//g_IstuFilter2[0].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_2_0), _T("dat")));
	g_IstuFilter2[0].pflt_B0 = g_pflt_2step_3_0;
	//2_1
	g_IstuFilter2[1].flt_10 = 0.00999999977648258f;
	g_IstuFilter2[1].nPatternSize = 6;
	g_IstuFilter2[1].nCntStep = 4;
	g_IstuFilter2[1].pfltFloorThreshold = g_pfltFloorThreshold1_2_12;
	g_IstuFilter2[1].pfltCeilThreshold = g_pfltCeilThreshold1_2_12;
	g_IstuFilter2[1].pwCntStepPattern = g_pwCntStepPattern1_2;
	sprintf(strPathDat, "%s/2_Step_3_2.dat", sFilePath);
	g_IstuFilter2[1].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 13052); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter2[1].pstuPattern == NULL) return -1;
	//g_IstuFilter2[1].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_2_1), _T("dat")));
	g_IstuFilter2[1].pflt_B0 = g_pflt_2step_3_1;
	//2_2
	g_IstuFilter2[2].flt_10 = 0.00999999977648258f;
	g_IstuFilter2[2].nPatternSize = 6;
	g_IstuFilter2[2].nCntStep = 4;
	g_IstuFilter2[2].pfltFloorThreshold = g_pfltFloorThreshold1_2_12;
	g_IstuFilter2[2].pfltCeilThreshold = g_pfltCeilThreshold1_2_12;
	g_IstuFilter2[2].pwCntStepPattern = g_pwCntStepPattern1_2;
	sprintf(strPathDat, "%s/2_Step_3_3.dat", sFilePath);
	g_IstuFilter2[2].pstuPattern = (LPSTU_PATTERN2)read_file(strPathDat, 13052); //data\rcpr_38_pn_4_flt_4.mem
	if (g_IstuFilter2[2].pstuPattern == NULL) return -1;
	//g_IstuFilter2[2].pstuPattern = (LPSTU_PATTERN2)LoadResource(hDll, FindResource(hDll, MAKEINTRESOURCE(IDR_STEP1_2_2), _T("dat")));
	g_IstuFilter2[2].pflt_B0 = g_pflt_2step_3_2;
	return 0;
}

int InitFaceDetector(char *sFilePath)
{
	int result = 0;
	result = LoadFirstStepData(sFilePath);
	if (result < 0) return result;
	result = LoadSecondStepData(sFilePath);
	if (result < 0) return result;
	return 0;
}

void ReleaseFaceDetector()
{
	SafeMemFree(g_stuFilter0[0].pstuPattern);
	SafeMemFree(g_stuFilter0[1].pstuPattern);
	SafeMemFree(g_stuFilter0[2].pstuPattern);
	SafeMemFree(g_stuFilter1[0].pstuPattern);
	SafeMemFree(g_stuFilter1[1].pstuPattern);
	SafeMemFree(g_stuFilter1[2].pstuPattern);
	SafeMemFree(g_stuFilter2[0].pstuPattern);
	SafeMemFree(g_stuFilter2[1].pstuPattern);
	SafeMemFree(g_stuFilter2[2].pstuPattern);

	SafeMemFree(g_IstuFilter0[0].pstuPattern);
	SafeMemFree(g_IstuFilter0[1].pstuPattern);
	SafeMemFree(g_IstuFilter0[2].pstuPattern);
	SafeMemFree(g_IstuFilter1[0].pstuPattern);
	SafeMemFree(g_IstuFilter1[1].pstuPattern);
	SafeMemFree(g_IstuFilter1[2].pstuPattern);
	SafeMemFree(g_IstuFilter2[0].pstuPattern);
	SafeMemFree(g_IstuFilter2[1].pstuPattern);
	SafeMemFree(g_IstuFilter2[2].pstuPattern);
}

unsigned char Interpolation(unsigned char* Im, int cx, int cy, int x1024, int y1024)
{
	unsigned char pVal;
	unsigned long c = 0;
	int xq, xr, yq, yr, m, n;

	xq = x1024 >> 10;
	xr = x1024 - (xq << 10);
	yq = y1024 >> 10;
	n = yq * cx + xq;
	yr = y1024 - (yq << 10);

	if (xq < 0 || xq >= cx)	c = 0x10;
	if (yq < 0 || yq >= cy)	c |= 0x20;

	if (c == 0 && xr >= 0 && yr >= 0) {
		if (xq + 1 < cx) {
			if (yq + 1 < cy) {
				m = Im[n + 1] * (1024 - yr)*xr + Im[n + cx + 1] * yr*xr + Im[n + cx] * (1024 - xr)*yr + Im[n] * (1024 - xr)*(1024 - yr);
				m = m < 0 ? m + 1048575 : m; //1048575 = (1024*1024-1)
				pVal = m >> 20; return pVal;
			}
		}
		pVal = Im[n]; return pVal;
	}
	return 0;
}

void ImageStretch(unsigned char* imgIn, int cxIn, int cyIn, unsigned char* imgOut, int cxOut, int cyOut)
{
	int x, y, adr, yy;
	int x_ratio = (cxIn << 10) / cxOut;
	int y_ratio = (cyIn << 10) / cyOut;

	for (y = 0; y < cyOut; y++) {
		adr = y * cxOut;
		yy = y * y_ratio;
		for (x = 0; x < cxOut; x++) {
			imgOut[adr + x] = Interpolation(imgIn, cxIn, cyIn, x*x_ratio, yy);
		}
	}
}

void RGB2Gray(unsigned char* lpRGBBuff, unsigned char* lpGrayBuff, int nWidth, int nHeight)
{
#define descale(x, n)   (((x) + (1 << ((n)-1))) >> (n))
#define fix(x, n)		(int)((x)*(1 << (n)) + 0.5)

#define Shift			14
#define Gr				fix(0.299f, Shift) 
#define Gg				fix(0.114f, Shift)
#define Gb				((1 << Shift) - Gr - Gg) //0.587f

	unsigned char r, g, b;
	int x, y, t, adr = 0;

	for (y = 0; y < nHeight; y++) {
		for (x = 0; x < nWidth; x++) {
			r = lpRGBBuff[(y*nWidth + x) * 3 + 2];
			g = lpRGBBuff[(y*nWidth + x) * 3 + 1];
			b = lpRGBBuff[(y*nWidth + x) * 3 + 0];
			t = (int)r*Gr + (int)g*Gb + (int)b*Gg;
			t = descale(t, Shift);
			lpGrayBuff[adr++] = (unsigned char)t;
		}
	}
}

void ImageStretchEx(unsigned char* imgIn, int cxIn, int cyIn, CF_Rect *rt, unsigned char* imgOut, int cxOut, int cyOut)
{
	if (rt->width == cxOut && rt->height == cyOut)
	{
		for (int y = 0; y < cyOut; y++)
		{
			memcpy(&imgOut[y*cxOut], &imgIn[(rt->y + y)*cxIn + rt->x], sizeof(unsigned char)*cxOut);
		}
		return;
	}

	int		nCountX, nCountY, x1024, y1024;
	int		xq, yq, xr, yr, nVal;
	int		limX = rt->x + rt->width - 1, limY = rt->y + rt->height - 1;
	int x_ratio = (rt->width << 10) / cxOut;
	int y_ratio = (rt->height << 10) / cyOut;

	unsigned char	*ptrOut = imgOut, *ptrIn = NULL;

	y1024 = (rt->y << 10);
	nCountY = cyOut;
	while (nCountY--)
	{
		x1024 = (rt->x << 10);
		nCountX = cxOut;
		while (nCountX--)
		{
			xq = x1024 >> 10;			yq = y1024 >> 10;
			xr = x1024 - (xq << 10);	yr = y1024 - (yq << 10);
			ptrIn = &imgIn[yq * cxIn + xq];

			if (xq < limX && yq < limY)
			{
				nVal = ptrIn[1] * (1024 - yr)*xr + ptrIn[cxIn + 1] * yr*xr + ptrIn[cxIn] * (1024 - xr)*yr + ptrIn[0] * (1024 - xr)*(1024 - yr);
				(*ptrOut) = (nVal < 0) ? (nVal + 1048575) >> 20 : nVal >> 20; //1048575 = (1024*1024-1)
			}
			else
				(*ptrOut) = (*ptrIn);

			ptrOut++;
			x1024 += x_ratio;
		}
		y1024 += y_ratio;
	}
}

int FaceAPI_DetectFaceForImage(unsigned char* i_pImage, int i_nWidth, int i_nHeight, int i_nBitCnt, SFaceInfo* o_pFaceInfo)
{
	LPNRECT pRecT = NULL, piRect;
	int i = 0, nMaxWidth = 0, n_half_wid, n_half_hei;
	float ratio = 1.0f;
	LPNRECT ppOutRect = NULL;
	if (min(i_nWidth, i_nHeight) < NORMSIZE) {
		ratio = 1.0f;
	}
	o_pFaceInfo->nMaxFaceIdx = 0;
	if (ratio < 1) {
		n_half_wid = (int)(i_nWidth  * ratio);
		n_half_hei = (int)(i_nHeight * ratio);;
		unsigned char *TmpImage = (unsigned char *)malloc(n_half_wid * n_half_hei * sizeof(unsigned char));
		ImageStretch(i_pImage, i_nWidth, i_nHeight, TmpImage, n_half_wid, n_half_hei);
		DetectFaces(TmpImage, n_half_wid, n_half_hei, 8, &ppOutRect);
		SafeMemFree(TmpImage);
	}
	else {
		DetectFaces(i_pImage, i_nWidth, i_nHeight, 8, &ppOutRect);
	}
	int nMaxArea = 0;
	for (piRect = ppOutRect; piRect != NULL; piRect = piRect->pNext, i++) {
		piRect->r = (int)(piRect->r / ratio);
		piRect->l = (int)(piRect->l / ratio);
		piRect->t = (int)(piRect->t / ratio);
		piRect->b = (int)(piRect->b / ratio);

		o_pFaceInfo->pFace[i].x = piRect->l;
		o_pFaceInfo->pFace[i].y = piRect->t;
		o_pFaceInfo->pFace[i].width = piRect->r - piRect->l;
		o_pFaceInfo->pFace[i].height = piRect->b - piRect->t;
		o_pFaceInfo->nFaces++;
	}
	FreeRectList(&ppOutRect);

	for (i = 0; i < o_pFaceInfo->nFaces; i++) {
		if (nMaxArea < o_pFaceInfo->pFace[i].width * o_pFaceInfo->pFace[i].height) {
			nMaxArea = o_pFaceInfo->pFace[i].width * o_pFaceInfo->pFace[i].height;
			o_pFaceInfo->nMaxFaceIdx = i;
		}
	}
	if (o_pFaceInfo->nFaces > 50) return -1;
	if (o_pFaceInfo->nFaces < 1) return -1;
	return 0;
}

int FaceDetectFunction(unsigned char* i_pImage, int cx, int cy, int bc, SFaceInfo *o_pFaceInfo)
{
	int i = 0, max_width = 0, faceindex = 0;
	unsigned char* pGrayImage = NULL;

	if (bc != 8 && bc != 24 && bc != 32)
		return -1;

	memset(o_pFaceInfo, 0, sizeof(SFaceInfo));

	if (bc == 24) {
		pGrayImage = (unsigned char*)malloc(cx*cy * sizeof(unsigned char));
		RGB2Gray(i_pImage, pGrayImage, cx, cy);
	}
	else if (bc == 8) {
		pGrayImage = (unsigned char*)malloc(cx*cy * sizeof(unsigned char));
		memcpy(pGrayImage, i_pImage, cx * cy * sizeof(unsigned char));
	}

	if (pGrayImage == 0) return -1;
	float ratio = 1;
	if (cx > NORMSIZE || cy > NORMSIZE) {
		ratio = cx > cy ? NORMSIZE / (float)cx : NORMSIZE / (float)cy;
		CF_Rect rect;
		rect.x = 0, rect.y = 0, rect.width = cx, rect.height = cy;
		int realW = (int)(cx * ratio);
		int realH = (int)(cy * ratio);
		unsigned char *pStretchImg = (unsigned char*)malloc(realW * realH);
		ImageStretchEx(pGrayImage, cx, cy, &rect, pStretchImg, realW, realH);
		SafeMemFree(pGrayImage);
		pGrayImage = pStretchImg;
		cx = realW;
		cy = realH;
	}

	if (FaceAPI_DetectFaceForImage(pGrayImage, cx, cy, 8, o_pFaceInfo) != 0) {
		SafeMemFree(pGrayImage);
		return -1;
	}
	for (int i = 0; i < o_pFaceInfo->nFaces; i++) {
		o_pFaceInfo->pFace[i].x = (int)(o_pFaceInfo->pFace[i].x / ratio);
		o_pFaceInfo->pFace[i].y = (int)(o_pFaceInfo->pFace[i].y / ratio);
		o_pFaceInfo->pFace[i].width = (int)(o_pFaceInfo->pFace[i].width / ratio);
		o_pFaceInfo->pFace[i].height = (int)(o_pFaceInfo->pFace[i].height / ratio);
	}
	return 0;
}
