#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "segmentation.h"
#include "graphmatch.h"
#include "placedetection.h"
#include "segmenttrack.h"
#include "TSC/tsc.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pd = new PlaceDetection(ui->coherency_plot, ui->ssg_map);
    seg_track = pd->seg_track;
    gm = pd->seg_track->gm;
    seg = pd->seg_track->seg;

    ui->scroll_sigma->setValue(seg->getParSigma()*100);
    ui->scroll_k->setValue(seg->getParK());
    ui->scroll_min_size->setValue(seg->getParMinSize());

    ui->scroll_tau_n->setValue(pd->tau_n);
    ui->scroll_tau_c->setValue(pd->tau_c*10);
    ui->scroll_tau_f->setValue(pd->tau_f);
    ui->scroll_disapp_coeff1->setValue(pd->coeff_node_disappear1*10);
    ui->scroll_disapp_coeff2->setValue(pd->coeff_node_disappear2*10);
    ui->scroll_appear_coeff->setValue(pd->coeff_node_appear*10);
    ui->scroll_coh_base->setValue(pd->coeff_coh_exp_base);
    ui->scroll_appear_thres->setValue(pd->coeff_coh_appear_thres*100);

    ui->scroll_tau_w->setValue(seg_track->tau_w);
    ui->scroll_tau_m->setValue(seg_track->tau_m*1000);

    ui->scroll_color_coeff->setValue(gm->color_weight*10);
    ui->scroll_pos_coeff->setValue(gm->pos_weight*10);
    ui->scroll_area_coeff->setValue(gm->area_weight*10);


    ui->cb_framebyframe->setChecked(false);
    pd->frameByFrameProcess = false;

    QObject::connect(gm, SIGNAL(showMatchImage(QImage)),
                     this, SLOT(showMatchImage(QImage)));
    QObject::connect(seg_track, SIGNAL(showImg1(QImage)),
                     this, SLOT(showImg1(QImage)));
    QObject::connect(seg_track, SIGNAL(showImg2(QImage)),
                     this, SLOT(showImg2(QImage)));
    QObject::connect(seg_track, SIGNAL(showMap(QImage)),
                     this, SLOT(showMap(QImage)));
    QObject::connect(pd, SIGNAL(showSSG(QImage)),
                     this, SLOT(showSSG(QImage)));
    QObject::connect(pd, SIGNAL(showImg1(QImage)),
                     this, SLOT(showImg1(QImage)));
    QObject::connect(pd, SIGNAL(showImg2(QImage)),
                     this, SLOT(showImg2(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImg2(QImage img)
{
    ui->lbl_img2->setPixmap(QPixmap::fromImage(img));
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

    seg->setParSigma(value/100.0);
}

void MainWindow::on_scroll_k_valueChanged(int value)
{
    ui->tb_k->setText(QString::number(value));

    seg->setParK(value);
}

void MainWindow::on_scroll_min_size_valueChanged(int value)
{
    ui->tb_min_size->setText(QString::number(value));

    seg->setParMinSize(value);
}

void MainWindow::on_scroll_tau_n_valueChanged(int value)
{
    ui->tb_tau_n->setText(QString::number(value));
    pd->tau_n = value;
}

void MainWindow::on_scroll_tau_c_valueChanged(int value)
{
    ui->tb_tau_c->setText(QString::number(value/10.0));
    pd->tau_c = value/10.0;
}

void MainWindow::on_scroll_tau_w_valueChanged(int value)
{
    ui->tb_tau_w->setText(QString::number(value));
    seg_track->tau_w = value;
}

void MainWindow::on_scroll_tau_f_valueChanged(int value)
{
    ui->tb_tau_f->setText(QString::number(value));
    pd->tau_f = value;
}

void MainWindow::on_scroll_tau_m_valueChanged(int value)
{
    ui->tb_tau_m->setText(QString::number(value/1000.0));
    seg_track->tau_m = value/1000.0;
}

void MainWindow::on_scroll_color_coeff_valueChanged(int value)
{
    ui->tb_color->setText(QString::number(value/10.0));
    gm->color_weight = value/10.0;
}

void MainWindow::on_scroll_pos_coeff_valueChanged(int value)
{
    ui->tb_pos->setText(QString::number(value/10.0));
    gm->pos_weight = value/10.0;
}

void MainWindow::on_scroll_area_coeff_valueChanged(int value)
{
    ui->tb_area->setText(QString::number(value/10.0));
    gm->area_weight = value/10.0;
}

void MainWindow::on_scroll_disapp_coeff1_valueChanged(int value)
{
    ui->tb_disapp1->setText(QString::number(value/10.0));
    pd->coeff_node_disappear1 = value/10.0;
}

void MainWindow::on_scroll_disapp_coeff2_valueChanged(int value)
{
    ui->tb_disapp2->setText(QString::number(value/10.0));
    pd->coeff_node_disappear1 = value/10.0;
}

void MainWindow::on_scroll_appear_coeff_valueChanged(int value)
{
    ui->tb_appear->setText(QString::number(value/10.0));
    pd->coeff_node_appear = value/10.0;
}

void MainWindow::on_scroll_coh_base_valueChanged(int value)
{
    ui->tb_coh_exp_base->setText(QString::number(value));
    pd->coeff_coh_exp_base = value;
}

void MainWindow::on_scroll_appear_thres_valueChanged(int value)
{
    ui->tb_coh_app_thres->setText(QString::number(value/100.0));
    pd->coeff_coh_appear_thres = value/100.0;
}

void MainWindow::on_cb_framebyframe_clicked()
{
    pd->frameByFrameProcess = ui->cb_framebyframe->isChecked();
}

void MainWindow::on_btn_tsc_process_clicked()
{
    TSC *tsc = new TSC();
    tsc->processImages();
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
    if(pd->isProcessing == false)
        pd->processImages();
}

void MainWindow::on_btn_next_clicked()
{
    pd->process_next_frame = true;
}

void MainWindow::on_btn_tracking_start_clicked()
{
    if(seg_track == NULL || seg_track->M.empty())
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
