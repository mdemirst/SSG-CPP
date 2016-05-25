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

    ssg_params = new SSGParams(INIT_TAU_N,
                               INIT_TAU_C,
                               INIT_TAU_F,
                               INIT_TAU_D,
                               INIT_TAU_P,
                               COEFF_NODE_DISAPPEAR_1,
                               COEFF_NODE_DISAPPEAR_2,
                               COEFF_NODE_APPEAR,
                               COEFF_COH_EXP_BASE,
                               COEFF_COH_APPEAR_THRES);

    seg_track_params = new SegmentTrackParams (INIT_TAU_W,
                                               INIT_TAU_M);

    seg_params = new SegmentationParams (SEG_SIGMA,
                                         SEG_K,
                                         SEG_MIN_SIZE);

    gm_params = new GraphMatchParams (POS_WEIGHT,
                                      COLOR_WEIGHT,
                                      AREA_WEIGHT);

    rec_params = new RecognitionParams (INIT_TAU_R,
                                        INIT_TREE_PLOT_H,
                                        INIT_TREE_PLOT_W,
                                        INIT_TREE_SSG_H,
                                        INIT_TREE_SSG_W,
                                        INIT_TAU_V);

    string par_filename = string(OUTPUT_FOLDER) + string("/parameters.txt");

    readParameters(par_filename,ssg_params,seg_track_params,seg_params,gm_params,rec_params);



    ui->scroll_sigma->setValue(seg_params->sigma*100);
    ui->scroll_k->setValue(seg_params->k);
    ui->scroll_min_size->setValue(seg_params->min_size);

    ui->scroll_tau_n->setValue(ssg_params->tau_n);
    ui->scroll_tau_c->setValue(ssg_params->tau_c*10);
    ui->scroll_tau_f->setValue(ssg_params->tau_f);
    ui->scroll_tau_p->setValue(ssg_params->tau_p*100);
    ui->scroll_disapp_coeff1->setValue(ssg_params->coeff_node_disappear1*10);
    ui->scroll_disapp_coeff2->setValue(ssg_params->coeff_node_disappear2*10);
    ui->scroll_appear_coeff->setValue(ssg_params->coeff_node_appear*10);
    ui->scroll_coh_base->setValue(ssg_params->coeff_coh_exp_base);
    ui->scroll_appear_thres->setValue(ssg_params->coeff_coh_appear_thres*100);


    ui->scroll_tau_w->setValue(seg_track_params->tau_w);
    ui->scroll_tau_m->setValue(seg_track_params->tau_m*1000);

    ui->scroll_color_coeff->setValue(gm_params->color_weight*10);
    ui->scroll_pos_coeff->setValue(gm_params->pos_weight*10);
    ui->scroll_area_coeff->setValue(gm_params->area_weight*10);

    ui->scroll_tau_r->setValue(rec_params->tau_r*100);
    ui->scroll_tau_v->setValue(rec_params->tau_v*100);

    ui->tb_tree_plot_h->setText(QString::number(rec_params->plot_h));
    ui->tb_tree_plot_w->setText(QString::number(rec_params->plot_w));
    ui->tb_tree_ssg_h->setText(QString::number(rec_params->ssg_h));
    ui->tb_tree_ssg_w->setText(QString::number(rec_params->ssg_w));

    ui->cb_framebyframe->setChecked(false);

    string filename = string(OUTPUT_FOLDER) + string("/parameters.txt");
    saveParameters(filename,ssg_params,seg_track_params,seg_params,gm_params,rec_params);

//    QObject::connect(gm, SIGNAL(showMatchImage(QImage)),
//                     this, SLOT(showMatchImage(QImage)));
//    QObject::connect(seg_track, SIGNAL(showImg1(QImage)),
//                     this, SLOT(showImg1(QImage)));
//    QObject::connect(seg_track, SIGNAL(showImg2(QImage)),
//                     this, SLOT(showImg2(QImage)));
//    QObject::connect(seg_track, SIGNAL(showMap(QImage)),
//                     this, SLOT(showMap(QImage)));
//    QObject::connect(pd, SIGNAL(showSSG(QImage)),
//                     this, SLOT(showSSG(QImage)));
//    QObject::connect(pd, SIGNAL(showImg1(QImage)),
//                     this, SLOT(showImg1(QImage)));
//    QObject::connect(pd, SIGNAL(showImg2(QImage)),
//                     this, SLOT(showImg2(QImage)));

//    ui->lbl_map->installEventFilter(pd);
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

    seg_params->sigma = value/100.0;
}

void MainWindow::on_scroll_k_valueChanged(int value)
{
    ui->tb_k->setText(QString::number(value));

    seg_params->k = value;
}

void MainWindow::on_scroll_min_size_valueChanged(int value)
{
    ui->tb_min_size->setText(QString::number(value));

    seg_params->min_size = value;
}

