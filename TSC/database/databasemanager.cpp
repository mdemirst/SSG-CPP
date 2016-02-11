#include "databasemanager.h"
#include "../utility.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QVariant>
#include <QDebug>
#include <QVector>


//static QSqlDatabase bubbledb;
//static QSqlDatabase invariantdb;

//static QSqlDatabase db;
static QVector<int> placeLabels;

DatabaseManager::DatabaseManager(QObject *parent) :
    QObject(parent)
{
}
bool DatabaseManager::openDB(QString filePath, QString connectionName)
{
    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE",connectionName);

#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    //QString path(QDir::home().path());

    //path.append(QDir::separator()).append("Development").append(QDir::separator()).append("ISL").append(QDir::separator()).append("Datasets").append(QDir::separator()).append("ImageClef2012").append(QDir::separator()).append("bubble.bubbledb");
    QString path = QDir::toNativeSeparators(filePath);

    db.setDatabaseName(path);
    //Development/ISL/Datasets/ImageClef2012
#else
    // NOTE: File exists in the application private folder, in Symbian Qt implementation
    //  bubbledb.setDatabaseName("my.bubbledb.sqlite");
#endif

    // Open databasee
    return db.open();

}
bool DatabaseManager::openDB(QString filePath)
{
    if(!db.isOpen())
    {
        // Find QSLite driver
        db = QSqlDatabase::addDatabase("QSQLITE");

#ifdef Q_OS_LINUX
        // NOTE: We have to store database file into user home folder in Linux
        //QString path(QDir::home().path());

        //path.append(QDir::separator()).append("Development").append(QDir::separator()).append("ISL").append(QDir::separator()).append("Datasets").append(QDir::separator()).append("ImageClef2012").append(QDir::separator()).append("bubble.bubbledb");
        QString path = QDir::toNativeSeparators(filePath);

        db.setDatabaseName(path);
        //Development/ISL/Datasets/ImageClef2012
#else
        // NOTE: File exists in the application private folder, in Symbian Qt implementation
        //  bubbledb.setDatabaseName("my.bubbledb.sqlite");
#endif

        // Open databasee
        return db.open();
    }

    db.close();

    db.removeDatabase("QSQLITE");

    db = QSqlDatabase::addDatabase("QSQLITE");

#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    //QString path(QDir::home().path());

    //path.append(QDir::separator()).append("Development").append(QDir::separator()).append("ISL").append(QDir::separator()).append("Datasets").append(QDir::separator()).append("ImageClef2012").append(QDir::separator()).append("bubble.bubbledb");
    QString path = QDir::toNativeSeparators(filePath);

    db.setDatabaseName(path);
    //Development/ISL/Datasets/ImageClef2012
#else
    // NOTE: File exists in the application private folder, in Symbian Qt implementation
    //  bubbledb.setDatabaseName("my.bubbledb.sqlite");
#endif


    return db.open();
}
/*DatabaseManager::~DatabaseManager()
{

    if(bubbledb.isOpen()) bubbledb.close();
}*/

QSqlError DatabaseManager::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();

}
bool DatabaseManager::isOpen()
{
    return db.isOpen();
}
void DatabaseManager::closeDB()
{
    if(db.isOpen()) db.close();


}

