#ifndef TSC_H
#define TSC_H

#include "bubble/bubbleprocess.h"
#include "imageprocess/imageprocess.h"
#include "database/databasemanager.h"
#include "placedetector.h"
#include "utility.h"
#include "qcustomplot.h"
#include "defs.h"
#include <opencv2/opencv.hpp>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include "utilTypes.h"


class TSC : public QObject
{
    Q_OBJECT
public:
    TSC(Dataset* dataset);
    float processImage(const Mat& cur_img, bool isLastImage);
    bool createDirectories(QString previousMemoryPath);
    bool saveParameters(QString filepath);
    double compareHKCHISQR(Mat input1, Mat input2);
    void writeInvariant(cv::Mat inv, int count);
    double compareHistHK( InputArray _H1, InputArray _H2, int method );

public:
    PlaceDetector detector;
    DatabaseManager dbmanager;
    std::vector<BasePoint> basepoints;

    QString mainDirectoryPath;
    QString imagesPath;
    
signals:
    
public slots:
    
};

#endif // TSC_H
