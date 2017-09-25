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

  pen3.setWidth(PEN_WIDTH3);
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

  //this->ssg_plot->graph(PLOT_VPC_IDX)->setPen(pen2);
  //this->ssg_plot->graph(PLOT_VPC_IDX)->setLineStyle(QCPGraph::lsNone);
  //this->ssg_plot->graph(PLOT_VPC_IDX)->setScatterStyle(QCPScatterStyle::ssCircle);

  this->ssg_plot->graph(PLOT_ODOM_IDX)->setPen(pen2);
  //this->ssg_plot->graph(PLOT_ODOM_IDX)->setLineStyle(QCPGraph::lsNone);
  //this->ssg_plot->graph(PLOT_ODOM_IDX)->setScatterStyle(QCPScatterStyle::ssCircle);

  this->ssg_plot->graph(PLOT_PLACES_IDX)->setPen(pen1);
  //this->ssg_plot->graph(PLOT_PLACES_IDX)->setLineStyle(QCPGraph::lsNone);
  //this->ssg_plot->graph(PLOT_PLACES_IDX)->setScatterStyle(QCPScatterStyle::ssCross);

  this->ssg_plot->graph(PLOT_ROT_IDX)->setPen(pen4);

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


vector<OdomScan> getOdomScans(string path)
{
  vector<OdomScan> odom_scans;
  string line;
  ifstream myfile (path);

  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
        stringstream ss(line);
        OdomScan odom_scan;

        double dum;
        for(int i = 0; i < 16; i++)
        {
          ss >> dum;
        }

        for(int i = 0; i < 181; i++)
        {
          double range;
          ss >> range;
          odom_scan.scan.push_back(range);
        }

        odom_scans.push_back(odom_scan);
    }
    myfile.close();
  }

  return odom_scans;
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

    if(i<10)
    {
      frame_info.odom_diff = getDist(getCoordCold2(img_files[0]), getCoordCold2(img_files[i]));
    }
    else
    {
      frame_info.odom_diff = getDist(getCoordCold2(img_files[i-10]), getCoordCold2(img_files[i]));
    }

    frame_infos.push_back(frame_info);
  }

  return frame_infos;
}

bool Pipeline::calcCoherencyHistogram(const Mat& coh_matrix, Mat& hist, Mat& hist2)
{
  if(coh_matrix.cols < params.seg_track_params.tau_w)
  {
    return false;
  }

  Mat roi = coh_matrix(cv::Rect(coh_matrix.cols - params.seg_track_params.tau_w, 0, params.seg_track_params.tau_w, coh_matrix.rows));
  //cout << roi.rows << " " << roi.cols << endl;

  for(int i = 0; i < coh_matrix.rows; ++i)
  {
    //cout << roi.row(i) << endl;
    int appeared = checkIfAppeared(roi.row(i));
    int disappeared = checkIfDisappeared(roi.row(i));

    if(appeared > 0)
      hist.at<float>(appeared,0) = hist.at<float>(appeared,0) + 1;

    if(disappeared > 0)
      hist2.at<float>(disappeared,0) = hist2.at<float>(disappeared,0) + 1;
  }



  return true;
}

void Pipeline::normalizeSVM(svm_problem* problem, int dim, std::vector<double>& scaling)
{
  std::vector<double> max_vals(dim);

  for(int d = 0; d < dim; ++d)
  {
    max_vals[d] = 0.0;
    for(int i = 0; i < problem->l; ++i)
    {
      if(problem->x[i][d].value > max_vals[d])
        max_vals[d] = problem->x[i][d].value;
    }

    if(max_vals[d] > 0.0)
    {
      for(int i = 0; i < problem->l; ++i)
      {
        problem->x[i][d].value /= max_vals[d];
      }
      scaling.push_back(1/max_vals[d]);
    }
    else
    {
      scaling.push_back(1.0);
    }
  }
}

