#ifndef GRAPHMATCH_H
#define GRAPHMATCH_H

#include <QObject>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <iostream>
#include <fstream>

#include "segmentation.h"
#include "utilTypes.h"
#include "qcustomplot.h"

using namespace cv;

#define MATCH_LINE_COLOR Scalar(0,0,0)
#define MATCH_LINE_WIDTH 2
#define MATCH_CIRCLE_RADIUS 20

#define MULTIPLIER_1000 10000.0

#define IMG_RESIZE_RATIO 0.3

class GraphMatchParams
{
public:
    GraphMatchParams(float pos_weight,
                     float color_weight,
                     float area_weight);
    float pos_weight;
    float color_weight;
    float area_weight;
};

class GraphMatch : public QObject
{
    Q_OBJECT
public:
    GraphMatch(int img_width, int img_height, GraphMatchParams* params);
    float drawMatches(vector<NodeSig> ns1, vector<NodeSig> ns2,
                      Mat img1, Mat img2);
    float matchTwoImages(vector<NodeSig> ns1, vector<NodeSig> ns2,
                         Mat& assignment, Mat& cost);
    double calcN2NDistance(NodeSig s1, NodeSig s2);
    GraphMatchParams* params;
    int img_width, img_height;
    
private:

signals:
    void showMatchImage(QImage img);

public slots:

    
};

#endif // GRAPHMATCH_H
