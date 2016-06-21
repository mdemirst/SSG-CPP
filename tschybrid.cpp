#include "tschybrid.h"
#include "experimentalData.h"
#include "GraphSegmentation/ColoredSegments/coloredSegments.h"

TSCHybrid::TSCHybrid(QCustomPlot* tsc_plot,
                     QCustomPlot* ssg_plot,
                     QCustomPlot* place_map,
                     QCustomPlot* tsc_avg_plot,
                     Parameters* params,
                     Dataset* dataset)
{
    this->tsc_plot = tsc_plot;
    this->ssg_plot = ssg_plot;
    this->tsc_avg_plot = tsc_avg_plot;
    this->params = params;
    this->dataset = dataset;


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

//    for(int i = 0; i <= PLOT_PLACES_IDX; i++)
//        this->place_map->addGraph();

//    this->place_map->axisRect()->setAutoMargins(QCP::msNone);
//    this->place_map->axisRect()->setMargins(QMargins(0,0,0,0));
//    this->place_map->setMaximumWidth(COLD_MAP_W);
//    this->place_map->setMinimumWidth(COLD_MAP_W);
//    this->place_map->setMinimumHeight(COLD_MAP_H);
//    this->place_map->setMaximumHeight(COLD_MAP_H);
//    this->place_map->xAxis->grid()->setVisible(false);
//    this->place_map->yAxis->grid()->setVisible(false);
//    this->place_map->xAxis->setTicks(false);
//    this->place_map->yAxis->setTicks(false);
//    this->place_map->xAxis->setVisible(false);
//    this->place_map->yAxis->setVisible(false);
//    this->place_map->xAxis->setRange(COLD_MAP_L, COLD_MAP_R);
//    this->place_map->yAxis->setRange(COLD_MAP_T, COLD_MAP_B);

//    //Plot dataset map
//    QCPItemPixmap *plot_bg_img = new QCPItemPixmap(this->place_map);
//    string overlay_img = string(OUTPUT_FOLDER)+"cold.png";
//    plot_bg_img->setPixmap(QPixmap(overlay_img.c_str()));
//    plot_bg_img->topLeft->setType(QCPItemPosition::ptViewportRatio);
//    plot_bg_img->topLeft->setCoords(0,0);
//    this->place_map->addLayer("imageLayer");
//    this->place_map->addItem(plot_bg_img);
//    this->place_map->addLayer("plotLayer");
//    this->place_map->setCurrentLayer("plotLayer");


    seg_track = new SegmentTrack(params,dataset);

    recognition = new Recognition (params,
                                   dataset,
                                   seg_track->gm,
                                   seg_track->seg);

    //Bubble process init
    bubbleProcess::calculateImagePanAngles(FOCAL_LENGHT_PIXELS, params->ssg_params.img_width, params->ssg_params.img_height);
    bubbleProcess::calculateImageTiltAngles(FOCAL_LENGHT_PIXELS, params->ssg_params.img_width, params->ssg_params.img_height);

    //Filters init
    string filters_dir = OUTPUT_FOLDER+string("visual_filters");
    ImageProcess::readFilter(QString(filters_dir.c_str()).append("/filtre0.txt"),29,false,false,false);
    ImageProcess::readFilter(QString(filters_dir.c_str()).append("/filtre6.txt"),29,false,false,false);
    ImageProcess::readFilter(QString(filters_dir.c_str()).append("/filtre12.txt"),29,false,false,false);
    ImageProcess::readFilter(QString(filters_dir.c_str()).append("/filtre18.txt"),29,false,false,false);
    ImageProcess::readFilter(QString(filters_dir.c_str()).append("/filtre36.txt"),29,false,false,false);

    is_processing = false;
    stop_processing = false;
    next_frame = true;

    perform_recognition = false;
    save2database = false;


    //If you're running TSC model, erase comments
    //tsc = new TSC(dataset);


}

TSCHybrid::~TSCHybrid()
{
    delete recognition;
    delete seg_track;
}

void TSCHybrid::readFromDB()
{
    //remove comments on places you like to load from DB
    vector<int> places;
    //places.push_back(SITE_FR1);
    //places.push_back(SITE_SA1);
    //places.push_back(SITE_LJ1);
    //places.push_back(SITE_NC1);
    places.push_back(SITE_FR2);
    //places.push_back(SITE_SA2);
    //places.push_back(SITE_LJ2);
    //places.push_back(SITE_NC2);



    for(int i = 0; i < places.size(); i++)
    {
        vector<SSG> readSSG;
        DatabaseHandler db;
        db.setConnName("conn"+QString::number(places[i]).toStdString());
        db.openDB(OUTPUT_FOLDER+string("dataset_")+QString::number(places[i]).toStdString()+string("_places.db"));
        readSSG = db.getAllSSGsNew();
        db.closeDB();
        SSGs.insert(SSGs.end(), readSSG.begin(), readSSG.end());
        readSSG.clear();
    }


//    DatabaseHandler db2;
//    db2.setConnName("conn2");
//    db2.openDB(OUTPUT_FOLDER+string("dataset_1.db"));
//    db2.createTables();
//    for(int i = 0; i < allSSG1.size(); i++)
//        db2.insertSSG(allSSG1[i]);
//    db2.closeDB();

}

void TSCHybrid::clearPastData()
{
    SSGs.clear();
    img_files.clear();
    seg_track->getM().release();
    seg_track->getM_ns().clear();
    coords.clear();

    for(int i = 0; i < tsc_plot->graphCount(); i++)
        tsc_plot->graph(i)->clearData();
    for(int i = 0; i < ssg_plot->graphCount(); i++)
        ssg_plot->graph(i)->clearData();
    for(int i = 0; i < place_map->graphCount(); i++)
        place_map->graph(i)->clearData();
    for(int i = 0; i < tsc_avg_plot->graphCount(); i++)
        tsc_avg_plot->graph(i)->clearData();

}

void TSCHybrid::stopProcessing()
{
    stop_processing = true;
}