void Pipeline::processImages(DBUsage db_usage)
{
    is_processing = true;

    vector<string> img_files = getFiles(dataset.location+"/std_cam");
    //map<string,string> place_cats = getPlaceCategories(dataset.location+"/localization/places.lst");
    vector<FrameInfo> frame_infos = getFrameInfos(dataset.location);
    vector<OdomScan> odom_scans = getOdomScans(dataset.location+"/odom_scans/scans.tdf");

    for(int i = 0; i < odom_scans[0].scan.size(); i++)
      cout << odom_scans[0].scan[i] << " ";

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

    Mat hist_img = Mat::zeros(params.seg_track_params.tau_w,dataset.end_idx,CV_32F);
    Mat hist_img2 = Mat::zeros(params.seg_track_params.tau_w,dataset.end_idx,CV_32F);


    struct svm_parameter ssg_svm_param;
    // default values
    ssg_svm_param.svm_type = ONE_CLASS;
    ssg_svm_param.kernel_type = LINEAR;
    ssg_svm_param.degree = 3;
    ssg_svm_param.gamma = 0;	// 1/num_features
    ssg_svm_param.coef0 = 0;
    ssg_svm_param.nu = 0.5;
    ssg_svm_param.cache_size =500;
    ssg_svm_param.C = 1;
    ssg_svm_param.eps = 1e-6;
    ssg_svm_param.p = 0.1;
    ssg_svm_param.shrinking = 1;
    ssg_svm_param.probability = 0;
    ssg_svm_param.nr_weight = 0;
    ssg_svm_param.weight_label = NULL;
    ssg_svm_param.weight = NULL;

    struct svm_model *ssg_svm_model;

    svm_problem ssg_svm_problem;
    ssg_svm_problem.l = dataset.end_idx-dataset.start_idx-1-params.seg_track_params.tau_w;

    ssg_svm_problem.y = new double[ssg_svm_problem.l-1];
    ssg_svm_problem.x = new struct svm_node*[ssg_svm_problem.l-1];
    for(int i = 0; i < ssg_svm_problem.l-1; i++)
      ssg_svm_problem.x[i] = new struct svm_node[params.seg_track_params.tau_w*2+1];

    double transition_thres = 0.5;

    bool svm_prediction_enabled = true;
    string model_output_path = OUTPUT_FOLDER;
    model_output_path += "/svm_model.model";
    string scaling_output_path = OUTPUT_FOLDER;
    scaling_output_path += "/scaling.vector";
    std::vector<double> scaling;


    if(svm_prediction_enabled == true && (ssg_svm_model=svm_load_model(model_output_path.c_str()))==0)
    {
      std::cout << "Cannot load model" << std::endl;
      return;
    }
    if(svm_prediction_enabled)
    {
      readVector<double>(scaling_output_path, scaling);
    }

    int total_correct_prediction = 0;
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

          img_org = imread(dataset.location+"/std_cam" + "/" + img_files[frame_no]);

          resize(img_org, img, cv::Size(params.ssg_params.img_width, params.ssg_params.img_height));
          emit showImgOrg(mat2QImage(img));

          seg_track->processImageFromNS(basepoint.rag,ns_vec);
        }
        else
        {
          img_org = imread(dataset.location+"/std_cam" + "/" + img_files[frame_no]);

          resize(img_org, img, cv::Size(params.ssg_params.img_width, params.ssg_params.img_height));

          seg_track->processImage(img, ns_vec);
        }

        Mat hist = Mat::zeros(cv::Size(1,params.seg_track_params.tau_w),CV_32F);
        Mat hist2 = Mat::zeros(cv::Size(1,params.seg_track_params.tau_w),CV_32F);
        calcCoherencyHistogram(seg_track->getM(), hist, hist2);

        //std::cout << frame_no << "-"<< seg_track->getM().size() << endl;

        hist.copyTo(hist_img(cv::Rect(frame_no,0,1,params.seg_track_params.tau_w)));
        hist2.copyTo(hist_img2(cv::Rect(frame_no,0,1,params.seg_track_params.tau_w)));


        if((frame_no-dataset.start_idx) >= params.seg_track_params.tau_w )
        {
          int svm_index = frame_no-dataset.start_idx -  params.seg_track_params.tau_w;
          double trans_prob = frame_infos[frame_no-dataset.start_idx-params.seg_track_params.tau_w/2].transition_prob;
          int label = trans_prob > transition_thres ? 1 : -1;



          for(int i = 0; i < hist.rows; ++i)
          {
            ssg_svm_problem.x[svm_index][i].index = i+1;
            if(svm_prediction_enabled)
              ssg_svm_problem.x[svm_index][i].value = scaling[i]*hist.at<float>(i,0);
            else
              ssg_svm_problem.x[svm_index][i].value = hist.at<float>(i,0);

            ssg_svm_problem.x[svm_index][i+hist.rows].index = i+1+params.seg_track_params.tau_w;

            if(svm_prediction_enabled)
              ssg_svm_problem.x[svm_index][i+hist.rows].value = scaling[i+hist.rows]*hist2.at<float>(i,0);
            else
              ssg_svm_problem.x[svm_index][i+hist.rows].value = hist2.at<float>(i,0);


            ssg_svm_problem.y[svm_index] = label;

//            ssg_svm_problem.x[svm_index][i].index = i+1;
//            if(svm_prediction_enabled)
//              ssg_svm_problem.x[svm_index][i].value = scaling[i]*trans_prob;
//            else
//              ssg_svm_problem.x[svm_index][i].value = trans_prob;

//            ssg_svm_problem.x[svm_index][i+hist.rows].index = i+1+params.seg_track_params.tau_w;

//            if(svm_prediction_enabled)
//              ssg_svm_problem.x[svm_index][i+hist.rows].value = scaling[i+hist.rows]*trans_prob;
//            else
//              ssg_svm_problem.x[svm_index][i+hist.rows].value = trans_prob;


//            ssg_svm_problem.y[svm_index] = label;



          }

          ssg_svm_problem.x[svm_index][hist.rows*2].index = -1;


          if(svm_prediction_enabled)
          {
            int predict_label = svm_predict(ssg_svm_model,ssg_svm_problem.x[svm_index]);
            std::cout << label << " " << predict_label << std::endl;

            if(label == predict_label)
              total_correct_prediction++;
          }
        }





        emit(showHist(mat2QImage(hist_img)));
        emit(showHist2(mat2QImage(hist_img2)));


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

        //cout << frame_infos[frame_no].cat << " " << frame_infos[frame_no].transition_prob << " " << frame_infos[frame_no].translation << endl;

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

    std::cout << "Prediction rate: " << (double)total_correct_prediction/ssg_svm_problem.l << std::endl;

    if(svm_prediction_enabled == false)
    {
      normalizeSVM(&ssg_svm_problem,params.seg_track_params.tau_w*2,scaling);

      writeVector<double>(scaling_output_path,scaling);

      const char *error_msg = svm_check_parameter(&ssg_svm_problem,&ssg_svm_param);

      if(error_msg)
      {
        std::cout << "Error in SVM problem construction" << std::endl;
      }

      ssg_svm_model = svm_train(&ssg_svm_problem,&ssg_svm_param);


      if(svm_save_model(model_output_path.c_str(),ssg_svm_model))
      {
        std::cout << "Cannot save svm model" << std::endl;
      }

      std::cout << "Finished saving model " << std::endl;

      std::cout << "Freed model " << std::endl;
    }

    svm_free_and_destroy_model(&ssg_svm_model);
    svm_destroy_param(&ssg_svm_param);
    //delete[] ssg_svm_problem.y;
    //delete[] ssg_svm_problem.x;

    if(db_usage == DBUsage::READ_FROM_DB || db_usage == DBUsage::SAVE_TO_DB)
    {
      db.closeDB();
    }

    is_processing = false;

    std::cout << "Finished" << std::endl;
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
                //this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::red));
                //this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 80)));

                QPen dumPen;
                dumPen.setWidth(1);
                dumPen.setColor(QColor(255, 0, 0, 50));
            }
            else
            {
                //Transition
                //this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                //this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 80)));

                QPen dumPen;
                dumPen.setWidth(3);
                dumPen.setColor(QColor(0, 0, 255, 50));
            }
        }

        //Add the new point
        ssg_plot->graph(graph_idx)->addData(detected_places.size(),1.0+margin);

        //Add coh score
        //ssg_plot->graph(PLOT_SCORES_IDX)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]+margin);

        vector<double> stats_data;
        for(int i = coherency_scores.size()-5; i < coherency_scores.size(); i++)
        {
          stats_data.push_back(coherency_scores[i]);
        }
        Statistics stats(stats_data);

        //ssg_plot->graph(PLOT_ROT_IDX)->addData(detected_places.size(),stats.getMean()+margin);
        //ssg_plot->graph(PLOT_ODOM_IDX)->addData(detected_places.size(),10*stats.getVariance());




    }

    //Add thresold line
