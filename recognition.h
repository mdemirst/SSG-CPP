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


enum RecognitionMethod {REC_TYPE_SSG_NORMAL, REC_TYPE_SSG_VOTING, REC_TYPE_BD_NORMAL, REC_TYPE_BD_COLOR, REC_TYPE_BD_COLOR_LOG, REC_TYPE_BD_VOTING};
/////////////////////////////////////////////
//Performs recognition on hierarchical tree//
//SSGs are used as tree elements           //
/////////////////////////////////////////////
class Recognition : public QObject
{
    Q_OBJECT
public:
    Parameters* params;
    Dataset* dataset;
    GraphMatch *gm;
    Segmentation *seg;
    vector<string> img_files;
    int plot_offset;
    int rec_method;

public:
    Recognition(Parameters* params,
                Dataset* dataset,
                GraphMatch* gm,
                Segmentation* seg);
    ~Recognition();
    int performRecognition(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy);
    int performRecognitionHybrid(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy);
    void calculateRecPerformance(TreeNode* root);
    void calculateRecPerformanceWithHValues(TreeNode* root);
    void calculateN2NTreeDistanceMatrix(TreeNode* root);
    void calculateSSGDistanceMatrix(TreeNode* root);
    void calculateBDDistanceMatrix(TreeNode* root);
    void testRecognition();
    void setRecognitionMethod(int method);
    double getDistance(PlaceSSG& p1, PlaceSSG& p2);
    void calculateN2NDistanceMatrix(TreeNode* root_node);
    PlaceSSG* mergeSSGs(PlaceSSG* p1, PlaceSSG* p2, int id);
    double** calculateDistanceMatrix(vector<PlaceSSG>& places);
    void calculateRecPerformanceDist2Match(vector<PlaceSSG>& places, TreeNode* root);
    void calculateRecPerformanceHybridWithoutTree(vector<PlaceSSG>& places);
    vector<vector<int> > calculateBestSSGCandidates(vector<PlaceSSG>& places);
    Node* solveSLink(int nrows, int ncols, double** data);
    void generateRAGs(const Node* tree, int nTree, vector<vector<NodeSig> >& rags, vector<Mat>& images);
    Mat saveRAG(const vector<NodeSig> ns, string name);
    void drawTree(TreeNode* root_node, int size, int height, int width);
    double calculateDistanceSSGVoting(SSG& old_place, SSG& detected_place);
    double calculateDistanceTSC(SSG& old_place, SSG& detected_place);
    TreeNode* findNodeWithPlaceLabel(int label, TreeNode* root);
    TreeNode* findNodeWithSSGLabel(int site, int label, TreeNode* root);
    TreeNode** convert2Tree(Node* tree, int nrNodes, int nrPlaces, vector<PlaceSSG>& places);
    void sortTerminalNodes(TreeNode* node, int* last_pos);
    void drawBranch(Mat& img, TreeNode* node, int height, double scale_x, double scale_y);
    void processTree(Node* tree, int size);
    void drawSSG(Mat& img, SSG ssg, Point coord);
    void drawSSGWithImages(Mat& img, SSG ssg, Point coord);
    void drawInnerSSG(Mat& img, SSG ssg, Point coord);
    int calculateN2NTreeDistance(TreeNode* node1, TreeNode* node2);
    void getTerminalNodes(TreeNode* node, vector<TreeNode*>& terminal_nodes);

    
signals:
    void showTree(QImage img);
    void printMessage(QString str);
    
public slots:
    
};

#endif // RECOGNITION_H