void TSCHybrid::processImagesHierarchical(const string folder, const int start_idx, const int end_idx, int dataset_id)
{
    is_processing = true;

    //Read dataset image files
    img_files = getFiles(folder);

    Mat img_org, img;
    qint64 last_time;

    //SSG related variables
    static vector<vector<NodeSig> > ns_vec;  //Stores last tau_w node signatures
    static vector<float> coherency_scores_ssg;   //Stores all coherency scores
    static vector<int> detected_places_unfiltered;
    static vector<int> detected_places; //Stores all detected place ids
    static SSG temp_SSG(0); temp_SSG.setStartFrame(0);
    static TreeNode* hierarchy_tree;
    static vector<PlaceSSG> places;

    static int frame_count = 0;
    last_time = QDateTime::currentMSecsSinceEpoch();
    static float ssg_best_coherency_score = 0;

    static queue<Mat> img_history;
    static queue<Mat> img_resized_history;
    int delay = params->seg_track_params.tau_w/2 + params->ssg_params.tau_n;
    static queue<int> hist;

    //Create database
    DatabaseHandler db;
    if(save2database)
    {
        stringstream ss_db;
        ss_db << "conn_db_" << dataset_id;
        db.setConnName(ss_db.str());
        stringstream ss;
        ss << OUTPUT_FOLDER << "dataset_" << dataset_id << "_places.db";
        db.openDB(ss.str());
        db.createTables();
    }


    //Process all images
    for(int frame_no = start_idx; frame_no < end_idx-1; frame_no++)
    {
        if(stop_processing)
        {
            clearPastData();
            break;
        }

        while(next_frame == false)
        {
            waitKey(1);
        }

        img_org = imread(folder + img_files[frame_no]);
        resize(img_org, img, cv::Size(params->ssg_params.img_width, params->ssg_params.img_height));

        //Remove comments if you want to use
        //Gokce's segmentation results
        img = segmentImageGokce(img);

        //Keep last tau_w/2+tau_n images.
        img_history.push(img_org);
        img_resized_history.push(img);
        hist.push(frame_count);
        if(img_history.size() > delay)
        {
            img_history.pop();
            img_resized_history.pop();
            hist.pop();
        }
        emit showImgOrg(mat2QImage(img_resized_history.front()));

        ///////////////
        //Process SSG//
        ///////////////

        seg_track->processImage(img, ns_vec);

        //Calculate coherency based on existence map
        calcCohScore(seg_track, coherency_scores_ssg);


        //Show connectivity map
        showMap(seg_track->getM());

        //Decide last frame is whether transition or place
        //Results are written into detected places
        int detection_result = detectPlace(coherency_scores_ssg,detected_places_unfiltered,detected_places);

        //cv::Point2f coord = getCoordCold(img_files[dataset->start_idx+detected_places.size()]);
        //coords.push_back(coord);

        //Plot transition and place regions
        plotScoresSSG(coherency_scores_ssg, detected_places);

        //If started for new place
        //Create new SSG
        if(detection_result == DETECTION_PLACE_STARTED)
        {
            //Clear SSG
            temp_SSG.nodes.clear();
            temp_SSG.mean_invariant.release();
            temp_SSG.member_invariants.release();
            temp_SSG.setId(temp_SSG.getId()+1);
            temp_SSG.setStartFrame(frame_no);
            Mat map = seg_track->getM();
            Mat map_col = map.col(map.size().width - 1 - delay);
            vector<NodeSig> ns = ns_vec[ns_vec.size() - 1 - delay];
            SSGProc::updateSSG(temp_SSG, ns, map_col);
            SSGProc::updateSSGInvariants(temp_SSG, img_history.front(), params);
            qDebug() << "Place frame " << hist.front();

            //Reset coherency score
            ssg_best_coherency_score = coherency_scores_ssg.back();
        }
        else if(detection_result == DETECTION_PLACE_ENDED)
        {
            temp_SSG.setEndFrame(frame_no);

            if(temp_SSG.member_invariants.empty() == false)
            {
                SSGs.push_back(temp_SSG);
                if(save2database)
                {
                    db.insertSSG(temp_SSG);
                }
            }

            qDebug() << "Place detected" << SSGs.size();

            if(perform_recognition)
            {
                SSGProc::filterSummarySegments(temp_SSG, params->ssg_params.tau_p);
                emit showSSG(mat2QImage(SSGProc::drawSSG(temp_SSG, img)));

                PlaceSSG new_place(temp_SSG.getId(), temp_SSG);
                recognition->performRecognition(places, new_place, &hierarchy_tree);
            }

        }
        else if(detection_result == DETECTION_IN_PLACE)
        {
            temp_SSG.basepoints.push_back(ns_vec.back());
            Mat map = seg_track->getM();
            Mat map_col = map.col(map.size().width - 1 - delay);
            vector<NodeSig> ns = ns_vec[ns_vec.size() - 1 - delay];
            SSGProc::updateSSG(temp_SSG, ns, map_col);
            SSGProc::updateSSGInvariants(temp_SSG, img_history.front(), params);
            qDebug() << "Place frame " << hist.front();

            //If current frame is more coherent, set this frame as sample frame of SSG
            if(ssg_best_coherency_score < coherency_scores_ssg.back())
            {
                temp_SSG.setSampleFrame(folder + img_files[frame_no]);
                temp_SSG.setColor(dataset_id);
                ssg_best_coherency_score = coherency_scores_ssg.back();
            }
        }

        frame_count++;

        //Free variables
        img.release();
        img_org.release();

        //Wait a little for GUI processing
        waitKey(1);

        //next_frame = false;
    }

    if(save2database)
    {
        qDebug() << "Finished creating" << dataset_id << "th" << "dataset in " << (QDateTime::currentMSecsSinceEpoch()-last_time)/1000.0 << "seconds";
        db.closeDB();
    }


    is_processing = false;
    stop_processing = false;
}

void TSCHybrid::TSCPlaces2SSGPlaces(vector<Place>& TSC_places, vector<SSG>& SSG_places)
{
    for(int i = 0; i < TSC_places.size(); i++)
    {
        SSG new_ssg(i%17);
        new_ssg.member_invariants = TSC_places[i].memberInvariants.clone();
        new_ssg.mean_invariant = TSC_places[i].meanInvariant.clone();
        new_ssg.setStartFrame((int)TSC_places[i].members[0].id);
        new_ssg.setEndFrame((int)TSC_places[i].members.back().id);
        new_ssg.setColor(TSC_places[i].color);
        SSG_places.push_back(new_ssg);
    }
}

