#include "tschybrid.h"
#include "TSC/tsc.h"

TSCHybrid::TSCHybrid(QCustomPlot* tsc_plot,
                     QCustomPlot* ssg_plot,
                     QCustomPlot* place_map,
                     QCustomPlot* tsc_avg_plot,
                     SSGParams* ssg_params,
                     SegmentTrackParams* seg_track_params,
                     SegmentationParams* seg_params,
                     GraphMatchParams* gm_params)
{
    this->tsc_plot = tsc_plot;
    this->ssg_plot = ssg_plot;
    this->tsc_avg_plot = tsc_avg_plot;
    this->params = ssg_params;
    this->seg_track_params = seg_track_params;
    this->seg_params = seg_params;
    this->gm_params = gm_params;


    ///////////////////
    // Plot settings //
    ///////////////////

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



    //Settings for tsc plot
    for(int i = 0; i <= PLOT_PLACES_IDX; i++)
        this->tsc_plot->addGraph();

    this->tsc_plot->graph(PLOT_THRES_IDX)->setPen(pen1);
    this->tsc_plot->graph(PLOT_SCORES_IDX)->setPen(pen2);
    this->tsc_plot->setMaximumWidth(COH_PLOT_W);
    this->tsc_plot->setMinimumWidth(COH_PLOT_W);
    this->tsc_plot->setMinimumHeight(COH_PLOT_H);
    this->tsc_plot->setMaximumHeight(COH_PLOT_H);
    this->tsc_plot->yAxis->setTickLabels(false);
    this->tsc_plot->plotLayout()->setAutoMargins(QCP::msNone);
    this->tsc_plot->plotLayout()->setMargins(plot_margin);

    this->tsc_plot->graph(PLOT_UNINFORMATIVE_IDX)->setPen(pen1);
    this->tsc_plot->graph(PLOT_UNINFORMATIVE_IDX)->setLineStyle(QCPGraph::lsNone);
    this->tsc_plot->graph(PLOT_UNINFORMATIVE_IDX)->setScatterStyle(QCPScatterStyle::ssTriangle);

    this->tsc_plot->graph(PLOT_INCOHERENT_IDX)->setPen(pen2);
    this->tsc_plot->graph(PLOT_INCOHERENT_IDX)->setLineStyle(QCPGraph::lsNone);
    this->tsc_plot->graph(PLOT_INCOHERENT_IDX)->setScatterStyle(QCPScatterStyle::ssCircle);



    //Settings for tsc avg plot
    for(int i = 0; i <= PLOT_PLACES_IDX; i++)
        this->tsc_avg_plot->addGraph();


    this->tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+2)->setPen(pen2);
    this->tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+2)->setLineStyle(QCPGraph::lsNone);
    this->tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+2)->setScatterStyle(QCPScatterStyle::ssDot);

    this->tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+3)->setPen(pen3);
    this->tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+3)->setLineStyle(QCPGraph::lsNone);
    this->tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+3)->setScatterStyle(QCPScatterStyle::ssCircle);

    this->tsc_avg_plot->graph(PLOT_SCORES_IDX)->setPen(pen1);
    this->tsc_avg_plot->setMaximumWidth(COH_PLOT_W);
    this->tsc_avg_plot->setMinimumWidth(COH_PLOT_W);
    this->tsc_avg_plot->setMinimumHeight(COH_PLOT_H);
    this->tsc_avg_plot->setMaximumHeight(COH_PLOT_H);
    this->tsc_avg_plot->yAxis->setTickLabels(false);
    this->tsc_avg_plot->plotLayout()->setAutoMargins(QCP::msNone);
    this->tsc_avg_plot->plotLayout()->setMargins(plot_margin);

    //Traveled places plot initialization
    this->place_map = place_map;

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
    plot_bg_img->setPixmap(QPixmap("./cold-sa.svg"));
    plot_bg_img->topLeft->setType(QCPItemPosition::ptViewportRatio);
    plot_bg_img->topLeft->setCoords(0,0);
    this->place_map->addLayer("imageLayer");
    this->place_map->addItem(plot_bg_img);
    this->place_map->addLayer("plotLayer");
    this->place_map->setCurrentLayer("plotLayer");


    seg_track = new SegmentTrack(seg_track_params, seg_params, gm_params);


}

