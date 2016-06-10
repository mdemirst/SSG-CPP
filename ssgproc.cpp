#include "ssgproc.h"
#include "TSC/imageprocess/imageprocess.h"
#include "TSC/bubble/bubbleprocess.h"


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

#ifdef BOW_APPROACH_USED
    //BOW hist
    if(ns.second == 0)
        ns.first.bow_hist = new_ns.bow_hist;
    else
        ns.first.bow_hist = ns.first.bow_hist * ns.second +
                        new_ns.bow_hist;

    ns.first.bow_hist /= (float)ns.second + 1;
#endif


    //Edges

    //Increment
    ns.second += 1;
}

void SSGProc::updateSSGInvariants(SSG& ssg, Mat& current_image, Parameters* params)
{
    Mat gray_image;

    cvtColor(current_image, gray_image, CV_BGR2GRAY);

    Mat hue_channel = ImageProcess::generateChannelImage(current_image, 0, TSC_SAT_LOWER, TSC_SAT_UPPER, TSC_SAT_LOWER, TSC_VAL_UPPER);
    Mat hue_channel_filtered;
    cv::medianBlur(hue_channel, hue_channel_filtered, 3);
    vector<bubblePoint> hue_bubble = bubbleProcess::convertGrayImage2Bub(hue_channel_filtered, FOCAL_LENGHT_PIXELS, 180);
    vector<bubblePoint> hue_bubble_red = bubbleProcess::reduceBubble(hue_bubble);


    Mat val_channel= ImageProcess::generateChannelImage(current_image, 2, TSC_SAT_LOWER, TSC_SAT_UPPER, TSC_SAT_LOWER, TSC_VAL_UPPER);
    vector<bubblePoint> val_bubble = bubbleProcess::convertGrayImage2Bub(val_channel, FOCAL_LENGHT_PIXELS, 255);
    vector<bubblePoint> val_bubble_red = bubbleProcess::reduceBubble(val_bubble);

    bubbleStatistics stats_val =  bubbleProcess::calculateBubbleStatistics(val_bubble_red, 255);

    //qDebug() << stats_val.mean << stats_val.variance;
    //If frame is not informative, return..
    if(stats_val.mean <=  params->tsc_params.tau_mu || stats_val.variance <= params->tsc_params.tau_sigma)
    {
        //qDebug() << "Uninformative" << stats_val.mean << stats_val.variance;
        return;
    }


    vector<Mat> filter_outputs = ImageProcess::applyFilters(gray_image);

    Mat invariants_total, invariants_total_log;

    DFCoefficients df_coeff = bubbleProcess::calculateDFCoefficients(hue_bubble_red, NR_HARMONICS, NR_HARMONICS);
    Mat hue_invariants = bubbleProcess::calculateInvariantsMat(df_coeff, NR_HARMONICS, NR_HARMONICS);
    invariants_total = hue_invariants.clone();


    for(uint i = 0; i < filter_outputs.size(); i++)
    {
        vector<bubblePoint> img_bubble = bubbleProcess::convertGrayImage2Bub(filter_outputs[i], FOCAL_LENGHT_PIXELS, 255);

        vector<bubblePoint> img_bubble_red = bubbleProcess::reduceBubble(img_bubble);

        DFCoefficients df_coeff =  bubbleProcess::calculateDFCoefficients(img_bubble_red, NR_HARMONICS, NR_HARMONICS);

        Mat invariants = bubbleProcess::calculateInvariantsMat(df_coeff, NR_HARMONICS, NR_HARMONICS);

//        if(i == 0)
//            invariants_total = invariants.clone();
//        else
//        {
//            hconcat(invariants_total, invariants, invariants_total);
//        }
        cv::hconcat(invariants_total, invariants, invariants_total);
    }
    //qDebug() << invariants_total.size().width << invariants_total.size().height;


    cv::log(invariants_total,invariants_total_log);
    invariants_total_log = invariants_total_log / 25;
    cv::transpose(invariants_total_log,invariants_total_log);

    for(int i = 0; i < invariants_total_log.rows; i++)
    {
        if(invariants_total_log.at<float>(i,0) < 0)
            invariants_total_log.at<float>(i,0) = 0.5;
    }

    if(ssg.member_invariants.empty())
    {
        ssg.member_invariants = invariants_total_log;
    }
    else
    {
        hconcat(ssg.member_invariants, invariants_total_log, ssg.member_invariants);
    }

    cv::reduce(ssg.member_invariants, ssg.mean_invariant, 1, CV_REDUCE_AVG);

}

void SSGProc::updateSSGInvariantsFromDB(SSG& ssg, Mat& current_image, Parameters* params, FrameDesc& frame_desc)
{
    Mat gray_image;

    cvtColor(current_image, gray_image, CV_BGR2GRAY);


    bubbleStatistics stats_val =  frame_desc.bs;

    //qDebug() << stats_val.mean << stats_val.variance;
    //If frame is not informative, return..
    if(stats_val.mean <=  params->tsc_params.tau_mu || stats_val.variance <= params->tsc_params.tau_sigma)
    {
        //qDebug() << "Uninformative" << stats_val.mean << stats_val.variance;
        return;
    }


    Mat invariants_total_log = frame_desc.bd;

    if(ssg.member_invariants.empty())
    {
        ssg.member_invariants = invariants_total_log;
    }
    else
    {
        hconcat(ssg.member_invariants, invariants_total_log, ssg.member_invariants);
    }

    cv::reduce(ssg.member_invariants, ssg.mean_invariant, 1, CV_REDUCE_AVG);

}

//Updates SSG
void SSGProc::updateSSG(SSG& ssg, vector<NodeSig>& ns, Mat& map_col)
{
    //If SSG contains less number of elements
    //then initialize
    for(int i = ssg.nodes.size(); i < map_col.size().height; i++)
    {
        NodeSig ns;
        pair<NodeSig, int> new_node(ns, 0);
        ssg.nodes.push_back(new_node);
    }

    //For each node that appears at the last frame(inspected using map),
    //add weighted version of it to SSG
    for(int i = 0; i < map_col.size().height; i++)
    {
        //If ith node appears at the last frame
        //then, add weighted
        if(map_col.at<int>(i, 0) > 0)
        {
            updateNodeSig(ssg.nodes[i], ns[map_col.at<int>(i, 0)]);
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
Mat SSGProc::drawSSG(SSG& ssg, Mat &input)
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

        float rad = sqrt(ssg.nodes[i].first.area/M_PI)/10.0;
        circle(bg_img, ssg.nodes[i].first.center, rad,
               Scalar(ssg.nodes[i].first.colorR, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorB),-1);
        circle(bg_img, ssg.nodes[i].first.center, rad,
               Scalar(0,0,0),1);
    }


    for(int i = 0; i < ssg.nodes.size(); i++)
    {
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

