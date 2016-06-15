#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T14:01:04
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += console

TARGET = SegmentSummaryGraphs
TEMPLATE = app

QMAKE_CXXFLAGS += -Wno-sign-compare

SOURCES += main.cpp\
      mainwindow.cpp \
      segmentation.cpp \
      graphmatch.cpp \
      GraphSegmentation/segment-image.cpp \
      GraphSegmentation/segment-graph.cpp \
      GraphSegmentation/pnmfile.cpp \
      GraphSegmentation/misc.cpp \
      GraphSegmentation/imutil.cpp \
      GraphSegmentation/imconv.cpp \
      GraphSegmentation/image.cpp \
      GraphSegmentation/filter.cpp \
      GraphSegmentation/disjoint-set.cpp \
      GraphSegmentation/convolve.cpp \
      utils.cpp \
      hungarian.cpp \
      qcustomplot.cpp \
      placedetection.cpp \
      rag.cpp \
      ssgproc.cpp \
      TSC/bubble/invariants.cpp \
      TSC/bubble/dfc.cpp \
      TSC/bubble/bubbleprocess.cpp \
      TSC/database/databasemanager.cpp \
      TSC/imageprocess/imageprocess.cpp \
      TSC/tsc.cpp \
      TSC/placedetector.cpp \
      TSC/utility.cpp \
      segmenttrack.cpp \
    tschybrid.cpp \
    cluster.c \
    recognition.cpp \
    utilTypes.cpp \
    databasehandler.cpp

HEADERS  += mainwindow.h \
      segmentation.h \
      graphmatch.h \
      GraphSegmentation/segment-image.h \
      GraphSegmentation/segment-graph.h \
      GraphSegmentation/pnmfile.h \
      GraphSegmentation/misc.h \
      GraphSegmentation/imutil.h \
      GraphSegmentation/imconv.h \
      GraphSegmentation/image.h \
      GraphSegmentation/filter.h \
      GraphSegmentation/disjoint-set.h \
      GraphSegmentation/convolve.h \
      utilTypes.h \
      utils.h \
      hungarian.h \
      qcustomplot.h \
      placedetection.h \
      rag.h \
      ssgproc.h \
      TSC/bubble/bubbleprocess.h \
      TSC/database/databasemanager.h \
      TSC/imageprocess/imageprocess.h \
      TSC/tsc.h \
      TSC/placedetector.h \
      TSC/utility.h \
      segmenttrack.h \
      defs.h \
    tschybrid.h \
    cluster.h \
    recognition.h \
    SimpleMatrix.h \
    databasehandler.h

FORMS    += mainwindow.ui

LIBS += `pkg-config opencv --libs`
