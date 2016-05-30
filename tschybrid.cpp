#include "tschybrid.h"
#include "TSC/tsc.h"

TSCHybrid::TSCHybrid(QCustomPlot* tsc_plot,
                     QCustomPlot* ssg_plot,
                     QCustomPlot* place_map,
                     QCustomPlot* tsc_avg_plot,
                     SSGParams* ssg_params,
                     SegmentTrackParams* seg_track_params,
                     SegmentationParams* seg_params,
                     GraphMatchParams* gm_params,
                     RecognitionParams* rec_params)
{
    vector<string> img_files = getFiles(DATASET_FOLDER);
    Mat img = imread(DATASET_FOLDER + img_files[START_IDX]);
    int img_height = img.size().height;
    int img_width = img.size().width;


    this->tsc_plot = tsc_plot;
    this->ssg_plot = ssg_plot;
    this->tsc_avg_plot = tsc_avg_plot;
    this->params = ssg_params;
    this->seg_track_params = seg_track_params;
    this->seg_params = seg_params;
    this->gm_params = gm_params;
    this->rec_params = rec_params;


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
    string overlay_img = string(OUTPUT_FOLDER)+"cold.png";
    plot_bg_img->setPixmap(QPixmap(overlay_img.c_str()));
    plot_bg_img->topLeft->setType(QCPItemPosition::ptViewportRatio);
    plot_bg_img->topLeft->setCoords(0,0);
    this->place_map->addLayer("imageLayer");
    this->place_map->addItem(plot_bg_img);
    this->place_map->addLayer("plotLayer");
    this->place_map->setCurrentLayer("plotLayer");


    seg_track = new SegmentTrack(seg_track_params, seg_params, gm_params);

    recognition = new Recognition (rec_params,
                                   ssg_params,
                                   seg_track_params,
                                   seg_params,
                                   gm_params,
                                   seg_track->gm,
                                   seg_track->seg);

    //Bubble process init
    bubbleProcess::calculateImagePanAngles(FOCAL_LENGHT_PIXELS, img_width, img_height);
    bubbleProcess::calculateImageTiltAngles(FOCAL_LENGHT_PIXELS, img_width, img_height);

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


}