bool DatabaseManager::deleteDB()
{
    // Close database
    db.close();

#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("my.bubbledb.sqlite");
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
#else

    // Remove created database binary file
    return QFile::remove("my.bubbledb.sqlite");
#endif

}
bool DatabaseManager::insertRowtoBubble(int type, int number, int pan, int tilt, double val)
{
    bool ret = false;

    if (db.isOpen())
    {
        //http://www.sqlite.org/autoinc.html
        // NULL = is the keyword for the autoincrement to generate next value

        QSqlQuery query;

        bool exists = query.exec(QString("select * from bubble where type = %1 and number = %2 and pan = %3 and tilt = %4").arg(type).arg(number).arg(pan).arg(tilt));

        if(exists){
            if(!query.next())
                ret = query.exec(QString("insert into bubble values('%1', '%2', '%3', '%4', %5)").arg(type).arg(number).arg(pan).arg(tilt).arg(val));
            else
                ret = query.exec(QString("update bubble set val = %5 where type = %1 and number = %2 and pan = %3 and tilt = %4").arg(type).arg(number).arg(pan).arg(tilt).arg(val));
        }

    }

    return ret;
}
bool DatabaseManager::insertRowtoBubble(int type, int number, bubblePoint row)
{
    bool ret = false;

    if (db.isOpen())
    {
        //http://www.sqlite.org/autoinc.html
        // NULL = is the keyword for the autoincrement to generate next value

        QSqlQuery query;

        bool exists = query.exec(QString("select * from bubble where type = %1 and number = %2 and pan = %3 and tilt = %4").arg(type).arg(number).arg(row.panAng).arg(row.tiltAng));

        if(exists)
        {
            if(!query.next())
                ret = query.exec(QString("insert into bubble values('%1', '%2', '%3', '%4', %5)").arg(type).arg(number).arg(row.panAng).arg(row.tiltAng).arg(row.val));
            else
                ret = query.exec(QString("update bubble set val = %5 where type = %1 and number = %2 and pan = %3 and tilt = %4").arg(type).arg(number).arg(row.panAng).arg(row.tiltAng).arg(row.val));
        }

    }

    return ret;
}
std::vector<bubblePoint> DatabaseManager::readBubble(int type, int number)
{
    QSqlQuery query(QString("select * from bubble where type = %1 and number = %2").arg(type).arg(number));

    std::vector<bubblePoint> bubble;

    while(query.next())
    {
        bubblePoint pt ;

        pt.panAng = query.value(3).toInt();

        pt.tiltAng = query.value(4).toInt();

        pt.val = query.value(5).toDouble();

        bubble.push_back(pt);

        //qDebug()<<"value is"<<query.value(4).toDouble();
    }
    // select * from bubble where number = 2 and type = 0


    return bubble;
}
bool DatabaseManager::insertBubble(int type, int number, std::vector<bubblePoint> bubble)
{

    // bool ret = false;

    if (db.isOpen())
    {

        QSqlQuery query;

        //  query.exec("PRAGMA journal_mode = MEMORY");
        //  query.exec("PRAGMA synchronous = OFF");
        query.prepare(QString("replace into bubble values( ?, ?, ?, ?, ?, ?)"));

        //    query.prepare(QString("insert into bubble values( :type, :number, :pan, :tilt, :val) ON DUPLICATE KEY update val = :val)"));

        //    updateQuery.prepare("update bubble set val = :val where type = %type and number = %2 and pan = %3 and tilt = %4");
        QVariantList typee;
        QVariantList numberr;
        QVariantList placeLabel;
        QVariantList pan;
        QVariantList tilt;
        QVariantList val;

        db.transaction();

        // Insert new bubble
        for(uint i = 0; i < bubble.size(); i++)
        {

            bubblePoint pt;

            pt = bubble.at(i);

            typee<<type;
            numberr<<number;
            pan<<pt.panAng;
            tilt<<pt.tiltAng;
            val<<pt.val;

            if(placeLabels.size() >= number)
            {
                placeLabel<<placeLabels.at(number-1);
            }

            else
                placeLabel<<-1;


            /*      query.bindValue(":type",type);
                 query.bindValue(":number",number);
                  query.bindValue(":pan",pt.panAng);
                   query.bindValue(":tilt",pt.tiltAng);
                   query.bindValue(":val",pt.val);*/
            //  query.exec();

            //query.exec(QString("insert into bubble values('%1', '%2', '%3', '%4', %5)").arg(type).arg(number).arg(pt.panAng).arg(pt.tiltAng).arg(pt.val));


        }



        query.addBindValue(typee);
        query.addBindValue(numberr);
        query.addBindValue(placeLabel);
        query.addBindValue(pan);
        query.addBindValue(tilt);
        query.addBindValue(val);


        if (!query.execBatch())
            qDebug() << query.lastError();

        db.commit();

        return true;

    }



    return false;



}
bool DatabaseManager::insertInvariants(int type, int number, std::vector< std::vector<float> > invariants)
{

    //bool ret = false;

    if (db.isOpen())
    {

        QSqlQuery query;

        // First check, if a bubble has already entered to the table
        /*    bool exists = query.exec(QString("select * from invariant where type = %1 and number = %2").arg(type).arg(number));

        // If query is successfully executed
        if(exists)
        {
            // if there are elements received from the table, then there exists a bubble, we should delete those entries
            if(query.next())
            {
                ret = query.exec(QString("delete from invariant where type = %1 and number = %2").arg(type).arg(number));

                // If deletion is not successfuly executed return false
                if(!ret)

                    return false;

            }
*/
        //else return false;

        // Speed up the multiple-row insertion by using transactions
        //query.exec(QString("BEGIN TRANSACTION"));

        query.prepare(QString("replace into invariant values(?, ?, ?, ?)"));

        QVariantList typee;
        QVariantList numberr;
        QVariantList placeLabel;
        QVariantList val;


        db.transaction();

        // Insert new bubble
        for(uint i = 0; i <invariants.size(); i++)
        {
            for(uint j = 0; j < invariants[i].size();j++){


                val<< invariants[i][j];
                typee<<type;
                numberr<<number;

                if(placeLabels.size() >= number)
                {
                    placeLabel<<placeLabels.at(number-1);
                }

                else
                    placeLabel<<-1;


                //query.exec(QString("replace into invariant values('%1', '%2', '%3')").arg(type).arg(number).arg(val));


            }
        }

        query.addBindValue(typee);
        query.addBindValue(numberr);
        query.addBindValue(placeLabel);
        query.addBindValue(val);

        // query.exec(QString("COMMIT TRANSACTION"));

        if (!query.execBatch())
            qDebug() << query.lastError();
        db.commit();

        return true;

    }



    //  }

    return false;

}
void DatabaseManager::determinePlaceLabels(QString filePath)
{
    QStringList list;

    QFile file(filePath);

    if(!file.open(QFile::ReadOnly))
    {

        return;
    }
    QTextStream* stream = new QTextStream(&file);

    QString st = stream->readLine();

    QStringList mainList;

    while(st != NULL)
    {
        st.replace(" ","");

        list =  st.split(":",QString::SkipEmptyParts);

        mainList.push_back(list.at(1));

        st = stream->readLine();
    }

    file.close();

    QMap<QString, int> valueMap;

    QList<QString> keys;

    int frameCounter = 0;
    int labelCounter = -1;
    foreach (QString str, mainList)
    {
        // QStringList line = str.split(";",QString::SkipEmptyParts);
        //  QString key = line.at(1);

        QString key = str;
        // int val = line.at(0).toInt();
        if(!valueMap.contains(key))
        {
            labelCounter++;
            valueMap.insert(key,labelCounter);
        }
        else
        {
            //valueMap.insert(frameCounter,labelCounter);
        }

        //  counter++;

    }
    QStringList listt = valueMap.keys();

    qSort(listt.begin(), listt.end());

    qDebug()<<listt;

    for(int i = 0; i<listt.size();i++)
    {
        valueMap.insert(listt.at(i),i);
    }
    int counter = 0;

    // QVector<int> labels;

    if(placeLabels.size() > 0)placeLabels.clear();

    placeLabels.resize(mainList.size());

    int count = 0;
    foreach (QString kkey, mainList)
    {
        int val = valueMap.value(kkey);

        placeLabels[count] = val;
        count++;
    }

    // qDebug()<<valueMap;
    //  qDebug()<<placeLabels.at(990);
}
bool DatabaseManager::insertBubbleStatistics(int type, int number, bubbleStatistics stats)
{
    if(db.isOpen())
    {

        QSqlQuery query;


        int placeLabel = -1;

        if(placeLabels.size() >= number)
            placeLabel = placeLabels.at(number-1);

        bool ret = query.exec(QString("insert into bubblestats values(%1, %2, %3, %4, %5, %6)").arg(type).arg(number).arg(placeLabel).arg(stats.mean).arg(stats.variance).arg(stats.maxDist));


        return ret;
    }

    return false;

}
bool DatabaseManager::insertBasePoint(const BasePoint &basepoint)
{
    QByteArray arr = mat2ByteArray(basepoint.invariants);

    if(db.isOpen())
    {
        QSqlQuery query;

        query.prepare(QString("replace into basepoint values(?, ?, ?, ?, ?, ?, ?)"));

        query.addBindValue(basepoint.id);
        query.addBindValue(basepoint.avgVal);
        query.addBindValue(basepoint.varVal);
        query.addBindValue(basepoint.avgLas);
        query.addBindValue(basepoint.varLas);
        query.addBindValue(arr);
        query.addBindValue(basepoint.status);

        bool ret = query.exec();

        return ret;

    }

    return false;

}
bool DatabaseManager::insertBasePoints(const std::vector<BasePoint> basepoints)
{

    if(db.isOpen())
    {
        QSqlQuery query;

        query.prepare(QString("replace into basepoint values(?, ?, ?, ?, ?, ?, ?)"));

        QVariantList ids;
        QVariantList avgVals;
        QVariantList varVals;
        QVariantList avgLass;
        QVariantList varLass;
        QVariantList arrs;
        QVariantList statuses;

        db.transaction();

        for(int i = 0; i < basepoints.size(); i++)
        {
            QByteArray arr = mat2ByteArray(basepoints[i].invariants);



            ids<<basepoints[i].id;
            avgVals<<basepoints[i].avgVal;
            varVals<<basepoints[i].varVal;
            avgLass<<basepoints[i].avgLas;
            varLass<<basepoints[i].varLas;
            arrs<<arr;
            statuses<<basepoints[i].status;

        }

        query.addBindValue(ids);
        query.addBindValue(avgVals);
        query.addBindValue(varVals);
        query.addBindValue(avgLass);
        query.addBindValue(varLass);
        query.addBindValue(arrs);
        query.addBindValue(statuses);

        if (!query.execBatch()){

            qDebug() << query.lastError();

            return false;
        }
        db.commit();

        return true;

    }


    return false;

}

