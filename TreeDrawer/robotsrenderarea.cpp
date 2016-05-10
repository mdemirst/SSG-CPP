#include <QtGui>
#include "robotsrenderarea.h"

double maxVal = -1;

float xMain = 0, yMain = 0;
double sliderVal = 1;
const int sliderMax = 100;

const bool rulerSplitAuto = true;
int rulerSplitNum = 4;

const int rulerSplitHeight = 50;
const int leftSpacing = 60;
const int radius = 30;
const int spacing = 10;
const int straightLineLength = 12;

const float drawScale = 1;
const float saveScale = 3;
const int fontSize = 10;
const int rulerPrecision = 4;

robotsRenderArea::robotsRenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    main = NULL;
}

void robotsRenderArea::setMain(MainWindow *m){
    main = m;
}

void robotsRenderArea::pushButton_clicked(){
    QPixmap bmp(correctedWidth() * saveScale, correctedHeight() * saveScale);
    int xMainYedek = xMain, yMainYedek = yMain;
    xMain = 0;
    yMain = 0;

    QPainter painter(&bmp);

    painter.fillRect(0, 0, bmp.width(), bmp.height(), Qt::white);
    draw(painter, saveScale);

    bmp.save("image.png", 0, 100);

    xMain = xMainYedek;
    yMain = yMainYedek;
}

void robotsRenderArea::correctedDrawLine(QPainter &painter, int x1, int y1, int x2, int y2, int mainScale){
    painter.drawLine(mainScale * (x1 + xMain), mainScale * (y1 + yMain), mainScale * (x2 + xMain), mainScale * (y2 + yMain));
}

void robotsRenderArea::correctedDrawEllipse(QPainter &painter, int x, int y, int w, int h, int mainScale){
    painter.drawEllipse(mainScale * (x + xMain), mainScale * (y + yMain), mainScale * w, mainScale * h);
}

void robotsRenderArea::correctedDrawText(QPainter &painter, int x, int y, int w, int h, int flags, QString &text, int mainScale){
    QFont font;
    font.setPixelSize(fontSize * mainScale);
    painter.setFont(font);
    painter.drawText(mainScale * (x + xMain), mainScale * (y + yMain), mainScale * w, mainScale * h, flags, text);
}

int robotsRenderArea::correctedHeight(){
    int minHeight = 2 * straightLineLength + 2 * radius + 2 * spacing;

    if(sliderVal < 1)
        return (int)((height() - minHeight) * std::exp(sliderVal - 1) + minHeight);
    else
        return (int)(height() * sliderVal);
}

int robotsRenderArea::correctedWidth(){
    int maxWdth = 0;
    for(int i = 0; i < main->innerNodes.count(); i++){
        InnerNode& in = main->innerNodes[i];
        int right = in.getPosition().x() + in.visibleCount() * (2 * radius + spacing);
        if(maxWdth < right) maxWdth = right;
    }
    return maxWdth;
}

void robotsRenderArea::drawInnerNode(QPainter &painter, InnerNode &node, int mainScale){
    int x = node.getPosition().x(), y = node.getPosition().y();
    int i = 0;


    foreach(TerminalNode tn, node.getNodes()){
        if (!tn.isVisible()) continue;

        pen.setColor(tn.getColor());
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
        correctedDrawEllipse(painter, x + i * (2 * radius + spacing), y + straightLineLength, radius * 2, radius * 2, mainScale);

        QString text = "";
        foreach (QString id ,tn.getNameIDs())
            text += id + "\n";
        text.remove(text.length() - 1, 1);
        correctedDrawText(painter, x + i * (2 * radius + spacing), y + straightLineLength, radius * 2, radius * 2,
                         Qt::AlignCenter, text, mainScale);


        pen.setColor(Qt::black);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
        correctedDrawLine(painter, x + i * (2 * radius + spacing) + radius, y + straightLineLength, x + i * (2 * radius + spacing) + radius, y, mainScale);
        if (i != node.visibleCount() - 1)
            correctedDrawLine(painter, x + i * (2 * radius + spacing) + radius, y, x + (i + 1) * (2 * radius + spacing) + radius, y, mainScale);



        i++;
    }
}

void robotsRenderArea::connectTwoInnerNodes(QPainter &painter, InnerNode &node1, InnerNode &node2, int mainScale) {
    int x1 = node1.getPosition().x(), y1 = node1.getPosition().y(),
            x2 = node2.getPosition().x(), y2 = node2.getPosition().y();
    int width1 = node1.visibleCount() * (2 * radius + spacing) - spacing,
            width2 = node2.visibleCount() * (2 * radius + spacing) - spacing;

    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    correctedDrawLine(painter, x1 + width1 / 2, y1, x1 + width1 / 2, y2 - straightLineLength, mainScale);
    correctedDrawLine(painter, x1 + width1 / 2, y2 - straightLineLength, x2 + width2 / 2, y2 - straightLineLength, mainScale);
    correctedDrawLine(painter, x2 + width2 / 2, y2 - straightLineLength, x2 + width2 / 2, y2, mainScale);
}

