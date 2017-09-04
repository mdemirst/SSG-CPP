#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "placedetection.h"
#include "segmenttrack.h"
#include "tschybrid.h"
#include "recognition.h"
#include "pipeline.h"
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    SegmentTrack* seg_track;
    GraphMatch* gm;
    Segmentation* seg;
    TSCHybrid* tsc_hybrid;

    Pipeline* pipeline;
    Parameters* params;
    vector<Parameters*> params_all;
    vector<Dataset> datasets;
    Recognition* recognition;

private:
    bool auto_tune;
    
private slots:
    void showImgOrg(QImage img);
    void showMatchImage(QImage img);
    void showMap(QImage img);
    void showSSG(QImage img);
    void showTree(QImage img);
    void printMessage(QString str);

    void showTrackMap(QImage img);
    void showTrackSegment(QImage img);

    void on_btn_process_all_clicked();
    void on_btn_next_clicked();
    void on_scroll_sigma_valueChanged(int value);

    void on_scroll_k_valueChanged(int value);

    void on_scroll_min_size_valueChanged(int value);

    void on_scroll_tau_n_valueChanged(int value);

    void on_scroll_tau_c_valueChanged(int value);

    void on_scroll_tau_w_valueChanged(int value);

    void on_scroll_tau_f_valueChanged(int value);

    void on_scroll_tau_m_valueChanged(int value);

    void on_scroll_color_coeff_valueChanged(int value);

    void on_scroll_pos_coeff_valueChanged(int value);

    void on_scroll_area_coeff_valueChanged(int value);

    void on_scroll_disapp_coeff1_valueChanged(int value);

    void on_scroll_disapp_coeff2_valueChanged(int value);

    void on_scroll_appear_coeff_valueChanged(int value);

    void on_scroll_coh_base_valueChanged(int value);

    void on_scroll_appear_thres_valueChanged(int value);

    void on_cb_framebyframe_clicked();

    void on_btn_tracking_start_clicked();

    void on_lbl_find_coherent_clicked();

    void on_btn_stop_process_clicked();

    void on_scroll_tau_r_valueChanged(int value);

    void on_btn_process_hierarchical_clicked();

    void on_scroll_tau_p_valueChanged(int value);

    void on_tb_tree_plot_h_editingFinished();

    void on_tb_tree_plot_w_editingFinished();

    void on_tb_tree_ssg_h_editingFinished();

    void on_tb_tree_ssg_w_editingFinished();

    void on_btn_save_settings_clicked();

    void on_btn_hierarchical_stop_clicked();

    void on_cb_auto_tune_clicked();

    void on_scroll_tau_p_sliderReleased();

    void on_scroll_tau_r_sliderReleased();

    void on_scroll_tau_v_valueChanged(int value);

    void on_scroll_tau_v_sliderReleased();

    void on_scroll_bow_coeff_valueChanged(int value);

    void on_btn_next_frame_clicked();

    void on_btn_init_from_db_clicked();

    void on_cb_place_circle_clicked();

    void on_cb_show_inner_ssgs_clicked();

    void on_btn_init_from_db_2_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