void TSCHybrid::processImages(const string folder, const int start_idx, const int end_idx)
{
//    plotEsensPlaces();
//    return;

    //Read dataset image files
    img_files = getFiles(folder);

    Mat img_org, img;
    qint64 start_time, stop_time;

    //SSG related variables
    vector<vector<NodeSig> > ns_vec;  //Stores last tau_w node signatures
    vector<float> coherency_scores_ssg;   //Stores all coherency scores
    vector<int> detected_places_unfiltered;
    vector<int> detected_places; //Stores all detected place ids

    //TSC related variables
    vector<float> scores_tsc;
    TSC* tsc = new TSC();
    vector<float> tsc_avg_scores;

    vector<int> overlap_scores;
    vector<int> tsc_detected_places(END_IDX-START_IDX);


    //Process all images
    for(int frame_no = start_idx; frame_no < end_idx-1; frame_no++)
    {
        img_org = imread(folder + img_files[frame_no]);
        resize(img_org, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        //emit showImgOrg(mat2QImage(img));

        ///////////////
        //Process SSG//
        ///////////////

        start_time = QDateTime::currentMSecsSinceEpoch();

        seg_track->processImage(img, ns_vec);

        //Calculate coherency based on existence map
        calcCohScore(seg_track, coherency_scores_ssg);

        stop_time = QDateTime::currentMSecsSinceEpoch();
        //cout << "SSG time elapsed: " << stop_time-start_time << endl;

        //Show connectivity map
        showMap(seg_track->getM());

        //Decide last frame is whether transition or place
        //Results are written into detected places
        int detection_result = detectPlace(coherency_scores_ssg,detected_places_unfiltered,detected_places);

        cv::Point2f coord = getCoordCold(img_files[START_IDX+detected_places.size()]);

        //cv::Point2f coord(0,0);

        //Plot transition and place regions
        plotScoresSSG(coherency_scores_ssg, detected_places, coord);

        ///////////////
        //Process TSC//
        ///////////////
        start_time = QDateTime::currentMSecsSinceEpoch();

        //Process image
        bool isLastImage = frame_no == end_idx-2;
        float score = tsc->processImage(img_org, isLastImage);
        scores_tsc.push_back(score);

        stop_time = QDateTime::currentMSecsSinceEpoch();
        //cout << "TSC time elapsed: " << stop_time-start_time << endl;


        //Plot TSC scores
        if(!isLastImage)
            plotScoresTSC(scores_tsc, tsc->detector.currentPlace, tsc->detector.currentBasePoint, tsc->detector.detectedPlaces, tsc_detected_places);

        //Plot Avg TSC scores
        plotAvgScoresTSC(scores_tsc, tsc->detector.wholebasepoints, tsc_avg_scores);

        //Wait a little for GUI processing
        waitKey(1);
    }


    ////////////////////
    // Debug purposes //
    ////////////////////

    qDebug() << detected_places.size() << tsc_detected_places.size();

    int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
    for(int i = 0; i < detected_places.size(); i++)
    {
        if(detected_places[i] > 0 && tsc_detected_places[i] == 0)
        {
            a++;
        }
        else if(detected_places[i] < 0 && tsc_detected_places[i] == 0)
        {
            b++;
        }
        else if(detected_places[i] > 0 && tsc_detected_places[i] == 1)
        {
            c++;
        }
        else if(detected_places[i] < 0 && tsc_detected_places[i] == 1)
        {
            d++;
        }
        else if(detected_places[i] > 0 && tsc_detected_places[i] == -1)
        {
            e++;
        }
        else if(detected_places[i] < 0 && tsc_detected_places[i] == -1)
        {
            f++;
        }
    }

    qDebug() << a << b << c << d << e << f;
    qDebug() << "Transition recall: " << (float)d/(c+d) << "Place recall:" << (float)a/(a+b);

    for(int i = 0; i < detected_places.size(); i++)
        tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+2)->addData(i, detected_places[i]);

    for(int i = 0; i < tsc_detected_places.size(); i++)
        tsc_avg_plot->graph(PLOT_INCOHERENT_IDX+3)->addData(i, tsc_detected_places[i]);

    tsc_avg_plot->rescaleAxes();
    tsc_avg_plot->replot();

}

