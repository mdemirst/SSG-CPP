#include "rag.h"

RAG::RAG()
{
    nrNodes = 0;
    nrEdges = 0;
}

int RAG::addNode(RAGNode node)
{
    nodes.push_back(node);
    return nrNodes++;
}

int RAG::addEdge(RAGEdge edge)
{
    edges.push_back(edge);

    nodes[edge.firstNode].neighs.push_back(edge.secondNode);
    nodes[edge.secondNode].neighs.push_back(edge.firstNode);
    return nrEdges++;
}

int RAG::deleteNode(int id)
{
    nodes.erase(nodes.begin()+id);
    return nrNodes--;
}

int RAG::deleteEdge(int id)
{
    edges.erase(edges.begin()+id);
    return nrEdges--;
}

RAGNode RAG::getNode(int id)
{
    return nodes[id];
}

RAGEdge RAG::getEdge(int id)
{
    return edges[id];
}

int RAG::getNrNodes()
{
    return nrNodes;
}

int RAG::getNrEdges()
{
    return nrEdges;
}


