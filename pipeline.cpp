#include "pipeline.h"


Pipeline::Pipeline(Parameters* params,
                   Dataset* dataset,
                   QCustomPlot* ssg_plot)
{

  this->dataset = *dataset;
  this->params = *params;
  this->ssg_plot = ssg_plot;

  seg_track = new SegmentTrack(params,dataset);

  QPen pen1, pen2, pen3, pen4;

  pen1.setWidth(PEN_WIDTH1);
  pen1.setColor(Qt::red);

  pen2.setWidth(PEN_WIDTH2);
  pen2.setColor(Qt::black);

  pen3.setWidth(PEN_WIDTH2);
  pen3.setColor(Qt::red);

  pen4.setWidth(PEN_WIDTH3);
  pen4.setColor(Qt::green);

  QMargins plot_margin(COH_PLOT_MARGIN,0,COH_PLOT_MARGIN,0);

  //Settings for ssg plot
  for(int i = 0; i <= PLOT_PLACES_IDX; i++)
      this->ssg_plot->addGraph();

  this->ssg_plot->graph(PLOT_TRACK_IDX)->setPen(pen4);
  this->ssg_plot->graph(PLOT_TRACK_IDX)->setLineStyle(QCPGraph::lsNone);
  this->ssg_plot->graph(PLOT_TRACK_IDX)->setScatterStyle(QCPScatterStyle::ssDisc);

  this->ssg_plot->graph(PLOT_THRES_IDX)->setPen(pen1);
  this->ssg_plot->graph(PLOT_SCORES_IDX)->setPen(pen2);
  this->ssg_plot->setMaximumWidth(COH_PLOT_W);
  this->ssg_plot->setMinimumWidth(COH_PLOT_W);
  this->ssg_plot->setMinimumHeight(COH_PLOT_H);
  this->ssg_plot->setMaximumHeight(COH_PLOT_H);
  this->ssg_plot->yAxis->setTickLabels(false);
  this->ssg_plot->plotLayout()->setAutoMargins(QCP::msNone);
  this->ssg_plot->plotLayout()->setMargins(plot_margin);

  this->ssg_plot->graph(PLOT_VPC_IDX)->setPen(pen2);
  this->ssg_plot->graph(PLOT_VPC_IDX)->setLineStyle(QCPGraph::lsNone);
  this->ssg_plot->graph(PLOT_VPC_IDX)->setScatterStyle(QCPScatterStyle::ssCircle);

  this->ssg_plot->graph(PLOT_VPC_IDX+1)->setPen(pen2);
  this->ssg_plot->graph(PLOT_VPC_IDX+1)->setLineStyle(QCPGraph::lsNone);
  this->ssg_plot->graph(PLOT_VPC_IDX+1)->setScatterStyle(QCPScatterStyle::ssCircle);

  is_processing = false;
  stop_processing = false;
}

Pipeline::~Pipeline()
{
  delete seg_track;
}



double getDist(cv::Point3f a, cv::Point3f b)
{
  return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}

double getDistAlpha(cv::Point3f a, cv::Point3f b)
{
  return fabs(a.z - b.z);
}

vector<FrameInfo> getFrameInfos(string path)
{
  vector<FrameInfo> frame_infos;

  vector<string> img_files = getFiles(path+"/std_cam");
  map<string,string> place_cats = getPlaceCategories(path+"/localization/places.lst");

  vector<int> distance_from_transition;

  int dist = 0;
  for(int i = 0; i < img_files.size()-1; ++i)
  {
    if(place_cats[img_files[i]] == place_cats[img_files[i+1]])
    {
      dist++;
      //same place
      distance_from_transition.push_back(dist);
    }
    else
    {
      dist = 0;
      distance_from_transition.push_back(dist);
    }
  }
  vector<int> distance_from_transition_rev;
  distance_from_transition_rev.resize(distance_from_transition.size());

  dist = 0;
  for(int i = 0; i < img_files.size()-1; ++i)
  {
    if(place_cats[img_files[img_files.size()-1-i]] == place_cats[img_files[img_files.size()-2-i]])
    {
      dist++;
      //same place
      distance_from_transition_rev[distance_from_transition_rev.size()-1-i] = (dist);
    }
    else
    {
      dist = 0;
      distance_from_transition_rev[distance_from_transition_rev.size()-1-i] = (dist);
    }
  }

  vector<int> distance_from_transition_merged;

  for(int i = 0; i < distance_from_transition.size(); ++i)
  {
    distance_from_transition_merged.push_back( min(distance_from_transition[i],distance_from_transition_rev[i]));
  }

  for(int i = 0; i < img_files.size()-1; ++i)
  {
    FrameInfo frame_info;

    frame_info.frame_path = img_files[i];
    frame_info.transition_prob = exp(-0.1*distance_from_transition_merged[i]);
    frame_info.translation = getDist(getCoordCold2(img_files[i]), getCoordCold2(img_files[i+1]));
    frame_info.cat = place_cats[img_files[i]];
    frame_info.rotation = getDistAlpha(getCoordCold2(img_files[i]), getCoordCold2(img_files[i+1]));

    frame_infos.push_back(frame_info);
  }

  return frame_infos;
}