float TSCHybrid::calcCohScore(SegmentTrack* seg_track, vector<float>& coh_scores)
{
    Mat M = seg_track->getM();
    vector<pair<NodeSig, int> > M_ns = seg_track->getM_ns();
    float tau_w = seg_track_params->tau_w;

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
                if(M_ns[i].second > params->tau_f)
                    incoherency += params->coeff_node_disappear1;
                else
                    incoherency += params->coeff_node_disappear2;
            }

            //Incoherency: Appeared node
            if(M.at<int>(i, j) == 0 && M.at<int>(i, j+1) > 0)
            {
                    incoherency += params->coeff_node_appear;
            }

            //Coherency: Stabile node
            if(M.at<int>(i,j) > 0)
                nr_appear++;
        }

        //Active nodes are only the ones that appeared at least one time through window
        if(nr_appear > 0)
            active_nodes++;

        //If node appeared at least certain percent then it's a stabile node.
        if((float)nr_appear/tau_w > params->coeff_coh_appear_thres)
            coherency += (float)nr_appear/tau_w;
    }

    // Coherency score calculation ( -1 < coh_range < 1)
    // coherency_score = (1 - a^(-coherency)) - (incoherency)
    coh_score = 1-pow(params->coeff_coh_exp_base,-1*coherency) - (incoherency/active_nodes);

    coh_scores.push_back(coh_score);

    return coh_score;
}

void TSCHybrid::plotEsensPlaces()
{
    static int graph_idx = PLOT_PLACES_IDX;

    vector<string> img_files = getFiles(DATASET_FOLDER);
    vector<pair<int,int> > places;

//    places.push_back(pair<int, int>(1,45));
//    places.push_back(pair<int, int>(116,183));
//    places.push_back(pair<int, int>(195,204));
//    places.push_back(pair<int, int>(222,362));
//    places.push_back(pair<int, int>(389,399));
//    places.push_back(pair<int, int>(502,604));
//    places.push_back(pair<int, int>(666,751));
//    places.push_back(pair<int, int>(843,891));
//    places.push_back(pair<int, int>(941,1104));
//    places.push_back(pair<int, int>(1112,1121));
//    places.push_back(pair<int, int>(1128,1166));
//    places.push_back(pair<int, int>(1268,1307));

    places.push_back(pair<int, int>(1,47));
    places.push_back(pair<int, int>(116,184));
    places.push_back(pair<int, int>(195,204));
    places.push_back(pair<int, int>(213,410));
    places.push_back(pair<int, int>(488,615));
    places.push_back(pair<int, int>(657,755));
    places.push_back(pair<int, int>(843,896));
    places.push_back(pair<int, int>(941,1104));
    places.push_back(pair<int, int>(1112,1121));
    places.push_back(pair<int, int>(1128,1166));
    places.push_back(pair<int, int>(1174,1187));
    places.push_back(pair<int, int>(1268,1308));

    for(int i = 0; i < places.size(); i++)
    {
        this->place_map->addGraph();
        graph_idx++;

        QPen dumPen;
        dumPen.setWidth(1);
        dumPen.setColor(QColor(255, 0, 0, 50));

        this->place_map->graph(graph_idx)->setPen(dumPen);
        this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
        this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);

        for(int j = places[i].first; j <= places[i].second; j++)
        {
            cv::Point2f coord = getCoordCold(img_files[j-1]);
            qDebug() << coord.x << coord.y;

            place_map->graph(graph_idx)->addData(coord.x, coord.y);
        }


        this->place_map->addGraph();
        graph_idx++;

        dumPen.setWidth(3);
        dumPen.setColor(QColor(0, 0, 255, 50));

        this->place_map->graph(graph_idx)->setPen(dumPen);
        this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
        this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);

        for(int j = places[i].second+1; j < places[i+1].first; j++)
        {
            cv::Point2f coord = getCoordCold(img_files[j-1]);
            qDebug() << coord.x << coord.y;

            place_map->graph(graph_idx)->addData(coord.x, coord.y);
        }
    }

    place_map->replot();

}