TSCHybrid::~TSCHybrid()
{
    delete recognition;
    delete seg_track;
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

void TSCHybrid::processImages(const string folder, const int start_idx, const int end_idx)
{
    is_processing = true;

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
        if(stop_processing)
        {
            clearPastData();
            break;
        }



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
        detectPlace(coherency_scores_ssg,detected_places_unfiltered,detected_places);

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
        cout << "TSC time elapsed: " << stop_time-start_time << endl;


        //Plot TSC scores
        if(!isLastImage)
            plotScoresTSC(scores_tsc, tsc->detector.currentPlace, tsc->detector.currentBasePoint, tsc->detector.detectedPlaces, tsc_detected_places);

        //Plot Avg TSC scores
        plotAvgScoresTSC(scores_tsc, tsc->detector.wholebasepoints, tsc_avg_scores);

        //Wait a little for GUI processing
        waitKey(1);
    }


    if(!stop_processing)
    {
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

    is_processing = false;
    stop_processing = false;

}

void TSCHybrid::stopProcessing()
{
    stop_processing = true;
}

void TSCHybrid::processImagesHierarchical(const string folder, const int start_idx, const int end_idx)
{
// For parameter tuning purpose
//    findBestParameters();
//    return;

    is_processing = true;

    //Read dataset image files
    img_files = getFiles(folder);

    Mat img_org, img;
    qint64 last_time;

    //SSG related variables
    vector<vector<NodeSig> > ns_vec;  //Stores last tau_w node signatures
    vector<float> coherency_scores_ssg;   //Stores all coherency scores
    vector<int> detected_places_unfiltered;
    vector<int> detected_places; //Stores all detected place ids
    SSG temp_SSG(0); temp_SSG.setStartFrame(0);
    TreeNode* hierarchy_tree = NULL;
    vector<PlaceSSG> places;

    int frame_count = 0;
    last_time = QDateTime::currentMSecsSinceEpoch();


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
        resize(img_org, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        //emit showImgOrg(mat2QImage(img));

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

        cv::Point2f coord = getCoordCold(img_files[START_IDX+detected_places.size()]);
        coords.push_back(coord);

        //Plot transition and place regions
        plotScoresSSG(coherency_scores_ssg, detected_places, coord, frame_no==start_idx);

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
            SSGProc::updateSSG(temp_SSG, ns_vec.back(), seg_track->getM());
            SSGProc::updateSSGInvariants(temp_SSG, img_org);
        }
        else if(detection_result == DETECTION_PLACE_ENDED)
        {
            //Commented out for experimental purpose
            //SSGProc::filterSummarySegments(temp_SSG, params->tau_p);
            //emit showSSG(mat2QImage(SSGProc::drawSSG(temp_SSG, img, params->tau_p)));

            temp_SSG.setEndFrame(frame_no);
            temp_SSG.setSampleFrame(getMostCoherentFrame(coherency_scores_ssg,temp_SSG.getStartFrame(),temp_SSG.getEndFrame()));
            //PlaceSSG new_place(temp_SSG.getId(), temp_SSG);

            qDebug() << temp_SSG.member_invariants.size().width;
            //Expermental purpose
            if(temp_SSG.member_invariants.empty() == false)
                SSGs.push_back(temp_SSG);

            //Commented out for experimental purpose
            //recognition->performRecognition(places, new_place, hierarchy_tree);
        }
        else if(detection_result == DETECTION_IN_PLACE)
        {
            temp_SSG.basepoints.push_back(ns_vec.back());
            SSGProc::updateSSG(temp_SSG, ns_vec.back(), seg_track->getM());
            SSGProc::updateSSGInvariants(temp_SSG, img_org);
        }

//        frame_count++;
//        if(frame_count%100 == 0)
//        {
//            qDebug() << "Time elapsed in 100 frames: " << frame_count << QDateTime::currentMSecsSinceEpoch() - last_time;
//            last_time = QDateTime::currentMSecsSinceEpoch();
//        }

        //Free variables
        img.release();
        img_org.release();

        //Wait a little for GUI processing
        waitKey(1);

        //next_frame = false;
    }


    //autoTryParameters();
    //qDebug() << places.size();
    savePlacesFrameInfo(places);

    is_processing = false;
    stop_processing = false;
}

void TSCHybrid::performBOWTrain(const string folder, const int start_idx, const int end_idx, int step)
{
    int dict_size = BOW_DICT_SIZE;
    TermCriteria term_cri(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, BOW_TC_COUNT, BOW_TC_EPSILON);

    Ptr<DescriptorExtractor> desc_extractor;
    #if BOW_DESC_TYPE == SIFT
        desc_extractor = DescriptorExtractor::create("SIFT");
    #elif BOW_DESC_TYPE == SURF
        desc_extractor = DescriptorExtractor::create("SURF");
    #elif BOW_DESC_TYPE == MSER
        desc_extractor = DescriptorExtractor::create("MSER");
    #endif

    BOW_DESC_TYPE feature_detector;

    BOWKMeansTrainer bow_trainer(dict_size);

    qDebug() << "Training started";

    img_files = getFiles(folder);


    for(int i = start_idx; i < end_idx; i = i + step)
    {
        vector<KeyPoint> keypoints;
        Mat descriptors;
        Mat img = imread(folder+img_files[i]);
        resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        //imshow("fd",img);
        //waitKey(0);
        feature_detector.detect(img, keypoints);
        desc_extractor->compute(img, keypoints, descriptors);

        bow_trainer.add(descriptors);
        qDebug() << "Descriptor" << i << "added.";
    }


    qint64 last_time = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "Clustering started";
    Mat dictionary = bow_trainer.cluster();
    qDebug() << "Clustering finished in" << (QDateTime::currentMSecsSinceEpoch() - last_time)/1000 << "sec";


    FileStorage fs(OUTPUT_FOLDER+string(BOW_DICT_NAME), FileStorage::WRITE);

    fs << "Dict" << dictionary;

    qDebug() << "Written to file";

    fs.release();
}

void TSCHybrid::reRecognize()
{
    TreeNode* hierarchy_tree = NULL;
    vector<PlaceSSG> places;

    vector<SSG> SSGs = this->SSGs;

    for(int i = 0; i < SSGs.size(); i++)
    {
        SSGProc::filterSummarySegments(SSGs[i], params->tau_p);
        PlaceSSG new_place(SSGs[i].getId(), SSGs[i]);
        recognition->performRecognition(places, new_place, hierarchy_tree);
    }

}

//For parameter tuning purposes
//After the observation of output plots for different set of parameters
//human user select some plots as successful. This function prints out the
//paramters in a sorted order so that you can see which parameters
//observed much than others. If some of the parameters is always observed than
//you can say you found the optimal parameter.
void TSCHybrid::findBestParameters()
{
    vector<string> files;
//    files.push_back("939");
//    files.push_back("963");
//    files.push_back("987");
//    files.push_back("1095");
//    files.push_back("1119");
//    files.push_back("1143");
//    files.push_back("1407");
//    files.push_back("1827");
//    files.push_back("1831");
//    files.push_back("1839");
//    files.push_back("1851");
//    files.push_back("1863");
//    files.push_back("1867");
//    files.push_back("1875");
//    files.push_back("1882");
//    files.push_back("1899");
//    files.push_back("1911");
//    files.push_back("1935");
//    files.push_back("1947");
//    files.push_back("1959");
//    files.push_back("2139");
//    files.push_back("2247");
//    files.push_back("2439");
//    files.push_back("2463");
//    files.push_back("2475");
//    files.push_back("2571");
//Output for this set
//coeff_node_disappear1: 0.5 0.5 0.5 0.5 0.5 0.5 0.7 0.7 0.7 0.7 0.7 0.7 0.9 0.9 0.9 0.9 0.9 0.9 0.9 0.9 0.9 0.9 0.9 0.9 0.9 0.9
//coeff_node_disappear2: 0.4 0.4 0.4 0.4 0.4 0.4 0.4 0.4 0.4 0.4 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6
//coeff_node_appear: 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3
//coeff_coh_exp_base: 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 5 5 7
//coeff_coh_appear_thres: 0.65 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85 0.85
//tau_f: 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 20 20 20 30 30 30 30
//tau_n: 2 2 2 2 2 2 2 2 2 2 2 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
//tau_w: 10 10 10 10 10 10 10 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15

//    files.push_back("2");
//    files.push_back("6");
//    files.push_back("10");
//    files.push_back("14");
//    files.push_back("18");

//    SSGParams* ssg_params;
//    SegmentTrackParams* seg_track_params;
//    SegmentationParams* seg_params;
//    GraphMatchParams* gm_params;
//    RecognitionParams* rec_params;

    vector<float> coeff_node_disappear1;
    vector<float> coeff_node_disappear2;
    vector<float> coeff_node_appear;
    vector<float> coeff_coh_exp_base;
    vector<float> coeff_coh_appear_thres;
    vector<float> tau_f;
    vector<float> tau_n;
    vector<float> tau_w;

    for(int i = 0; i < files.size(); i++)
    {

        string filename = "/home/isl-mahmut/Output/Per May 19 2016-19:01:53/" + files[i] + ".txt";
        readParameters(filename,
                       params,
                       seg_track_params,
                       seg_params,
                       gm_params,
                       rec_params);


        coeff_node_disappear1.push_back(params->coeff_node_disappear1);
        coeff_node_disappear2.push_back(params->coeff_node_disappear2);
        coeff_node_appear.push_back(params->coeff_node_appear);
        coeff_coh_exp_base.push_back(params->coeff_coh_exp_base);
        coeff_coh_appear_thres.push_back(params->coeff_coh_appear_thres);
        tau_f.push_back(params->tau_f);
        tau_n.push_back(params->tau_n);
        tau_w.push_back(seg_track_params->tau_w);

    }

    sort(coeff_node_disappear1.begin(),coeff_node_disappear1.end());
    sort(coeff_node_disappear2.begin(),coeff_node_disappear2.end());
    sort(coeff_node_appear.begin(),coeff_node_appear.end());
    sort(coeff_coh_exp_base.begin(),coeff_coh_exp_base.end());
    sort(coeff_coh_appear_thres.begin(),coeff_coh_appear_thres.end());
    sort(tau_f.begin(),tau_f.end());
    sort(tau_n.begin(),tau_n.end());
    sort(tau_w.begin(),tau_w.end());


    cout << "coeff_node_disappear1: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << coeff_node_disappear1[i] << " ";
    cout << endl;

    cout << "coeff_node_disappear2: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << coeff_node_disappear2[i] << " ";
    cout << endl;

    cout << "coeff_node_appear: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << coeff_node_appear[i] << " ";
    cout << endl;

    cout << "coeff_coh_exp_base: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << coeff_coh_exp_base[i] << " ";
    cout << endl;

    cout << "coeff_coh_appear_thres: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << coeff_coh_appear_thres[i] << " ";
    cout << endl;

    cout << "tau_f: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << tau_f[i] << " ";
    cout << endl;

    cout << "tau_n: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << tau_n[i] << " ";
    cout << endl;

    cout << "tau_w: ";
    for(int i = 0; i < coeff_node_disappear1.size(); i++)
        cout << tau_w[i] << " ";
    cout << endl;
}

