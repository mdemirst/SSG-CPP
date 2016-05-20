#ifndef DEFS_H
#define DEFS_H

#define OUTPUT_FOLDER "/home/isl-mahmut/Output/"

#define START_IDX 0

#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_cloudy1/std_cam/"
#define END_IDX 1458

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_cloudy2/std_cam/"
//#define END_IDX 1832

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_cloudy3/std_cam/"
//#define END_IDX 1672

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_night1/std_cam/"
//#define END_IDX 1911

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_night2/std_cam/"
//#define END_IDX 1582

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_night3/std_cam/"
//#define END_IDX 1703

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_sunny1/std_cam/"
//#define END_IDX 1598

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_sunny2/std_cam/"
//#define END_IDX 1514

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_sunny3/std_cam/"
//#define END_IDX 1551

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq1_sunny4/std_cam/"
//#define END_IDX 1777

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/seq2_cloudy1/std_cam/"
//#define END_IDX 2267

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/sa_seq1_cloudy1/std_cam/"
//#define END_IDX 997

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/New-College/"
//#define END_IDX 7855

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/CamVid-8-10/"
//#define END_IDX 10

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/Cagatay/"
//#define END_IDX 4

//#define DATASET_FOLDER "/home/isl-mahmut/Datasets/CamVid-Full/"
//#define END_IDX 500


#define PEN_WIDTH1 1
#define PEN_WIDTH2 2
#define PEN_WIDTH3 3
#define IMG_RESCALE_RAT 0.5

#define INIT_TAU_N 10
#define INIT_TAU_C 0.5
#define INIT_TAU_W 30
#define INIT_TAU_M 0.05
#define INIT_TAU_F 15
#define INIT_TAU_D 1.0
#define INIT_TAU_P 0.6

#define COEFF_NODE_DISAPPEAR_1 0.0
#define COEFF_NODE_DISAPPEAR_2 0.0
#define COEFF_NODE_APPEAR 0.0
#define COEFF_COH_EXP_BASE 10
#define COEFF_COH_APPEAR_THRES 0.90

//Predefined segmentation parameters
#define SEG_SIGMA       0.8
#define SEG_K           300
#define SEG_MIN_SIZE    1500
#define SEG_SCALE       1.0

#define POS_WEIGHT 0.8
#define COLOR_WEIGHT 0.5
#define AREA_WEIGHT 0.7

//Recognition parameters
#define INIT_TAU_R 0.075

#define INIT_TREE_PLOT_H 1000
#define INIT_TREE_PLOT_W 3000
#define INIT_TREE_SSG_H 90
#define INIT_TREE_SSG_W 60

#define MAP_FILTER_SIZE 3

#define REG_PLACE 1
#define REG_TRANS -1

#define PLOT_THRES_IDX 1
#define PLOT_SCORES_IDX 2
#define PLOT_TRACK_IDX 3
#define PLOT_UNINFORMATIVE_IDX 4
#define PLOT_INCOHERENT_IDX 5
#define PLOT_PLACES_IDX 10

#define COH_PLOT_W 1290
#define COH_PLOT_H 150
#define COH_PLOT_MARGIN -15

#define TSC_PLOT_W 1000
#define TSC_PLOT_H 150
#define TSC_PLOT_MARGIN -15

#define EXISTENCE_MAP_W 1290
#define EXISTENCE_MAP_H 250


//Defs for Cold-Sa
//#define COLD_MAP_H 450
//#define COLD_MAP_W 210
//#define COLD_MAP_T 30
//#define COLD_MAP_B -20
//#define COLD_MAP_L -20
//#define COLD_MAP_R 20


//Defs for Cold-Lj
//#define COLD_MAP_H 450//424
//#define COLD_MAP_W 210//201
//#define COLD_MAP_T 80
//#define COLD_MAP_B 0
//#define COLD_MAP_L -8
//#define COLD_MAP_R 22

//Defs for COLD-Fr-STD
#define COLD_MAP_H 750
#define COLD_MAP_W 232
#define COLD_MAP_T 6.5
#define COLD_MAP_B -17.5
#define COLD_MAP_L -1.5
#define COLD_MAP_R 5.8

//#define PROCESS_EDGES



#endif // DEFS_H
