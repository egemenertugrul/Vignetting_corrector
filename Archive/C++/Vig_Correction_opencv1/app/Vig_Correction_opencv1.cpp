#include <iostream> // for standard I/O
#include <string>   // for strings
//#include<cv.h>
//#include<highgui.h>
//#include<cxcore.h>
#include "ColorCorrection.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int VignettingCorrect(IplImage* pImage)
{
	int ht = pImage->height;
	int wd = pImage->width;
	int scanWd = pImage->widthStep;
	int nChannels = pImage->nChannels;

	cout << "scanWd = " << scanWd << endl;
	double ratio = 1;
	if (wd>75)
		ratio = 75.0 / double(wd);

	cout << "resize the image " << endl;
	//resize the image
	int sht = ht*ratio + 0.5;
	int swd = wd*ratio + 0.5;
	IplImage* pSmallImage = cvCreateImage(cvSize(swd, sht), 8, nChannels);
	cvResize(pImage, pSmallImage);


	cout << "convert image to gray" << endl; 
	//convert from image to gray
	IplImage* pGrayImage = NULL;
	if (nChannels == 3)
	{
		pGrayImage = cvCreateImage(cvSize(swd, sht), 8, 1);
		cvCvtColor(pSmallImage, pGrayImage, CV_BGR2GRAY); 
                Mat matpGrayImage = cvarrToMat(pGrayImage);
                imwrite("../data/gray.jpg", matpGrayImage);
	}
	else
	{
		pGrayImage = cvCloneImage(pSmallImage);
	}

	unsigned char* pImageBuffer = (unsigned char*)malloc(sht*swd);
	for (int j = 0; j < sht; j++)
		for (int i = 0; i < swd; i++)
		{
			pImageBuffer[j*swd + i]
				= (unsigned char)(pGrayImage->imageData[j*pGrayImage->widthStep + i]);
		}
	// Vignetting correction
		vector<double> vp;
	int flag=VignettingCorrectionUsingRG(pImageBuffer, sht, swd, vp);

	if (flag == 0)
	{
		int nV = vp.size();
		for (int i = 0; i < nV; i++)
		{
			vp[i] = exp(vp[i]);
			//printf("%lf \n", vp[i]);
		}


		//int nV = vp.size();
		double maxVr = vp[0];
		for (int i = 0; i < nV; i++)
		{
			vp[i] = vp[i] / maxVr;
			//printf("%lf ", vp[i]);
		}

		int halfHt = ht*0.5;
		int halfWd = wd*0.5;
		int shalfHt = sht*0.5;
		int shalfWd = swd*0.5;
		//apply the vignetting correction
		for (int j = 0; j < ht; j++)
			for (int i = 0; i < wd; i++)
			{
				double cx = i - halfWd;
				double cy = j - halfHt;

				double radius = sqrt(cx*cx + cy*cy) + 0.5;
				radius *= ratio;

				//shd near interpolation
				double vValue = 1;
				int nR = int(radius);
				if (nR == 0)
				{
					vValue = vp[0];
				}
				else if (nR < nV)
				{
					double dr = radius - nR;
					vValue = vp[nR - 1] * (1 - dr) + vp[nR] * dr;
				}
				else
				{
					vValue = vp[nV - 1];
				}

				//radius = max(0, min(nV-1, radius) );
				//double scale = 1.0 / vp[radius];
				double scale = 1.0 / vValue;

				int r = (unsigned char)(pImage->imageData[j*scanWd + i * 3]);
				int g = (unsigned char)(pImage->imageData[j*scanWd + i * 3 + 1]);
				int b = (unsigned char)(pImage->imageData[j*scanWd + i * 3 + 2]);
				r *= scale;
				g *= scale;
				b *= scale;

				//pImageColor->imageData[(ht-j-1)*scanWd+i*3]   = min(255,r);
				//pImageColor->imageData[(ht-j-1)*scanWd+i*3+1] = min(255,g);
				//pImageColor->imageData[(ht-j-1)*scanWd+i*3+2] = min(255,b);
				pImage->imageData[(j)*scanWd + i * 3] = min(255, r);
				pImage->imageData[(j)*scanWd + i * 3 + 1] = min(255, g);
				pImage->imageData[(j)*scanWd + i * 3 + 2] = min(255, b);
			}
		//Estimate the Correction
		IplImage* Estimate_temp = cvCreateImage(cvSize(swd, sht), 8, 1);
		IplImage* Estimate = cvCreateImage(cvSize(wd, ht), 8, 1);
		for (int i = 0; i < Estimate_temp->height; i++)
		{
			uchar* data = (uchar*)Estimate_temp->imageData + i*Estimate_temp->widthStep;
			for (int j = 0; j < Estimate_temp->width; j++)
			{
				int cx = i - shalfWd;
				int cy = j - shalfHt;
				int r = sqrt(double(cx*cx + cy*cy)) + 0.5;
				if (r > 0 && r < nV + 1 && vp[r-1]<1)
					data[j] = 255 * vp[r - 1];

				else
					data[j] = 255;

			}
		}
		cvResize(Estimate_temp, Estimate);
		
		Mat estimate = cvarrToMat(Estimate);

		imwrite("../data/estimate.jpg", estimate);

		cout << "releasing estimates" << endl;
		cvReleaseImage(&Estimate_temp);
		cvReleaseImage(&Estimate);
		//cvNamedWindow("Estimate");
		//cvShowImage("Estimate", Estimate);
		//cvSaveImage("white1_Est.png", Estimate);
	}
	free(pImageBuffer);
	cvReleaseImage(&pGrayImage);
	cvReleaseImage(&pSmallImage);

	return 0;
}

int main(int argc, char *argv[])
{
	// set a default image if non is specified via command line arguemnts
        String imageName( "../data/test.jpg" );
        if( argc > 1){
                 imageName = argv[1];
        }

	cout << "loading image..." << endl;
	Mat image; //definde varible images as cv::Mat
 	image = imread(imageName, IMREAD_COLOR ); // Read the file
	//IplImage* img = cvLoadImage(argv[1]);

	
	namedWindow("Correction", WINDOW_AUTOSIZE );

	// convert cv:Mat to IplImage (legacy C API type)
	IplImage temp_image = image;
	IplImage* img = &temp_image;

	cout << "correct... image" << endl;
	if (VignettingCorrect(img) == 0)
	{
		cout << "correction worked image" << endl;
	
		Mat corrected = cvarrToMat(img);
		imwrite("../data/corrected.jpg", corrected);
	}
	else
	{
		cout << "error" << endl;
	}

	
	return(0);

}
