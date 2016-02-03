#ifndef BUBBLEPROCESS_H
#define BUBBLEPROCESS_H
//#include "globals.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <QFile>
#include <QRgb>
#include <opencv2/opencv.hpp>

class point{
public:
    long r;
    double theta; // laser beam angle in radians
    double phi; // tilt angle (pantilt System)
    double rho; //pan angle (pantilt System)
    double x;
    double y;
    double z;

    QRgb color;
    double red;
    double green;
    double blue;

    int px; // x pixel value of the point
    int py; // y pixel value of the point
};

struct bubblePoint{

    int panAng;
    int tiltAng;
    double val;


};
struct bubbleStatistics
{
    // mean of the bubble Surface
    double mean;

    // variance of the bubble Surface
    double variance;

    // maximum value of the bubbble point
    double maxDist;

};
struct bubblePointXYZ{

    double x;
    double y;
    double z;


};

// A structure for holding the odometry data
struct positionData{

    double x;
    double y;
    double z;

    double headingD;


};
struct DFCoefficients
{
    std::vector< std::vector<float> > a;
    std::vector< std::vector<float> > b;
    std::vector< std::vector<float> > c;
    std::vector< std::vector<float> > d;

};

//struct bubblePointXYZ;
//struct bubblePoint;

using std::vector;

class bubbleProcess
{
public:
    
	bubbleProcess();

    static void calculateDFCoefficients(const std::vector <bubblePoint>& bubble, QString path, QString fileName, int itemNo,int harmonic1, int harmonic2);

    static DFCoefficients calculateDFCoefficients(const std::vector <bubblePoint>& bubble, int harmonic1, int harmonic2);

    static std::vector< std::vector< float > > calculateInvariants(const std::vector <bubblePoint>& bubble, QString path, QString fileName, int itemNo, int harmonic1, int harmonic2);

    static std::vector< std::vector< float > > calculateInvariants(DFCoefficients coeff, int harmonic1, int harmonic2);

    static  cv::Mat calculateInvariantsMat(DFCoefficients coeff, int harmonic1, int harmonic2);

    static std::vector<double> calculateEuclideanDiff(vector <bubblePoint> bubble1, vector <bubblePoint> bubble2);

    void setBubblesXYZ(std::vector< std::vector<bubblePointXYZ> > bubblesXYZ);

    std::vector< std::vector<bubblePointXYZ> > getBubblesXYZ();

    static bubbleStatistics calculateBubbleStatistics(const vector<bubblePoint>& bubble, float maxDist);

    // Add a single bubble to our bubble vector
    static void addToBubbles(std::vector< bubblePoint > aBubble);

    // Read position data file
    static vector<positionData> readPositionData(QFile* file);

    // Set position data
    static void setPositionData(vector<positionData> poseVec);

    // Get position data
    static vector <positionData> getPositionData();

    // Set bubbles
    static void setBubbles(std::vector< std::vector<bubblePoint> > bubbles);

    // Get bubbles
    static vector< vector<bubblePoint> > getBubbles();

    // Round double to int
    static double round(double num);

    // Set bubbles root directory
	static void setBubblesRootDirectory(QString rootDirectory);

    // Get bubbles root directory
	static QString getBubblesRootDirectory();



    //// This setter getter is used for setting and getting the folder list of bubble root folder
    static void setBubblesFolderList(QStringList folderList);

    static QStringList getBubblesFolderList();

	
	static void set3DFilesRootDirectory(QString rootDirectory);

	static QString get3DFilesRootDirectory();

	
	static void setWriteBubblesRootDirectory(QString rootDirectory);

	static QString getWriteBubblesRootDirectory();

	
	
	static void setBubblesFileList(QStringList fileList);
	
	static QStringList getBubblesFileList();

	static void set3DFileList(QStringList fileList);
	
	static QStringList get3DFileList();

	// Reduces the number of points in a bubble by combining points falling in the same patch
	static vector<bubblePoint> reduceBubble(vector<bubblePoint>bubble); 
	
	// Converts given laser Points to bubble Points
	static vector<bubblePoint> convertlP2bP(vector<point> laserPoints, double panAng, double tiltAng); 
	
	// Saves the bubble
	static void saveBubble(QFile* file, vector<bubblePoint> bubble); 

    // Reads the bubble
	static vector<bubblePoint> readBubble(QFile* file); 

	// Converts bubble in XYZ coordinates to a bubble in spherical coordinates
    static vector<bubblePoint> convertBubXYZ2BubSpherical(const vector<point>& bubbleXYZ, double maxRange);

    // Converts bubble in XYZ coordinates to a bubble in spherical coordinates
    static vector<bubblePoint> convertBubXYZ2BubSpherical(const vector<bubblePointXYZ>& bubbleXYZ, double maxRange);

	// Converts bubble in XYZ coordinates to a bubble in spherical coordinates including heading information
    static vector<bubblePoint> convertBubXYZ2BubSpherical(const vector<point>& bubbleXYZ, int heading, double maxRange);

    static vector<bubblePointXYZ> convertBubSph2BubXYZ(vector <bubblePoint> bubble, double maxRange);

    static vector<bubblePoint> convertGrayImage2Bub(cv::Mat grayImage, int focalLengthPixels, int maxval);

    static vector<vector <int> > calculateImagePanAngles(int focalLengthPixels,int imageWidth,int imageHeight);

    static vector<vector <int> > calculateImageTiltAngles(int focalLengthPixels,int imageWidth,int imageHeight);

private:
	
    vector< vector<bubblePointXYZ> > bubblesXYZ;

    vector< vector<bubblePoint> > bubbles;


};

#endif // BUBBLEPROCESS_H
