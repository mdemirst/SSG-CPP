#ifndef PIPELINE_H
#define PIPELINE_H

#include <QObject>
#include "segmenttrack.h"
#include "placedetection.h"
#include "qcustomplot.h"
#include "TSC/utility.h"
#include "recognition.h"
#include <queue>
#include "databasehandler.h"
#include "utilTypes.h"

#include "experimentalData.h"
#include "GraphSegmentation/ColoredSegments/coloredSegments.h"
#include "VPC_toolkit/VPC.h"
#include "VPC_toolkit/VPC.h"


namespace pipeline
{
enum{
    DETECTION_NOT_STARTED,
    DETECTION_PLACE_STARTED,
    DETECTION_PLACE_ENDED,
    DETECTION_IN_PLACE,
    DETECTION_IN_TRANSITION
};
}

enum class DBUsage
{
  DONT_USE_DB,
  READ_FROM_DB,
  SAVE_TO_DB
};


class Pipeline: public QObject
{
  Q_OBJECT
public:
  QCustomPlot* ssg_plot;

  SegmentTrack* seg_track;

  Parameters params;
  Dataset dataset;
  bool is_processing;
  bool stop_processing;


public:
  Pipeline(Parameters* params,
           Dataset* dataset,
           QCustomPlot* ssg_plot);
  ~Pipeline();
  void processImages(DBUsage db_usage);
  float calcCohScore(SegmentTrack* seg_track, vector<float>& coh_scores);
  void showMap(const Mat& M);
  void plotScoresSSG(vector<float> coherency_scores, vector<int> detected_places, vector<FrameInfo>& frame_infos, bool reset);

  int  detectPlace(vector<float> coherency_scores,
                   vector<int>& detected_places_unfiltered,
                   vector<int>& detected_places);


public slots:

signals:
    void showSSG(QImage img);
    void showImgOrg(QImage img);
    void showMap(QImage img);

};

#endif // PIPELINE_H
