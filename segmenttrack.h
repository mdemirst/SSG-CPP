#ifndef SEGMENTTRACK_H
#define SEGMENTTRACK_H

#include "defs.h"
#include "utils.h"
#include "graphmatch.h"
#include "segmentation.h"
#include "ssgproc.h"



class SegmentTrack 
{

public:
    explicit SegmentTrack();
    GraphMatch* gm;
    Segmentation* seg;
    void drawCursor(Mat& img);
    void processImage(const Mat cur_img, vector<vector<NodeSig> > &ns_vec);
    void processImagesOnline(Mat img, vector<vector<NodeSig>> &ns_vec);
    vector<pair<int, float> > getCoherentSegments(const Mat map, const Mat img, float thres, Mat& img_seg);
    float fillNodeMap(Mat& M, vector<pair<NodeSig, int> >& M_ns, const vector<vector<NodeSig> > ns_vec);


public:
    Mat M;
    vector<pair<NodeSig, int> > M_ns; //Average node signatures
    Point cursor;
    int mapScaleFactor;
    int tau_w;
    float tau_m;
    int img_height;
    int img_width;
    
};

#endif // SEGMENTTRACK_H
