#ifndef TSCHYBRID_H
#define TSCHYBRID_H

#include <QObject>
#include "segmenttrack.h"
#include "placedetection.h"
#include "qcustomplot.h"
#include "TSC/utility.h"

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
              GraphMatchParams* gm_params);
    void processImages(const string folder, const int start_idx, const int end_idx);
    SegmentTrack* seg_track;
    bool eventFilter( QObject* watched, QEvent* event );


private:
    QCustomPlot* tsc_plot;
    QCustomPlot* ssg_plot;
    QCustomPlot* place_map;
    QCustomPlot* tsc_avg_plot;
    SSGParams* params;
    SegmentTrackParams* seg_track_params;
    SegmentationParams* seg_params;
    GraphMatchParams* gm_params;
    int cursor;
    vector<string> img_files;
    float calcCohScore(SegmentTrack* seg_track, vector<float>& coh_scores);
    void plotScoresSSG(vector<float> coherency_scores, vector<int> detected_places, cv::Point2f coord);
    void plotScoresTSC(vector<float> scores, Place* current_place, BasePoint cur_base_point, vector<Place> detected_places, vector<int>& tsc_detected_places);
    void plotAvgScoresTSC(vector<float>& scores, std::vector<BasePoint> wholebasepoints, vector<float>& avg_scoress);
    void showMap(const Mat& M);
    int  detectPlace(vector<float> coherency_scores,
                     vector<int>& detected_places_unfiltered,
                     vector<int>& detected_places);
    void plotEsensPlaces();

public slots:
    
signals:
    void showSSG(QImage img);
    void showImgOrg(QImage img);
    void showMap(QImage img);
    
};

#endif // TSCHYBRID_H
