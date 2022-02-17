// DVTGKLDCamSDK.h : main header file for the DVTGKLDCamSDK DLL
//

#pragma once

#ifndef _DVTGKLDCAMSDK_H_
#define _DVTGKLDCAMSDK_H_

#define DVTGKLDCAMSDK_API __declspec(dllexport)
#define STDCALL _stdcall 

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/



	//-------------------------------------------------------------------------------------------------------
	//Video format type definitions
	typedef enum
	{
		LD_FMT_UNKOWN,
		LD_FMT_YUY2,
		LD_FMT_MJPG,
		LD_FMT_YUYV,
		LD_FMT_UYVY,
		LD_FMT_I420,
		LD_FMT_RGB24,
		LD_FMT_RGB32
	}LD_FMT;
	//========================================================================================================

	//-----------------------------------------------------------------------------
	//Detecting text
	typedef enum
	{
		TEXT_SUCCESS,       //detection succeed 
		TEXT_FAIL_TIMEOUT,  //detection failed, timeout.
		TEXT_ONGOING,		//detection ongoing
		ITEM_CNT
	}TEXT_ITEM;

	//--------------------------------------------------------------------------------------------------
	//Image Type definitions
	typedef enum
	{
		IMG_RGB24 = 0,            //RGB24 image
		IMG_JPG,				  //JPG image
		IMG_RGB24_BASE64,         //RGB24 image with base64 encoded
		IMG_JPG_BASE64			  //JPG image with base64 encoded
	}LD_IMG_TYPE;

	//---------------------------------------------------------------------------------------------------------
	//API return definitions

#define LD_RET_OK						0
#define LD_RET_NOCAM					-1
#define LD_RET_NO_VISCAM				-2
#define LD_RET_NO_NIRCAM				-3
#define LD_RET_ERROR_SET_FORMAT			-4
#define LD_RET_ERROR_START_CAM			-5
#define LD_RET_OPEND_ALREADY			-6
#define LD_RET_ERROR_OPEN				-8
#define LD_RET_NO_MEMORY				-9
#define LD_RET_INVALID_PARAMETER		-10
#define LD_RET_VIDEO_NOT_STARTED		-11
#define LD_RET_DETECT_STARTED_ALREADY	-12
#define LD_RET_TIMEOUT					-13
#define LD_RET_NO_DATA					-14
#define LD_RET_BUFF_TOO_SMALL			-15
#define LD_RET_ERROR					-16
#define LD_RET_ERROR_START_DETECT		-17
#define LD_RET_ERROR_ENGINE				-18
#define LD_RET_ERROR_CONFIG_FILE		-19
#define LD_RET_ERROR_NOT_INIT			-20
#define LD_RET_ERROR_NOT_SUPPORT		-21
	//============================================================================================================





	//---------------------------------------------------------------------------------------------------------------
	//Event ID and event callback definition
#define EVENT_ONGOING	     0        // detection ongoing
#define EVENT_OK			 1        // all OK
#define EVENT_TIMEOUT		-1		  // detection timeout with no qualified picture.
#define EVENT_CALLCED		-2		  // user canceled	
#define EVENT_ERROR			-3        // operation error
#define EVENT_VIS_CAM_LOST	-4		  // visual camera lost
#define EVENT_NIR_CAM_LOST	-5		  // nir camera lost

	// 活体检测捕获时状态iFrameStatus的值