void Pipeline::processImages(DBUsage db_usage)
{
    is_processing = true;

    vector<string> img_files = getFiles(dataset.location+"/std_cam");
    //map<string,string> place_cats = getPlaceCategories(dataset.location+"/localization/places.lst");
    vector<FrameInfo> frame_infos = getFrameInfos(dataset.location);

    Mat img_org, img;

    //SSG related variables
    static vector<vector<NodeSig> > ns_vec;  //Stores last tau_w node signatures
    static vector<float> coherency_scores_ssg;   //Stores all coherency scores
    static vector<int> detected_places_unfiltered;
    static vector<int> detected_places; //Stores all detected place ids

    static int frame_count = 0;

    //Create database
    DatabaseHandler db;
    if(db_usage == DBUsage::READ_FROM_DB || db_usage == DBUsage::SAVE_TO_DB)
    {
        stringstream ss_db;
        ss_db << "conn_db_" << dataset.dataset_id;
        db.setConnName(ss_db.str());
        stringstream ss;
        ss << OUTPUT_FOLDER << "dataset_" << dataset.dataset_id << "_places.db";
        db.openDB(ss.str());
    }

    if(db_usage == DBUsage::SAVE_TO_DB)
    {
      db.createTables();
    }


    //Process all images
    for(int frame_no = dataset.start_idx; frame_no < dataset.end_idx-1; frame_no++)
    {
        if(stop_processing)
        {
          stop_processing = false;
          break;
        }

//        cv::Point2f coord;
//        coord = getCoordCold(dataset.location+"/std_cam/" + img_files[frame_no]);

        if(db_usage == DBUsage::READ_FROM_DB)
        {
          vector<NodeSig> ns;
          BasePointSSG basepoint;
          db.getBasepoint(frame_no, basepoint);

          seg_track->processImageFromNS(basepoint.rag,ns_vec);
        }
        else
        {
          img_org = imread(dataset.location+"/std_cam" + "/" + img_files[frame_no]);

          resize(img_org, img, cv::Size(params.ssg_params.img_width, params.ssg_params.img_height));

          seg_track->processImage(img, ns_vec);
        }


        if(db_usage == DBUsage::SAVE_TO_DB)
        {
          BasePointSSG basepoint;
          basepoint.filename = dataset.location + "/" + img_files[frame_no];
          basepoint.number = frame_no;
          basepoint.rag = ns_vec.back();
          db.insertBasepoint(basepoint);
        }


        //Calculate coherency based on existence map
        calcCohScore(seg_track, coherency_scores_ssg);

        int time_depth;
        int items_size;
        //Mat transition_map = Mat::zeros(time_depth, items_size);

//        if(seg_track->getM().size().width >= time_depth)
//        {

//        }

        cout << frame_infos[frame_no].cat << " " << frame_infos[frame_no].transition_prob << " " << frame_infos[frame_no].translation << endl;

        //Show connectivity map
        showMap(seg_track->getM());

        //Decide last frame is whether transition or place
        //Results are written into detected places
        int detection_result = detectPlace(coherency_scores_ssg,detected_places_unfiltered,detected_places);

        //Plot transition and place regions
        plotScoresSSG(coherency_scores_ssg, detected_places, frame_infos, false);

        //If started for new place
        //Create new SSG
        if(detection_result == pipeline::DETECTION_PLACE_STARTED)
        {
        }
        else if(detection_result == pipeline::DETECTION_PLACE_ENDED)
        {
        }
        else if(detection_result == pipeline::DETECTION_IN_PLACE)
        {
        }

        frame_count++;

        //Wait a little for GUI processing
        waitKey(1);
    }

    if(db_usage == DBUsage::READ_FROM_DB || db_usage == DBUsage::SAVE_TO_DB)
    {
      db.closeDB();
    }

    is_processing = false;
}

// Plots places and coherency scores
void Pipeline::plotScoresSSG(vector<float> coherency_scores, vector<int> detected_places, vector<FrameInfo>& frame_infos, bool reset)
{
    static int graph_idx = PLOT_PLACES_IDX;
    static int thres_cursor = 0;

    if(reset)
    {
        graph_idx = PLOT_PLACES_IDX;
        thres_cursor = 0;
    }
    float margin = 1.0;
    if(detected_places.size() > 1)
    {
        //Place->Transition or vice versa region change occur
        if(graph_idx == PLOT_PLACES_IDX || ( detected_places.back() != detected_places.rbegin()[1]) )
        {
            //Add new graph to change color
            this->ssg_plot->addGraph();
            //this->place_map->addGraph();
            graph_idx++;

            if(detected_places.back() > 0)
            {
                //Place
                this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::red));
                this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 80)));

                QPen dumPen;
                dumPen.setWidth(1);
                dumPen.setColor(QColor(255, 0, 0, 50));
            }
            else
            {
                //Transition
                this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 80)));

                QPen dumPen;
                dumPen.setWidth(3);
                dumPen.setColor(QColor(0, 0, 255, 50));
            }
        }

        //Add the new point
        ssg_plot->graph(graph_idx)->addData(detected_places.size(),1.0+margin);

        //Add coh score
        ssg_plot->graph(PLOT_SCORES_IDX)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]+margin);

    }

    //Add thresold line
    ssg_plot->graph(PLOT_THRES_IDX)->addData(thres_cursor,params.ssg_params.tau_c+margin);
    ssg_plot->graph(PLOT_ODOM_IDX)->addData(thres_cursor,10*frame_infos[coherency_scores.size()].rotation);
    thres_cursor++;

    ssg_plot->rescaleAxes();
    ssg_plot->replot();
}

