#ifndef SEGMENTATION_H
#define SEGMENTATION_H

//Predefined segmentation parameters
#define SEG_SIGMA       0.3
#define SEG_K           100
#define SEG_MIN_SIZE    750
#define SEG_SCALE       1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <iostream>
#include <fstream>

#include "utilTypes.h"
#include "GraphSegmentation/image.h"
#include "GraphSegmentation/misc.h"
#include "GraphSegmentation/pnmfile.h"
#include "GraphSegmentation/segment-image.h"

using namespace std;
using namespace cv;

class Segmentation 
 {
public:
    explicit Segmentation();
    vector<NodeSig> segmentImage(const Mat &img_org, Mat &img_seg);
    void getSegmentByIds(const Mat &img_org, Mat &img_seg, vector<int> ids);
    void setSegmentationParameters(float sigma, float k, float min_size);
    void setParSigma(float sigma);
    void setParK(float k);
    void setParMinSize(float min_size);
    float getParSigma();
    float getParK();
    float getParMinSize();



private:
    //Segmentation parameters
    float sigma;
    float k;
    int min_size;

    template <class T>
    Mat convert2Mat(image<T>* img);
    Mat blendImages(Mat img1, Mat img2, float alpha);
    vector<NodeSig> constructSegmentsGraph(Mat img, vector<BlobStats> blobs);
    vector<BlobStats> calcBlobStats(Mat img, universe* segments);
    Mat drawBlobs(Mat img, vector<BlobStats> blobs, int id = -1);
    
};

#endif // SEGMENTATION_H
