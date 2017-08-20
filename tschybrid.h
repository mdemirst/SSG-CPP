#ifndef TSCHYBRID_H
#define TSCHYBRID_H

#include <QObject>
#include "segmenttrack.h"
#include "placedetection.h"
#include "qcustomplot.h"
#include "TSC/utility.h"
#include "recognition.h"
//#include "opencv2/nonfree/features2d.hpp"
#include "TSC/tsc.h"
#include <queue>
#include "databasehandler.h"

enum{
    DETECTION_NOT_STARTED,
    DETECTION_PLACE_STARTED,
    DETECTION_PLACE_ENDED,
    DETECTION_IN_PLACE,
    DETECTION_IN_TRANSITION
};

using namespace std;

class TSCHybrid : public QObject
{
    Q_OBJECT
public:
    SegmentTrack* seg_track;
    Recognition* recognition;
    QCustomPlot* tsc_plot;
    QCustomPlot* ssg_plot;
    QCustomPlot* place_map;
    QCustomPlot* tsc_avg_plot;
    Parameters* params;
    Dataset* dataset;
    int cursor;
    vector<string> img_files;
    vector<SSG> SSGs; //Stores SSGs
    vector<SSG> SSGs_second_visit; //Stores SSGs
    vector<SSG> SSGs_fromTSC; //Stores SSGs
    vector<cv::Point2f> coords;
    vector<PlaceSSG> places_unprocessed;
    TSC* tsc;
    bool is_processing;
    bool stop_processing;
    bool next_frame;
    bool perform_recognition;
    bool save2database;

    TSCHybrid(QCustomPlot* tsc_plot,
              QCustomPlot* ssg_plot,
              QCustomPlot* place_map,
              QCustomPlot* tsc_avg_plot,
              Parameters* params,
              Dataset* dataset);
    ~TSCHybrid();
    void processImagesHierarchical(const string folder, const int start_idx, const int end_idx, int dataset_id);
    void processImagesHierarchicalVPC(const string folder, const int start_idx, const int end_idx, int dataset_id);
    void processImagesHierarchicalTSC(const string folder, const int start_idx, const int end_idx, int dataset_id);
    void TSCPlaces2SSGPlaces(vector<Place>& TSC_places, vector<SSG>& SSG_places);
    void createDatabase(const string folder, const int start_idx, const int end_idx, int dataset_id);
    bool eventFilter( QObject* watched, QEvent* event );
    void stopProcessing();
    void autoTryParameters();
    void reRecognizeInOrderTSCMethod(int method);
    void reRecognizeInOrder(int method);
    void reRecognizeBatch(int method);
    void readFromDB();
    float calcCohScore(SegmentTrack* seg_track, vector<float>& coh_scores);
    void plotScoresSSGOneShot(vector<float> coherency_scores, vector<int> detected_places, bool reset = false);
    void plotScoresSSG(vector<float> coherency_scores, vector<int> detected_places, vector<int>& vpc, cv::Point2f coord = cv::Point2f(0,0), bool = false);
    void plotScoresTSC(vector<float> scores, Place* current_place, BasePoint cur_base_point, vector<Place> detected_places, vector<int>& tsc_detected_places);
    void plotScoresTSC_(vector<float> scores, Place* current_place, BasePoint cur_base_point, vector<Place> detected_places);
    void plotAvgScoresTSC(vector<float>& scores, std::vector<BasePoint> wholebasepoints, vector<float>& avg_scoress);
    void plotDetectedPlaces(vector<SSG> SSGs, const vector<string>& image_files, Dataset* dataset);
    void plotDetectedPlacesX(vector<SSG> SSGs, const vector<string>& image_files);
    void showMap(const Mat& M);
    int  detectPlace(vector<float> coherency_scores,
                     vector<int>& detected_places_unfiltered,
                     vector<int>& detected_places);
    void clearPastData();
    void reRecognizeInOrderSecondVisits(int method);
    void plotDetectedPlacesBD(vector<SSG> SSGs, const vector<string>& image_files, Dataset* dataset);

public slots:
    
signals:
    void showSSG(QImage img);
    void showImgOrg(QImage img);
    void showMap(QImage img);
    
};

#endif // TSCHYBRID_H
