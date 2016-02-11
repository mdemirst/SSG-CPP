#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "../bubble/bubbleprocess.h"
#include <opencv2/opencv.hpp>
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QFile>
#include <QDir>

//#define DB_PATH "/home/hakan/Development/ISL/Datasets/ImageClef2012/training3v2.db"
//#define INVARIANTS_DB_PATH "/home/hakan/Development/ISL/Datasets/ImageClef2012/invariants.db"

#define LASER_TYPE 55
#define HUE_TYPE 56
#define SAT_TYPE 57
#define VAL_TYPE 58

class Place;
class TemporalWindow;
class BasePoint;
class LearnedPlace;
class Level;
//class TopologicalMap;

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = 0);
  //  ~DatabaseManager();

    bool openDB(QString filePath);

    // For accessing multiple databases
    bool openDB(QString filePath, QString connectionName);

    void closeDB();

    bool deleteDB();

   bool isOpen();

    // Type = 55:Lazer, 56:Hue, XX:Filter_Number, Number: the current bubble number-> obtained from frame number
    bool insertRowtoBubble(int type, int number, bubblePoint row);

    bool insertRowtoBubble(int type, int number, int pan, int tilt, double val);

    bool insertBubble(int type,int number, std::vector<bubblePoint> bubble);

    bool insertInvariants(int type,int number, std::vector< std::vector<float> > invariants);

    bool insertBubbleStatistics(int type, int number, bubbleStatistics stats);

    std::vector<bubblePoint> readBubble(int type, int number);

    void determinePlaceLabels(QString filePath);

    bool insertTemporalWindow(const TemporalWindow& twindow);

    bool insertBasePoint(const BasePoint& basepoint);

    bool insertBasePoints(const std::vector<BasePoint> basepoints);

    bool insertPlace(const Place& place);

    bool insertLearnedPlace(const LearnedPlace& learnedplace);

    int getLearnedPlaceMaxID();

    LearnedPlace getLearnedPlace(int id);

    bool insertTopologicalMapRelation(int id, std::pair<int,int> relation);

    bool insertBDSTLevel(int id, const Level& aLevel);

  //  TopologicalMap getTopologicalMap(int id);

    cv::Mat getPlaceMeanInvariant(int id);

    Place getPlace(int id);

    cv::Mat getPlaceMemberIds(int id);

    QByteArray mat2ByteArray(const cv::Mat &image);

    cv::Mat byteArray2Mat(const QByteArray & byteArray);

   QSqlError lastError();

private:
  QSqlDatabase db;

    
signals:
    
public slots:
    
};

#endif // DATABASEMANAGER_H
