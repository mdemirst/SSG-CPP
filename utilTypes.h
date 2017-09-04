#ifndef UTILTYPES_H
#define UTILTYPES_H

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include "TSC/bubble/bubbleprocess.h"

using namespace std;
using namespace cv;



class SegmentationParams
{
public:
    float sigma;
    float k;
    int min_size;
};

class SSGParams{
public:
    int img_org_width;
    int img_org_height;
    int img_width;
    int img_height;
    int tau_n;
    float tau_c;
    int tau_f;
    float tau_d;
    float tau_p;
    float coeff_node_disappear1;
    float coeff_node_disappear2;
    float coeff_node_appear;
    float coeff_coh_exp_base;
    float coeff_coh_appear_thres;
};

class RecognitionParams
{
public:
    float tau_r;
    int plot_h;
    int plot_w;
    int ssg_h;
    int ssg_w;
    float tau_v;
};

class GraphMatchParams
{
public:
    float pos_weight;
    float color_weight;
    float area_weight;
    float bow_weight;
};

class SegmentTrackParams{
public:
    int tau_w;
    float tau_m;
};

class TSCParams{
public:
    float tau_mu;
    float tau_sigma;

};

class Parameters{
public:
    SSGParams ssg_params;
    RecognitionParams rec_params;
    GraphMatchParams gm_params;
    SegmentTrackParams seg_track_params;
    SegmentationParams seg_params;
    TSCParams tsc_params;
};

class Dataset{
public:
    Dataset(){}
    Dataset(string location, int start_idx, int end_idx, int dataset_id);
    int nr_files;
    int start_idx;
    int end_idx;
    int dataset_id;
    string location;
    vector<string> files;
    Parameters params;
};

typedef struct
{
    int pixelsSize;
    float avgHue;
    float avgSat;
    float avgVal;
    float avgR;
    float avgG;
    float avgB;
    float centerX;
    float centerY;
    std::vector<cv::Point> pixels;
    cv::Mat img;
    cv::Mat bow_hist;
}BlobStats;

class NodeSig
{
    public:
    int id;
    cv::Point center;
    float colorR;
    float colorG;
    float colorB;
    int   area;
    std::vector<std::pair<int,float> > edges;
    NodeSig()
    {
        colorR = 0;
        colorG = 0;
        colorB = 0;
        area = 0;
        center = cv::Point(0,0);
        id = 0;
    }
    cv::Mat bow_hist;

    static void write2File(NodeSig node, std::string filename)
    {
        std::ofstream file;
        filename.append("_graph.txt");
        file.open(filename.c_str(), std::ios::app);
        file << node.id << endl;
        file << node.center.x << endl;
        file << node.center.y << endl;
        file << node.colorR << endl;
        file << node.colorG << endl;
        file << node.colorB << endl;
        file << node.area << endl;
        file << node.edges.size() << endl;
        for (unsigned int i = 0; i < node.edges.size(); i++)
        {
            file << node.edges[i].first << " " << node.edges[i].second << endl;
        }
        file.close();
    }
    static void resetGraphFile(int nrNodes, std::string filename)
    {
        ofstream file;
        filename.append("_graph.txt");
        file.open(filename.c_str());
        file << nrNodes << endl;
        file.close();
    }
};

class SSG
{
    int id;
    int start_frame;
    int end_frame;
    string sample_frame;
    Point2f sample_coord;
    int color;
public:
    SSG(int id){this->id = id;}
    SSG(int id, vector<NodeSig> nodes)
    {
        this->id = id;
        for(int i = 0; i < nodes.size(); i++)
            this->nodes.push_back(make_pair(nodes[i],1));
    }
    int getId(){return id;}
    void setId(int id){this->id = id;}
    void setStartFrame(int frame){start_frame = frame;}
    void setEndFrame(int frame){end_frame = frame;}
    int getStartFrame(){return start_frame;}
    int getEndFrame(){return end_frame;}
    void setSampleFrame(string sample_frame){this->sample_frame = sample_frame;}
    string getSampleFrame(){return sample_frame;}
    void setSampleCoord(Point2f sample_coord){this->sample_coord = sample_coord;}
    Point2f getSampleCoord(){return sample_coord;}
    void setColor(int color){this->color = color;}
    int getColor(){return color;}
public:
    vector<pair<NodeSig, int> > nodes;
    vector<vector<NodeSig> > basepoints;
    Mat member_invariants;
    Mat mean_invariant;
};

class RAG_m
{
public:
    RAG_m(){}
    RAG_m(vector<NodeSig>& nodes){this->nodes = nodes;}
    vector<NodeSig> nodes;
};


///////////////////////////////
//BasePoint Class Declaration//
///////////////////////////////

class BasePointSSG
{
  public:
    int number;
    string filename;
    vector<NodeSig> rag;

    BasePointSSG(){}
    BasePointSSG(int number, string filename, vector<NodeSig>& rag)
    {
        this->number = number;
        this->filename = filename;
        this->rag = rag;
    }
    int getNumber(){return number;}
    string getFilename(){return filename;}
    vector<NodeSig>& getRAG(){return rag;}
};

///////////////////////////
//Place Class Declaration//
///////////////////////////

class PlaceSSG
{
    int id;
    vector<SSG> members;
public:
    PlaceSSG(int id, SSG member)
    {
        this->id = id;
        this->members.push_back(member);
    }
    void addMember(SSG member){this->members.push_back(member);}
    SSG& getMember(int member_id)
    {
        return members[member_id];
    }
    int getCount() {return members.size();}
    int getId() {return id;}
};

//////////////////////////////
//TreeNode Class Declaration//
//////////////////////////////

class TreeNode
{
    int label;
    double val;
    double x_pos;
    TreeNode* parent;
    vector<TreeNode*> children;
    PlaceSSG* descriptor;

public:
    TreeNode();
    TreeNode(int label, double val);
    void setLabel(int label){this->label = label;}
    int getLabel(){return label;}
    void setVal(double val){this->val = val;}
    double getVal(){return val;}
    void setXPos(double x_pos){this->x_pos = x_pos;}
    double getXPos(){return x_pos;}
    void setParent(TreeNode* parent){this->parent = parent;}
    TreeNode* getParent(){return parent;}
    void addChild(TreeNode* node);
    vector<TreeNode*>& getChildren();
    void setDescriptor(PlaceSSG* descriptor){this->descriptor = descriptor;}
    PlaceSSG* getDescriptor(){return descriptor;}
    bool isTerminal(){return children.size() == 0;}
    bool isRoot(){return parent == NULL;}

};

class FrameDesc
{
public:
    int frame_nr;
    string filename;
    Mat bd;
    bubbleStatistics bs;
    vector<NodeSig> ns;
};

struct FrameInfo
{
  double translation;
  double rotation;
  string cat;
  double transition_prob;
  string frame_path;
};

#endif // UTILTYPES_H
