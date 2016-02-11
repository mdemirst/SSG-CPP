#include "placedetection.h"
#include "utils.h"
#include "unistd.h"

#include "rag.h"

PlaceDetection::PlaceDetection(QCustomPlot* coherency_plot)
{
    seg_track = new SegmentTrack();

    // Coherency plot related settings
    pen.setWidth(PEN_WIDTH);
    pen.setColor(Qt::red);

    this->coherency_plot = coherency_plot;

    for(int i = 0; i <= PLOT_PLACES_IDX; i++)
        this->coherency_plot->addGraph();

    this->coherency_plot->graph(PLOT_THRES_IDX)->setPen(pen);
    this->coherency_plot->setMaximumWidth(COH_PLOT_W);
    this->coherency_plot->setMinimumWidth(COH_PLOT_W);

    this->coherency_plot->setMinimumHeight(COH_PLOT_H);
    this->coherency_plot->setMaximumHeight(COH_PLOT_H);

    this->coherency_plot->yAxis->setTickLabels(false);
    this->coherency_plot->plotLayout()->setAutoMargins(QCP::msNone);

    QMargins plot_margin(COH_PLOT_MARGIN,0,COH_PLOT_MARGIN,0);
    this->coherency_plot->plotLayout()->setMargins(plot_margin);

    // Parameter initialization
    tau_n = INIT_TAU_N;
    tau_c = INIT_TAU_C;
    tau_f = INIT_TAU_F;
    tau_d = INIT_TAU_D;

    coeff_node_disappear1 = COEFF_NODE_DISAPPEAR_1;
    coeff_node_disappear2 = COEFF_NODE_DISAPPEAR_2;
    coeff_node_appear = COEFF_NODE_APPEAR;
    coeff_coh_exp_base = COEFF_COH_EXP_BASE;
    coeff_coh_appear_thres = COEFF_COH_APPEAR_THRES;

    frameByFrameProcess = false;
    isProcessing = false;
}

// Plots places and coherency scores
void PlaceDetection::plotPlaces(vector<float> coherency_scores, vector<int> detected_places)
{
    static int graph_idx = PLOT_PLACES_IDX;
    if(detected_places.size() > 1)
    {

        //Place->Transition or vice versa region change occur
        if(graph_idx == PLOT_PLACES_IDX || ( detected_places.back() != detected_places.rbegin()[1]) )
        {
            //For visualization purposes
            if(detected_places.rbegin()[1] > 0)
            {
                this->coherency_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->coherency_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 50)));
                coherency_plot->graph(graph_idx)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]);
            }
            else
            {
                this->coherency_plot->graph(graph_idx)->setPen(QPen(Qt::red));
                this->coherency_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 50)));
                coherency_plot->graph(graph_idx)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]);
            }

            //Add new graph to change color
            this->coherency_plot->addGraph();
            graph_idx++;

            if(detected_places.back() > 0)
            {
                this->coherency_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->coherency_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 50)));
            }
            else
            {
                this->coherency_plot->graph(graph_idx)->setPen(QPen(Qt::red));
                this->coherency_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 50)));
            }
        }

        //Add the new point
        coherency_plot->graph(graph_idx)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]);
        //Add thresold line
        coherency_plot->graph(PLOT_THRES_IDX)->addData(detected_places.size(),tau_c);

        // set axes ranges, so we see all data:
        coherency_plot->yAxis->setRange(*min_element(coherency_scores.begin(),coherency_scores.end()), *max_element(coherency_scores.begin(),coherency_scores.end()));
        coherency_plot->xAxis->setRange(0, detected_places.size()+1);

        coherency_plot->replot();
    }
}

