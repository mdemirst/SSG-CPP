#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QList>
#include <algorithm>
#include <QPoint>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QByteArray>
#include <utility>

class InnerNode;

class TerminalNode {
    QColor color;
    QStringList ids;
 //   QStringList otherIds;
    bool visible;
public:
    TerminalNode(QColor color, QStringList ids, bool visible):color(color), ids(ids),  visible(visible) {}
    inline QStringList getNameIDs() { return ids; }
  //  inline QStringList getOtherIDs() { return otherIds; }
    inline QColor getColor() { return color; }
    inline bool isVisible() { return visible; }

    inline bool operator==(const TerminalNode& n){
        return n.ids == ids && n.color == color && n.visible == visible;
    }
};

class InnerNode {
    QList<TerminalNode> terminalNodes;
    QPoint position;
    QStringList otherIds;
    double value;
    QString toID;
public:
    InnerNode():toID("-1") {}
    inline void setOtherIDs(QStringList ids){otherIds =ids; }
    inline QStringList getOtherIDs() { return otherIds; }
    inline QList<TerminalNode>& getNodes(){ return terminalNodes; }
    inline void addTerminalNode(TerminalNode node){
        terminalNodes.append(node);
    }
    inline bool removeTerminalNode(TerminalNode node){
        return terminalNodes.removeOne(node);
    }
  /*  TerminalNode& getTerminalNode(QString id){
        return *std::find_if (terminalNodes.begin(), terminalNodes.end(), [id](TerminalNode& n){ return n.getOtherIDs().contains(id); });
    }*/
    bool contains(QString id){
        qDebug()<<otherIds<<id;
        return otherIds.contains(id);
       // return std::any_of (otherIds.begin(), otherIds.end(), [id]{ return otherIds.contains(id); });
    }
    int count(){
        return terminalNodes.count();
    }
    int visibleCount(){
        return std::count_if(terminalNodes.begin(), terminalNodes.end(), [](TerminalNode &ter){ return ter.isVisible(); });
    }
    QPoint getPosition(){
        return position;
    }
    void setPosition(QPoint pos){
        position = pos;
    }
    void setValue(double val){
        value = val;
    }
    double getValue(){
        return value;
    }
    QString getToID(){
        return toID;
    }
    void setToID(QString id){
        toID = id;
    }

    static bool isToIDNull(QString id){
        return id.contains("-1");
    }

    inline bool operator==(const InnerNode& n){
        return n.position == position && n.value == value && n.toID == toID && n.terminalNodes == terminalNodes;
    }
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QApplication &app, QWidget *parent = 0);
    ~MainWindow();

    QList<InnerNode> innerNodes;

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
