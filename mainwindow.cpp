#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "segmentation.h"
#include "graphmatch.h"
#include "placedetection.h"
#include "TSC/tsc.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pd = new PlaceDetection(ui->coherency_plot);

    ui->scroll_sigma->setValue(pd->seg->getParSigma()*100);
    ui->scroll_k->setValue(pd->seg->getParK());
    ui->scroll_min_size->setValue(pd->seg->getParMinSize());
    ui->scroll_tau_n->setValue(pd->tau_n);
    ui->scroll_tau_c->setValue(pd->tau_c);
    ui->scroll_tau_w->setValue(pd->tau_w);
    ui->scroll_tau_f->setValue(pd->tau_f);
    ui->scroll_tau_m->setValue(pd->tau_m*1000);
    ui->scroll_color_coeff->setValue(pd->gm->color_weight*10);
    ui->scroll_pos_coeff->setValue(pd->gm->pos_weight*10);
    ui->scroll_area_coeff->setValue(pd->gm->area_weight*10);

    ui->scroll_disapp_coeff1->setValue(pd->coeff_node_disappear1*10);
    ui->scroll_disapp_coeff2->setValue(pd->coeff_node_disappear2*10);
    ui->scroll_appear_coeff->setValue(pd->coeff_node_appear*10);
    ui->scroll_coh_base->setValue(pd->coeff_coh_exp_base);
    ui->scroll_appear_thres->setValue(pd->coeff_coh_appear_thres*100);

    ui->cb_framebyframe->setChecked(false);
    pd->frameByFrameProcess = false;

    QObject::connect(pd->gm, SIGNAL(showMatchImage(QImage)),
                     this, SLOT(showMatchImage(QImage)));
    QObject::connect(pd, SIGNAL(showImg1(QImage)),
                     this, SLOT(showImg1(QImage)));
    QObject::connect(pd, SIGNAL(showImg2(QImage)),
                     this, SLOT(showImg2(QImage)));
    QObject::connect(pd, SIGNAL(showMap(QImage)),
                     this, SLOT(showMap(QImage)));
    QObject::connect(pd, SIGNAL(showSSG(QImage)),
                     this, SLOT(showSSG(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImg1(QImage img)
{
    ui->lbl_img1->setPixmap(QPixmap::fromImage(img));
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

void MainWindow::on_btn_process_all_clicked()
{
    pd->processImages();
}

void MainWindow::on_btn_next_clicked()
{
    pd->process_next_frame = true;
}

void MainWindow::on_scroll_sigma_valueChanged(int value)
{
    ui->tb_sigma->setText(QString::number(value/100.0));

    pd->seg->setParSigma(value/100.0);
}

void MainWindow::on_scroll_k_valueChanged(int value)
{
    ui->tb_k->setText(QString::number(value));

    pd->seg->setParK(value);
}

void MainWindow::on_scroll_min_size_valueChanged(int value)
{
    ui->tb_min_size->setText(QString::number(value));

    pd->seg->setParMinSize(value);
}

void MainWindow::on_scroll_tau_n_valueChanged(int value)
{
    ui->tb_tau_n->setText(QString::number(value));
    pd->tau_n = value;
}

void MainWindow::on_scroll_tau_c_valueChanged(int value)
{
    ui->tb_tau_c->setText(QString::number(value));
    pd->tau_c = value;
}

void MainWindow::on_scroll_tau_w_valueChanged(int value)
{
    ui->tb_tau_w->setText(QString::number(value));
    pd->tau_w = value;
}

void MainWindow::on_scroll_tau_f_valueChanged(int value)
{
    ui->tb_tau_f->setText(QString::number(value));
    pd->tau_f = value;
}

void MainWindow::on_scroll_tau_m_valueChanged(int value)
{
    ui->tb_tau_m->setText(QString::number(value/1000.0));
    pd->tau_m = value/1000.0;
}

void MainWindow::on_scroll_color_coeff_valueChanged(int value)
{
    ui->tb_color->setText(QString::number(value/10.0));
    pd->gm->color_weight = value/10.0;
}

void MainWindow::on_scroll_pos_coeff_valueChanged(int value)
{
    ui->tb_pos->setText(QString::number(value/10.0));
    pd->gm->pos_weight = value/10.0;
}

void MainWindow::on_scroll_area_coeff_valueChanged(int value)
{
    ui->tb_area->setText(QString::number(value/10.0));
    pd->gm->area_weight = value/10.0;
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