float PlaceDetection::calcCohScore(Mat& M, vector<pair<NodeSig, int> > M_ns, vector<float>& coh_scores)
{
    //Wait until at least tau_w frames
    if(M.size().width < seg_track->tau_w)
        return 0.0;
    //Fill first tau_w frames with zeros
    else if(M.size().width == seg_track->tau_w)
        coh_scores.resize(seg_track->tau_w/2, 0);

    int active_nodes = 0;
    float coh_score = 0;

    float incoherency = 0.0;
    float coherency = 0.0;


    //Detect each appeared or disappeared node at the last tau_w frame
    for(int i = 0; i < M.size().height; i++)
    {
        //Keeps how long node appeared through window
        int nr_appear = 0;

        for(int j = M.size().width-seg_track->tau_w; j < M.size().width-1; j++)
        {
            //Incoherency: Disappeared node
            if(M.at<int>(i, j) == 0 && M.at<int>(i, j+1) > 0)
            {
                if(M_ns[i].second > tau_f)
                    incoherency += coeff_node_disappear1;
                else
                    incoherency += coeff_node_disappear2;
            }

            //Incoherency: Appeared node
            if(M.at<int>(i, j) == 0 && M.at<int>(i, j+1) > 0)
            {
                    incoherency += coeff_node_appear;
            }

            //Coherency: Stabile node
            if(M.at<int>(i,j) > 0)
                nr_appear++;
        }

        //Active nodes are only the ones that appeared at least one time through window
        if(nr_appear > 0)
            active_nodes++;

        //If node appeared at least certain percent then it's a stabile node.
        if((float)nr_appear/seg_track->tau_w > coeff_coh_appear_thres)
            coherency += (float)nr_appear/seg_track->tau_w;
    }

    // Coherency score calculation ( -1 < coh_range < 1)
    // coherency_score = (1 - a^(-coherency)) - (incoherency)
    coh_score = 1-pow(coeff_coh_exp_base,-1*coherency) - (incoherency/active_nodes);

    //Put new score into vector
    coh_scores.push_back(coh_score);

    return coh_score;
}

int PlaceDetection::getMedian(vector<int> v)
{
    std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
    return v[v.size()/2];
}

// returns true iff all the elements of v are identical
bool PlaceDetection::getRegionStatus(vector<int> v)
{
    for(int i = 0; i < v.size()-1; i++)
    {
        if(v[i] != v[i+1])
        {
            return false;
        }
    }

    return true;
}

//Detect places based on coherency scores and puts places ids to
//detected_places vector.
//Returns 1 if new place is started
//Returns -1 if transition is started
int  PlaceDetection::detectPlace(vector<float> coherency_scores,
                                  vector<int>& detected_places_unfiltered,
                                  vector<int>& detected_places)
{
    if(coherency_scores.size() == 0)
        return 0;

    //Pre-assign last frame as either transition or place candidate
    detected_places_unfiltered.push_back(coherency_scores.back() < tau_c ? 1 : -1);


    //Place/transition region is decided according to median of
    //last tau_w frames. Therefore wait minimum number of
    //frames to be larger than tau_w
    if(detected_places_unfiltered.size() >= tau_n)
    {
        //For the first iteration, fill detected_places with a region type
        if(detected_places.size() == 0)
        {
            detected_places.push_back(detected_places_unfiltered.front());
        }

        int last_place_type = detected_places.back();

        bool region_status = getRegionStatus(vector<int>(detected_places_unfiltered.end()-tau_n,
                                                         detected_places_unfiltered.end()));


        //If last tau_w frames are same type
        if(region_status == 1)
        {
            //last frame is place and next frame is transition
            if(detected_places.back() > 0 && detected_places_unfiltered.back() < 0)
            {
                int min_id = *min_element(detected_places.begin(),detected_places.end());
                min_id = min(-1,min_id);
                detected_places.push_back(min_id-1);
            }
            //last frame is transition and next frame is place
            else if(detected_places.back() < 0 && detected_places_unfiltered.back() > 0)
            {
                int max_id = *max_element(detected_places.begin(),detected_places.end());
                max_id = max(1,max_id);
                detected_places.push_back(max_id+1);
            }
            //last frame and next frame is same type
            else
            {
                detected_places.push_back(detected_places.back());
            }
        }
        else
        {
            detected_places.push_back(detected_places.back());
        }

        //Place type changed
        if(last_place_type != detected_places.back())
        {
            if(detected_places.back() < 0)
                return -1;
            else
                return 1;
        }
        else
            return 0;
    }
    else
    {
        return 0;
    }
}