void TSCHybrid::processImagesHierarchicalTSC(const string folder, const int start_idx, const int end_idx, int dataset_id)
{
    is_processing = true;

    //Read dataset image files
    img_files = getFiles(folder);

    Mat img_org, img;
    qint64 last_time;

    //SSG related variables
    static vector<vector<NodeSig> > ns_vec;  //Stores last tau_w node signatures
    static vector<float> coherency_scores_ssg;   //Stores all coherency scores
    static vector<int> detected_places_unfiltered;
    static vector<int> detected_places; //Stores all detected place ids
    static SSG temp_SSG(0); temp_SSG.setStartFrame(0);
    static TreeNode* hierarchy_tree = NULL;
    static vector<PlaceSSG> places;

    //TSC related variables
    static vector<float> scores_tsc;


    static int frame_count = 0;
    last_time = QDateTime::currentMSecsSinceEpoch();
    static float ssg_best_coherency_score = 0;


    //Process all images
    for(int frame_no = start_idx; frame_no < end_idx-1; frame_no++)
    {
        if(stop_processing)
        {
            //clearPastData();
            //break;
        }

        while(next_frame == false)
        {
            waitKey(1);
        }

        img_org = imread(folder + img_files[frame_no]);
        //resize(img_org, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        resize(img_org, img, cv::Size(params->ssg_params.img_width, params->ssg_params.img_height));
        //emit showImgOrg(mat2QImage(img));

//        ///////////////
//        //Process SSG//
//        ///////////////

//        seg_track->processImage(img, ns_vec);

//        //Calculate coherency based on existence map
//        calcCohScore(seg_track, coherency_scores_ssg);


//        //Show connectivity map
//        //showMap(seg_track->getM());

//        //Decide last frame is whether transition or place
//        //Results are written into detected places
//        int detection_result = detectPlace(coherency_scores_ssg,detected_places_unfiltered,detected_places);


//        //cv::Point2f coord = getCoordCold(img_files[dataset->start_idx+detected_places.size()]);
//        //coords.push_back(coord);

//        //Plot transition and place regions
//        //plotScoresSSG(coherency_scores_ssg, detected_places);


//        //If started for new place
//        //Create new SSG
//        if(detection_result == DETECTION_PLACE_STARTED)
//        {
//            //Clear SSG
//            temp_SSG.nodes.clear();
//            temp_SSG.mean_invariant.release();
//            temp_SSG.member_invariants.release();
//            temp_SSG.setId(temp_SSG.getId()+1);
//            temp_SSG.setStartFrame(frame_no);
//            SSGProc::updateSSG(temp_SSG, ns_vec.back(), seg_track->getM());
//            SSGProc::updateSSGInvariants(temp_SSG, img_org, params);

//            //Reset coherency score
//            ssg_best_coherency_score = coherency_scores_ssg.back();
//        }
//        else if(detection_result == DETECTION_PLACE_ENDED)
//        {
//            //Commented out for experimental purpose
//            //SSGProc::filterSummarySegments(temp_SSG, params->tau_p);
//            //emit showSSG(mat2QImage(SSGProc::drawSSG(temp_SSG, img, params->tau_p)));

//            temp_SSG.setEndFrame(frame_no);
//            //PlaceSSG new_place(temp_SSG.getId(), temp_SSG);


//            //Expermental purpose
//            if(temp_SSG.member_invariants.empty() == false)
//                SSGs.push_back(temp_SSG);
//            qDebug() << "Place detected" << SSGs.size();

//            //Commented out for experimental purpose
//            //recognition->performRecognition(places, new_place, hierarchy_tree);
//        }
//        else if(detection_result == DETECTION_IN_PLACE)
//        {
//            temp_SSG.basepoints.push_back(ns_vec.back());
//            SSGProc::updateSSG(temp_SSG, ns_vec.back(), seg_track->getM());
//            SSGProc::updateSSGInvariants(temp_SSG, img_org, params);

//            //If current frame is more coherent, set this frame as sample frame of SSG
//            if(ssg_best_coherency_score < coherency_scores_ssg.back())
//            {
//                temp_SSG.setSampleFrame(folder + img_files[frame_no]);
//                temp_SSG.setColor(dataset_id);
//                ssg_best_coherency_score = coherency_scores_ssg.back();
//            }
//        }


        ///////////////
        //Process TSC//
        ///////////////

        //Process image
        bool isLastImage = frame_no == end_idx-2;
        float score = tsc->processImage(img_org, false);
        if(tsc->detector.detectedPlaces.size() > 0 && tsc->detector.detectedPlaces.back().color == 0)
        {
            tsc->detector.detectedPlaces.back().color = dataset_id;
        }
        scores_tsc.push_back(score);


        //Plot TSC scores
        if(!isLastImage)
            plotScoresTSC_(scores_tsc, tsc->detector.currentPlace, tsc->detector.currentBasePoint, tsc->detector.detectedPlaces);

        frame_count++;
        if(frame_count%100 == 0)
        {
            qDebug() << "Time elapsed in 100 frames: " << frame_count << (QDateTime::currentMSecsSinceEpoch() - last_time)/1000.0;
            last_time = QDateTime::currentMSecsSinceEpoch();
        }

        //Free variables
        img.release();
        img_org.release();

        //Wait a little for GUI processing
        waitKey(1);

        //next_frame = false;
    }

    //autoTryParameters();
    //qDebug() << places.size();
    //savePlacesFrameInfo(places);

    is_processing = false;
    stop_processing = false;
}


void TSCHybrid::reRecognizeInOrderTSCMethod(int method)
{
    TreeNode* hierarchy_tree;
    vector<PlaceSSG> places;

    //TSC places to SSG places conversion
    SSGs_fromTSC.clear();
    TSCPlaces2SSGPlaces(tsc->detector.detectedPlaces, SSGs_fromTSC);
    vector<SSG> SSGs = this->SSGs_fromTSC;

    vector<string> image_files = getFiles(dataset->location);
    plotDetectedPlaces(SSGs, image_files, dataset);

    recognition->setRecognitionMethod(method);

    for(int i = 0; i < SSGs.size(); i++)
    {
        PlaceSSG new_place(SSGs[i].getId(), SSGs[i]);
        recognition->performRecognition(places, new_place, &hierarchy_tree);
    }
}

void TSCHybrid::reRecognizeInOrder(int method)
{
    TreeNode* hierarchy_tree;
    vector<PlaceSSG> places;

    recognition->setRecognitionMethod(method);

    for(int i = 0; i < SSGs.size(); i++)
    {
        SSGProc::filterSummarySegments(SSGs[i], params->ssg_params.tau_p);
        PlaceSSG new_place(SSGs[i].getId(), SSGs[i]);
        recognition->performRecognitionHybrid(places, new_place, &hierarchy_tree);
    }

    //qDebug() << "N2N Tree Distances:";
    //recognition->calculateN2NDistanceMatrix(hierarchy_tree);
    recognition->calculateRecPerformance(hierarchy_tree);

}

