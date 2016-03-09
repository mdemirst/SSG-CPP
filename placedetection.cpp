#include "placedetection.h"
#include "utils.h"
#include "unistd.h"
#include <dirent.h>

#include "rag.h"

PlaceDetection::PlaceDetection(QCustomPlot* coherency_plot, QCustomPlot* map)
{
    //Read dataset image files
    img_files = getFiles(DATASET_FOLDER);

    seg_track = new SegmentTrack();

    // Coherency plot related settings
    this->coherency_plot = coherency_plot;

    for(int i = 0; i <= PLOT_PLACES_IDX; i++)
        this->coherency_plot->addGraph();

    pen.setWidth(PEN_WIDTH2);
    pen.setColor(Qt::red);
    this->coherency_plot->graph(PLOT_THRES_IDX)->setPen(pen);

    pen.setWidth(PEN_WIDTH2);
    pen.setColor(Qt::black);
    this->coherency_plot->graph(PLOT_SCORES_IDX)->setPen(pen);

    this->coherency_plot->setMaximumWidth(COH_PLOT_W);
    this->coherency_plot->setMinimumWidth(COH_PLOT_W);

    this->coherency_plot->setMinimumHeight(COH_PLOT_H);
    this->coherency_plot->setMaximumHeight(COH_PLOT_H);

    this->coherency_plot->yAxis->setTickLabels(false);
    this->coherency_plot->plotLayout()->setAutoMargins(QCP::msNone);

    QMargins plot_margin(COH_PLOT_MARGIN,0,COH_PLOT_MARGIN,0);
    this->coherency_plot->plotLayout()->setMargins(plot_margin);


    // Parameters initialization
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
    stopProcessing = false;


    //Traveled places plot initialization
    this->place_map = map;

    for(int i = 0; i <= PLOT_PLACES_IDX; i++)
        this->place_map->addGraph();

    this->place_map->axisRect()->setAutoMargins(QCP::msNone);
    this->place_map->axisRect()->setMargins(QMargins(0,0,0,0));
    this->place_map->setMaximumWidth(COLD_MAP_W);
    this->place_map->setMinimumWidth(COLD_MAP_W);
    this->place_map->setMinimumHeight(COLD_MAP_H);
    this->place_map->setMaximumHeight(COLD_MAP_H);
    this->place_map->xAxis->grid()->setVisible(false);
    this->place_map->yAxis->grid()->setVisible(false);
    this->place_map->xAxis->setTicks(false);
    this->place_map->yAxis->setTicks(false);
    this->place_map->xAxis->setVisible(false);
    this->place_map->yAxis->setVisible(false);
    this->place_map->xAxis->setRange(COLD_MAP_L, COLD_MAP_R);
    this->place_map->yAxis->setRange(COLD_MAP_T, COLD_MAP_B);

    //Plot dataset map
    QCPItemPixmap *plot_bg_img = new QCPItemPixmap(this->place_map);
    plot_bg_img->setPixmap(QPixmap("./cold.svg"));
    plot_bg_img->topLeft->setType(QCPItemPosition::ptViewportRatio);
    plot_bg_img->topLeft->setCoords(0,0);
    this->place_map->addLayer("imageLayer");
    this->place_map->addItem(plot_bg_img);
    this->place_map->addLayer("plotLayer");
    this->place_map->setCurrentLayer("plotLayer");
}

