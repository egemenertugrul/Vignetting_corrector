#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#define MAXRESPONSES 3

using namespace std;
using namespace cv;

int  main()
{

    vector<Mat> v (5);
    
    const int s[2] ={2,2} ;
    
    
    for (int i; i < v.size(); i++){
        v[i] = Mat::zeros(2, 2, CV_64FC(3));
        cout << v[i] << endl;
    }
    
    std::vector<double> sigmas {1.0, 1.5, 2.0}; // a vector container of sigmas
    std::vector<double> spacing {1,1,1};
    std::vector<double> dimensions;
    std::vector<double> angles {180};
    std::vector<double> maxclusters{10};
    double maxsigma = *std::max_element(sigmas.begin(), sigmas.end());
    int maxradius = ceil(3 * maxsigma);
    int nwedges = 6;
    int plot = 0; // check if that is actually neeeded since it is some matplat stuff?!?
    int dims[2];
    int numangles = angles.size();
    int imgrows = 32;
    int imgcols = 32;
    
    dimensions.push_back(maxradius);
    dimensions.push_back(maxradius);
    dimensions.push_back(imgrows - maxradius);
    dimensions.push_back(imgcols - maxradius);
    
    // allocate output arrays
    /* There are 4 output arrays. Each array is a Numeric array of shape
       (numsigmas, numangles). The entries are Numeric array of shape (rows,
       cols, numresponses) where rows, cols, and numresponses can vary from
       array to array. In the original Compass code, this stuff was mostly in
       compassmex.c.
    */
    
    std::vector<cv::Mat> strength(sigmas.size() * angles.size());
    std::vector<cv::Mat> abnormality(sigmas.size() * angles.size());
    std::vector<cv::Mat> orientation(sigmas.size() * angles.size());
    std::vector<cv::Mat> uncertainty(sigmas.size() * angles.size());
    
    
    for (int j = 0; j < sigmas.size(); j++) { /* sigmas */
        for (int k = 0; k < numangles; k++) { /* angles */
            /* Create a 3 dim array: (rows, cols, responses). */
            dims[0] = (int) (imgrows - 2 * maxradius)/(int) spacing[j] + 1;
            dims[1] = (int) (imgcols - 2 * maxradius)/(int) spacing[j] + 1;
            cout << dims [0] << " " << dims [1] << endl;
            strength[j * numangles + k] = cv::Mat::zeros(dims[0], dims[1], CV_64FC(1));
            abnormality[j * numangles + k] = cv::Mat::zeros(dims[0], dims[1], CV_64FC(1));
            orientation[j * numangles + k] = cv::Mat::zeros(dims[0], dims[1], CV_64FC(MAXRESPONSES));
            uncertainty[j * numangles + k] = cv::Mat::zeros(dims[0], dims[1], CV_64FC(MAXRESPONSES));
        }
    }
    
    return 0;
}


