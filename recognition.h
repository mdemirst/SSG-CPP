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


enum RecognitionMethod {REC_TYPE_SSG_NORMAL, REC_TYPE_SSG_VOTING, REC_TYPE_BD_NORMAL, REC_TYPE_BD_COLOR, REC_TYPE_BD_COLOR_LOG, REC_TYPE_BD_VOTING, REC_TYPE_HYBRID};
/////////////////////////////////////////////
//Performs recognition on hierarchical tree//
//SSGs are used as tree elements           //
/////////////////////////////////////////////
class Recognition : public QObject
{
    Q_OBJECT
private:
    Parameters* params;
    Dataset* dataset;
    GraphMatch *gm;
    Segmentation *seg;
    vector<string> img_files;
    float thumbnail_scale;
    int plot_offset;
    int rec_method;
    int norm_type;

    Node* solveSLink(int nrows, int ncols, double** data);
    void generateRAGs(const Node* tree, int nTree, vector<vector<NodeSig> >& rags, vector<Mat>& images);
    Mat saveRAG(const vector<NodeSig> ns, string name);
    void drawTree(TreeNode* root_node, int size, int height, int width);
    double** calculateDistanceMatrix(vector<PlaceSSG>& places);
    double calculateDistance(SSG& old_place, SSG& detected_place);
    double calculateDistanceTSC(SSG& old_place, SSG& detected_place);
    TreeNode* findNode(int label, TreeNode* root);
    TreeNode* findNode(int site, int label, TreeNode* root);
    TreeNode** convert2Tree(Node* tree, int nrNodes, int nrPlaces, vector<PlaceSSG>& places);
    void sortTerminalNodes(TreeNode* node, int* last_pos);
    void drawBranch(Mat& img, TreeNode* node, int height, double scale_x, double scale_y);
    void processTree(Node* tree, int size);
    void drawSSG(Mat& img, SSG ssg, Point coord);
    void drawSSG2(Mat& img, SSG ssg, Point coord);
    void drawInnerSSG(Mat& img, SSG ssg, Point coord);
    int calculateN2NTreeDistance(TreeNode* node1, TreeNode* node2);
    void getTerminalNodes(TreeNode* node, vector<TreeNode*>& terminal_nodes);



public:
    Recognition(Parameters* params,
                Dataset* dataset,
                GraphMatch* gm,
                Segmentation* seg);
    ~Recognition();
    int performRecognition(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy);
    int performRecognition2(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy);
    void calculateRecPerformance(TreeNode* root);
    void testRecognition();
    void setRecognitionMethod(int method);
    void setNormType(int method);
    double getDistance(PlaceSSG& p1, PlaceSSG& p2);
    void calculateN2NDistanceMatrix(TreeNode* root_node);
    PlaceSSG* mergeSSGs(PlaceSSG* p1, PlaceSSG* p2);
    bool next;

    
signals:
    void showTree(QImage img);
    void printMessage(QString str);
    
public slots:
    
};

#endif // RECOGNITION_H