void TSCHybrid::reRecognizeBatch(int method)
{
    TreeNode* hierarchy_tree;
    vector<PlaceSSG> places;

    //Plot place in map
//    vector<string> image_files = getFiles("/home/isl-mahmut/Datasets/fr_seq2_cloudy2/std_cam/");
//    plotDetectedPlacesX(SSGs, image_files);

    recognition->setRecognitionMethod(method);

    int i = 0;
    for(i = 0; i < SSGs.size()-1; i++)
    {
        SSG ssg = SSGs[i];
        SSGProc::filterSummarySegments(ssg, params->ssg_params.tau_p);
        PlaceSSG new_place(ssg.getId(), ssg);
        places.push_back(new_place);
    }

    SSG ssg = SSGs[i];
    SSGProc::filterSummarySegments(ssg, params->ssg_params.tau_p);

    PlaceSSG new_place(ssg.getId(), ssg);

    recognition->performRecognitionHybrid(places, new_place, &hierarchy_tree);

    //recognition->calculateRecPerformance(places.size(), dist_matrix, hierarchy_tree);
    qDebug() << "N2N";
    recognition->calculateN2NTreeDistanceMatrix(hierarchy_tree);
    qDebug() << "BD";
    recognition->calculateBDDistanceMatrix(hierarchy_tree);
    qDebug() << "SSG";
    recognition->calculateSSGDistanceMatrix(hierarchy_tree);
}

float TSCHybrid::calcCohScore(SegmentTrack* seg_track, vector<float>& coh_scores)
{
    Mat M = seg_track->getM();
    vector<pair<NodeSig, int> > M_ns = seg_track->getM_ns();
    float tau_w = params->seg_track_params.tau_w;

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
                if(M_ns[i].second > params->ssg_params.tau_f)
                    incoherency += params->ssg_params.coeff_node_disappear1;
                else
                    incoherency += params->ssg_params.coeff_node_disappear2;
            }

            //Incoherency: Appeared node
            if(M.at<int>(i, j) == 0 && M.at<int>(i, j+1) > 0)
            {
                    incoherency += params->ssg_params.coeff_node_appear;
            }

            //Coherency: Stabile node
            if(M.at<int>(i,j) > 0)
                nr_appear++;
        }

        //Active nodes are only the ones that appeared at least one time through window
        if(nr_appear > 0)
            active_nodes++;

        //If node appeared at least certain percent then it's a stabile node.
        if((float)nr_appear/tau_w > params->ssg_params.coeff_coh_appear_thres)
            coherency += (float)nr_appear/tau_w;
    }

    // Coherency score calculation ( -1 < coh_range < 1)
    // coherency_score = (1 - a^(-coherency)) - (incoherency)
    coh_score = 1-pow(params->ssg_params.coeff_coh_exp_base,-1*coherency) - (incoherency/active_nodes);

    coh_scores.push_back(coh_score);

    return coh_score;
}

// Plots places and coherency scores
void TSCHybrid::plotScoresSSGOneShot(vector<float> coherency_scores, vector<int> detected_places, bool reset)
{
    static int graph_idx = PLOT_PLACES_IDX;

    if(reset)
    {
        graph_idx = PLOT_PLACES_IDX;
    }

    float margin = 1.0;

    for(int i = 1; i < detected_places.size(); i++)
    {
        //Place->Transition or vice versa region change occur
        if(graph_idx == PLOT_PLACES_IDX || ( detected_places[i] != detected_places[i-1]) )
        {
            //Add new graph to change color
            this->ssg_plot->addGraph();
            //this->place_map->addGraph();
            graph_idx++;

            if(detected_places[i] > 0)
            {
                //Place
                this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::red));
                this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(255, 0, 0, 80)));

                QPen dumPen;
                dumPen.setWidth(1);
                dumPen.setColor(QColor(255, 0, 0, 50));

                //this->place_map->graph(graph_idx)->setPen(dumPen);
                //this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                //this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
            else
            {
                //Transition
                this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 80)));

                QPen dumPen;
                dumPen.setWidth(3);
                dumPen.setColor(QColor(0, 0, 255, 50));

                //this->place_map->graph(graph_idx)->setPen(dumPen);
                //this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                //this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
        }

        //Add the new point
        ssg_plot->graph(graph_idx)->addData(i,1.0+margin);
        //place_map->graph(graph_idx)->addData(coords[i].x, coords[i].y);

        //Add coh score
        ssg_plot->graph(PLOT_SCORES_IDX)->addData(i,coherency_scores[i]+margin);

        //Add thresold line
        ssg_plot->graph(PLOT_THRES_IDX)->addData(i,params->ssg_params.tau_c+margin);
    }

    ssg_plot->rescaleAxes();
    ssg_plot->replot();
    //place_map->rescaleAxes();
    //place_map->replot();
}

