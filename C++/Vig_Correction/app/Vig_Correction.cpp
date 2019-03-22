#include <iostream> // for standard I/O
#include <string>   // for strings
#include "ColorCorrection.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int VignettingCorrect(Mat matImage, String exePath)
{
	int ht = matImage.rows;
	int wd = matImage.cols;
	int scanWd = matImage.step;
	int nChannels = matImage.channels();

	double ratio = 1;
	if (wd>75)
		ratio = 75.0 / double(wd);

	//resize the image
	int sht = ht*ratio + 0.5;
	int swd = wd*ratio + 0.5;
	Mat matSmallImage = Mat(sht, swd, CV_8UC(nChannels));
	resize(matImage, matSmallImage, matSmallImage.size());

	//convert from image to gray
	Mat matGrayImage(sht, swd, CV_8UC1, Scalar(0));
	if (nChannels == 3)
	{
		cvtColor(matSmallImage, matGrayImage, cv::COLOR_RGB2GRAY);
	}
	else
	{
		matGrayImage = matSmallImage.clone();
	}

	unsigned char* pImageBuffer = (unsigned char*)malloc(sht*swd);
	for (int j = 0; j < sht; j++)
		for (int i = 0; i < swd; i++)
		{
			pImageBuffer[j*swd + i] = (unsigned char)(matGrayImage.data[j*matGrayImage.step + i]);
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
		unsigned char* matImagePtr = (unsigned char*)(matImage.data);
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

				double scale = 1.0 / vValue;
				int r = (unsigned char)(matImagePtr[j*scanWd + i * 3]);
				int g = (unsigned char)(matImagePtr[j*scanWd + i * 3 + 1]);
				int b = (unsigned char)(matImagePtr[j*scanWd + i * 3 + 2]);
				r *= scale;
				g *= scale;
				b *= scale;	
				
				matImagePtr[(j)*scanWd + i * 3] = min(255, r);
				matImagePtr[(j)*scanWd + i * 3 + 1] = min(255, g);
				matImagePtr[(j)*scanWd + i * 3 + 2] = min(255, b);				
			}
		
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
		
		String estimateImageOutputPath;
		estimateImageOutputPath += exePath + string("../data/corrected.jpg");
		imwrite(estimateImageOutputPath, Estimate);
	}
	free(pImageBuffer);

	return 0;
}

int main(int argc, char *argv[])
{
	String executablePath(argv[0]);
	executablePath.erase(executablePath.rfind('/') + 1);
	// set a default image if non is specified via command line arguemnts
	String imageName = executablePath + string("../data/test.jpg");
        if( argc > 1){
                 imageName = argv[1];
        }

	Mat image; //definde varible images as cv::Mat
 	image = imread(imageName, IMREAD_COLOR ); // Read the file

	if (VignettingCorrect(image, executablePath) == 0)
	{
		String correctedImageOutputPath;
		correctedImageOutputPath += executablePath + string("../data/corrected.jpg");
		imwrite(correctedImageOutputPath, image);
	}
	else
	{
		cout << "error" << endl;
	}

	
	return(0);

}
