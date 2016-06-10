#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <string>
#include <opencv2/opencv.hpp>
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QFile>
#include <QDir>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QVariant>
#include <QDebug>
#include <QVector>

#include "utilTypes.h"
#include "TSC/bubble/bubbleprocess.h"

class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    DatabaseHandler();
    explicit DatabaseHandler(string conn_name);
    bool openDB(string file);
    bool createTables();
    bool insertSSG(SSG& ssg);
    bool insertFrame(FrameDesc frame_desc);
    FrameDesc getFrame(int frame_nr);
    vector<SSG> getAllSSGs();
    vector<SSG> getAllSSGsNew();
    bool isOpen();
    void closeDB();
    void setConnName(string conn_name);
    vector<pair<NodeSig,int> > getNodeSigs(int ssg_id);
    QByteArray mat2ByteArray(const Mat &image);
    Mat byteArray2Mat(const QByteArray & byteArray);

private:
    QString conn_name;
    QSqlDatabase db;

signals:

public slots:

};

#endif // DATABASEHANDLER_H
