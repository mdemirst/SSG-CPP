#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <string.h>
#include <dirent.h>
#include "utilTypes.h"
#include "graphmatch.h"
#include "defs.h"
#include <QDir>
#include <csv.h>
#include <fstream>



using namespace cv;

QImage mat2QImage(const Mat& mat);
Mat vec2mat(vector< vector<int> > vec_matrix);
Mat vec2matInt(vector< vector<int> > vec_matrix);
Mat array2Mat32F(int** C, int rows, int cols);
Mat array2Mat8U(int** C, int rows, int cols);
int getPermuted(Mat& P, int j);
int getIndexByCol(Mat& M, int col, int val);
std::string getFilePath(std::string dir, std::string folder, std::string prefix, int frame_no);
void scaleUpMap(Mat &img, Mat &img_scaled, int factor_x, int factor_y);
std::vector<std::string> getFiles(std::string dir);
cv::Point2f getCoordCold(std::string filename);
cv::Point3f getCoordCold2(std::string filename);

int getMedian(vector<int> v);
bool getRegionStatus(vector<int> v);
string getOutputFolder(bool = false);
void savePlacesFrameInfo(vector<PlaceSSG>& places);

void readDatasets(string filename,
                  vector<Dataset>& datasets);

void saveParameters(string filename,
                    Parameters* params);

void readParameters(string filename,
                    Parameters* params);
int getMostCoherentFrame(vector<float> coh_scores, int start_frame, int end_frame, int start_idx);

void writeFrameSec(float sec);
void writeDetectedPlace(int results);
void saveNodeSigAsCSV(std::vector<NodeSig> ns_vec, string filename);
bool readNodeSigFromCSV(std::vector<NodeSig>& ns_vec, string filename);
std::map<string, string> getPlaceCategories(string filename);

class Statistics
{
public:
    vector<double> data;

    Statistics(vector<double> data)
    {
        this->data = data;
    }

    double getMean()
    {
        double sum = 0.0;
        for(int i = 0; i < data.size(); ++i)
            sum += data[i];
        return sum/data.size();
    }

    double getVariance()
    {
        double mean = getMean();
        double temp = 0;
        for(int i = 0; i < data.size(); ++i)
            temp += (data[i]-mean)*(data[i]-mean);
        return temp/(data.size()-1);
    }

    double getStdDev()
    {
        return sqrt(getVariance());
    }
};
#endif // UTILS_H
