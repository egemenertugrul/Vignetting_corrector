#include <iostream> // for standard I/O
#include <string>   // for strings
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <vignettingCorrection.hpp>
#include <experimental/filesystem>
#include <unistd.h>

using namespace std;
using namespace experimental;
using namespace cv;

// this script handles the import of images into memory and the image writing
// to drive. 

static bool ends_with(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static bool starts_with(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

int main(int argc, char *argv[])
{
    if(!(argc > 1))
    {
        cerr << "Not enough input arguments!" << endl;
        cerr << "Usage:\n"
             << "\t-h\tHELP\tshow this help message.\n"
             << "\t-i\tINPUT\tdirectory containing image files\n"
             << "\t-o\tOUTPUT\toutput directory for corrected files.\n"
              << "\t\t\twill be created if missing" << endl;
        return 1;
    }
    
    int opt;
    string inputPath;
    string outputPath;
    
    while((opt = getopt(argc, argv, "i:o:h")) != -1)
    {
        switch (opt) {
            case 'i':
                inputPath = optarg;
                break;
            case 'o':
                outputPath = optarg;
                break;
            case 'h':
                cout << "Usage:\n"
                     << "\t-h\tHELP\tshow this help message.\n"
                     << "\t-i\tINPUT\tdirectory containing image files\n"
                     << "\t-o\tOUTPUT\toutput directory for corrected files.\n"
                     << "\t\t\twill be created if missing" << endl;
                return 0;
            case '?':
                cerr << "Unknown option. Usage:\n"
                     << "\t-h\tINPUT\tshow this help message.\n"
                     << "\t-i\tINPUT\tdirectory containing image files\n"
                     << "\t-o\tOUTPUT\toutput directory for corrected files.\n"
                     << "\t  \t      \twill be created if missing" << endl;
                return 1;
        }
    }
 
//    filesystem::path outputPath = path + string("/vignettingCorrected");
    
    filesystem::create_directory(outputPath);
    
    double totalImageFiles = 0;
    for(const auto & entry : filesystem::directory_iterator(inputPath))
    {
        string file(entry.path());
        if (ends_with(file, ".jpg"))
        {
            totalImageFiles += 1;
        }
    }
    
    cout << "starting to process " << totalImageFiles << " images..." << endl;
    cout << endl;
    double numberProcessedImages = 0;
    for (const auto & entry : filesystem::directory_iterator(inputPath))
    {
        double percentDone = numberProcessedImages / totalImageFiles;
        int barWidth = 70;
        std::cout << "[";
        int pos = barWidth * percentDone;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(percentDone  * 100.0) << " %\r";
        std::cout.flush(); 

        string file(entry.path());
        if (ends_with(file, ".jpg"))
        {
            string fileName (file);
            fileName.erase(0, file.rfind('/') + 1).erase(fileName.rfind('.'));
            cout << "\033[1A\033[Correcting: " << fileName << endl;
            cout.flush();
            Mat image; //definde varible images as cv::Mat
            image = imread(file, IMREAD_COLOR ); // Read the file
            if (vignettingCorrection(image) == 0)
            {
                string correctedImageOutput;
                correctedImageOutput += static_cast<string>(outputPath) + string("/") + fileName + ("_corrected.jpg");
                imwrite(correctedImageOutput, image);
            }
               else
            {
                   cout << "error" << endl;
            }
        }
        numberProcessedImages += 1;
    }
    cout << endl;
    cout << "all done!" << endl;
	
	return(0);

}
