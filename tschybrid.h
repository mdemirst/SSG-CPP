#ifndef TSCHYBRID_H
#define TSCHYBRID_H

#include <QObject>
#include "segmenttrack.h"
#include "placedetection.h"
#include "qcustomplot.h"
#include "TSC/utility.h"
#include "recognition.h"

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
    TSCHybrid(QCustomPlot* tsc_plot,
              QCustomPlot* ssg_plot,
              QCustomPlot* place_map,
              QCustomPlot* tsc_avg_plot,
              SSGParams* ssg_params,
              SegmentTrackParams* seg_track_params,
              SegmentationParams* seg_params,
              GraphMatchParams* gm_params,
              RecognitionParams* rec_params);
    ~TSCHybrid();
    void processImages(const string folder, const int start_idx, const int end_idx);
    void processImagesHierarchical(const string folder, const int start_idx, const int end_idx);
    SegmentTrack* seg_track;
    Recognition* recognition;
    bool eventFilter( QObject* watched, QEvent* event );
    bool is_processing;
    bool stop_processing;
    void stopProcessing();
    void autoTryParameters(vector<cv::Point2f> coords);



private:
    QCustomPlot* tsc_plot;
    QCustomPlot* ssg_plot;
    QCustomPlot* place_map;
    QCustomPlot* tsc_avg_plot;
    SSGParams* params;
    SegmentTrackParams* seg_track_params;
    SegmentationParams* seg_params;
    GraphMatchParams* gm_params;
    RecognitionParams* rec_params;
    int cursor;
    vector<string> img_files;
    vector<SSG*> SSGs; //Stores SSGs
    void calcCohScoreOneShot(SegmentTrack* seg_track, vector<float>& coh_scores,
                             vector<int>& detected_places_unfiltered,
                             vector<int>& detected_places);
    float calcCohScore(SegmentTrack* seg_track, vector<float>& coh_scores);
    void plotScoresSSGOneShot(vector<float> coherency_scores, vector<int> detected_places, vector<cv::Point2f> coords, bool reset);
    void plotScoresSSG(vector<float> coherency_scores, vector<int> detected_places, cv::Point2f coord, bool = false);
    void plotScoresTSC(vector<float> scores, Place* current_place, BasePoint cur_base_point, vector<Place> detected_places, vector<int>& tsc_detected_places);
    void plotAvgScoresTSC(vector<float>& scores, std::vector<BasePoint> wholebasepoints, vector<float>& avg_scoress);
    void showMap(const Mat& M);
    int  detectPlace(vector<float> coherency_scores,
                     vector<int>& detected_places_unfiltered,
                     vector<int>& detected_places);
    void plotEsensPlaces();
    void clearPastData();
    void findBestParameters();

public slots:
    
signals:
    void showSSG(QImage img);
    void showImgOrg(QImage img);
    void showMap(QImage img);
    
};

#endif // TSCHYBRID_H