bool DatabaseManager::insertTemporalWindow(const TemporalWindow &twindow)
{

    if(db.isOpen())
    {
        QSqlQuery query;

        query.prepare(QString("replace into temporalwindow values(?, ?, ?)"));

        query.addBindValue(twindow.id);
        query.addBindValue(twindow.startPoint);
        query.addBindValue(twindow.endPoint);



        bool ret = query.exec();

        return ret;

    }

    return false;

}
bool DatabaseManager::insertTopologicalMapRelation(int id, std::pair<int,int> relation)
{
    if(db.isOpen())
    {
        QSqlQuery query(QSqlDatabase::database("knowledge"));

        query.prepare(QString("replace into topologicalmap values(?, ?, ?)"));

        query.addBindValue(id);
        query.addBindValue(relation.first);
        query.addBindValue(relation.second);

        bool ret = query.exec();

        return ret;

    }

    return false;

}
int DatabaseManager::getLearnedPlaceMaxID()
{
    if(db.isOpen())
    {
            QSqlQuery query(QString("select MAX(id) from learnedplace"),QSqlDatabase::database("knowledge"));

           // query.prepare();

           // query.exec();

            query.next();

            return query.value(0).toInt();



    }
}

bool DatabaseManager::insertLearnedPlace(const LearnedPlace &learnedplace)
{

    QByteArray arr= mat2ByteArray(learnedplace.memberPlaces);

    QByteArray arr2 = mat2ByteArray(learnedplace.memberIds);

    QByteArray arr3 = mat2ByteArray(learnedplace.meanInvariant);

    QByteArray arr4 = mat2ByteArray(learnedplace.memberInvariants);


    if(db.isOpen())
    {

        QSqlQuery query(QSqlDatabase::database("knowledge"));

        query.prepare(QString("replace into learnedplace values(?, ?, ?, ?, ?)"));

        query.addBindValue(learnedplace.id);
        query.addBindValue(arr);
        query.addBindValue(arr2);
        query.addBindValue(arr3);
        query.addBindValue(arr4);

        bool ret = query.exec();

        return ret;

    }

    return false;

}
bool DatabaseManager::insertBDSTLevel(int id, const Level &aLevel)
{
    Mat members(aLevel.members);
    QByteArray arr= mat2ByteArray(members);

    //  QByteArray arr2 =

    Mat meanInvariant(aLevel.meanInvariant);
    QByteArray arr2 = mat2ByteArray(meanInvariant);

    if(db.isOpen())
    {

        QSqlQuery query(QSqlDatabase::database("knowledge"));

        query.prepare(QString("replace into cuetree values(?, ?, ?, ?, ?)"));

        query.addBindValue(id);
        query.addBindValue(arr);
        query.addBindValue(aLevel.connectionIndex);
        query.addBindValue(arr2);
        query.addBindValue(aLevel.val);


        bool ret = query.exec();

        return ret;

    }

    return false;

}