float Pipeline::calcCohScore(SegmentTrack* seg_track, vector<float>& coh_scores)
{
    Mat M = seg_track->getM();
    vector<pair<NodeSig, int> > M_ns = seg_track->getM_ns();
    float tau_w = params.seg_track_params.tau_w;

    //Wait until at least tau_w frames
    if(M.size().width < tau_w)
        return 0.0;
    //Fill first tau_w frames with zeros
    else if(M.size().width == tau_w)
        coh_scores.resize(tau_w/2, 0);

    int active_nodes = 0;
    float coh_score = 0;

    float incoherency = 0.0;
    float coherency = 0.0;


    //Detect each appeared or disappeared node at the last tau_w frame
    for(int i = 0; i < M.size().height; i++)
    {
        //Keeps how long node appeared through window
        int nr_appear = 0;

        for(int j = M.size().width-tau_w; j < M.size().width-1; j++)
        {
            //Incoherency: Disappeared node
            if(M.at<int>(i, j) == 0 && M.at<int>(i, j+1) > 0)
            {
                if(M_ns[i].second > params.ssg_params.tau_f)
                    incoherency += params.ssg_params.coeff_node_disappear1;
                else
                    incoherency += params.ssg_params.coeff_node_disappear2;
            }

            //Incoherency: Appeared node
            if(M.at<int>(i, j) == 0 && M.at<int>(i, j+1) > 0)
            {
                    incoherency += params.ssg_params.coeff_node_appear;
            }

            //Coherency: Stabile node
            if(M.at<int>(i,j) > 0)
                nr_appear++;
        }

        //Active nodes are only the ones that appeared at least one time through window
        if(nr_appear > 0)
            active_nodes++;

        //If node appeared at least certain percent then it's a stabile node.
        if((float)nr_appear/tau_w > params.ssg_params.coeff_coh_appear_thres)
            coherency += (float)nr_appear/tau_w;
    }

    // Coherency score calculation ( -1 < coh_range < 1)
    // coherency_score = (1 - a^(-coherency)) - (incoherency)
    coh_score = 1-pow(params.ssg_params.coeff_coh_exp_base,-1*coherency) - (incoherency/active_nodes);

    //cout << coh_score << endl;
    coh_scores.push_back(coh_score);

    return coh_score;
}

// Plots node existence map
void Pipeline::showMap(const Mat& M)
{
    Mat img;
    // For drawing purposes convert positive values to 255, zero to 0.
    // Resulting drawing will be black and white existence map drawing
    M.convertTo(img, CV_8U, 255, 0);

    if(img.size().height < EXISTENCE_MAP_H)
    {
        copyMakeBorder(img,img,0,EXISTENCE_MAP_H-img.size().height,0,0,BORDER_CONSTANT,0);
    }

    resize(img, img, Size(EXISTENCE_MAP_W, EXISTENCE_MAP_H),INTER_LINEAR);

    emit showMap(mat2QImage(img));
}


//Detect places based on coherency scores and puts places ids to
//detected_places vector.
//Returns 1 if new place is started
//Returns -1 if transition is started
int Pipeline::detectPlace(vector<float> coherency_scores,
                           vector<int>& detected_places_unfiltered,
                           vector<int>& detected_places)
{
    if(coherency_scores.size() == 0)
    {
        return pipeline::DETECTION_NOT_STARTED;
    }


    //Pre-assign last frame as either transition or place candidate
    detected_places_unfiltered.push_back(coherency_scores.back() < params.ssg_params.tau_c ? -1 : 1);


    //Place/transition region is decided according to median of
    //last tau_w frames. Therefore wait minimum number of
    //frames to be larger than tau_n
    if(detected_places_unfiltered.size() >= params.ssg_params.tau_n)
    {
        if(detected_places.size() == 0)
        {
            for(int i = 0; i < coherency_scores.size()-params.ssg_params.tau_n; i++)
            {
                detected_places.push_back(detected_places_unfiltered.back());
            }
        }
        int last_place_type = detected_places.back();

        bool region_status = getRegionStatus(vector<int>(detected_places_unfiltered.end()-params.ssg_params.tau_n,
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
                return pipeline::DETECTION_PLACE_ENDED;
            else
                return pipeline::DETECTION_PLACE_STARTED;
        }
        else
            return last_place_type > 0 ? pipeline::DETECTION_IN_PLACE : pipeline::DETECTION_IN_TRANSITION;
    }
    else
    {
        return pipeline::DETECTION_NOT_STARTED;
    }
}
