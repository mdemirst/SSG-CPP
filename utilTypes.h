#ifndef UTILTYPES_H
#define UTILTYPES_H

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>

using namespace std;

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
public:
    vector<pair<NodeSig, int> > nodes;
};


#endif // UTILTYPES_H
