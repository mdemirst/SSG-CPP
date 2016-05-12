#ifndef PLACEDETECTION_H
#define PLACEDETECTION_H

#include "defs.h"
#include "qcustomplot.h"
#include "ssgproc.h"
#include "segmenttrack.h"

#include <QObject>

using namespace std;

class SSGParams{
public:
    SSGParams(int tau_n,
              float tau_c,
              int tau_f,
              float tau_d,
              float tau_p,
              float coeff_node_disappear1,
              float coeff_node_disappear2,
              float coeff_node_appear,
              float coeff_coh_exp_base,
              float coeff_coh_appear_thres);
    int tau_n;
    float tau_c;
    int tau_f;
    float tau_d;
    float tau_p;
    float coeff_node_disappear1;
    float coeff_node_disappear2;
    float coeff_node_appear;
    float coeff_coh_exp_base;
    float coeff_coh_appear_thres;
};

class PlaceDetection : public QObject
{
    Q_OBJECT
public:
    PlaceDetection(QCustomPlot* coherency_plot, QCustomPlot* map,
                   SSGParams* params,
                   SegmentTrackParams* seg_track_params,
                   SegmentationParams* seg_params,
                   GraphMatchParams* gm_params);
    SSGParams* params;
    int detectPlace(vector<float> coherency_scores,
                    vector<int>& detected_places_unfiltered,
                    vector<int>& detected_places);
    int getMedian(vector<int> v);
    bool getRegionStatus(vector<int> v);
    void clearPastData();
    void constructSceneGist(Mat& M, vector<pair<NodeSig, int> > M_ns);
    bool eventFilter( QObject* watched, QEvent* event );

public:
    vector<float> coherency_scores; //Stores all coherency scores
    vector<int> detected_places_unfiltered;
    vector<int> detected_places; //Stores all detected place ids
    vector<SSG> SSGs; //Stores SSGs
    bool isProcessing;
    bool stopProcessing;
    vector<string> img_files;

public:
    bool frameByFrameProcess;
    bool process_next_frame;


private:
    QPen pen;
    int cursor;

signals:
    void showSSG(QImage img);
    void showImg1(QImage img);
    void showImg2(QImage img);

public slots:

};

#endif // PLACEDETECTION_H
