#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "placedetection.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    PlaceDetection *pd;
    
private slots:
    void showImg1(QImage img);
    void showImg2(QImage img);
    void showMatchImage(QImage img);
    void showMap(QImage img);
    void showSSG(QImage img);
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

    void on_btn_tsc_process_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
