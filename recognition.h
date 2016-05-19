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
#include <QDate>


extern "C" {
#include "cluster.h"
}

using namespace std;

enum RecognitionStatus {RECOGNITION_ERROR, NOT_RECOGNIZED, RECOGNIZED};



/////////////////////////////////////////////
//Performs recognition on hierarchical tree//
//SSGs are used as tree elements           //
/////////////////////////////////////////////
class Recognition : public QObject
{
    Q_OBJECT
private:
    SSGParams* ssg_params;
    SegmentTrackParams* seg_track_params;
    SegmentationParams* seg_params;
    GraphMatchParams* gm_params;
    GraphMatch *gm;
    Segmentation *seg;
    RecognitionParams* rec_params;
    vector<string> img_files;
    int img_width;
    int img_height;
    float thumbnail_scale;
    int plot_offset;

    Node* solveSLink(int nrows, int ncols, double** data);
    void generateRAGs(const Node* tree, int nTree, vector<vector<NodeSig> >& rags, vector<Mat>& images);
    Mat saveRAG(const vector<NodeSig> ns, string name);
    void drawTree(TreeNode* root_node, int size, int height, int width);
    double** calculateDistanceMatrix(vector<PlaceSSG>& places);
    TreeNode* findNode(int label, TreeNode* root);
    TreeNode* convert2Tree(Node* tree, int nrNodes, int nrPlaces, vector<PlaceSSG>& places);
    void sortTerminalNodes(TreeNode* node, int* last_pos);
    void drawBranch(Mat& img, TreeNode* node, int height, double scale_x, double scale_y);
    void processTree(Node* tree, int size);
    void drawSSG(Mat& img, SSG ssg, Point coord);

public:
    Recognition(RecognitionParams* rec_params,
                SSGParams* ssg_params,
                SegmentTrackParams* seg_track_params,
                SegmentationParams* seg_params,
                GraphMatchParams* gm_params);
    ~Recognition();
    int performRecognition(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode* hierarchy);
    void testRecognition();
    bool next;

    
signals:
    void showTree(QImage img);
    void printMessage(QString str);
    
public slots:
    
};

#endif // RECOGNITION_H