void PlaceDetection::processImages()
{
    static Mat img1, img2;
    Mat img_seg1, img_seg2;
    vector<NodeSig> ns1, ns2;
    vector<vector<NodeSig> > ns_vec;  //Stores last tau_w node signatures
    float matching_cost = 0;

    bool in_place = false; //Flag whether we are in place or transition

    isProcessing = true;

    //Process all images
    for(int frame_no = START_IDX; frame_no < END_IDX-1; frame_no++)
    {
        //Wait until process_next_frame set to true
        while(frameByFrameProcess == true && process_next_frame == false)
        {
            waitKey(1);
        }
        //Set it back to false
        process_next_frame = false;

        //getFilePath returns the path of the file given frame number
        string filepath1 = getFilePath(DATASET_FOLDER, DATASET_NO, IMG_FILE_PREFIX, frame_no);
        string filepath2 = getFilePath(DATASET_FOLDER, DATASET_NO, IMG_FILE_PREFIX, frame_no+1);

        //Segment images
        if(frame_no == START_IDX)
        {
            img2 = imread(filepath1);
            resize(img2, img2, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
            ns2 = seg_track->seg->segmentImage(img2,img_seg2);
        }
        ns1 = ns2;
        img1 = img2;
        img_seg1 = img_seg2;

        img2 = imread(filepath2);
        resize(img2, img2, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        ns2 = seg_track->seg->segmentImage(img2,img_seg2);


        //Show original images on the window
        //emit seg_track->shshowImg1(mat2QImage(img1));
        //emit seg_track->showImg2(mat2QImage(img2));

        if(frame_no == START_IDX)
        {
            //Initialize M matrix (Existence "M"ap)
            seg_track->M = Mat(ns1.size(), 1, CV_32S, -1);

            for(int i = 0; i < ns1.size(); i++)
            {
                seg_track->M.at<int>(i,0) = i; //Nodes are placed in order in the first column of M

                //Initialize avg node signatures list
                pair<NodeSig, int> new_node(ns1[i], 1);

                seg_track->M_ns.push_back(new_node);
            }

            //Insert first node sig for the first time
            ns_vec.push_back(ns1);
        }

        //Insert next node sig
        ns_vec.push_back(ns2);

        //Keep max size tau_w
        if(ns_vec.size() > seg_track->tau_w)
        {
            ns_vec.erase(ns_vec.begin());
        }

        //Drawing purposes only
        seg_track->gm->drawMatches(ns1, ns2, img_seg1, img_seg2);

        //Fill node existence map
        matching_cost = seg_track->fillNodeMap(seg_track->M, seg_track->M_ns, ns_vec);


        //Plot connectivity map
        seg_track->plotMap(seg_track->M);

        //Calculate coherency based on existence map
        calcCohScore(seg_track->M, seg_track->M_ns, coherency_scores);

        //Decide last frame is whether transition or place
        //Results are written into detected places
        int detection_result = detectPlace(coherency_scores,detected_places_unfiltered,detected_places);

        //If started for new place
        //Create new SSG
        if(detection_result == 1)
        {
            SSG new_ssg;
            SSGs.push_back(new_ssg);
            in_place = true;
        }
        else if(detection_result == -1)
        {
            in_place = false;
        }

        //Fill SSG if frame is as place
        if(in_place)
        {
            SSGProc::updateSSG(SSGs.back(), ns2, seg_track->M);
        }

        //Show last SSG
        if(SSGs.size() > 0)
        {
            img2 = imread(filepath2);
            //emit showSSG(mat2QImage(SSGProc::drawSSG(SSGs.back(), img2)));
        }

        //Plot transition and place regions
        plotPlaces(coherency_scores, detected_places);

        //Wait a little for GUI processing
        waitKey(1);
    }

    isProcessing = false;
}

void PlaceDetection::clearPastData()
{
    coherency_scores.clear(); //Stores all coherency score.clear();
    detected_places.clear();
    SSGs.clear(); //Stores SSGs
}