// Plots places and coherency scores
void TSCHybrid::plotDetectedPlacesX(vector<SSG> SSGs, const vector<string>& image_files)
{
    //Cold-Fr
    int MAP_W = 1000;
    int MAP_H = 750;
    int MAP_L = -20;
    int MAP_R = 20;
    int MAP_T = 10;
    int MAP_B = -20;

    qDebug() << "size" << SSGs.size();
    int graph_idx = -1;

    //Traveled places plot initialization
    this->place_map = place_map;

    this->place_map->axisRect()->setAutoMargins(QCP::msAll);
    this->place_map->axisRect()->setMargins(QMargins(10,10,10,10));
    this->place_map->setMaximumWidth(MAP_W);
    this->place_map->setMinimumWidth(MAP_W);
    this->place_map->setMinimumHeight(MAP_H);
    this->place_map->setMaximumHeight(MAP_H);
    this->place_map->xAxis->grid()->setVisible(false);
    this->place_map->yAxis->grid()->setVisible(false);
    this->place_map->xAxis->setTicks(false);
    this->place_map->yAxis->setTicks(false);
    this->place_map->xAxis->setVisible(false);
    this->place_map->yAxis->setVisible(false);
    this->place_map->xAxis->setRange(MAP_L, MAP_R);
    this->place_map->yAxis->setRange(MAP_T, MAP_B);


    for(int i = 0; i < SSGs.size(); i++)
    {
        QPen dumPen;
        dumPen.setWidth(0.5);
        dumPen.setColor(QColor(255, 0, 0, 50));

        this->place_map->addGraph();
        graph_idx++;
        this->place_map->graph(graph_idx)->setPen(dumPen);
        this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
        this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);

        qDebug() << "k" << SSGs[i].getStartFrame() << SSGs[i].getEndFrame();
        for(int j = SSGs[i].getStartFrame(); j < SSGs[i].getEndFrame(); j++)
        {
            Point2f coord = getCoordCold(image_files[j]);
            place_map->graph(graph_idx)->addData(coord.x, coord.y);
            qDebug() << "a" << coord.x << coord.y;
        }
    }
    for(int i = 0; i < SSGs.size()-1; i++)
    {
        QPen dumPen;
        dumPen.setWidth(0.5);
        dumPen.setColor(QColor(0, 0, 255, 50));

        this->place_map->addGraph();
        graph_idx++;
        this->place_map->graph(graph_idx)->setPen(dumPen);
        this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
        this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);

        qDebug() << "t" << SSGs[i].getEndFrame() << SSGs[i+1].getStartFrame();
        for(int j = SSGs[i].getEndFrame(); j < SSGs[i+1].getStartFrame(); j++)
        {
            Point2f coord = getCoordCold(image_files[j]);
            place_map->graph(graph_idx)->addData(coord.x, coord.y);
            qDebug() << "b" << coord.x << coord.y;
        }
    }

    place_map->replot();
    QString save_loc = QString(OUTPUT_FOLDER).append("detected_places").append(QString::number(5)).append(".png");
    this->place_map->savePng(save_loc);


    for(int i = 0; i < SSGs.size(); i++)
    {

        float shift = 0;

        //fr-1
//        if(SSGs[i].getId() == 0)
//            shift = -1;
//        else if(SSGs[i].getId() == 1)
//            shift = 1;
//        else if(SSGs[i].getId() == 2)
//            shift = 1;
//        else if(SSGs[i].getId() == 3)
//            shift = 1;
//        else if(SSGs[i].getId() == 4)
//            shift = 1;
//        else if(SSGs[i].getId() == 5)
//            shift = 2;
//        else if(SSGs[i].getId() == 6)
//            shift = -1;
//        else if(SSGs[i].getId() == 7)
//            shift = 1;
//        else if(SSGs[i].getId() == 8)
//            shift = 2;
//        else if(SSGs[i].getId() == 9)
//            shift = 1;
//        else if(SSGs[i].getId() == 10)
//            shift = -1;
//        else if(SSGs[i].getId() == 11)
//            shift = -0.7;
//        else if(SSGs[i].getId() == 12)
//            shift = 0;
//        else if(SSGs[i].getId() == 13)
//            shift = 1;
//        else if(SSGs[i].getId() == 14)
//            shift = 1;
//        else if(SSGs[i].getId() == 15)
//            shift = 1;
//        else if(SSGs[i].getId() == 16)
//            shift = 1;
//        else if(SSGs[i].getId() == 17)
//            shift = -2.5;
//        else if(SSGs[i].getId() == 18)
//            shift = -2;
//        else if(SSGs[i].getId() == 19)
//            shift = 1;


        //fr-2
        if(SSGs[i].getId() == 0)
            shift = -1;
        else if(SSGs[i].getId() == 1)
            shift = 1;
        else if(SSGs[i].getId() == 2)
            shift = -1;
        else if(SSGs[i].getId() == 3)
            shift = 1;
        else if(SSGs[i].getId() == 4)
            shift = 1;
        else if(SSGs[i].getId() == 5)
            shift = 0;
        else if(SSGs[i].getId() == 6)
            shift = 0.5;
        else if(SSGs[i].getId() == 7)
            shift = -1;
        else if(SSGs[i].getId() == 8)
            shift = -2;
        else if(SSGs[i].getId() == 9)
            shift = 1;
        else if(SSGs[i].getId() == 10)
            shift = 0.5;
        else if(SSGs[i].getId() == 11)
            shift = 0;
        else if(SSGs[i].getId() == 12)
            shift = 0;
        else if(SSGs[i].getId() == 13)
            shift = 0;
        else if(SSGs[i].getId() == 14)
            shift = -1;
        else if(SSGs[i].getId() == 15)
            shift = 1;
        else if(SSGs[i].getId() == 16)
            shift = -1;
        else if(SSGs[i].getId() == 17)
            shift = -1;
        else if(SSGs[i].getId() == 18)
            shift = -2.5;
        else if(SSGs[i].getId() == 19)
            shift = -1;
        else if(SSGs[i].getId() == 20)
            shift = 0.5;
        else if(SSGs[i].getId() == 21)
            shift = -1;
        else if(SSGs[i].getId() == 22)
            shift = -1;



        int sample_frame_id = (SSGs[i].getEndFrame() + SSGs[i].getStartFrame()) / 2;
        QCPItemText *textLabel = new QCPItemText(this->place_map);
        this->place_map->addItem(textLabel);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        Point2f coord = getCoordCold(image_files[sample_frame_id]);
        textLabel->position->setCoords(QPointF(coord.x+shift, coord.y+1));
        textLabel->setFont(QFont(QFont().family(),16)); // make font a bit larger
        textLabel->setText(QString::number(SSGs[i].getId()));
        textLabel->setPen(QPen(Qt::black)); // show black border around text

        // add the arrow:
        QCPItemLine *arrow = new QCPItemLine(this->place_map);
        this->place_map->addItem(arrow);
        arrow->start->setParentAnchor(textLabel->bottom);
        arrow->end->setCoords(coord.x, coord.y);
        arrow->setHead(QCPLineEnding::esSpikeArrow);
    }
    //place_map->rescaleAxes();
    place_map->replot();
    save_loc = QString(OUTPUT_FOLDER).append("detected_places").append(5).append("_numbered.png");
    this->place_map->savePng(save_loc);
}

