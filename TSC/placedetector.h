#ifndef PLACEDETECTOR_H
#define PLACEDETECTOR_H

#include "utility.h"
#include "database/databasemanager.h"
#include <QString>

class PlaceDetector
{
public:

    PlaceDetector();
    PlaceDetector(int tau_w, int tau_n, int tau_p);
    float processImage();
    double compareHKCHISQR(Mat input1, Mat input2);
    bool shouldProcess;
    bool isProcessing;
    bool debugMode;
    bool usePreviousMemory;
    std::string previousMemoryPath;
    std::string debugFilePath;
    QString imagesPath;
    int debugFileNo;
    int tau_w;
    int tau_n;
    int tau_p;
    double tau_avgdiff;
    double tau_inv;
    int image_width;
    int image_height;
    int focalLengthPixels;
    double tau_val_mean;
    double tau_val_var;
    int satLower;
    int satUpper;
    int valLower;
    int valUpper;
    int noHarmonics;

    cv::Mat currentImage;
    uint image_counter;
    int  twindow_counter;

    Place* currentPlace;
    vector<Place> detectedPlaces;
    uint placeID;

    std::vector<BasePoint> wholebasepoints;

    DatabaseManager* dbmanager;

private:

    TemporalWindow* tempwin;
    int img_counter;
    BasePoint previousBasePoint;
    BasePoint currentBasePoint;
    std::vector<BasePoint> basepointReservoir;
};

#endif // PLACEDETECTOR_H