// Plots places and coherency scores
void TSCHybrid::plotScoresSSG(vector<float> coherency_scores, vector<int> detected_places, cv::Point2f coord)
{
    static int graph_idx = PLOT_PLACES_IDX;
    static int thres_cursor = 0;
    float margin = 1.0;
    if(detected_places.size() > 1)
    {
        //Place->Transition or vice versa region change occur
        if(graph_idx == PLOT_PLACES_IDX || ( detected_places.back() != detected_places.rbegin()[1]) )
        {
            //Add new graph to change color
            this->ssg_plot->addGraph();
            this->place_map->addGraph();
            graph_idx++;

            if(detected_places.back() > 0)
            {
                //Place
                this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::red));
                this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 80)));

                QPen dumPen;
                dumPen.setWidth(1);
                dumPen.setColor(QColor(255, 0, 0, 50));

                this->place_map->graph(graph_idx)->setPen(dumPen);
                this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
            else
            {
                //Transition
                this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 80)));

                QPen dumPen;
                dumPen.setWidth(3);
                dumPen.setColor(QColor(0, 0, 255, 50));

                this->place_map->graph(graph_idx)->setPen(dumPen);
                this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
        }

        //Add the new point
        ssg_plot->graph(graph_idx)->addData(detected_places.size(),1.0+margin);
        place_map->graph(graph_idx)->addData(coord.x, coord.y);

        //Add coh score
        ssg_plot->graph(PLOT_SCORES_IDX)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]+margin);
    }

    //Add thresold line
    ssg_plot->graph(PLOT_THRES_IDX)->addData(thres_cursor,params->tau_c+margin);
    thres_cursor++;

    // set axes ranges, so we see all data:
    //coherency_plot->yAxis->setRange(*min_element(coherency_scores.begin(),coherency_scores.end()), *max_element(coherency_scores.begin(),coherency_scores.end()));
    //coherency_plot->xAxis->setRange(0, detected_places.size()+1);

    ssg_plot->rescaleAxes();
    ssg_plot->replot();
    //place_map->rescaleAxes();
    //place_map->replot();
}

void TSCHybrid::plotScoresTSC(vector<float> scores, Place* current_place, BasePoint cur_base_point, vector<Place> detected_places, vector<int>& tsc_detected_places)
{
    static int last_base_point = 0;
    static int last_detected_place_id = 0;
    static int graph_idx = PLOT_PLACES_IDX;
    float height = 2.0;


    if(scores.size() > 0)
    {
        //Add coh score
        tsc_plot->graph(PLOT_SCORES_IDX)->addData(scores.size()-1,scores.back());
    }

    if(detected_places.size() > 0 && detected_places.back().id != last_detected_place_id)
    {
        last_detected_place_id = detected_places.back().id;

        //////////////////////
        //Plot as transition//
        //////////////////////

        //Add new graph to change color
        this->tsc_plot->addGraph();
        graph_idx++;

        //Add basepoints as transition
        this->tsc_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
        this->tsc_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 80)));
        for(int i = last_base_point; i < current_place->members[0].id; i++)
        {
            tsc_plot->graph(graph_idx)->addData(i-START_IDX, height/3);
            if(tsc_detected_places[i-START_IDX] != -1)
                tsc_detected_places[i-START_IDX] = 1;
        }

        //////////////////
        //Plot as place //
        //////////////////

        //Add new graph to change color
        this->tsc_plot->addGraph();
        graph_idx++;

        //plot first tau_p basepoints as places in loop
        //after tau_p points we will draw one-by-one
        this->tsc_plot->graph(graph_idx)->setPen(QPen(Qt::red));
        this->tsc_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 80)));
        for(int i = current_place->members.front().id; i < current_place->members.back().id; i++)
        {
            tsc_plot->graph(graph_idx)->addData(i-START_IDX, height/3);
            if(tsc_detected_places[i-START_IDX] != -1)
                tsc_detected_places[i-START_IDX] = 0;
        }
    }

    //////////////////
    //Plot as place //
    //////////////////
    //plot base-points as place one-by-one
    if(current_place->members.size() > 0 && (last_base_point != current_place->members.back().id))
    {
        last_base_point = current_place->members.back().id;
        this->tsc_plot->graph(graph_idx)->setPen(QPen(Qt::red));
        this->tsc_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 80)));
        this->tsc_plot->graph(graph_idx)->addData(last_base_point-START_IDX, height/3);
        tsc_detected_places[last_base_point-START_IDX] = 0;
    }

    //Uninformative
    if(cur_base_point.status == 1)
    {
        this->tsc_plot->graph(PLOT_UNINFORMATIVE_IDX)->addData(cur_base_point.id-START_IDX, height/2/3);
        tsc_detected_places[cur_base_point.id-START_IDX] = -1;
    }
    else if(cur_base_point.status == 2)
    {
        this->tsc_plot->graph(PLOT_INCOHERENT_IDX)->addData(cur_base_point.id-START_IDX, 3*height/4/3);
    }



    tsc_plot->yAxis->setRange(0,height/3);
    tsc_plot->xAxis->rescale();
    //tsc_plot->rescaleAxes();
    tsc_plot->replot();
}