LearnedPlace DatabaseManager::getLearnedPlace(int id)
{
    LearnedPlace place;

    if(db.isOpen())
    {
        QSqlQuery query(QString("select* from learnedplace where id = %1").arg(id), QSqlDatabase::database("knowledge"));

        query.next();

        int id = query.value(0).toInt();

        // id;
        qDebug()<<"Learned Place id"<<id;
        QByteArray array = query.value(1).toByteArray();
        place.memberPlaces = DatabaseManager::byteArray2Mat(array);

        QByteArray array2 = query.value(2).toByteArray();
        place.memberIds = DatabaseManager::byteArray2Mat(array2);

        QByteArray array3 = query.value(3).toByteArray();
        place.meanInvariant = DatabaseManager::byteArray2Mat(array3);

        QByteArray array4 = query.value(4).toByteArray();
        place.memberInvariants = DatabaseManager::byteArray2Mat(array4);

        //qDebug()<<meanInv.rows<<members.rows;

        place.id = id;


        //  QByteArray array = query.value(0).toByteArray();

        // return byteArray2Mat(array);
    }


    return place;

}

bool DatabaseManager::insertPlace(const Place &place)
{
    QByteArray arr = mat2ByteArray(place.meanInvariant);

    QByteArray arr2 = mat2ByteArray(place.memberIds);

    QByteArray arr3 = mat2ByteArray(place.memberInvariants);

    if(db.isOpen())
    {
        QSqlQuery query;

        query.prepare(QString("replace into place values(?, ?, ?, ?)"));

        query.addBindValue(place.id);
        query.addBindValue(arr);
        query.addBindValue(arr2);
        query.addBindValue(arr3);

        bool ret = query.exec();

        return ret;

    }

    return false;



}
cv::Mat DatabaseManager::getPlaceMeanInvariant(int id)
{
    if(db.isOpen())
    {
        QSqlQuery query(QString("select meaninvariant from place where id = %1").arg(id));

        query.next();

        QByteArray array = query.value(0).toByteArray();

        return byteArray2Mat(array);
    }



}
cv::Mat DatabaseManager::getPlaceMemberIds(int id)
{
    if(db.isOpen())
    {
        QSqlQuery query(QString("select memberIds from place where id = %1").arg(id));

        query.next();

        QByteArray array = query.value(0).toByteArray();

        return byteArray2Mat(array);
    }



}
Place DatabaseManager::getPlace(int id)
{
    Place place;

    if(db.isOpen())
    {
        QSqlQuery query(QString("select* from place where id = %1").arg(id));

        query.next();

        int id = query.value(0).toInt();

        // id;
        qDebug()<<id;
        QByteArray array = query.value(1).toByteArray();
        place.meanInvariant = DatabaseManager::byteArray2Mat(array);
        QByteArray array2 = query.value(2).toByteArray();
        place.memberIds = DatabaseManager::byteArray2Mat(array2);
        QByteArray array3 = query.value(3).toByteArray();
        place.memberInvariants = DatabaseManager::byteArray2Mat(array3);

        //qDebug()<<meanInv.rows<<members.rows;

        place.id = id;


        //  QByteArray array = query.value(0).toByteArray();

        // return byteArray2Mat(array);
    }


    return place;

}
QByteArray DatabaseManager::mat2ByteArray(const cv::Mat &image)
{
    QByteArray byteArray;
    QDataStream stream( &byteArray, QIODevice::WriteOnly );
    stream << image.type();
    stream << image.rows;
    stream << image.cols;
    const size_t data_size = image.cols * image.rows * image.elemSize();
    QByteArray data = QByteArray::fromRawData( (const char*)image.ptr(), data_size );
    stream << data;
    return byteArray;
}

cv::Mat DatabaseManager::byteArray2Mat(const QByteArray & byteArray)
{
    QDataStream stream(byteArray);
    int matType, rows, cols;
    QByteArray data;
    stream >> matType;
    stream >> rows;
    stream >> cols;
    stream >> data;
    cv::Mat mat(rows, cols, matType, (void*)data.data() );
    return mat.clone();
}