bool robotsRenderArea::isRectIntersectsWithVerticalLine(int x1, int y1, int w1, int h1, int x2, int y2, int y3){
    if(x2 > x1 && x2 < x1 + w1){
        if(y2 < y1 && y3 < y1)
            return false;
        if(y2 > y1 + h1 && y3 > y1 + h1)
            return false;
        return true;
    }
    return false;
}

int robotsRenderArea::isNodeIntersectsWithVerticalLines(InnerNode &in){
    int x = in.getPosition().x(), y = in.getPosition().y();
    int w = in.visibleCount() * (2 * radius + spacing) - spacing, h = 2 * radius + straightLineLength;

    foreach(InnerNode node,main->innerNodes){
        if (node == in) continue;
        QString toID = node.getToID();
        if (InnerNode::isToIDNull(toID)) continue;

        InnerNode node2 = *std::find_if(main->innerNodes.begin(), main->innerNodes.end(),
                               [toID](InnerNode &_in){ return _in.contains(toID); });

        int x1 = node.getPosition().x(), y1 = node.getPosition().y(), y2 = node2.getPosition().y();
        int width1 = node.visibleCount() * (2 * radius + spacing) - spacing;

        if(isRectIntersectsWithVerticalLine(x, y, w, h,
                                    x1 + width1 / 2, y1, y2 - straightLineLength)){
            if(x > x1)
                return main->innerNodes.indexOf(in);

            else
                return main->innerNodes.indexOf(node);
        }
    }
    return -1;
}

bool robotsRenderArea::isRectIntersects(int x1, int x2, int y1, int y2, int w1, int w2, int h1, int h2){
    if (y1 < y2 && y2 - y1 < h2){
        if (x1 < x2 && x2 - x1 < w1)
            return true;
        if (x1 >= x2 && x1 - x2 < w2)
            return true;
    }
    if (y1 >= y2 && y1 - y2 < h1){
        if (x1 < x2 && x2 - x1 < w1)
            return true;
        if (x1 >= x2 && x1 - x2 < w2)
            return true;
    }
    return false;
}

std::pair<int, int> robotsRenderArea::checkIntersections(){
    for(int i = 0; i < main->innerNodes.count(); i++){
        if(main->innerNodes[i].getPosition().x() == -1) continue;

        for(int j = i + 1; j < main->innerNodes.count(); j++){
            if(main->innerNodes[j].getPosition().x() == -1) continue;

            if(isRectIntersects(main->innerNodes[i].getPosition().x(), main->innerNodes[j].getPosition().x(),
                    main->innerNodes[i].getPosition().y(), main->innerNodes[j].getPosition().y(),
                    main->innerNodes[i].visibleCount() * (2 * radius + spacing) - spacing, main->innerNodes[j].visibleCount() * (2 * radius + spacing) - spacing,
                    2 * radius + straightLineLength, 2 * radius + straightLineLength))
                return std::pair<int, int>(i, j);
        }

        int inter = isNodeIntersectsWithVerticalLines(main->innerNodes[i]);
        if(inter != -1)
            return std::pair<int, int>(inter, -1);
    }
    return std::pair<int, int>(-1, -1);
}

