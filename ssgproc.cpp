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
void SSGProc::updateSSG(SSG& ssg, vector<NodeSig>& ns, Mat& map)
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

#ifdef PROCESS_EDGES
    //Update edges - add edges in new ns to ssg
    for(int i = 0; i < ns.size(); i++)
    {
        NodeSig ns_ = ns[i];

        int source_node = -1;

        //find global id of node
        for(int k = 0; k < map.size().height; k++)
        {
            //If last column of map has local node id -> get global id of node
            if(map.at<int>(k, map.size().width-1) == i)
            {
                source_node = k;
            }

        }

        //if source node is placed in last column of map
        if(source_node != -1)
        {
            for(int j = 0; j < ns_.edges.size(); j++)
            {
                int target_node = -1;
                std::pair<int,float> edge = ns_.edges[j];

                //find target node edge points
                for(int k = 0; k < map.size().height; k++)
                {
                    //If last column of map has local node id -> get global id of node
                    if(map.at<int>(k,map.size().width-1) == edge.first)
                    {
                        target_node = k;
                    }
                }

                if(target_node != -1)
                {
                    //there is an edge from source_node -> target_node
                    std::pair<int,float> edge_;
                    edge_.first = target_node;
                    edge_.second = edge.second;

                    ssg.nodes[source_node].first.edges.push_back(edge_);
                }
            }
        }

    }
#endif

}

//Draws given SSG structure on image
Mat SSGProc::drawSSG(SSG& ssg, Mat &input, float tau_p)
{
    Mat bg_img = Mat::zeros(input.size(), CV_8UC3);
    bg_img = Scalar(255,255,255);

    for(int i = 0; i < ssg.nodes.size(); i++)
    {
#ifdef PROCESS_EDGES
        //Draw edges
        for(int j = 0; j < ssg.nodes[i].first.edges.size(); j++)
        {
            //If both two connecting nodes appear in SSG
            //
            int idx = ssg.nodes[i].first.edges[i].first;
            if(ssg.nodes.size() > idx && ssg.nodes[idx].second > longestApp*tau_p)
            {
                Point p1 = ssg.nodes[i].first.center;
                Point p2 = ssg.nodes[idx].first.center;

                line(bg_img,p1,p2,Scalar(0,0,0),2);
            }
        }
#endif

        float img_area = bg_img.size().height*bg_img.size().width;
        float rad = sqrt(ssg.nodes[i].first.area/M_PI)/10.0;
        circle(bg_img, ssg.nodes[i].first.center, rad,
               Scalar(ssg.nodes[i].first.colorR, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorB),-1);
        circle(bg_img, ssg.nodes[i].first.center, rad,
               Scalar(0,0,0),1);
    }


    for(int i = 0; i < ssg.nodes.size(); i++)
    {
        float img_area = bg_img.size().height*bg_img.size().width;
        float rad = sqrt(ssg.nodes[i].first.area/M_PI)/2.0;
        circle(bg_img, ssg.nodes[i].first.center, rad,
               Scalar(ssg.nodes[i].first.colorR, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorB),-1);
        circle(bg_img, ssg.nodes[i].first.center, rad,
               Scalar(0,0,0),1);
    }

    //Save SSG
    string outName = getOutputFolder()+QString::number(ssg.getId()).toStdString()+".jpg";
    imwrite(outName, bg_img);


    return bg_img;
}


void SSGProc::filterSummarySegments(SSG& ssg, float tau_p)
{
    //Find longest appear
    int longestApp = 0;
    for(int i = 0; i < ssg.nodes.size(); i++)
    {
        if(ssg.nodes[i].second > longestApp)
            longestApp = ssg.nodes[i].second;
    }


    for(int i = 0; i < ssg.nodes.size();)
    {
        if(ssg.nodes[i].second < longestApp*tau_p)
        {
            ssg.nodes.erase(ssg.nodes.begin()+i);
        }
        else
            i++;
    }
}

