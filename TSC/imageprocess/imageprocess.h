#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <opencv2/opencv.hpp>
#include <QString>

using namespace cv;

class ImageProcess
{
public:

    ImageProcess();

    //Mat img;

    //Mat filter;

    static void readFilter(QString fileName, int filterSize, bool transpose, bool save, bool show);

    static Mat getFilter();

    static Mat loadImage(QString fileName, bool show);

    static Mat applyFilter(Mat singleChannelImage);

    static std::vector<Mat> applyFilters(Mat singleChannelImage);

    static void setDataSetPath(QString path);

    static QString getDataSetPath();

    static Mat generateChannelImage(const Mat& rgbimage, int channelNo, int satLower, int satUpper, int valLower, int valUpper);

    static Mat generateHueImage(int satLower, int satUpper, int valLower, int valUpper);

    static void setImage(Mat image);

    static Mat getImage();

    // For extracting the frame number from the fileName
    static int getFrameNumber(QString fullFilePath);

};

#endif // IMAGEPROCESS_H