void robotsRenderArea::draw(QPainter &painter, int mainScale){
    painter.setRenderHint(QPainter::Antialiasing, true);
    pen.setWidth(mainScale);

    if(main != NULL && main->innerNodes.count() != 0){
        std::sort(main->innerNodes.begin(), main->innerNodes.end(),
                  [](InnerNode& in1, InnerNode& in2){ return in1.getValue() < in2.getValue(); });
        maxVal = main->innerNodes.back().getValue();
        double scale = (correctedHeight() - 2 * straightLineLength - 2 * radius - 2 * spacing) / maxVal;

        foreach(InnerNode in, main->innerNodes)
            in.setPosition(QPoint(-1,-1));

        bool first = true;
        std::pair<int, int> lastIntersection(-1, -1);
        while(true){
            if(!first && lastIntersection.first == -1)
                break;

            QList<bool> isSet;
            for(int i = 0; i < main->innerNodes.count(); i++)
                isSet.append(false);

            if(first){
                first = false;

                int rightMost = leftSpacing;
                for(int i = 0; i < main->innerNodes.count(); i++){
                    int idx = i;
                    while(true){
                        if(isSet[idx]) break;

                        main->innerNodes[idx].setPosition(QPoint(rightMost, correctedHeight() - (main->innerNodes[idx].getValue() * scale + 2 * radius + straightLineLength + spacing)));
                        rightMost += (main->innerNodes[idx].visibleCount() * (2 * radius + spacing) - spacing) / 2 + spacing;

                        isSet[idx] = true;

                        if(InnerNode::isToIDNull(main->innerNodes[idx].getToID()))
                            break;

                        QString toID = main->innerNodes[idx].getToID();
                        idx = std::find_if(main->innerNodes.begin(), main->innerNodes.end(),
                                           [toID](InnerNode& in){ return in.contains(toID); })
                                    - main->innerNodes.begin();
                    }
                }

                lastIntersection = checkIntersections();
                continue;
            }
            else{
                if(lastIntersection.second != -1){
                    if(main->innerNodes[lastIntersection.first].getPosition().x() <
                            main->innerNodes[lastIntersection.second].getPosition().x()){
                        int x = main->innerNodes[lastIntersection.second].getPosition().x();
                        int y = main->innerNodes[lastIntersection.second].getPosition().y();
                        main->innerNodes[lastIntersection.second].setPosition(QPoint(x + 2 * radius + spacing, y));
                    }
                    else{
                        int x = main->innerNodes[lastIntersection.first].getPosition().x();
                        int y = main->innerNodes[lastIntersection.first].getPosition().y();
                        main->innerNodes[lastIntersection.first].setPosition(QPoint(x + 2 * radius + spacing, y));
                    }
                }
                else{
                    int x = main->innerNodes[lastIntersection.first].getPosition().x();
                    int y = main->innerNodes[lastIntersection.first].getPosition().y();
                    main->innerNodes[lastIntersection.first].setPosition(QPoint(x + 2 * radius + spacing, y));
                }

                lastIntersection = checkIntersections();
                continue;
            }
        }

        foreach(InnerNode in, main->innerNodes){
          //  qDebug()<<"I am here";

            drawInnerNode(painter, in, mainScale);

            if(InnerNode::isToIDNull(in.getToID())) continue;

            QString toID = in.getToID();
            qDebug()<<toID;
            int idx = std::find_if(main->innerNodes.begin(), main->innerNodes.end(),
                               [toID](InnerNode &in){ return in.contains(toID); })
                        - main->innerNodes.begin();

            connectTwoInnerNodes(painter, in, main->innerNodes[idx], mainScale);
        }
    }

    int ymax = 2 * straightLineLength + radius + spacing;
    int ymin = correctedHeight() - spacing - radius;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (rulerSplitAuto)
        rulerSplitNum = std::abs(ymax - ymin) / rulerSplitHeight;
    if(rulerSplitNum < 1) rulerSplitNum = 1;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    correctedDrawLine(painter, spacing / 3, ymin, spacing / 3, ymax, mainScale);
    for(int i = 0; i < rulerSplitNum + 1; i++){
        correctedDrawLine(painter, spacing / 3, (ymax - ymin) * i / rulerSplitNum + ymin, 2 * spacing / 3, (ymax - ymin) * i / rulerSplitNum + ymin, mainScale);

        correctedDrawText(painter, spacing, (ymax - ymin) * i / rulerSplitNum + ymin - 25,
                         leftSpacing - spacing * 4 / 3, 50, Qt::AlignVCenter | Qt::AlignLeft,
                         QString::number(maxVal * i / rulerSplitNum, 'g', rulerPrecision), mainScale);
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(mainScale * xMain, mainScale * yMain, mainScale * correctedWidth() - 1, mainScale * correctedHeight() - 1));
}

bool isIn = false;
int inY = 0;
void robotsRenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    draw(painter, drawScale);

    if(isIn){
        pen.setColor(Qt::red);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
        correctedDrawLine(painter, spacing / 3, inY - yMain, spacing, inY - yMain, drawScale);
    }
}

bool isPressed = false;
int mouseX = -1, mouseY = -1;
bool robotsRenderArea::eventFilter(QObject * /*obj*/, QEvent *_event)
{
  if (_event->type() == QEvent::MouseMove)
  {
    QMouseEvent *event = static_cast<QMouseEvent*>(_event);
    if(isPressed){
        xMain += (event->x() - mouseX) / drawScale;
        yMain += (event->y() - mouseY) / drawScale;
        mouseX = event->x();
        mouseY = event->y();

        update();
    }
    else{
        if(event->x() > drawScale * xMain && event->x() < drawScale * (xMain + correctedWidth())
                && event->y() > drawScale * (yMain + 2 * straightLineLength + radius + spacing) && event->y() < drawScale * (yMain + correctedHeight() - spacing - radius)){
            isIn = true;
            inY = event->y() / drawScale;
        }
        else isIn = false;

        update();
    }
  }
  return false;
}
void robotsRenderArea::mousePressEvent(QMouseEvent *event){
    isPressed = true;

    mouseX = event->x();
    mouseY = event->y();
}
void robotsRenderArea::mouseReleaseEvent(QMouseEvent * /* event */){
    isPressed = false;
}
void robotsRenderArea::wheelEvent(QWheelEvent *event){
    sliderVal += event->delta() / 1200.0;

    update();
}
void robotsRenderArea::mouseDoubleClickEvent(QMouseEvent *){
    pushButton_clicked();
}
