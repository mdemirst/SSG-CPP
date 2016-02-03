#ifndef RAG_H
#define RAG_H

#include <iostream>
#include <vector>

struct RAGNode{
    int pixelsSize;
    float avgHue;
    float avgSat;
    float avgVal;
    float avgR;
    float avgG;
    float avgB;
    float centerX;
    float centerY;
    std::vector<int> neighs;
};

struct RAGEdge{
    int firstNode;
    int secondNode;
    float weight;
};

class RAG
{

public:
    RAG();
    std::vector<RAGNode> nodes;
    std::vector<RAGEdge> edges;
    int addNode(RAGNode node);
    int addEdge(RAGEdge edge);
    int deleteNode(int id);
    int deleteEdge(int id);
    RAGNode getNode(int id);
    RAGEdge getEdge(int id);
    int getNrNodes();
    int getNrEdges();
    int nrNodes;
    int nrEdges;
};



#endif // RAG_H
