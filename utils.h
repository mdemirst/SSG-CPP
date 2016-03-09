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

#endif // UTILS_H
