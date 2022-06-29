#include <iostream>
#include <chrono>
#include "math.h"
#include "matrix.h"
#include "vignettingCorrectionUsingRG.hpp"

using namespace std;

/* vignetting correction based on radial gradient
*/
int vignettingCorrectionUsingRG(unsigned char* pImage, int ht, int wd, vector<double>& vp)
{
    int halfWd = wd * 0.5 ;
    int halfHt = ht * 0.5 ;

    int nRadius = sqrt(double(halfHt * halfHt + halfWd * halfWd)) + 0.5 + 1 ;

    vp.resize(nRadius);

    double* weight = (double*)malloc(ht * wd * sizeof(double));
    for(int i = 0; i < ht * wd; i++)
    {
        weight[i] = 1;
    }

    double* rgImage = (double*)calloc(ht * wd, sizeof(double));
    double* sAtA =  (double*)malloc(nRadius*nRadius*sizeof(double));
    double* sAtL =  (double*)malloc(nRadius*sizeof(double));
    double* result = (double*)malloc(nRadius*sizeof(double));

    //calculate the radial gradients of image
    double shift = 1;
    double eps = 0.000001;
    for(int j = 1; j < ht; j++)
    {
        for(int i = 1; i < wd; i++)
        {
            int cx = i - halfWd;
            int cy = j - halfHt;
            //calculate the gradient
            double dx = log(double(pImage[j * wd + i]) + shift) - log(double(pImage[j * wd + i - 1]) + shift);
            double dy = log(double(pImage[j * wd + i]) + shift) - log(double(pImage[(j - 1) * wd + i]) + shift);

            //calculate the radial gradient
            double rg = double(cx * dx + cy * dy) / sqrt(double(cx * cx + cy * cy) + eps);
            rgImage[j * wd + i] = rg;
        }
    }

    //weighted least square solution
//    printf("\n WLS... \n");

    double lamdaS = 0.15 * 2 * (wd * ht)/double(nRadius);
    for(int iterIndex = 0; iterIndex < 5; iterIndex++)
    {
        memset(sAtA, 0, nRadius * nRadius * sizeof(double));
        memset(sAtL, 0, nRadius * sizeof(double));

        for(int j = 1; j < ht; j++)
        {
            for(int i = 1; i < wd; i++)
            {
                //calculate the radius
                int cx = i - halfWd;
                int cy = j - halfHt;
                int radius = sqrt(double(cx * cx + cy * cy)) + 0.5;
                double rg = rgImage[j * wd + i];
                double w2 = weight[j * wd + i] * weight[j * wd + i];
                double right = 0;

                //sum
                if (radius > 0 && radius < nRadius)
                {
                    sAtA[(radius - 1) * nRadius + radius - 1] += w2;
                    sAtA[(radius - 1) * nRadius + radius] += (-1 * w2);
                    sAtA[(radius) * nRadius + radius - 1] += (-1 * w2);
                    sAtA[(radius) * nRadius + radius] += w2;
                    right = rg;
                    sAtL[radius - 1] += -1 * right * w2;
                    sAtL[radius] += right * w2;
                }
            }
        }

        /////////////////////////////  adding constraints ///////////////////////
        //smooth constraint
        // first row
        sAtA[0] += lamdaS * lamdaS * 1;
        sAtA[1] += lamdaS * lamdaS * -2;
        sAtA[2] += lamdaS * lamdaS * 1;
        
        // second row
        sAtA[nRadius] += lamdaS * lamdaS * -2;
        sAtA[nRadius + 1] += lamdaS * lamdaS * 5;
        sAtA[nRadius + 2] += lamdaS * lamdaS * -4;
        sAtA[nRadius + 3] += lamdaS * lamdaS * 1;
        
        // second last row
        sAtA[nRadius * nRadius - nRadius - 4] += lamdaS * lamdaS * 1;
        sAtA[nRadius * nRadius - nRadius - 3] += lamdaS * lamdaS * -4;
        sAtA[nRadius * nRadius - nRadius - 2] += lamdaS * lamdaS * 5;
        sAtA[nRadius * nRadius - nRadius - 1] += lamdaS * lamdaS * -2;
        
        // last row
        sAtA[nRadius * nRadius - 3] += lamdaS * lamdaS * 1;
        sAtA[nRadius * nRadius - 2] += lamdaS * lamdaS * -2;
        sAtA[nRadius * nRadius - 1] += lamdaS * lamdaS * 1;
        
        for(int i = 2; i < nRadius; i++){
            sAtA[i * nRadius + i - 2] += lamdaS * lamdaS * 1 ;
            sAtA[i * nRadius + i - 1] += lamdaS * lamdaS * -4;
            sAtA[i * nRadius + i] += lamdaS * lamdaS * 6;
            sAtA[i * nRadius + i + 1] += lamdaS * lamdaS * -4;
            sAtA[i * nRadius + i + 2] += lamdaS * lamdaS * 1;
        }

        //vignetting value constraint, make them close to 1
        double eps = 0.03;
        for(int i = 0; i < nRadius; i++)
        {
            sAtA[i * nRadius + i] += eps;
        }
        //////////////////////////////////////////////////////////////////////////

        invertMatrix(sAtA, nRadius);
        multiplyMatrices(sAtA, sAtL, result, nRadius, nRadius, 1);

        for(int i = 0; i < nRadius; i++)
        {
            vp[i] = result[i];
        }

        //update weight
        double alpha = 0.6;
        for(int j = 1; j < ht; j++)
        {
            for(int i = 1; i < wd; i++)
            {
                int cx = i - halfWd;
                int cy = j - halfHt;
                
                //calculate the radius
                int radius = sqrt(double(cx * cx + cy * cy)) + 0.5;
                radius = max(1, min(nRadius - 1, radius));

                double rv = vp[radius] - vp[radius-1]; //radial gradient of image
                double rz = rgImage[j * wd + i]; //radial gradient of vignetting paras

                double s1 = fabs(rz-rv);
                double s2 = alpha * pow(s1, alpha-1);
                weight[j * wd + i] = exp(-s1) * (1 - exp(-s2));
            }
        }
    }
    free(sAtA);
    free(sAtL);
    free(result);
    free(weight);
    free(rgImage);

    return 0;
}
