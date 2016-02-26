#ifndef PLACEDETECTION_H
#define PLACEDETECTION_H

#include "defs.h"
#include "qcustomplot.h"
#include "ssgproc.h"
#include "segmenttrack.h"

#include <QObject>

using namespace std;

class PlaceDetection : public QObject
{
    Q_OBJECT
public:
    PlaceDetection(QCustomPlot* coherency_plot);
    void processImages();
    SegmentTrack* seg_track;
    QCustomPlot* coherency_plot;
    int detectPlace(vector<float> coherency_scores,
                    vector<int>& detected_places_unfiltered,
                    vector<int>& detected_places);
    void plotPlaces(vector<float> coherency_scores,
                    vector<int> detected_places);
    int getMedian(vector<int> v);
    bool getRegionStatus(vector<int> v);
    float calcCohScore(Mat& M, vector<pair<NodeSig, int> > M_ns, vector<float>& coh_scores);
    void clearPastData();
    void constructSceneGist(Mat& M, vector<pair<NodeSig, int> > M_ns);

public:
    vector<float> coherency_scores; //Stores all coherency scores
    vector<int> detected_places_unfiltered;
    vector<int> detected_places; //Stores all detected place ids
    vector<SSG> SSGs; //Stores SSGs
    bool isProcessing;

public:
    int tau_n;
    float tau_c;
    int tau_f;
    float tau_d;

    float coeff_node_disappear1;
    float coeff_node_disappear2;
    float coeff_node_appear;
    float coeff_coh_exp_base;
    float coeff_coh_appear_thres;

    bool frameByFrameProcess;
    bool process_next_frame;


private:
    QPen pen;

signals:
    void showSSG(QImage img);

public slots:

};

#endif // PLACEDETECTION_H
