#ifndef DEFS_H
#define DEFS_H

#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_night1/std_cam/"

#define START_IDX 200
#define END_IDX 1450

#define PEN_WIDTH1 1
#define PEN_WIDTH2 2
#define IMG_RESCALE_RAT 0.5

#define INIT_TAU_N 5 //10
#define INIT_TAU_C 0.2
#define INIT_TAU_W 30// 20 //15
#define INIT_TAU_M 0.015
#define INIT_TAU_F 30
#define INIT_TAU_D 1.0
#define INIT_TAU_P 0.8

#define COEFF_NODE_DISAPPEAR_1 2
#define COEFF_NODE_DISAPPEAR_2 0.5
#define COEFF_NODE_APPEAR 0.2
#define COEFF_COH_EXP_BASE 5
#define COEFF_COH_APPEAR_THRES 0.85//0.80

#define MAP_FILTER_SIZE 3

#define REG_PLACE 1
#define REG_TRANS -1

#define PLOT_THRES_IDX 1
#define PLOT_SCORES_IDX 2
#define PLOT_PLACES_IDX 10

#define COH_PLOT_W 1290
#define COH_PLOT_H 150
#define COH_PLOT_MARGIN -15

#define TSC_PLOT_W 1000
#define TSC_PLOT_H 150
#define TSC_PLOT_MARGIN -15

#define EXISTENCE_MAP_W 1290
#define EXISTENCE_MAP_H 250

#define COLD_MAP_H 750
#define COLD_MAP_W 232
#define COLD_MAP_T 6.5
#define COLD_MAP_B -17.5
#define COLD_MAP_L -1.5
#define COLD_MAP_R 5.8

//#define PROCESS_EDGES



#endif // DEFS_H
