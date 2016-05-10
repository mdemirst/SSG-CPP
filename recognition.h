#ifndef RECOGNITION_H
#define RECOGNITION_H

#include <QObject>
#include <QDebug>
#include "segmenttrack.h"
#include "placedetection.h"
#include "utilTypes.h"
#include <string>
#include <sstream>
#include "TSC/utility.h"
#include <QThread>

extern "C" {
#include "cluster.h"
}

using namespace std;

class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

enum RecognitionStatus {RECOGNITION_ERROR, NOT_RECOGNIZED, RECOGNIZED};

class TreeLeaf
{
public:
    int left;
    int right;
    float val;
    int parentConnection;
    bool isused;
};


class BDST : public QObject
{
    Q_OBJECT
public:
    QList<Level> levels;

signals:

public slots:

};



class Recognition : public QObject
{
    Q_OBJECT
public:
    Recognition(SSGParams* ssg_params,
                SegmentTrackParams* seg_track_params,
                SegmentationParams* seg_params,
                GraphMatchParams* gm_params);
    Node* solveSLink(int nrows, int ncols, double** data);
    void generateRAGs(const Node* tree, int nTree, vector<vector<NodeSig> >& rags, vector<Mat>& images);
    Mat saveRAG(const vector<NodeSig> ns, string name);
    void drawTree(TreeNode* root_node, int size, int height, int width);
    void testRecognition();
    int performRecognition(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode* hierarchy, double tau_r);
    double** calculateDistanceMatrix(vector<PlaceSSG>& places);
    TreeNode* findNode(int label, TreeNode* root);
    TreeNode* convert2Tree(Node* tree, int nrNodes, int nrPlaces, vector<PlaceSSG>& places);
    void sortTerminalNodes(TreeNode* node, int* last_pos);
    void drawBranch(Mat& img, TreeNode* node, int height, double scale_x, double scale_y);
    void processTree(Node* tree, int size);

    SSGParams* ssg_params;
    SegmentTrackParams* seg_track_params;
    SegmentationParams* seg_params;
    GraphMatchParams* gm_params;
    GraphMatch *gm;
    Segmentation *seg;
    vector<string> img_files;
    vector<vector<NodeSig> > ns_all;
    int img_width;
    int img_height;
    bool next;

    
signals:
    void showTree(QImage img);
    void printMessage(QString str);
    
public slots:
    
};

#endif // RECOGNITION_H
