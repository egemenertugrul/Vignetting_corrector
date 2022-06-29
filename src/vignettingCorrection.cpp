#include <iostream>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vignettingCorrectionUsingRG.hpp>
#include <vignettingCorrection.hpp>
#include <omp.h>

using namespace std;
using namespace cv;

int vignettingCorrection(Mat matImage)
{
	int ht = matImage.rows;
	int wd = matImage.cols;
	int scanWd = matImage.step;
	int nChannels = matImage.channels();

	double ratio = 1;
    if (wd > 75)
        ratio = 75.0 / double(wd);

	//resize the image
	int sht = ht * ratio + 0.5;
	int swd = wd * ratio + 0.5;
	Mat smallImage = Mat(sht, swd, CV_8UC(nChannels));
	resize(matImage, smallImage, smallImage.size());

	//convert from image to gray
	Mat grayImage(sht, swd, CV_8UC1, Scalar(0));
	if (nChannels == 3)
	{
		cvtColor(smallImage, grayImage, cv::COLOR_RGB2GRAY);
	}
	else
	{
		grayImage = smallImage.clone();
	}

	unsigned char* pImageBuffer = (unsigned char*)malloc(sht * swd);
	unsigned char* grayImageData = (unsigned char*)(grayImage.data);
	int grayImageStep = grayImage.step;

	for (int j = 0; j < sht; j++)
		for (int i = 0; i < swd; i++)
		{
			pImageBuffer[j * swd + i] = grayImageData[j * grayImageStep + i];
		}

	// Vignetting correction
	vector<double> vp;
    
//    auto startTime = chrono::high_resolution_clock::now();
	int flag = vignettingCorrectionUsingRG(pImageBuffer, sht, swd, vp);
//    auto endTime = chrono::high_resolution_clock::now();
//    cout << "vignettingCorrectionUsingRG took ";
//    cout << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
//    cout << " ms" << endl;

	if (flag == 0)
	{
		int nV = vp.size();
		for (int i = 0; i < nV; i++)
		{
			vp[i] = exp(vp[i]);
		}


		double maxVr = vp[0];
		for (int i = 0; i < nV; i++)
		{
			vp[i] = vp[i] / maxVr;
		}

		int halfHt = ht * 0.5;
		int halfWd = wd * 0.5;
		int shalfHt = sht * 0.5;
		int shalfWd = swd * 0.5;

//      auto startTime = chrono::high_resolution_clock::now();
//      apply the vignetting correction
        unsigned char* matPtr = (unsigned char*)(matImage.data);
#pragma omp parallel for schedule(static, 90)
        for (int j = 0; j < ht; j++)
        {
            for (int i = 0; i < wd; i++)
            {
                double cx = i - halfWd;
                double cy = j - halfHt;

                double radius = sqrt(cx * cx + cy * cy) + 0.5;
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

                double scale = 1.0 / vValue;
                int r = (unsigned char)(matPtr[j * scanWd + i * 3]);
                int g = (unsigned char)(matPtr[j * scanWd + i * 3 + 1]);
                int b = (unsigned char)(matPtr[j * scanWd + i * 3 + 2]);
                r *= scale;
                g *= scale;
                b *= scale;
                
                matPtr[j * scanWd + i * 3] = min(255, r);
                matPtr[j * scanWd + i * 3 + 1] = min(255, g);
                matPtr[j * scanWd + i * 3 + 2] = min(255, b);
            }
        }
		
//        auto endTime = chrono::high_resolution_clock::now();
//        cout << "apply the vignetting correction took ";
//        cout << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
//        cout << " ms" << endl;
        
		//Estimate the Correction
		Mat EstimateTemp(sht, swd, CV_8UC1, Scalar(0));
		for (int i = 0; i < EstimateTemp.rows; i++)
		{
			unsigned char* data = (unsigned char*)(EstimateTemp.data) + i * EstimateTemp.step;
			for (int j = 0; j < EstimateTemp.cols; j++)
			{
				int cx = i - shalfWd;
				int cy = j - shalfHt;
				int r = sqrt(double(cx*cx + cy*cy)) + 0.5;
				if (r > 0 && r < nV + 1 && vp[r-1]<1)
				{
					data[j] = 255 * vp[r - 1];
				}
				else
				{
					data[j] = 255;
				}
			}
		}
		Mat Estimate(ht, wd, CV_8UC1, Scalar(0));
		resize(EstimateTemp, Estimate, Estimate.size());
		
	}
	free(pImageBuffer);

	return 0;
}