// Plots places and coherency scores
void PlaceDetection::plotPlaces(vector<float> coherency_scores, vector<int> detected_places, cv::Point2f coord)
{
    static int graph_idx = PLOT_PLACES_IDX;
    float margin = 1.0;
    if(detected_places.size() > 1)
    {
        //Place->Transition or vice versa region change occur
        if(graph_idx == PLOT_PLACES_IDX || ( detected_places.back() != detected_places.rbegin()[1]) )
        {
            //Add new graph to change color
            this->coherency_plot->addGraph();
            this->place_map->addGraph();
            graph_idx++;

            if(detected_places.back() > 0)
            {
                //Place
                this->coherency_plot->graph(graph_idx)->setPen(QPen(Qt::red));
                this->coherency_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 80)));

                this->place_map->graph(graph_idx)->setPen(QPen(Qt::red));
                this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
            else
            {
                //Transition
                this->coherency_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->coherency_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 80)));

                this->place_map->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
        }

        //Add the new point
        coherency_plot->graph(graph_idx)->addData(detected_places.size(),1.0+margin);
        place_map->graph(graph_idx)->addData(coord.x, coord.y);

        //Add thresold line
        coherency_plot->graph(PLOT_THRES_IDX)->addData(detected_places.size(),tau_c+margin);

        //Add coh score
        coherency_plot->graph(PLOT_SCORES_IDX)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]+margin);

        // set axes ranges, so we see all data:
        //coherency_plot->yAxis->setRange(*min_element(coherency_scores.begin(),coherency_scores.end()), *max_element(coherency_scores.begin(),coherency_scores.end()));
        //coherency_plot->xAxis->setRange(0, detected_places.size()+1);

        coherency_plot->rescaleAxes();
        coherency_plot->replot();
        //place_map->rescaleAxes();
        place_map->replot();
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
    {
        return 0;
    }


    //Pre-assign last frame as either transition or place candidate
    detected_places_unfiltered.push_back(coherency_scores.back() < tau_c ? -1 : 1);


    //Place/transition region is decided according to median of
    //last tau_w frames. Therefore wait minimum number of
    //frames to be larger than tau_n
    if(detected_places_unfiltered.size() >= tau_n)
    {
        if(detected_places.size() == 0)
        {
            for(int i = 0; i < coherency_scores.size()-tau_n; i++)
            {
                detected_places.push_back(detected_places_unfiltered.back());
            }
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

    clearPastData();

    isProcessing = true;

    //Process all images
    for(int frame_no = START_IDX; frame_no < END_IDX-1; frame_no++)
    {
        if(stopProcessing == true)
        {
            stopProcessing = false;
            break;
        }

        //Wait until process_next_frame set to true
        while(frameByFrameProcess == true && process_next_frame == false)
        {
            waitKey(1);
        }
        //Set it back to false
        process_next_frame = false;

        string filepath1 = DATASET_FOLDER + img_files[frame_no];
        string filepath2 = DATASET_FOLDER + img_files[frame_no+1];


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
        //emit showImg1(mat2QImage(img1));
        emit showImg2(mat2QImage(img2));

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

            if(SSGs.size() > 1)
                emit showSSG(mat2QImage(SSGProc::drawSSG(SSGs.rbegin()[1], img2)));
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

        cv::Point2f coord = getCoordCold(img_files[START_IDX+detected_places.size()]);

        //Plot transition and place regions
        plotPlaces(coherency_scores, detected_places, coord);

        //Wait a little for GUI processing
        waitKey(1);
    }

    //Constructs average gist from all frames
    //constructSceneGist(seg_track->M, seg_track->M_ns);

    isProcessing = false;
}

void PlaceDetection::clearPastData()
{
    coherency_scores.clear(); //Stores all coherency score.clear();
    detected_places.clear();
    detected_places_unfiltered.clear();
    SSGs.clear(); //Stores SSGs
    seg_track->M.release();
    seg_track->M_ns.clear();
    for(int i = 0; i < coherency_plot->graphCount(); i++)
        coherency_plot->graph(i)->clearData();
    for(int i = 0; i < place_map->graphCount(); i++)
        place_map->graph(i)->clearData();
}

void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
  cv::Mat &output, cv::Point2i location)
{
  background.copyTo(output);


  // start at the row indicated by location, or at row 0 if location.y is negative.
  for(int y = std::max(location.y , 0); y < background.rows; ++y)
  {
    int fY = y - location.y; // because of the translation

    // we are done of we have processed all rows of the foreground image.
    if(fY >= foreground.rows)
      break;

    // start at the column indicated by location,

    // or at column 0 if location.x is negative.
    for(int x = std::max(location.x, 0); x < background.cols; ++x)
    {
      int fX = x - location.x; // because of the translation.

      // we are done with this row if the column is outside of the foreground image.
      if(fX >= foreground.cols)
        break;

      // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
      double opacity =
        ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

        / 255.;


      // and now combine the background and foreground pixel, using the opacity,

      // but only if opacity > 0.
      for(int c = 0; opacity > 0 && c < output.channels(); ++c)
      {
        unsigned char foregroundPx =
          foreground.data[fY * foreground.step + fX * foreground.channels() + c];
        unsigned char backgroundPx =
          background.data[y * background.step + x * background.channels() + c];
        output.data[y*output.step + output.channels()*x + c] =
          backgroundPx * (1.-opacity) + foregroundPx * opacity;
      }
    }
  }
}

