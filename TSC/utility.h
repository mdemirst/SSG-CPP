#ifndef UTILITY_H
#define UTILITY_H
#include <opencv2/opencv.hpp>

using namespace cv;

// STATUS OF BASEPOINT
// 0: normal
// 1: uninformative
// 2: incoherent


class BasePoint
{
public:
    uint id;
    float avgVal;
    float varVal;
    float avgLas;
    float varLas;
    cv::Mat invariants;
    int status;
};

class Place
{
public:
    Place();
    Place(int id);
    uint id;
    cv::Mat memberIds;
    std::vector<BasePoint> members;
    Mat memberInvariants;
    Mat meanInvariant;
    void calculateMeanInvariant();
};

class LearnedPlace
{
public:
    LearnedPlace();
    LearnedPlace(int id);
    uint id;
    // This is a Mx1 vector that will store merged place ids
    cv::Mat memberPlaces;
    // This will be MXN matrix instead of Mx1 because multiple places can be merged in one learned place
    cv::Mat memberIds;
   // std::vector <std::vector<BasePoint> > members;
    Mat memberInvariants;
    Mat meanInvariant;
    void calculateMeanInvariant();
};

class TemporalWindow
{
public:
    TemporalWindow();
    bool checkExtensionStatus(uint currentID);
    int id;
    int startPoint;
    int endPoint;
    int tau_w;
    int tau_n;
    std::vector<BasePoint> members;
    std::vector<BasePoint> cohMembers;
    // Holds the value of total incoherency inside the temporal window
    float totalDiff;
};

class TopologicalMap
{
public:
    std::vector< std::pair<int,int> > connections;
};

class Level
{
public:
    std::vector<int> members;
    std::vector<int> parentNodes;
    double val;
    //  Connection index bize max parent node numarasini veriyor. Bu node bu noktadan bagli
    int connectionIndex;
    // Mean Invariant bize bu level in ortalama invariant vectorunu veriyor
    std::vector<float> meanInvariant;
};

#endif // UTILITY_H
