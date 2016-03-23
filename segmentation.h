#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <iostream>
#include <fstream>
#include <QObject>
#include <QImage>

#include "utilTypes.h"
#include "GraphSegmentation/image.h"
#include "GraphSegmentation/misc.h"
#include "GraphSegmentation/pnmfile.h"
#include "GraphSegmentation/segment-image.h"

using namespace std;
using namespace cv;

class SegmentationParams
{
public:
    SegmentationParams(float sigma,
                       float k,
                       int min_size);
    float sigma;
    float k;
    int min_size;
};

class Segmentation : public QObject
{
    Q_OBJECT
public:
    Segmentation(SegmentationParams* params);
    SegmentationParams* params;
    vector<NodeSig> segmentImage(const Mat &img_org, Mat &img_seg);
    vector<Mat> segmentImage(const Mat &img);
    void getSegmentByIds(const Mat &img_org, Mat &img_seg, vector<int> ids);
    void setSegmentationParameters(float sigma, float k, float min_size);


private:
    template <class T>
    Mat convert2Mat(image<T>* img);
    Mat blendImages(Mat img1, Mat img2, float alpha);
    vector<NodeSig> constructSegmentsGraph(Mat img, vector<BlobStats> blobs);
    vector<BlobStats> calcBlobStats(Mat img, universe* segments);
    Mat drawBlobs(Mat img, vector<BlobStats> blobs, int id = -1);
    
public slots:
};

#endif // SEGMENTATION_H
