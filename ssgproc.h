#ifndef SSGPROC_H
#define SSGPROC_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <iostream>
#include <fstream>

#include "utilTypes.h"

using namespace std;
using namespace cv;

class SSGProc 
{
public:
    static void updateSSG(SSG& ssg, vector<NodeSig> ns, Mat& map);
    static Mat drawSSG(SSG& ssg, Mat input);
    static void updateNodeSig(pair<NodeSig, int>& ns, NodeSig new_ns);

};

#endif // SSGPROC_H