// Plots places and coherency scores
void TSCHybrid::plotDetectedPlaces(vector<SSG> SSGs, const vector<string>& image_files, Dataset* dataset)
{
    //Cold-Fr
    int MAP_W = 1000;
    int MAP_H = 750;
    int MAP_L = -20;
    int MAP_R = 20;
    int MAP_T = 10;
    int MAP_B = -20;

    //Cold-Sa
//    int MAP_W = 1000;
//    int MAP_H = 750;
//    int MAP_L = -15;
//    int MAP_R = 15;
//    int MAP_T = 15;
//    int MAP_B = -15;

    //Cold-Lj-1
//    int MAP_W = 1000;
//    int MAP_H = 750;
//    int MAP_L = -10;
//    int MAP_R = 30;
//    int MAP_T = 100;
//    int MAP_B = 0;

    //Cold-Lj-2
//    int MAP_W = 1000;
//    int MAP_H = 750;
//    int MAP_L = -15;
//    int MAP_R = 35;
//    int MAP_T = 100;
//    int MAP_B = 0;
    qDebug() << "size" << SSGs.size();
    int graph_idx = -1;

    //Traveled places plot initialization
    this->place_map = place_map;

    this->place_map->axisRect()->setAutoMargins(QCP::msAll);
    this->place_map->axisRect()->setMargins(QMargins(10,10,10,10));
    this->place_map->setMaximumWidth(MAP_W);
    this->place_map->setMinimumWidth(MAP_W);
    this->place_map->setMinimumHeight(MAP_H);
    this->place_map->setMaximumHeight(MAP_H);
    this->place_map->xAxis->grid()->setVisible(false);
    this->place_map->yAxis->grid()->setVisible(false);
    this->place_map->xAxis->setTicks(false);
    this->place_map->yAxis->setTicks(false);
    this->place_map->xAxis->setVisible(false);
    this->place_map->yAxis->setVisible(false);
    this->place_map->xAxis->setRange(MAP_L, MAP_R);
    this->place_map->yAxis->setRange(MAP_T, MAP_B);


    for(int i = 0; i < SSGs.size(); i++)
    {
        QPen dumPen;
        dumPen.setWidth(0.5);
        dumPen.setColor(QColor(255, 0, 0, 50));

        this->place_map->addGraph();
        graph_idx++;
        this->place_map->graph(graph_idx)->setPen(dumPen);
        this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
        this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);

        qDebug() << "k" << SSGs[i].getStartFrame() << SSGs[i].getEndFrame();
        for(int j = SSGs[i].getStartFrame(); j < SSGs[i].getEndFrame(); j++)
        {
            Point2f coord = getCoordCold(image_files[j]);
            place_map->graph(graph_idx)->addData(coord.x, coord.y);
            qDebug() << "a" << coord.x << coord.y;
        }
    }
    for(int i = 0; i < SSGs.size()-1; i++)
    {
        QPen dumPen;
        dumPen.setWidth(0.5);
        dumPen.setColor(QColor(0, 0, 255, 50));

        this->place_map->addGraph();
        graph_idx++;
        this->place_map->graph(graph_idx)->setPen(dumPen);
        this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
        this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);

        qDebug() << "t" << SSGs[i].getEndFrame() << SSGs[i+1].getStartFrame();
        for(int j = SSGs[i].getEndFrame(); j < SSGs[i+1].getStartFrame(); j++)
        {
            Point2f coord = getCoordCold(image_files[j]);
            place_map->graph(graph_idx)->addData(coord.x, coord.y);
            qDebug() << "b" << coord.x << coord.y;
        }
    }

    place_map->replot();
    QString save_loc = QString(OUTPUT_FOLDER).append("detected_places").append(QString::number(dataset->dataset_id)).append(".png");
    this->place_map->savePng(save_loc);


    for(int i = 0; i < SSGs.size(); i++)
    {

        float shift = 0;

        //fr-1
        if(SSGs[i].getId() == 0)
            shift = -1;
        else if(SSGs[i].getId() == 1)
            shift = 1;
        else if(SSGs[i].getId() == 2)
            shift = 1;
        else if(SSGs[i].getId() == 3)
            shift = 1;
        else if(SSGs[i].getId() == 4)
            shift = 1;
        else if(SSGs[i].getId() == 5)
            shift = 2;
        else if(SSGs[i].getId() == 6)
            shift = -1;
        else if(SSGs[i].getId() == 7)
            shift = 1;
        else if(SSGs[i].getId() == 8)
            shift = 2;
        else if(SSGs[i].getId() == 9)
            shift = 1;
        else if(SSGs[i].getId() == 10)
            shift = -1;
        else if(SSGs[i].getId() == 11)
            shift = -0.7;
        else if(SSGs[i].getId() == 12)
            shift = 0;
        else if(SSGs[i].getId() == 13)
            shift = 1;
        else if(SSGs[i].getId() == 14)
            shift = 1;
        else if(SSGs[i].getId() == 15)
            shift = 1;
        else if(SSGs[i].getId() == 16)
            shift = 1;
        else if(SSGs[i].getId() == 17)
            shift = -2.5;
        else if(SSGs[i].getId() == 18)
            shift = -2;
        else if(SSGs[i].getId() == 19)
            shift = 1;


        //fr-2
//        if(SSGs[i].getId() == 0)
//            shift = -1;
//        else if(SSGs[i].getId() == 1)
//            shift = 1;
//        else if(SSGs[i].getId() == 2)
//            shift = -1;
//        else if(SSGs[i].getId() == 3)
//            shift = 1;
//        else if(SSGs[i].getId() == 4)
//            shift = 1;
//        else if(SSGs[i].getId() == 5)
//            shift = 0;
//        else if(SSGs[i].getId() == 6)
//            shift = 0.5;
//        else if(SSGs[i].getId() == 7)
//            shift = -1;
//        else if(SSGs[i].getId() == 8)
//            shift = -2;
//        else if(SSGs[i].getId() == 9)
//            shift = 1;
//        else if(SSGs[i].getId() == 10)
//            shift = 0.5;
//        else if(SSGs[i].getId() == 11)
//            shift = 0;
//        else if(SSGs[i].getId() == 12)
//            shift = 0;
//        else if(SSGs[i].getId() == 13)
//            shift = 0;
//        else if(SSGs[i].getId() == 14)
//            shift = -1;
//        else if(SSGs[i].getId() == 15)
//            shift = 1;
//        else if(SSGs[i].getId() == 16)
//            shift = -1;
//        else if(SSGs[i].getId() == 17)
//            shift = -1;
//        else if(SSGs[i].getId() == 18)
//            shift = -2.5;
//        else if(SSGs[i].getId() == 19)
//            shift = -1;
//        else if(SSGs[i].getId() == 20)
//            shift = 0.5;
//        else if(SSGs[i].getId() == 21)
//            shift = -1;
//        else if(SSGs[i].getId() == 22)
//            shift = -1;

//        //sa-1
//        if(SSGs[i].getId() == 0)
//            shift = 0.5;
//        else if(SSGs[i].getId() == 1)
//            shift = -1;
//        else if(SSGs[i].getId() == 2)
//            shift = 1;
//        else if(SSGs[i].getId() == 3)
//            shift = -0.5;
//        else if(SSGs[i].getId() == 4)
//            shift = -2;
//        else if(SSGs[i].getId() == 5)
//            shift = 1;
//        else if(SSGs[i].getId() == 6)
//            shift = -1;
//        else if(SSGs[i].getId() == 7)
//            shift = -1;
//        else if(SSGs[i].getId() == 8)
//            shift = -1;
//        else if(SSGs[i].getId() == 9)
//            shift = 1;
//        else if(SSGs[i].getId() == 10)
//            shift = 1;

        //sa-2
//        if(SSGs[i].getId() == 0)
//            shift = 0.5;
//        else if(SSGs[i].getId() == 1)
//            shift = -1;
//        else if(SSGs[i].getId() == 2)
//            shift = -1;
//        else if(SSGs[i].getId() == 3)
//            shift = 1;
//        else if(SSGs[i].getId() == 4)
//            shift = -0.5;
//        else if(SSGs[i].getId() == 5)
//            shift = -1;
//        else if(SSGs[i].getId() == 6)
//            shift = 1;
//        else if(SSGs[i].getId() == 7)
//            shift = -1.3;
//        else if(SSGs[i].getId() == 8)
//            shift = -1;
//        else if(SSGs[i].getId() == 9)
//            shift = 1;
//        else if(SSGs[i].getId() == 10)
//            shift = 1;
//        else if(SSGs[i].getId() == 11)
//            shift = 2;
//        else if(SSGs[i].getId() == 12)
//            shift = 1;
//        else if(SSGs[i].getId() == 13)
//            shift = -1;
//        else if(SSGs[i].getId() == 14)
//            shift = 2;
//        else if(SSGs[i].getId() == 15)
//            shift = 1.2;

//        //lj-1
//        if(SSGs[i].getId() == 0)
//            shift = -2;
//        else if(SSGs[i].getId() == 1)
//            shift = -1;
//        else if(SSGs[i].getId() == 2)
//            shift = -1;
//        else if(SSGs[i].getId() == 3)
//            shift = 1;
//        else if(SSGs[i].getId() == 4)
//            shift = 1;
//        else if(SSGs[i].getId() == 5)
//            shift = 2;
//        else if(SSGs[i].getId() == 6)
//            shift = 1;
//        else if(SSGs[i].getId() == 7)
//            shift = -1;
//        else if(SSGs[i].getId() == 8)
//            shift = -1;
//        else if(SSGs[i].getId() == 9)
//            shift = -1;
//        else if(SSGs[i].getId() == 10)
//            shift = 2;
//        else if(SSGs[i].getId() == 11)
//            shift = 2;
//        else if(SSGs[i].getId() == 12)
//            shift = 2;
//        else if(SSGs[i].getId() == 13)
//            shift = 2;
//        else if(SSGs[i].getId() == 14)
//            shift = 3;
//        else if(SSGs[i].getId() == 15)
//            shift = -1;
//        else if(SSGs[i].getId() == 16)
//            shift = 1;
//        else if(SSGs[i].getId() == 17)
//            shift = -1;
//        else if(SSGs[i].getId() == 18)
//            shift = -2.5;
//        else if(SSGs[i].getId() == 19)
//            shift = -3;
//        else if(SSGs[i].getId() == 20)
//            shift = -1.5;
//        else if(SSGs[i].getId() == 21)
//            shift = -1;
//        else if(SSGs[i].getId() == 22)
//            shift = -2;
//        else if(SSGs[i].getId() == 23)
//            shift = 1.5;
//        else if(SSGs[i].getId() == 24)
//            shift = 1;
//        else if(SSGs[i].getId() == 25)
//            shift = 1;
//        else if(SSGs[i].getId() == 26)
//            shift = 1;
//        else if(SSGs[i].getId() == 27)
//            shift = 1;
//        else if(SSGs[i].getId() == 28)
//            shift = 0.5;

//        //lj-2
//        if(SSGs[i].getId() == 0)
//            shift = -2;
//        else if(SSGs[i].getId() == 1)
//            shift = -1;
//        else if(SSGs[i].getId() == 2)
//            shift = -1;
//        else if(SSGs[i].getId() == 3)
//            shift = 1;
//        else if(SSGs[i].getId() == 4)
//            shift = 1;
//        else if(SSGs[i].getId() == 5)
//            shift = -4;
//        else if(SSGs[i].getId() == 6)
//            shift = -2;
//        else if(SSGs[i].getId() == 7)
//            shift = -1;
//        else if(SSGs[i].getId() == 8)
//            shift = -1;
//        else if(SSGs[i].getId() == 9)
//            shift = -1;
//        else if(SSGs[i].getId() == 10)
//            shift = -1;
//        else if(SSGs[i].getId() == 11)
//            shift = -2;
//        else if(SSGs[i].getId() == 12)
//            shift = 2;
//        else if(SSGs[i].getId() == 13)
//            shift = 1;
//        else if(SSGs[i].getId() == 14)
//            shift = 3;
//        else if(SSGs[i].getId() == 15)
//            shift = -1;
//        else if(SSGs[i].getId() == 16)
//            shift = -2;
//        else if(SSGs[i].getId() == 17)
//            shift = -1;
//        else if(SSGs[i].getId() == 18)
//            shift = 1;
//        else if(SSGs[i].getId() == 19)
//            shift = 1;
//        else if(SSGs[i].getId() == 20)
//            shift = 1;
//        else if(SSGs[i].getId() == 21)
//            shift = 1;
//        else if(SSGs[i].getId() == 22)
//            shift = 1;
//        else if(SSGs[i].getId() == 23)
//            shift = 0.5;
//        else if(SSGs[i].getId() == 24)
//            shift = 1.5;
//        else if(SSGs[i].getId() == 25)
//            shift = 1;
//        else if(SSGs[i].getId() == 26)
//            shift = 1;
//        else if(SSGs[i].getId() == 27)
//            shift = 1;
//        else if(SSGs[i].getId() == 28)
//            shift = 0.5;

        int sample_frame_id = (SSGs[i].getEndFrame() + SSGs[i].getStartFrame()) / 2;
        QCPItemText *textLabel = new QCPItemText(this->place_map);
        this->place_map->addItem(textLabel);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        Point2f coord = getCoordCold(image_files[sample_frame_id]);
        textLabel->position->setCoords(QPointF(coord.x+shift, coord.y+1));
        textLabel->setFont(QFont(QFont().family(),16)); // make font a bit larger
        textLabel->setText(QString::number(SSGs[i].getId()));
        textLabel->setPen(QPen(Qt::black)); // show black border around text

        // add the arrow:
        QCPItemLine *arrow = new QCPItemLine(this->place_map);
        this->place_map->addItem(arrow);
        arrow->start->setParentAnchor(textLabel->bottom);
        arrow->end->setCoords(coord.x, coord.y);
        arrow->setHead(QCPLineEnding::esSpikeArrow);
    }
    //place_map->rescaleAxes();
    place_map->replot();
    save_loc = QString(OUTPUT_FOLDER).append("detected_places").append(QString::number(dataset->dataset_id)).append("_numbered.png");
    this->place_map->savePng(save_loc);
}