#define LD_CAM_STATUS_CAPTURED				0							// 捕获到质量合格的照片
#define LD_CAM_STATUS_NO_FACE				1							// 没有检测到人脸
#define LD_CAM_STATUS_FACE_BEYOND_MASK		2							// 人脸超出蒙层
#define LD_CAM_STATUS_ANGLE				    3						    // 人脸角度不对、属于低头， 抬头， 左右转情况
#define LD_CAM_STATUS_TOO_BIG				4							// 人脸太大
#define LD_CAM_STATUS_TOO_SMALL			    5							// 人脸太小
#define LD_CAM_STATUS_NEAR_BOUNDARY			6							// 人脸靠近边界
#define LD_CAM_STATUS_FOREHEAD_COVERED		7						    // 额头被遮挡
#define LD_CAM_STATUS_EYE_COVERED			8							// 眼睛被遮挡
#define LD_CAM_STATUS_MOUTH_COVERED			9							// 嘴巴被遮挡
#define LD_CAM_STATUS_FACE_MOVED			10							// 人脸质量不合格
#define LD_CAM_LIVENESS_FAILURE			    11							// 非活体
#define LD_CAM_STATUS_MULTI_FACE			12							// 检测到多人脸


	typedef int(WINAPI *LDEventCallBackFun)(int event_id, PVOID context, int iFrameStatus);
	/*
	event_id:  refer to above definitions
	context:   user context  set in DVTGKLDCam_SetEventCallback,user can use this in the callback.
	*/

	//============================================================================================================


	//----------------------------------------------------------------------------------------------
	//Video frame arrival callback function definition, image format is RGB24 (actually BGR layout with image upside-down
	typedef int(WINAPI *LDFrameCallBackFun)(PVOID context, PUCHAR pFrameData, int nFrameLen);
	/*
	context:     context  set in DVTGKLDCam_SetFrameCallback, user can use this in the callback.
	pFrameData:  frame data buffer
	nFrameLen :  frame length

	*/
	//======================================================================================================================

	/*
	Function:   DVTGKLDCam_Init();
	Description:  Init camera. First Api to call to initial sdk environment, Call one time only
	Parameters:

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_Init();
	//=======================================================================================================================


	/*
	Function:   DVTGKLDCam_UnInit();
	Description:  UnInit camera.   Last Api to call to release sdk environment.
	Parameters:

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_UnInit();
	//================================================================================


	/*
	Function:   DVTGKLDCam_Open();
	Description:  Open the live-detect camera.
	Parameters:
	phCam -- receive the handle of live-detect camera
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_Open(HANDLE *phCam);
	//================================================================================


	/*
	Function:   DVTGKLDCam_Close();
	Description:  Close the live-detect camera.
	Parameters:
	hCam -- Handle of the opened camera.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_Close(HANDLE hCam);
	//====================================================================================================================


	/*
	Function:   DVTGKLDCam_SetDetectText();
	Description:  Set text for detecting notify information . must call this before DVTGKLDCam_StartVideo
	if not set,  system will use the embedded default.
	Parameters:
	hCam -- Handle of the opened camera.
	item -- item to set, refer to TEXT_ITEM
	text_height -- height of text, 35 is a moderate size.
	NOTE, all text share the same height, so the value of the last call of this function is valid.
	text  -- text content.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_SetDetectText(HANDLE hCam, TEXT_ITEM item, int text_height, char *text);
	//====================================================================================================================



	/*
	Function:   DVTGKLDCam_SetCamFormat();
	Description:  Set camera format, NOTE: before setting format, video must be stopped.
	Parameters:
	hCam -- Handle of the opened camera.
	nCamIndex -- Camera index, 0: visual camera, 1: near IR camera.
	fmt  -- format to set, must in the list camera supported.
	nWidth -- Video width
	nHeight -- Video height

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_SetCamFormat(HANDLE hCam, int nCamIndex, LD_FMT fmt, int nWidth, int nHeight);
	//====================================================================================================================


	/*
	Function:   DVTGKLDCam_SetEventCallback();
	Description:  Set callback functions for event incoming . must call this before DVTGKLDCam_StartVideo
	Parameters:
	hCam -- Handle of the opened camera.
	pCallback  -- callback function.
	context -- user context used in the callback function

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_SetEventCallback(HANDLE hCam, LDEventCallBackFun pCallback, PVOID context);
	//====================================================================================================================

	/*
	Function:   DVTGKLDCam_SetFrameCallback();
	Description:  Set callback functions for frame arrival . must call this before DVTGKLDCam_StartVideo
	Parameters:
	hCam -- Handle of the opened camera.
	nCamIndex -- 0: visual camera, 1: near ir camera
	pCallback  -- callback function.
	context -- user context used in the callback function

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_SetFrameCallback(HANDLE hCam, int nCamIndex, LDFrameCallBackFun pCallback, PVOID context);
	//====================================================================================================================


	/*
	Function:   DVTGKLDCam_StartVideo();
	Description:  Start video, detect not started yet.
	Parameters:
	hCam -- Handle of the opened camera.
	hParentWnd  -- Parent window of the video window.
	nX -- left start position of video window in parent window
	nY -- up start position of video window in parent window
	nVideoWindowWidth -- Video window width.  do not need to be same with the format set in DVTGKLDCam_SetVisCamFormat.
	nVideoWindowHeight -- video window height. do not need to be same with the format set in DVTGKLDCam_SetVisCamFormat.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_StartVideo(HANDLE hCam, HWND hParentWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight);
	//==========================================================================================================================================


	/*
	Function:   DVTGKLDCam_UpdateWindow();
	Description:  update video window position after start, you can can this for video size changing.
	Parameters:
	hCam -- Handle of the opened camera.
	hParentWnd  -- Parent window of the video window.
	nX -- left start position of video window in parent window
	nY -- up start position of video window in parent window
	nVideoWindowWidth -- Video window width.  do not need to be same with the format set in DVTGKLDCam_SetVisCamFormat.
	nVideoWindowHeight -- video window height. do not need to be same with the format set in DVTGKLDCam_SetVisCamFormat.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_UpdateWindow(HANDLE hCam, HWND hParentWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight);
	//==========================================================================================================================================


	/*
	Function:   DVTGKLDCam_PauseVideo();
	Description:  pause the video.
	Parameters:
	hCam -- Handle of the opened camera.


	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_PauseVideo(HANDLE hCam);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_ResumeVideo();
	Description:  resume the video.
	Parameters:
	hCam -- Handle of the opened camera.


	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_ResumeVideo(HANDLE hCam);
	//==========================================================================================================================================


	/*
	Function:   DVTGKLDCam_StopVideo();
	Description:  Stop the video.
	Parameters:
	hCam -- Handle of the opened camera.


	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_StopVideo(HANDLE hCam);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_StartDetection();
	Description:  Start live body detection.
	Parameters:
	hCam -- Handle of the opened camera.
	nDetectMilliSeconds -- Detecting time, the minimum time(in million seconds) to get a qualified image if got image within this time,
	if not, the real time is longer.
	nTimeoutMilliSeconds --Detecting timeout time, in million seconds.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_StartDetection(HANDLE hCam, int nDetectMilliSeconds, int nTimeoutMilliSeconds);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_StopDetection();
	Description:  Stop live body detection.
	Parameters:
	hCam -- Handle of the opened camera.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_StopDetection(HANDLE hCam);
	//==========================================================================================================================================


	/*
	Function:   DVTGKLDCam_GetImage();
	Description:  Get normal image after event returned EVENT_OK, 3 image types supported, refer to LD_IMG_TYPE .
	Parameters:
	hCam -- Handle of the opened camera.
	nImageType -- image type, refer to LD_IMG_TYPE.
	pBuff -- buffer to receive image data, recommended buffer length is width*height*5.
	nBuffLen -- length of buffer
	pRetLen -- returned data length.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_GetImage(HANDLE hCam, int nImageType, unsigned char* pBuff, int nBuffLen, int *pRetLen);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_GetImage();
	Description:  Get shrinked image after event returned EVENT_OK, 3 image types supported, refer to LD_IMG_TYPE .
	Parameters:
	hCam -- Handle of the opened camera.
	nImageType -- image type, refer to LD_IMG_TYPE.
	nShrinkWidth -- shrinked image width, less than original image width
	nShrinkHeight --shrinked image height, less than original image height
	pBuff -- buffer to receive image data, recommended buffer length is nShrinkWidth*nShrinkHeight*4.
	nBuffLen -- length of buffer
	pRetLen -- returned data length.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_GetShrinkImage(HANDLE hCam, int nImageType, int nShrinkWidth, int nShrinkHeight, unsigned char* pBuff, int nBuffLen, int *pRetLen);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_GetFaceRect();
	Description:  Get face rect region in the video window after live-body detected sucessfully.
	Parameters:
	hCam -- Handle of the opened camera.
	pLeft --   To receive the value of left position in window.
	pRight --  To receive the value of right position in window.
	pTop --    To receive the value of top position in window.
	pBottom -- To receive the value of bottom position in window.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_GetFaceRect(HANDLE hCam, int *pLeft, int *pRight, int *pTop, int *pBottom);
	//==========================================================================================================================================


	/*
	Function:   DVTGKLDCam_GetFaceImage();
	Description:  Get face image after live-body detected sucessfully.
	Parameters:
	hCam -- Handle of the opened camera.
	nImageType -- image type, refer to LD_IMG_TYPE.
	pBuff -- buffer to receive image data, recommended buffer length is width*height*4.
	nBuffLen -- length of buffer
	pFaceWidth    -- returned face width.
	pFaceHeight   -- returned face height.
	pRetLen  -- returned data length.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_GetFaceImage(HANDLE hCam, int nImageType, unsigned char* pBuff, int nBuffLen, int *pFaceWidth, int *pFaceHeight, int *pRetLen);


	/*
	Function:   DVTGKLDCam_GetResultInfo();
	Description:  Get result information.
	Parameters:
	hCam -- Handle of the opened camera.
	pBuff -- buffer to receive data, recommended buffer length is 20.
	nBuffLen -- length of buffer


	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_GetResultInfo(HANDLE hCam, char *pBuff, int nBuffLen);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_GetVersion();
	Description:  Get version information.
	Parameters:
	pBuff -- buffer to receive data, recommended buffer length is 20.
	nBuffLen -- length of buffer

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_GetVersion(char *pBuff, int nBuffLen);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_GetProduct();
	Description:  Get product information.
	Parameters:
	hCam -- Handle of the opened camera.
	pBuff -- buffer to receive data, recommended buffer length is 100.
	nBuffLen -- length of buffer

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_GetProduct(char *pBuff, int nBuffLen);
	//==========================================================================================================================================

	/*
	Function:   DVTGKLDCam_CheckStatus();
	Description:  Get camera status.
	
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_CheckStatus();
	//==========================================================================================================================================


	/*
	Function:   DVTGKLDCam_FaceCompFeature();
	Description:  Get two picture Similarity Degree.
	Parameters:
	hCam -- Handle of the opened camera.
	szFileName1 -- file1 name.
	szFileName2 -- file2 name.
	height1 -- file1 height.
	width1 -- file1 width.
	height2 -- file2 height.
	width2 -- file2 width.
	pfSimilarity -- Similarity Degree.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_FaceCompFeature(HANDLE hCam, const char* szFileName1, const char* szFileName2, int height1, int width1, int height2, int width2, float *pfSimilarity);
	//==========================================================================================================================================


	/*
	Function:   DVTGKLDCam_Snapshot();
	Description:  Do snapshot, 3 image types supported, refer to LD_IMG_TYPE .

	Parameters:
	hCam -- Handle of the opened camera.
	nImageType -- image type, refer to LD_IMG_TYPE.
	pBuff -- buffer to receive image data, recommended buffer length is width*height*5.
	nBuffLen -- length of buffer
	pRetLen -- returned data length.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTGKLDCam_Snapshot(HANDLE hCam, int nImageType, unsigned char* pBuff, int nBuffLen, int *pRetLen);
	//==========================================================================================================================================



#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif
