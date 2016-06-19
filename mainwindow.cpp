#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "segmentation.h"
#include "graphmatch.h"
#include "placedetection.h"
#include "segmenttrack.h"
#include "TSC/tsc.h"
#include "defs.h"
#include "recognition.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    auto_tune = false;
    ui->setupUi(this);

    tsc_hybrid = NULL;

    string datasets_filename = string(OUTPUT_FOLDER) + string("/datasets.txt");

    //Read dataset info
    readDatasets(datasets_filename, datasets);

    //Read parameters for each dataset
    for(int i = 0; i < datasets.size(); i++)
    {
        stringstream ss;
        ss << OUTPUT_FOLDER << "parameters-" << datasets[i].dataset_id << ".txt";
        Parameters* params = new Parameters();
        readParameters(ss.str(), params);
        params_all.push_back(params);
    }

    //Assign first parameter set as default
    params = params_all[0];

    //Set parameter scrollbars on GUI
    ui->scroll_sigma->setValue(params->seg_params.sigma*100);
    ui->scroll_k->setValue(params->seg_params.k);
    ui->scroll_min_size->setValue(params->seg_params.min_size);

    ui->scroll_tau_n->setValue(params->ssg_params.tau_n);
    ui->scroll_tau_c->setValue(params->ssg_params.tau_c*10);
    ui->scroll_tau_f->setValue(params->ssg_params.tau_f);
    ui->scroll_tau_p->setValue(params->ssg_params.tau_p*100);
    ui->scroll_disapp_coeff1->setValue(params->ssg_params.coeff_node_disappear1*10);
    ui->scroll_disapp_coeff2->setValue(params->ssg_params.coeff_node_disappear2*10);
    ui->scroll_appear_coeff->setValue(params->ssg_params.coeff_node_appear*10);
    ui->scroll_coh_base->setValue(params->ssg_params.coeff_coh_exp_base);
    ui->scroll_appear_thres->setValue(params->ssg_params.coeff_coh_appear_thres*100);


    ui->scroll_tau_w->setValue(params->seg_track_params.tau_w);
    ui->scroll_tau_m->setValue(params->seg_track_params.tau_m*1000);

    ui->scroll_color_coeff->setValue(params->gm_params.color_weight*10);
    ui->scroll_pos_coeff->setValue(params->gm_params.pos_weight*10);
    ui->scroll_area_coeff->setValue(params->gm_params.area_weight*10);
    ui->scroll_bow_coeff->setValue(params->gm_params.bow_weight*10);

    ui->scroll_tau_r->setValue(params->rec_params.tau_r*100);
    ui->scroll_tau_v->setValue(params->rec_params.tau_v*1000);

    ui->tb_tree_plot_h->setText(QString::number(params->rec_params.plot_h));
    ui->tb_tree_plot_w->setText(QString::number(params->rec_params.plot_w));
    ui->tb_tree_ssg_h->setText(QString::number(params->rec_params.ssg_h));
    ui->tb_tree_ssg_w->setText(QString::number(params->rec_params.ssg_w));

    ui->cb_framebyframe->setChecked(false);

    //Fill recognition method combobox
    ui->combo_rec_method->addItem("Bubble",REC_TYPE_BD_NORMAL);
    ui->combo_rec_method->addItem("Bubble-Voting",REC_TYPE_BD_VOTING);
    ui->combo_rec_method->addItem("Bubble-Color",REC_TYPE_BD_COLOR);
    ui->combo_rec_method->addItem("Bubble-Color-wlog",REC_TYPE_BD_COLOR_LOG);
    ui->combo_rec_method->addItem("SSG",REC_TYPE_SSG_NORMAL);
    ui->combo_rec_method->addItem("SSG-Voting",REC_TYPE_SSG_VOTING);
    ui->combo_rec_method->addItem("Hybrid",REC_TYPE_HYBRID);

    //Fill BD recognition norm type
    ui->combo_norm_type->addItem("BD NORM_L2",4);
    ui->combo_norm_type->addItem("BD NORM_L2SQR",5);
    ui->combo_norm_type->addItem("BD NORM_L1",2);
    ui->combo_norm_type->addItem("BD NORM_INF",1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImgOrg(QImage img)
{
    ui->lbl_img_org->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showMatchImage(QImage img)
{
    ui->lbl_match_results->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showMap(QImage img)
{
    ui->lbl_map->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showSSG(QImage img)
{
    ui->lbl_ssg->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_scroll_sigma_valueChanged(int value)
{
    ui->tb_sigma->setText(QString::number(value/100.0));

    params->seg_params.sigma = value/100.0;
}

void MainWindow::on_scroll_k_valueChanged(int value)
{
    ui->tb_k->setText(QString::number(value));

    params->seg_params.k = value;
}

void MainWindow::on_scroll_min_size_valueChanged(int value)
{
    ui->tb_min_size->setText(QString::number(value));

    params->seg_params.min_size = value;
}

void MainWindow::on_scroll_tau_n_valueChanged(int value)
{
    ui->tb_tau_n->setText(QString::number(value));
    params->ssg_params.tau_n = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_c_valueChanged(int value)
{
    ui->tb_tau_c->setText(QString::number(value/10.0));
    params->ssg_params.tau_c = value/10.0;
}

void MainWindow::on_scroll_tau_w_valueChanged(int value)
{
    ui->tb_tau_w->setText(QString::number(value));
    params->seg_track_params.tau_w = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_f_valueChanged(int value)
{
    ui->tb_tau_f->setText(QString::number(value));
    params->ssg_params.tau_f = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_m_valueChanged(int value)
{
    ui->tb_tau_m->setText(QString::number(value/1000.0));
    params->seg_track_params.tau_m = value/1000.0;
}

void MainWindow::on_scroll_tau_p_sliderReleased()
{
    on_scroll_tau_p_valueChanged(ui->scroll_tau_p->value());

    int method = ui->combo_rec_method->itemData(ui->combo_rec_method->currentIndex()).toInt();
    int norm_type = ui->combo_norm_type->itemData(ui->combo_norm_type->currentIndex()).toInt();
    if(ui->cb_auto_tune->isChecked())
    {
        if(ui->cb_rec_active->isChecked())
        {
            tsc_hybrid->reRecognize(method, norm_type);
        }
        else
        {
            tsc_hybrid->reRecognize2(method, norm_type);
        }
    }
}

void MainWindow::on_scroll_tau_r_sliderReleased()
{
    on_scroll_tau_r_valueChanged(ui->scroll_tau_r->value());

    int method = ui->combo_rec_method->itemData(ui->combo_rec_method->currentIndex()).toInt();
    int norm_type = ui->combo_norm_type->itemData(ui->combo_norm_type->currentIndex()).toInt();
    if(ui->cb_auto_tune->isChecked())
    {
        if(ui->cb_rec_active->isChecked())
        {
            tsc_hybrid->reRecognize(method, norm_type);
        }
        else
        {
            tsc_hybrid->reRecognize2(method, norm_type);
        }
    }
}

void MainWindow::on_scroll_tau_v_sliderReleased()
{
    on_scroll_tau_v_valueChanged(ui->scroll_tau_v->value());

    int method = ui->combo_rec_method->itemData(ui->combo_rec_method->currentIndex()).toInt();
    int norm_type = ui->combo_norm_type->itemData(ui->combo_norm_type->currentIndex()).toInt();
    if(ui->cb_auto_tune->isChecked())
    {
        if(ui->cb_rec_active->isChecked())
        {
            tsc_hybrid->reRecognize(method, norm_type);
        }
        else
        {
            tsc_hybrid->reRecognize2(method, norm_type);
        }
    }
}

void MainWindow::on_scroll_tau_p_valueChanged(int value)
{
    ui->tb_tau_p->setText(QString::number(value/100.0));
    params->ssg_params.tau_p = value/100.0;
}

void MainWindow::on_scroll_color_coeff_valueChanged(int value)
{
    ui->tb_color->setText(QString::number(value/10.0));
    params->gm_params.color_weight = value/10.0;
}

void MainWindow::on_scroll_pos_coeff_valueChanged(int value)
{
    ui->tb_pos->setText(QString::number(value/10.0));
    params->gm_params.pos_weight = value/10.0;
}

void MainWindow::on_scroll_area_coeff_valueChanged(int value)
{
    ui->tb_area->setText(QString::number(value/10.0));
    params->gm_params.area_weight = value/10.0;
}

void MainWindow::on_scroll_bow_coeff_valueChanged(int value)
{
    ui->tb_bow->setText(QString::number(value/10.0));
    params->gm_params.bow_weight = value/10.0;
}

void MainWindow::on_scroll_disapp_coeff1_valueChanged(int value)
{
    ui->tb_disapp1->setText(QString::number(value/10.0));
    params->ssg_params.coeff_node_disappear1 = value/10.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_disapp_coeff2_valueChanged(int value)
{
    ui->tb_disapp2->setText(QString::number(value/10.0));
    params->ssg_params.coeff_node_disappear2 = value/10.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_appear_coeff_valueChanged(int value)
{
    ui->tb_appear->setText(QString::number(value/10.0));
    params->ssg_params.coeff_node_appear = value/10.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_coh_base_valueChanged(int value)
{
    ui->tb_coh_exp_base->setText(QString::number(value));
    params->ssg_params.coeff_coh_exp_base = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_appear_thres_valueChanged(int value)
{
    ui->tb_coh_app_thres->setText(QString::number(value/100.0));
    params->ssg_params.coeff_coh_appear_thres = value/100.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_r_valueChanged(int value)
{
    ui->tb_tau_r->setText(QString::number(value/100.0));
    params->rec_params.tau_r = value/100.0;
}

void MainWindow::on_scroll_tau_v_valueChanged(int value)
{
    ui->tb_tau_v->setText(QString::number(value/1000.0));
    params->rec_params.tau_v = value/1000.0;
}

void MainWindow::on_cb_framebyframe_clicked()
{
}

void MainWindow::on_btn_tsc_process_clicked()
{
}

void MainWindow::showTrackMap(QImage img)
{
    ui->lbl_tracking_map->setAlignment(Qt::AlignTop);
    ui->lbl_tracking_map->setAlignment(Qt::AlignLeft);
    ui->lbl_tracking_map->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showTrackSegment(QImage img)
{
    ui->lbl_tracking_preview->setAlignment(Qt::AlignTop);
    ui->lbl_tracking_preview->setAlignment(Qt::AlignLeft);
    ui->lbl_tracking_preview->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_btn_process_all_clicked()
{
}

void MainWindow::on_btn_next_clicked()
{
}

void MainWindow::on_btn_tracking_start_clicked()
{
    if(seg_track == NULL)
        ui->te_tracking->insertPlainText(QString("Run place detection first!\n"));
    else
    {
        seg_track->mapScaleFactor = 5;

        QObject::connect(seg_track, SIGNAL(showTrackMap(QImage)),
                         this, SLOT(showTrackMap(QImage)));
        QObject::connect(seg_track, SIGNAL(showTrackSegment(QImage)),
                         this, SLOT(showTrackSegment(QImage)));
        ui->lbl_tracking_map->installEventFilter(seg_track);

        seg_track->drawMap();
    }
}

void MainWindow::on_lbl_find_coherent_clicked()
{
    seg_track->processImagesOnline();
}

void MainWindow::on_btn_stop_process_clicked()
{
}

void MainWindow::on_btn_merged_process_images_clicked()
{
    tsc_hybrid = new TSCHybrid(ui->custom_plot_merged_tsc, ui->custom_plot_merged_ssg, ui->place_map, ui->custom_plot_tsc_average,
                               params,
                               &datasets[0]);
    QObject::connect(tsc_hybrid, SIGNAL(showImgOrg(QImage)),
                     this, SLOT(showImgOrg(QImage)));
    QObject::connect(tsc_hybrid, SIGNAL(showSSG(QImage)),
                     this, SLOT(showSSG(QImage)));
    QObject::connect(tsc_hybrid, SIGNAL(showMap(QImage)),
                     this, SLOT(showMap(QImage)));
    QObject::connect(tsc_hybrid->seg_track, SIGNAL(showImgOrg(QImage)),
                     this, SLOT(showImgOrg(QImage)));
    QObject::connect(tsc_hybrid->seg_track->gm, SIGNAL(showMatchImage(QImage)),
                     this, SLOT(showMatchImage(QImage)));

    ui->lbl_map->installEventFilter(tsc_hybrid);
    tsc_hybrid->processImages(datasets[0].location, datasets[0].start_idx, datasets[0].end_idx);
}

void MainWindow::on_pushButton_clicked()
{
}

void MainWindow::showTree(QImage img)
{
    ui->lbl_tree_draw->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::printMessage(QString str)
{
    ui->te_recognition->insertPlainText(str+"\n");
}

void MainWindow::on_btn_test_next_clicked()
{
    recognition->next = true;
}



void MainWindow::on_btn_process_hierarchical_clicked()
{
    if(tsc_hybrid == NULL)
    {
        tsc_hybrid = new TSCHybrid(ui->custom_plot_merged_tsc, ui->custom_plot_merged_ssg, ui->place_map, ui->custom_plot_tsc_average,
                                   params,
                                   &datasets[0]);

        QObject::connect(tsc_hybrid, SIGNAL(showImgOrg(QImage)),
                         this, SLOT(showImgOrg(QImage)));
        QObject::connect(tsc_hybrid, SIGNAL(showSSG(QImage)),
                         this, SLOT(showSSG(QImage)));
        QObject::connect(tsc_hybrid, SIGNAL(showMap(QImage)),
                         this, SLOT(showMap(QImage)));
        QObject::connect(tsc_hybrid->seg_track, SIGNAL(showImgOrg(QImage)),
                         this, SLOT(showImgOrg(QImage)));
        QObject::connect(tsc_hybrid->seg_track->gm, SIGNAL(showMatchImage(QImage)),
                         this, SLOT(showMatchImage(QImage)));
        QObject::connect(tsc_hybrid->recognition, SIGNAL(showTree(QImage)),
                         this, SLOT(showTree(QImage)));
        QObject::connect(tsc_hybrid->recognition, SIGNAL(printMessage(QString)),
                         this, SLOT(printMessage(QString)));

        ui->lbl_map->installEventFilter(tsc_hybrid);
    }
    else
    {
        if(tsc_hybrid->is_processing)
        {
            qDebug() << "Stop process first!";
            return;
        }
    }

    for(int i = 0; i < datasets.size(); i++)
    {
        *params = *params_all[i];

        //Save parameters
        stringstream ss;
        ss << getOutputFolder(false) << "parameters-" << datasets[i].dataset_id << ".txt";
        saveParameters(ss.str(),params);

        //Bubble process init
        bubbleProcess::calculateImagePanAngles(FOCAL_LENGHT_PIXELS, params->ssg_params.img_org_width, params->ssg_params.img_org_height);
        bubbleProcess::calculateImageTiltAngles(FOCAL_LENGHT_PIXELS, params->ssg_params.img_org_width, params->ssg_params.img_org_height);

        tsc_hybrid->processImagesHierarchical_(datasets[i].location, datasets[i].start_idx, datasets[i].end_idx, datasets[i].dataset_id);
        //tsc_hybrid->processImagesHierarchical2(datasets[i].location, datasets[i].start_idx, datasets[i].end_idx, datasets[i].dataset_id);
        //tsc_hybrid->createDatabase(datasets[i].location, datasets[i].start_idx, datasets[i].end_idx, datasets[i].dataset_id);
        //tsc_hybrid->processImagesHierarchicalFromDB(datasets[i].location, datasets[i].start_idx, datasets[i].end_idx, datasets[i].dataset_id);
    }
}



void MainWindow::on_tb_tree_plot_h_editingFinished()
{
    params->rec_params.plot_h = ui->tb_tree_plot_h->text().toInt();
}

void MainWindow::on_tb_tree_plot_w_editingFinished()
{
    params->rec_params.plot_w = ui->tb_tree_plot_w->text().toInt();
}

void MainWindow::on_tb_tree_ssg_h_editingFinished()
{
    params->rec_params.ssg_h = ui->tb_tree_ssg_h->text().toInt();
}

void MainWindow::on_tb_tree_ssg_w_editingFinished()
{
    params->rec_params.ssg_w = ui->tb_tree_ssg_w->text().toInt();
}


void MainWindow::on_btn_save_settings_clicked()
{
    string filename = string(OUTPUT_FOLDER) + string("/parameters.txt");

    saveParameters(filename,params);
}

void MainWindow::on_btn_hierarchical_stop_clicked()
{
    if(tsc_hybrid != NULL)
    {
        tsc_hybrid->stopProcessing();
    }
}

void MainWindow::on_cb_auto_tune_clicked()
{
    if(ui->cb_auto_tune->isChecked() == true)
        this->auto_tune = true;
    else
        this->auto_tune = false;
}

void MainWindow::on_btn_bow_train_clicked()
{
    tsc_hybrid = new TSCHybrid(ui->custom_plot_merged_tsc, ui->custom_plot_merged_ssg, ui->place_map, ui->custom_plot_tsc_average,
                               params,
                               &datasets[0]);

    tsc_hybrid->performBOWTrain(datasets[0].location, datasets[0].start_idx, datasets[0].end_idx, 5);
}



void MainWindow::on_btn_next_frame_clicked()
{
    tsc_hybrid->next_frame = true;
}

void MainWindow::on_btn_init_from_db_clicked()
{
    tsc_hybrid = new TSCHybrid(ui->custom_plot_merged_tsc, ui->custom_plot_merged_ssg, ui->place_map, ui->custom_plot_tsc_average,
                               params,
                               &datasets[0]);

    QObject::connect(tsc_hybrid, SIGNAL(showImgOrg(QImage)),
                     this, SLOT(showImgOrg(QImage)));
    QObject::connect(tsc_hybrid, SIGNAL(showSSG(QImage)),
                     this, SLOT(showSSG(QImage)));
    QObject::connect(tsc_hybrid, SIGNAL(showMap(QImage)),
                     this, SLOT(showMap(QImage)));
    QObject::connect(tsc_hybrid->seg_track, SIGNAL(showImgOrg(QImage)),
                     this, SLOT(showImgOrg(QImage)));
    QObject::connect(tsc_hybrid->seg_track->gm, SIGNAL(showMatchImage(QImage)),
                     this, SLOT(showMatchImage(QImage)));
    QObject::connect(tsc_hybrid->recognition, SIGNAL(showTree(QImage)),
                     this, SLOT(showTree(QImage)));
    QObject::connect(tsc_hybrid->recognition, SIGNAL(printMessage(QString)),
                     this, SLOT(printMessage(QString)));

    ui->lbl_map->installEventFilter(tsc_hybrid);

    tsc_hybrid->readFromDB();
}

void MainWindow::on_btn_create_db_clicked()
{
    if(tsc_hybrid == NULL)
    {
        tsc_hybrid = new TSCHybrid(ui->custom_plot_merged_tsc, ui->custom_plot_merged_ssg, ui->place_map, ui->custom_plot_tsc_average,
                                   params,
                                   &datasets[0]);

        QObject::connect(tsc_hybrid, SIGNAL(showImgOrg(QImage)),
                         this, SLOT(showImgOrg(QImage)));
        QObject::connect(tsc_hybrid, SIGNAL(showSSG(QImage)),
                         this, SLOT(showSSG(QImage)));
        QObject::connect(tsc_hybrid, SIGNAL(showMap(QImage)),
                         this, SLOT(showMap(QImage)));
        QObject::connect(tsc_hybrid->seg_track, SIGNAL(showImgOrg(QImage)),
                         this, SLOT(showImgOrg(QImage)));
        QObject::connect(tsc_hybrid->seg_track->gm, SIGNAL(showMatchImage(QImage)),
                         this, SLOT(showMatchImage(QImage)));
        QObject::connect(tsc_hybrid->recognition, SIGNAL(showTree(QImage)),
                         this, SLOT(showTree(QImage)));
        QObject::connect(tsc_hybrid->recognition, SIGNAL(printMessage(QString)),
                         this, SLOT(printMessage(QString)));

        ui->lbl_map->installEventFilter(tsc_hybrid);
    }
    else
    {
        if(tsc_hybrid->is_processing)
        {
            qDebug() << "Stop process first!";
            return;
        }
    }

    for(int i = 0; i < datasets.size(); i++)
    {
        *params = *params_all[i];

        //Save parameters
        stringstream ss;
        ss << getOutputFolder(false) << "parameters-" << datasets[i].dataset_id << ".txt";
        saveParameters(ss.str(),params);

        //Bubble process init
        bubbleProcess::calculateImagePanAngles(FOCAL_LENGHT_PIXELS, params->ssg_params.img_org_width, params->ssg_params.img_org_height);
        bubbleProcess::calculateImageTiltAngles(FOCAL_LENGHT_PIXELS, params->ssg_params.img_org_width, params->ssg_params.img_org_height);

        tsc_hybrid->createDatabase(datasets[i].location, datasets[i].start_idx, datasets[i].end_idx, datasets[i].dataset_id);
    }
}
