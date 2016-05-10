#ifndef ROBOTSRENDERAREA_H
#define ROBOTSRENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <utility>
#include <cmath>
#include "mainwindow.h"

class robotsRenderArea : public QWidget
{
    Q_OBJECT

public:
    robotsRenderArea(QWidget *parent = 0);
    MainWindow *main;
    void setMain(MainWindow *m);
    void pushButton_clicked();

protected:
    void paintEvent(QPaintEvent *event);
    void drawInnerNode(QPainter &painter, InnerNode &node, int mainScale);
    void connectTwoInnerNodes(QPainter &painter, InnerNode &node1, InnerNode &node2, int mainScale);
    std::pair<int, int> checkIntersections();
    bool isRectIntersects(int x1, int x2, int y1, int y2, int w1, int w2, int h1, int h2);
    int isNodeIntersectsWithVerticalLines(InnerNode &in);
    bool isRectIntersectsWithVerticalLine(int x1, int y1, int w1, int h1, int x2, int y2, int y3);

    int correctedHeight();
    int correctedWidth();
    void correctedDrawLine(QPainter &painter, int x1, int y1, int x2, int y2, int mainScale);
    void correctedDrawEllipse(QPainter &painter, int x, int y, int w, int h, int mainScale);
    void correctedDrawText(QPainter &painter, int x, int y, int w, int h, int flags, QString &text, int mainScale);
    bool eventFilter(QObject *, QEvent *_event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);

    void draw(QPainter &painter, int mainScale = 1);

private:
    QPen pen;

};

#endif
