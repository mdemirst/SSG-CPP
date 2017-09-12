#ifndef DEFS_H
#define DEFS_H

#define OUTPUT_FOLDER "/home/mdemir/Software/SSG-CPP/Config/"


#define PEN_WIDTH1 1
#define PEN_WIDTH2 2
#define PEN_WIDTH3 3
#define IMG_RESCALE_RAT 0.5

#define BOW_DICT_SIZE 1000
#define BOW_NR_RETRIES 100
#define BOW_TC_COUNT 50
#define BOW_TC_EPSILON 0.001
#define BOW_DESC_TYPE SIFT
#define BOW_MATCHER_TYPE "FlannBased"
#define BOW_DICT_NAME "dict.xml"

#define MAP_FILTER_SIZE 3

#define REG_PLACE 1
#define REG_TRANS -1

#define PLOT_THRES_IDX 1
#define PLOT_SCORES_IDX 2
#define PLOT_TRACK_IDX 3
#define PLOT_UNINFORMATIVE_IDX 4
#define PLOT_INCOHERENT_IDX 5
#define PLOT_VPC_IDX 6
#define PLOT_ODOM_IDX 6
#define PLOT_ROT_IDX 7
#define PLOT_PLACES_IDX 10

#define COH_PLOT_W 1290
#define COH_PLOT_H 150
#define COH_PLOT_MARGIN -15

#define TSC_PLOT_W 1000
#define TSC_PLOT_H 150
#define TSC_PLOT_MARGIN -15

#define EXISTENCE_MAP_W 1290
#define EXISTENCE_MAP_H 500


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

#define FOCAL_LENGHT_PIXELS 550
#define NR_HARMONICS 10
#define TSC_SAT_LOWER 10
#define TSC_SAT_UPPER 255
#define TSC_VAL_LOWER 10
#define TSC_VAL_UPPER 255
//#define TSC_VAL_MEAN 0.2
//#define TSC_VAL_VAR 0.01




#endif // DEFS_H
