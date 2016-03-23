#ifndef SEGMENTTRACK_H
#define SEGMENTTRACK_H

#include "defs.h"
#include "utils.h"
#include "graphmatch.h"
#include "segmentation.h"
#include "ssgproc.h"
#include <QObject>

class SegmentTrackParams{
public:
    SegmentTrackParams(int tau_w, float tau_m);
    int tau_w;
    float tau_m;
};

class SegmentTrack : public QObject
{
    Q_OBJECT
public:
    SegmentTrack(SegmentTrackParams* params, SegmentationParams* seg_params, GraphMatchParams* gm_params);
    float fillNodeMap(const vector<vector<NodeSig> >& ns_vec);
    void processImagesOnline();
    bool eventFilter( QObject* watched, QEvent* event );
    void drawMap();
    void processImage(const Mat cur_img, vector<vector<NodeSig> > &ns_vec);

public:
    int mapScaleFactor;
    Mat getM();
    vector<pair<NodeSig, int > > getM_ns();

private:
    Mat M;
    vector<pair<NodeSig, int> > M_ns; //Average node signatures
    SegmentTrackParams* params;
    GraphMatch* gm;
    Segmentation* seg;
    void drawCursor(Mat& img);
    Point cursor;
    int img_height;
    int img_width;
    vector<string> img_files;
    vector<pair<int, float> > getCoherentSegments(const Mat map, const Mat img, float thres, Mat& img_seg);

    
signals:
    void showTrackSegment(QImage img);
    void showTrackMap(QImage img);
    void showImgOrg(QImage img);
    
public slots:
    
};

#endif // SEGMENTTRACK_H
