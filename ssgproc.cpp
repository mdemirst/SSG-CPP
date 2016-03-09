#include "ssgproc.h"


SSGProc::SSGProc()
{

}

// Given a node signature and its weight, it's value is updated
// with new node signature values.
void SSGProc::updateNodeSig(pair<NodeSig, int>& ns, NodeSig new_ns)
{
    //Areas
    ns.first.area = ns.first.area * ns.second +
                              new_ns.area;

    ns.first.area /= ns.second + 1;


    //Color - R
    ns.first.colorR = ns.first.colorR * ns.second +
                              new_ns.colorR;

    ns.first.colorR /= ns.second + 1;

    //Color - G
    ns.first.colorG = ns.first.colorG * ns.second +
                              new_ns.colorG;

    ns.first.colorG /= ns.second + 1;

    //Color - B
    ns.first.colorB = ns.first.colorB * ns.second +
                              new_ns.colorB;

    ns.first.colorB /= ns.second + 1;

    //Center
    ns.first.center.x = ns.first.center.x * ns.second +
                              new_ns.center.x;
    ns.first.center.y = ns.first.center.y * ns.second +
                              new_ns.center.y;

    ns.first.center.x /= (float)ns.second + 1;
    ns.first.center.y /= (float)ns.second + 1;


    //Edges

    //Increment
    ns.second += 1;
}

//Updates SSG
void SSGProc::updateSSG(SSG& ssg, vector<NodeSig> ns, Mat& map)
{
    int last_frame = map.size().width-1;//0 based frame no

    //If SSG contains less number of elements
    //then initialize
    for(int i = ssg.nodes.size(); i < map.size().height; i++)
    {
        NodeSig ns;
        pair<NodeSig, int> new_node(ns, 0);
        ssg.nodes.push_back(new_node);
    }

    //For each node that appears at the last frame(inspected using map),
    //add weighted version of it to SSG
    for(int i = 0; i < map.size().height; i++)
    {
        //If ith node appears at the last frame
        //then, add weighted
        if(map.at<int>(i, last_frame) > 0)
        {
            updateNodeSig(ssg.nodes[i], ns[map.at<int>(i, last_frame)]);
        }
    }
}

//Draws given SSG structure on image
Mat SSGProc::drawSSG(SSG& ssg, Mat &input)
{
    Mat bg_img = Mat::zeros(input.size(), CV_8UC3);
    bg_img = Scalar(255,255,255);

    //Find longest appear
    int longestApp = 0;
    for(int i = 0; i < ssg.nodes.size(); i++)
    {
        if(ssg.nodes[i].second > longestApp)
            longestApp = ssg.nodes[i].second;
    }

    for(int i = 0; i < ssg.nodes.size(); i++)
    {
        if(ssg.nodes[i].second > longestApp/2.0)
        {
            float img_area = bg_img.size().height*bg_img.size().width;
            float rad = sqrt(ssg.nodes[i].first.area/M_PI)/2.0;
            circle(bg_img, ssg.nodes[i].first.center, rad,
                   Scalar(ssg.nodes[i].first.colorR, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorB),-1);
            circle(bg_img, ssg.nodes[i].first.center, rad,
                   Scalar(0,0,0),1);
        }
    }

    return bg_img;
}