//This function is used to recalculate the coherency score based on already crated M matrix
//And plot places -- Used for reploting places after the parameters are changed for tuning purposes
void TSCHybrid::recalculateCoherencyAndPlot()
{
    for(int i = 0; i < tsc_plot->graphCount(); i++)
        tsc_plot->graph(i)->clearData();
    for(int i = 0; i < ssg_plot->graphCount(); i++)
        ssg_plot->graph(i)->clearData();
    for(int i = 0; i < place_map->graphCount(); i++)
        place_map->graph(i)->clearData();
    for(int i = 0; i < tsc_avg_plot->graphCount(); i++)
        tsc_avg_plot->graph(i)->clearData();


    vector<float> coherency_scores_ssg_one_shot;   //Stores all coherency scores
    vector<int> detected_places_unfiltered_one_shot;
    vector<int> detected_places_one_shot; //Stores all detected place ids

    calcCohScoreOneShot(seg_track,coherency_scores_ssg_one_shot, detected_places_unfiltered_one_shot,detected_places_one_shot);
    plotScoresSSGOneShot(coherency_scores_ssg_one_shot, detected_places_one_shot, coords, 1);
}

//For parameter tuning purpose
//First define the range of parameters and step size.
//After the first run of the dataset, this function tries different
//set of parameters and saves the output plot of detected places
//Only some set of paramters can be changed after the experiment is completed
void TSCHybrid::autoTryParameters()
{
    int try_count = 0;

    float tau_w_s = 22;
    float tau_w_e = 22;
    float tau_w_del = 2;

    float tau_f_s = 10;
    float tau_f_e = 10;
    float tau_f_del = 5;

    float tau_n_s = 3;
    float tau_n_e = 3;
    float tau_n_del = 2;

    float coeff_node_disappear1_s = 0.7;
    float coeff_node_disappear1_e = 0.7;
    float coeff_node_disappear1_del = 0.2;

    float coeff_node_disappear2_s = 0.5;
    float coeff_node_disappear2_e = 0.5;
    float coeff_node_disappear2_del = 0.2;

    float coeff_node_appear_s = 0.2;
    float coeff_node_appear_e = 0.2;
    float coeff_node_appear_del = 0.2;

    float coeff_coh_exp_base_s = 3;
    float coeff_coh_exp_base_e = 3;
    float coeff_coh_exp_base_del = 1;

    float coeff_coh_appear_thres_s = 0.85;
    float coeff_coh_appear_thres_e = 0.85;
    float coeff_coh_appear_thres_del = 0.05;

    vector<float> coherency_scores_ssg_one_shot;   //Stores all coherency scores
    vector<int> detected_places_unfiltered_one_shot;
    vector<int> detected_places_one_shot; //Stores all detected place ids

    for(float tau_w = tau_w_s; tau_w <= tau_w_e; tau_w += tau_w_del)
    {
        for(float tau_f = tau_f_s; tau_f <= tau_f_e; tau_f += tau_f_del)
        {
            for(float tau_n = tau_n_s; tau_n <= tau_n_e; tau_n += tau_n_del)
            {
                for(float coeff_node_disappear1 = coeff_node_disappear1_s; coeff_node_disappear1 <= coeff_node_disappear1_e; coeff_node_disappear1 += coeff_node_disappear1_del)
                {
                    for(float coeff_node_disappear2 = coeff_node_disappear2_s; coeff_node_disappear2 <= coeff_node_disappear2_e; coeff_node_disappear2 += coeff_node_disappear2_del)
                    {
                        for(float coeff_node_appear = coeff_node_appear_s; coeff_node_appear <= coeff_node_appear_e; coeff_node_appear += coeff_node_appear_del)
                        {
                            for(float coeff_coh_exp_base = coeff_coh_exp_base_s; coeff_coh_exp_base <= coeff_coh_exp_base_e; coeff_coh_exp_base += coeff_coh_exp_base_del)
                            {
                                for(float coeff_coh_appear_thres = coeff_coh_appear_thres_s; coeff_coh_appear_thres <= coeff_coh_appear_thres_e; coeff_coh_appear_thres += coeff_coh_appear_thres_del)
                                {

                                    params->coeff_node_disappear1 = coeff_node_disappear1;
                                    params->coeff_node_disappear2 = coeff_node_disappear2;
                                    params->coeff_node_appear = coeff_node_appear;
                                    params->coeff_coh_exp_base = coeff_coh_exp_base;
                                    params->coeff_coh_appear_thres = coeff_coh_appear_thres;
                                    params->tau_f = tau_f;
                                    params->tau_n = tau_n;
                                    seg_track_params->tau_w = tau_w;

                                    for(int i = 0; i < tsc_plot->graphCount(); i++)
                                        tsc_plot->graph(i)->clearData();
                                    for(int i = 0; i < ssg_plot->graphCount(); i++)
                                        ssg_plot->graph(i)->clearData();
                                    for(int i = 0; i < place_map->graphCount(); i++)
                                        place_map->graph(i)->clearData();
                                    for(int i = 0; i < tsc_avg_plot->graphCount(); i++)
                                        tsc_avg_plot->graph(i)->clearData();



                                    calcCohScoreOneShot(seg_track,coherency_scores_ssg_one_shot, detected_places_unfiltered_one_shot,detected_places_one_shot);
                                    plotScoresSSGOneShot(coherency_scores_ssg_one_shot, detected_places_one_shot, coords, 1);

                                    stringstream ss, ss2;
                                    ss << getOutputFolder() << try_count << ".png";
                                    this->ssg_plot->savePng(QString(ss.str().c_str()));


                                    ss2 << getOutputFolder() << try_count << ".txt";

                                    saveParameters(ss2.str(),
                                                   params,
                                                   seg_track_params,
                                                   seg_params,
                                                   gm_params,
                                                   rec_params);

                                    qDebug() << ss2.str().c_str();
                                    try_count++;
                                    coherency_scores_ssg_one_shot.clear();
                                    detected_places_unfiltered_one_shot.clear();
                                    detected_places_one_shot.clear();

                                    waitKey(1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }





}

//For parameter tuning purposes
void TSCHybrid::calcCohScoreOneShot(SegmentTrack* seg_track, vector<float>& coh_scores,
                                    vector<int>& detected_places_unfiltered,
                                    vector<int>& detected_places)
{
    Mat M_ = seg_track->getM();
    vector<pair<NodeSig, int> > M_ns = seg_track->getM_ns();
    float tau_w = seg_track_params->tau_w;

    for(int f = 1; f < M_.size().width; f++)
    {
        Mat M = M_(Rect(0,0,f,M_.size().height-1));

        //Wait until at least tau_w frames
        if(M.size().width < tau_w)
            continue;
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
        detectPlace(coh_scores,detected_places_unfiltered,detected_places);
    }
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
void TSCHybrid::plotScoresSSGOneShot(vector<float> coherency_scores, vector<int> detected_places, vector<cv::Point2f> coords, bool reset)
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
            this->place_map->addGraph();
            graph_idx++;

            if(detected_places[i] > 0)
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
        ssg_plot->graph(graph_idx)->addData(i,1.0+margin);
        place_map->graph(graph_idx)->addData(coords[i].x, coords[i].y);

        //Add coh score
        ssg_plot->graph(PLOT_SCORES_IDX)->addData(i,coherency_scores[i]+margin);

        //Add thresold line
        ssg_plot->graph(PLOT_THRES_IDX)->addData(i,params->tau_c+margin);
    }

    ssg_plot->rescaleAxes();
    ssg_plot->replot();
    //place_map->rescaleAxes();
    place_map->replot();
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
    place_map->replot();
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
        return DETECTION_NOT_STARTED;
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