void TSCHybrid::plotAvgScoresTSC(vector<float>& scores, std::vector<BasePoint> wholebasepoints, vector<float>& avg_scores)
{
    int window_size = seg_track_params->tau_w;
    if(scores.size() >= window_size)
    {
        float score = 0.0;
        for(int i = scores.size() - window_size; i < scores.size(); i++)
        {
            if(wholebasepoints[i].status == 1)
            {
                scores[i] = scores[i-1];
            }
            score += scores[i];
        }
        score /= window_size;


        if(scores.size() == window_size)
        {
            for(int i = 0; i < window_size/2; i++)
                avg_scores.push_back(score);
        }
        avg_scores.push_back(score);

        this->tsc_avg_plot->graph(PLOT_SCORES_IDX)->addData(avg_scores.size()-1, avg_scores.back());
    }
    else
    {
        this->tsc_avg_plot->graph(PLOT_SCORES_IDX)->addData(0,0);
    }

    tsc_avg_plot->rescaleAxes();
    tsc_avg_plot->replot();
}

// Plots node existence map
void TSCHybrid::showMap(const Mat& M)
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
int  TSCHybrid::detectPlace(vector<float> coherency_scores,
                                  vector<int>& detected_places_unfiltered,
                                  vector<int>& detected_places)
{
    if(coherency_scores.size() == 0)
    {
        return 0;
    }


    //Pre-assign last frame as either transition or place candidate
    detected_places_unfiltered.push_back(coherency_scores.back() < params->tau_c ? -1 : 1);


    //Place/transition region is decided according to median of
    //last tau_w frames. Therefore wait minimum number of
    //frames to be larger than tau_n
    if(detected_places_unfiltered.size() >= params->tau_n)
    {
        if(detected_places.size() == 0)
        {
            for(int i = 0; i < coherency_scores.size()-params->tau_n; i++)
            {
                detected_places.push_back(detected_places_unfiltered.back());
            }
        }
        int last_place_type = detected_places.back();

        bool region_status = getRegionStatus(vector<int>(detected_places_unfiltered.end()-params->tau_n,
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

bool TSCHybrid::eventFilter( QObject* watched, QEvent* event ) {
    int max_frames = END_IDX-START_IDX;

    if ( event->type() == QEvent::MouseButtonPress )
    {
        const QMouseEvent* const me = static_cast<const QMouseEvent*>( event );

        //might want to check the buttons here
        const QPoint p = me->pos();

        cursor = (p.x() / (float)COH_PLOT_W)*max_frames;
    }
    else if( event->type() == QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);

        if ( key->key() == Qt::Key_Left )
        {
            cursor = max(0, cursor-1);
        }
        else if ( key->key() == Qt::Key_Right )
        {
            cursor = min(max_frames-1, cursor+1);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    //Show cursor
    ssg_plot->graph(PLOT_TRACK_IDX)->clearData();
    ssg_plot->graph(PLOT_TRACK_IDX)->addData(cursor,INIT_TAU_C+1);
    ssg_plot->rescaleAxes();
    ssg_plot->replot();

    string filepath = DATASET_FOLDER + img_files[START_IDX + cursor];

    Mat img = imread(filepath);
    resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);

    //Show original images on the window
    emit showImgOrg(mat2QImage(img));


    return true;
}