//Constructs the average appearance of the scene by averaging specified
//segments.
void PlaceDetection::constructSceneGist(Mat& M, vector<pair<NodeSig, int> > M_ns)
{
    vector<int> segment_ids;

    int thres = 10;

    //First find coherent segments
    for(int i = 0; i < M_ns.size(); i++)
    {
        if( M_ns[i].second > thres )
        {
            segment_ids.push_back(i);
        }
    }

    //Stores average appearances of coherent segments
    vector< pair<Mat, int> > average_segments(M.size().height, make_pair(Mat::zeros(0,0,CV_8UC3), 0));

    //Search segments in M starting from first frame to last frame
    for(int i = 0; i < M.size().width; i++)
    {
        //Check if any of specified segments appear in this frame
        bool do_segmentation = false;
        for(int j = 0; j < segment_ids.size(); j++)
        {
            if(M.at<int>(segment_ids[j],i) != -1)
            {
                do_segmentation = true;
                break;
            }
        }
        //This frame does not contain interested segment, so continue to next
        if(do_segmentation == false)
            continue;

        Mat img = imread(DATASET_FOLDER + img_files[START_IDX + i]);
        resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);

        vector<Mat> segments = seg_track->seg->segmentImage(img);

        //Get interested segments in the frame and average them
        //with matched previous segments
        for(int j = 0; j < segment_ids.size(); j++)
        {
            int node_id = segment_ids[j];
            int segment_id = M.at<int>(node_id,i);

            if(segment_id != -1)
            {
                Mat img_gray;
                vector<vector<Point> > contours;
                vector<Vec4i> hierarchy;

                Mat img_seg = segments[segment_id];

                cvtColor(img_seg, img_gray, COLOR_BGR2GRAY);
                threshold(img_gray, img_gray, 1, 255, CV_THRESH_BINARY);
                findContours(img_gray.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

                //Adding alpha channel
                Mat rgb[3];
                split(img_seg,rgb);

                Mat rgba[4]={rgb[0],rgb[1],rgb[2],img_gray};
                merge(rgba,4,img_seg);

                Rect rect = boundingRect(contours[0]);

                Mat img_crop = img_seg(rect);

                Mat img_old = average_segments[node_id].first;
                float count = average_segments[node_id].second;
                int area1 = img_old.size().width * img_old.size().height;
                int area2 = img_crop.size().width * img_crop.size().height;

                if(area1 == 0)
                {
                    img_old = img_crop;
                    average_segments[node_id].first = img_old;
                    average_segments[node_id].second = count + 1;
                }
                else
                {
                    if(area1 > area2)
                    {
                        resize(img_crop, img_crop, img_old.size());
                    }
                    else
                    {
                        resize(img_old, img_old, img_crop.size());
                    }

                    float alpha = count / (count + 1);

                    cout << img_old.channels() << img_crop.channels() << endl;

                    addWeighted(img_old, alpha, img_crop, 1-alpha, 0.0, img_old, CV_8UC4);
                    average_segments[node_id].first = img_old;
                    average_segments[node_id].second = count + 1;
                }
            }
        }
    }

    //Draw segments together
    Mat img = imread(DATASET_FOLDER + img_files[START_IDX]);
    resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);

    Mat img_gist = Mat::zeros(img.size(),CV_8UC3);
    for(int i = 0; i < average_segments.size(); i++)
    {
        if(average_segments[i].second > 0)
        {
            Point center = M_ns[i].first.center;
            Mat img = average_segments[i].first;
            int x1 = center.x - img.size().width/2;
            int x2 = x1 + img.size().width;
            int y1 = center.y - img.size().height/2;
            int y2 = y1 + img.size().height;

            if(x1 < 0)
            {
                img = img(Rect(-1*x1,0,img.size().width+x1,img.size().height));
                x1 = 0;
            }
            if(x2 > img_gist.size().width)
            {
                img = img(Rect(0,0,img.size().width-(x2-img_gist.size().width),img.size().height));
                x2 = img_gist.size().width;
            }
            if(y1 < 0)
            {
                img = img(Rect(0,-1*y1,img.size().width,img.size().height+y1));
                y1 = 0;
            }
            if(y2 > img_gist.size().height)
            {
                img = img(Rect(0,0,img.size().width,img.size().height-(y2-img_gist.size().height)));
                y2 = img_gist.size().height;
            }

            Mat img_rect = img_gist(Rect(x1,y1,img.size().width,img.size().height));
            overlayImage(img_rect, img, img_rect, cv::Point(0,0));
        }
    }

    imshow("Gist", img_gist);
    cvWaitKey(0);
}