//    ssg_plot->graph(PLOT_THRES_IDX)->addData(thres_cursor,params.ssg_params.tau_c+margin);
//    ssg_plot->graph(PLOT_ODOM_IDX)->addData(thres_cursor,2*frame_infos[coherency_scores.size()].odom_diff);
    ssg_plot->graph(PLOT_PLACES_IDX)->addData(thres_cursor,frame_infos[coherency_scores.size()].transition_prob);
//    ssg_plot->graph(PLOT_ROT_IDX)->addData(thres_cursor,min(10*frame_infos[coherency_scores.size()].rotation,2));

    thres_cursor++;

    ssg_plot->rescaleAxes();
    ssg_plot->replot();
}

int Pipeline::checkIfAppeared(const Mat& segment)
{
  bool appeared = false;
  int appeared_size = 0;
  for(int i = 0; i < segment.cols-1; ++i)
  {
    if(segment.at<int>(0,i) >= 0)
      appeared_size++;

    if(segment.at<int>(0,i) == -1 && segment.at<int>(0,i+1) >= 0)
      appeared = true;
  }

  if(appeared == true)
    return appeared_size;
  else
    return 0;
}

int Pipeline::checkIfDisappeared(const Mat& segment)
{
  bool appeared = false;
  int appeared_size = 0;
  for(int i = 0; i < segment.cols-1; ++i)
  {
    if(segment.at<int>(0,i) >= 0)
      appeared_size++;

    if(segment.at<int>(0,i) >= 0 && segment.at<int>(0,i+1) == -1)
      appeared = true;
  }

  if(appeared == true)
    return appeared_size;
  else
    return 0;
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
