#ifndef PLACEDETECTION_H
#define PLACEDETECTION_H

#include "graphmatch.h"
#include "segmentation.h"
#include "qcustomplot.h"
#include "ssgproc.h"

#include <QObject>

#define DATASET_FOLDER "Dataset/"
#define IMG_FILE_PREFIX "image-"

#define DATASET_NO 25
#define START_IDX 1
#define END_IDX 300

//#define DATASET_NO 1
//#define START_IDX 1
//#define END_IDX 1500

#define PEN_WIDTH 5
#define IMG_RESCALE 1

#define INIT_TAU_N 1
#define INIT_TAU_C 20
#define INIT_TAU_W 7
#define INIT_TAU_M 0.015
#define INIT_TAU_F 30
#define INIT_TAU_D 1.0

#define COEFF_NODE_DISAPPEAR_1 2
#define COEFF_NODE_DISAPPEAR_2 0.5
#define COEFF_NODE_APPEAR 0.2
#define COEFF_COH_EXP_BASE 5
#define COEFF_COH_APPEAR_THRES 0.80

#define MAP_FILTER_SIZE 3

#define REG_PLACE 1
#define REG_TRANS -1

#define PLOT_THRES_IDX 1
#define PLOT_PLACES_IDX 10

#define COH_PLOT_W 1000
#define COH_PLOT_H 250
#define COH_PLOT_MARGIN -15

#define EXISTENCE_MAP_W 1000
#define EXISTENCE_MAP_H 250

using namespace std;

class PlaceDetection : public QObject
{
    Q_OBJECT
public:
    PlaceDetection(QCustomPlot* coherency_plot);
    void processImages();
    GraphMatch* gm;
    Segmentation* seg;
    string getFilePath(int frame_no);
    QCustomPlot* coherency_plot;
    int detectPlace(vector<float> coherency_scores,
                    vector<int>& detected_places_unfiltered,
                    vector<int>& detected_places);
    void plotPlaces(vector<float> coherency_scores,
                    vector<int> detected_places);
    int getMedian(vector<int> v);
    bool getRegionStatus(vector<int> v);
    void fillCoherencyWindow(Mat& coh_win, Mat& assignment, Mat& cost);
    float fillNodeMap(Mat& M, vector<pair<NodeSig, int> >& M_ns, const vector<vector<NodeSig> > ns_vec);
    void plotMap(Mat& M);
    float calcCohScore(Mat& M, vector<pair<NodeSig, int> > M_ns, vector<float>& coh_scores);

public:
    int tau_n;
    float tau_c;
    int tau_w;
    float tau_m;
    int tau_f;
    float tau_d;

    float coeff_node_disappear1;
    float coeff_node_disappear2;
    float coeff_node_appear;
    float coeff_coh_exp_base;
    float coeff_coh_appear_thres;

    bool frameByFrameProcess;
    bool process_next_frame;

    int img_height;
    int img_width;


private:
    QPen pen;

signals:
    void showImg1(QImage img);
    void showImg2(QImage img);
    void showMap(QImage img);
    void showSSG(QImage img);

public slots:

};

#endif // PLACEDETECTION_H