// Plots places and coherency scores
void TSCHybrid::plotScoresSSG(vector<float> coherency_scores, vector<int> detected_places, cv::Point2f coord, bool reset)
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

                //this->place_map->graph(graph_idx)->setPen(dumPen);
                //this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                //this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
            else
            {
                //Transition
                this->ssg_plot->graph(graph_idx)->setPen(QPen(Qt::blue));
                this->ssg_plot->graph(graph_idx)->setBrush(QBrush(QColor(0, 0, 255, 80)));

                QPen dumPen;
                dumPen.setWidth(3);
                dumPen.setColor(QColor(0, 0, 255, 50));

                //this->place_map->graph(graph_idx)->setPen(dumPen);
                //this->place_map->graph(graph_idx)->setLineStyle(QCPGraph::lsNone);
                //this->place_map->graph(graph_idx)->setScatterStyle(QCPScatterStyle::ssDisc);
            }
        }

        //Add the new point
        ssg_plot->graph(graph_idx)->addData(detected_places.size(),1.0+margin);
        //place_map->graph(graph_idx)->addData(coord.x, coord.y);

        //Add coh score
        ssg_plot->graph(PLOT_SCORES_IDX)->addData(detected_places.size(),coherency_scores[detected_places.size()-1]+margin);
    }

    //Add thresold line
    ssg_plot->graph(PLOT_THRES_IDX)->addData(thres_cursor,params->ssg_params.tau_c+margin);
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
            tsc_plot->graph(graph_idx)->addData(i-dataset->start_idx, height/3);
            if(tsc_detected_places[i-dataset->start_idx] != -1)
                tsc_detected_places[i-dataset->start_idx] = 1;
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
            tsc_plot->graph(graph_idx)->addData(i-dataset->start_idx, height/3);
            if(tsc_detected_places[i-dataset->start_idx] != -1)
                tsc_detected_places[i-dataset->start_idx] = 0;
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
        this->tsc_plot->graph(graph_idx)->addData(last_base_point-dataset->start_idx, height/3);
        tsc_detected_places[last_base_point-dataset->start_idx] = 0;
    }

    //Uninformative
    if(cur_base_point.status == 1)
    {
        this->tsc_plot->graph(PLOT_UNINFORMATIVE_IDX)->addData(cur_base_point.id-dataset->start_idx, height/2/3);
        tsc_detected_places[cur_base_point.id-dataset->start_idx] = -1;
    }
    else if(cur_base_point.status == 2)
    {
        this->tsc_plot->graph(PLOT_INCOHERENT_IDX)->addData(cur_base_point.id-dataset->start_idx, 3*height/4/3);
    }



    tsc_plot->yAxis->setRange(0,height/3);
    tsc_plot->xAxis->rescale();
    //tsc_plot->rescaleAxes();
    tsc_plot->replot();
}

