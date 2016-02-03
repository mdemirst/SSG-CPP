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

#define MATCH_LINE_COLOR Scalar(0,0,255)
#define MATCH_LINE_WIDTH 2
#define MATCH_CIRCLE_RADIUS 20

#define POS_WEIGHT 0.8
#define COLOR_WEIGHT 0.5
#define AREA_WEIGHT 0.7

#define MULTIPLIER_1000 10000.0

#define IMG_RESIZE_RATIO 0.3


class GraphMatch : public QObject
{
    Q_OBJECT
public:
    explicit GraphMatch(QObject *parent = 0, int img_width = 0, int img_height = 0);
    float drawMatches(vector<NodeSig> ns1, vector<NodeSig> ns2,
                      Mat img1, Mat img2);
    float matchTwoImages(vector<NodeSig> ns1, vector<NodeSig> ns2,
                         Mat& assignment, Mat& cost);
    double calcN2NDistance(NodeSig s1, NodeSig s2);
    int img_width, img_height;
    float color_weight;
    float pos_weight;
    float area_weight;
    
private:

signals:
    void showMatchImage(QImage img);

public slots:

    
};

#endif // GRAPHMATCH_H
