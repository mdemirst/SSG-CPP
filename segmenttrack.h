#ifndef SEGMENTTRACK_H
#define SEGMENTTRACK_H

#include "defs.h"
#include "utils.h"
#include "graphmatch.h"
#include "segmentation.h"
#include "ssgproc.h"
#include <QObject>


class SegmentTrack : public QObject
{
    Q_OBJECT
public:
    explicit SegmentTrack(QObject *parent = 0);
    GraphMatch* gm;
    Segmentation* seg;
    void drawMap();
    void drawCursor(Mat& img);
    bool eventFilter( QObject* watched, QEvent* event );
    void processImage(const Mat cur_img, vector<vector<NodeSig> > &ns_vec);
    void processImagesOnline();
    vector<pair<int, float> > getCoherentSegments(const Mat map, const Mat img, float thres, Mat& img_seg);
    void plotMap(Mat& M);
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

    
signals:
    void showTrackSegment(QImage img);
    void showTrackMap(QImage img);
    void showImg1(QImage img);
    void showImg2(QImage img);
    void showMap(QImage img);
    
public slots:
    
};

#endif // SEGMENTTRACK_H