void TSCHybrid::plotScoresTSC_(vector<float> scores, Place* current_place, BasePoint cur_base_point, vector<Place> detected_places)
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
            tsc_plot->graph(graph_idx)->addData(i-dataset->start_idx, height/3);
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
            tsc_plot->graph(graph_idx)->addData(i-dataset->start_idx, height/3);
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
        this->tsc_plot->graph(graph_idx)->addData(last_base_point-dataset->start_idx, height/3);
    }

    //Uninformative
    if(cur_base_point.status == 1)
    {
        this->tsc_plot->graph(PLOT_UNINFORMATIVE_IDX)->addData(cur_base_point.id-dataset->start_idx, height/2/3);
    }
    else if(cur_base_point.status == 2)
    {
        this->tsc_plot->graph(PLOT_INCOHERENT_IDX)->addData(cur_base_point.id-dataset->start_idx, 3*height/4/3);
    }



    tsc_plot->yAxis->setRange(0,height/3);
    tsc_plot->xAxis->rescale();
    //tsc_plot->rescaleAxes();
    tsc_plot->replot();
}

void TSCHybrid::plotAvgScoresTSC(vector<float>& scores, std::vector<BasePoint> wholebasepoints, vector<float>& avg_scores)
{
    int window_size = params->seg_track_params.tau_w;
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
        return DETECTION_NOT_STARTED;
    }


    //Pre-assign last frame as either transition or place candidate
    detected_places_unfiltered.push_back(coherency_scores.back() < params->ssg_params.tau_c ? -1 : 1);


    //Place/transition region is decided according to median of
    //last tau_w frames. Therefore wait minimum number of
    //frames to be larger than tau_n
    if(detected_places_unfiltered.size() >= params->ssg_params.tau_n)
    {
        if(detected_places.size() == 0)
        {
            for(int i = 0; i < coherency_scores.size()-params->ssg_params.tau_n; i++)
            {
                detected_places.push_back(detected_places_unfiltered.back());
            }
        }
        int last_place_type = detected_places.back();

        bool region_status = getRegionStatus(vector<int>(detected_places_unfiltered.end()-params->ssg_params.tau_n,
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
                return DETECTION_PLACE_ENDED;
            else
                return DETECTION_PLACE_STARTED;
        }
        else
            return last_place_type > 0 ? DETECTION_IN_PLACE : DETECTION_IN_TRANSITION;
    }
    else
    {
        return DETECTION_NOT_STARTED;
    }
}

bool TSCHybrid::eventFilter( QObject* watched, QEvent* event ) {
    int max_frames = dataset->end_idx-dataset->start_idx;

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
    ssg_plot->graph(PLOT_TRACK_IDX)->addData(cursor,params->ssg_params.tau_c+1);
    ssg_plot->rescaleAxes();
    ssg_plot->replot();

    string filepath = dataset->location + img_files[dataset->start_idx + cursor];

    Mat img = imread(filepath);
    resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);

    //Show original images on the window
    emit showImgOrg(mat2QImage(img));


    return true;
}