void MainWindow::on_scroll_tau_n_valueChanged(int value)
{
    ui->tb_tau_n->setText(QString::number(value));
    ssg_params->tau_n = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_c_valueChanged(int value)
{
    ui->tb_tau_c->setText(QString::number(value/10.0));
    ssg_params->tau_c = value/10.0;
}

void MainWindow::on_scroll_tau_w_valueChanged(int value)
{
    ui->tb_tau_w->setText(QString::number(value));
    seg_track_params->tau_w = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_f_valueChanged(int value)
{
    ui->tb_tau_f->setText(QString::number(value));
    ssg_params->tau_f = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_m_valueChanged(int value)
{
    ui->tb_tau_m->setText(QString::number(value/1000.0));
    seg_track_params->tau_m = value/1000.0;
}

void MainWindow::on_scroll_tau_p_sliderReleased()
{
    on_scroll_tau_p_valueChanged(ui->scroll_tau_p->value());

    if(auto_tune)
        tsc_hybrid->reRecognize();
}

void MainWindow::on_scroll_tau_v_sliderReleased()
{
    on_scroll_tau_v_valueChanged(ui->scroll_tau_v->value());

    if(auto_tune)
        tsc_hybrid->reRecognize();
}

void MainWindow::on_scroll_tau_p_valueChanged(int value)
{
    ui->tb_tau_p->setText(QString::number(value/100.0));
    ssg_params->tau_p = value/100.0;
}

void MainWindow::on_scroll_color_coeff_valueChanged(int value)
{
    ui->tb_color->setText(QString::number(value/10.0));
    gm_params->color_weight = value/10.0;
}

void MainWindow::on_scroll_pos_coeff_valueChanged(int value)
{
    ui->tb_pos->setText(QString::number(value/10.0));
    gm_params->pos_weight = value/10.0;
}

void MainWindow::on_scroll_area_coeff_valueChanged(int value)
{
    ui->tb_area->setText(QString::number(value/10.0));
    gm_params->area_weight = value/10.0;
}

void MainWindow::on_scroll_disapp_coeff1_valueChanged(int value)
{
    ui->tb_disapp1->setText(QString::number(value/10.0));
    ssg_params->coeff_node_disappear1 = value/10.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_disapp_coeff2_valueChanged(int value)
{
    ui->tb_disapp2->setText(QString::number(value/10.0));
    ssg_params->coeff_node_disappear2 = value/10.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_appear_coeff_valueChanged(int value)
{
    ui->tb_appear->setText(QString::number(value/10.0));
    ssg_params->coeff_node_appear = value/10.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_coh_base_valueChanged(int value)
{
    ui->tb_coh_exp_base->setText(QString::number(value));
    ssg_params->coeff_coh_exp_base = value;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_appear_thres_valueChanged(int value)
{
    ui->tb_coh_app_thres->setText(QString::number(value/100.0));
    ssg_params->coeff_coh_appear_thres = value/100.0;

    if(auto_tune)
        tsc_hybrid->recalculateCoherencyAndPlot();
}

void MainWindow::on_scroll_tau_r_sliderReleased()
{
    on_scroll_tau_r_valueChanged(ui->scroll_tau_r->value());

    if(auto_tune)
        tsc_hybrid->reRecognize();
}

void MainWindow::on_scroll_tau_r_valueChanged(int value)
{
    ui->tb_tau_r->setText(QString::number(value/100.0));
    rec_params->tau_r = value/100.0;
}

void MainWindow::on_scroll_tau_v_valueChanged(int value)
{
    ui->tb_tau_v->setText(QString::number(value/100.0));
    rec_params->tau_v = value/100.0;
}

void MainWindow::on_cb_framebyframe_clicked()
{
    pd->frameByFrameProcess = ui->cb_framebyframe->isChecked();
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
//    pd = new PlaceDetection(ui->coherency_plot, ui->ssg_map,
//                            ssg_params,
//                            seg_track_params,
//                            seg_params,
//                            gm_params);

//    if(pd->isProcessing == false)
//        pd->processImages();
}

void MainWindow::on_btn_next_clicked()
{
    pd->process_next_frame = true;
}

void MainWindow::on_btn_tracking_start_clicked()
{
    if(seg_track == NULL)
        ui->te_tracking->insertPlainText(QString("Run place detection first!\n"));
    else if(pd->isProcessing == true)
        ui->te_tracking->insertPlainText(QString("Wait place detection to finish!\n"));
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
    pd->stopProcessing = true;
}

void MainWindow::on_btn_merged_process_images_clicked()
{
    tsc_hybrid = new TSCHybrid(ui->custom_plot_merged_tsc, ui->custom_plot_merged_ssg, ui->place_map, ui->custom_plot_tsc_average,
                               ssg_params,
                               seg_track_params,
                               seg_params,
                               gm_params,
                               rec_params);
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
    tsc_hybrid->processImages(DATASET_FOLDER, START_IDX, END_IDX);
}

void MainWindow::on_pushButton_clicked()
{
    recognition = new Recognition (rec_params,
                                   ssg_params,
                                   seg_track_params,
                                   seg_params,
                                   gm_params);
    QObject::connect(recognition, SIGNAL(showTree(QImage)),
                     this, SLOT(showTree(QImage)));
    QObject::connect(recognition, SIGNAL(printMessage(QString)),
                     this, SLOT(printMessage(QString)));

    recognition->testRecognition();
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
                                   ssg_params,
                                   seg_track_params,
                                   seg_params,
                                   gm_params,
                                   rec_params);

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

    saveParameters(getOutputFolder(true)+"/parameters.txt",ssg_params,seg_track_params,seg_params,gm_params,rec_params);

    tsc_hybrid->processImagesHierarchical(DATASET_FOLDER, START_IDX, END_IDX);
}



void MainWindow::on_tb_tree_plot_h_editingFinished()
{
    rec_params->plot_h = ui->tb_tree_plot_h->text().toInt();
}

void MainWindow::on_tb_tree_plot_w_editingFinished()
{
    rec_params->plot_w = ui->tb_tree_plot_w->text().toInt();
}

void MainWindow::on_tb_tree_ssg_h_editingFinished()
{
    rec_params->ssg_h = ui->tb_tree_ssg_h->text().toInt();
}

void MainWindow::on_tb_tree_ssg_w_editingFinished()
{
    rec_params->ssg_w = ui->tb_tree_ssg_w->text().toInt();
}


void MainWindow::on_btn_save_settings_clicked()
{
    string filename = string(OUTPUT_FOLDER) + string("/parameters.txt");

    saveParameters(filename,ssg_params,seg_track_params,seg_params,gm_params,rec_params);
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





