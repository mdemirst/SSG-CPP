#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QApplication &app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->setMain(this);
    app.installEventFilter(ui->widget);


    QFile file("../tree.json");
    file.open(QIODevice::ReadOnly);
    QByteArray byteArray = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray);

    if(doc.isObject()){
        QJsonObject mainObj = doc.object();

        QJsonArray innerNodesArr = mainObj.value("innerNodes").toArray();
        for (int i = 0; i < innerNodesArr.size(); i++){
            QJsonObject obj = innerNodesArr[i].toObject();

            InnerNode in;
            in.setValue(obj.value("value").toDouble());
            in.setToID(obj.value("toID").toString("-1"));

            QJsonArray terminalNodes = obj.value("terminalNodes").toArray();
            for(int j = 0; j < terminalNodes.size(); j++){
                QJsonObject terNode = terminalNodes[j].toObject();

                QStringList ids;
                QJsonArray idsArr = terNode.value("ids").toArray();
                for(int k = 0; k < idsArr.size();k++)
                    ids.append(idsArr[k].toString());





                // Eğer herhangi bir tree idsi limitin dışındaysa visible false oluyor.
                bool visible = true;


                TerminalNode tn(QColor(terNode.value("color").toString("black")),
                                ids, visible);

                in.addTerminalNode(tn);
            }

            QStringList otherIds;
            QJsonArray otherIdsArr = obj.value("otherIds").toArray();
            qDebug()<<otherIdsArr;



            for(int z = 0; z < otherIdsArr.size(); z++ )
                otherIds.append(otherIdsArr[z].toString());

            in.setOtherIDs(otherIds);

            innerNodes.append(in);
        }
    }


    ui->widget->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->widget->pushButton_clicked();
}
