#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <Commdlg.h>
#include <stdio.h>
#include<exception>

OPENFILENAME ofn;
char szFile[300];

char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn)
{
	LPSTR pszOut = NULL;
	try
	{
		if (lpwszStrIn != NULL)
		{
			int nInputStrLen = wcslen(lpwszStrIn);
			int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
			pszOut = new char[nOutputStrLen];

			if (pszOut)
			{
				memset(pszOut, 0x00, nOutputStrLen);
				WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
			}
		}
	}
	catch (std::exception e)
	{
	}
	return pszOut;
}

char* flashreplace(char* file) {
	int size = strlen(file);
	for (int i = 0; i <= size; i++) {
		if (*(file + i) == '\\')
		{
			*(file + i) = '/';
		}
	}
	for (int i = 0; i <= size; i++) {
		printf("%c", *(file + i));
	}
	return file;
}

char* getfile() {
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		char* getf = ConvertLPWSTRToLPSTR(ofn.lpstrFile);
		getf = flashreplace(getf);
		return getf;
	}
	else
	{
		printf("user cancelled\n");
	}
}

int main()
{
	const char* filename = getfile();
	IplImage* inputimage = cvLoadImage(filename, -1);
	IplImage* smoothimage = cvCreateImage(cvSize(inputimage->width, inputimage->height), IPL_DEPTH_8U, inputimage->nChannels);
	IplImage* extensionimage = cvCreateImage(cvSize(inputimage->width + 2, inputimage->height + 2), IPL_DEPTH_8U, inputimage->nChannels);
	cvCopyMakeBorder(inputimage, extensionimage, cvPoint(1, 1), IPL_BORDER_REPLICATE);


	int gauss[3][3] = { 1,2,1,2,4,2,1,2,1 };
	for (int i = 0; i < inputimage->height; i++)
	{
		for (int j = 0; j < inputimage->width; j++)
		{
			for (int k = 0; k < inputimage->nChannels; k++)
			{
				float temp = 0;
				for (int m = 0; m < 3; m++)
				{
					for (int n = 0; n < 3; n++)
					{
						temp += gauss[m][n] * (unsigned char)extensionimage->imageData[(i + m) * extensionimage->widthStep + (j + n) * extensionimage->nChannels + k];
					}
				}
				smoothimage->imageData[i * smoothimage->widthStep + j * smoothimage->nChannels + k] = temp / 16.0;
			}

		}
	}

	cv::Mat smooth = cv::cvarrToMat(smoothimage);
	cv::imwrite("buildingsmooth.jpg", smooth);
	cvNamedWindow("inputimage", 0);
	cvNamedWindow("smoothimage", 0);

	cvShowImage("inputimage", inputimage);
	cvShowImage("smoothimage", smoothimage);
	cvWaitKey(0);

	cvDestroyWindow("inputimage");
	cvDestroyWindow("smoothimage");

	cvReleaseImage(&inputimage);
	cvReleaseImage(&smoothimage);
}
