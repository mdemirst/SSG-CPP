#include "databasehandler.h"

DatabaseHandler::DatabaseHandler()
{
    this->conn_name = "default_conn";
}

DatabaseHandler::DatabaseHandler(string conn_name)
{
    this->conn_name = QString(conn_name.c_str());
}

void DatabaseHandler::setConnName(string conn_name)
{
    this->conn_name = QString(conn_name.c_str());
}

bool DatabaseHandler::openDB(string file)
{
    if(!db.isOpen())
    {
        db = QSqlDatabase::addDatabase("QSQLITE",conn_name);
        QString path = QDir::toNativeSeparators(file.c_str());
        db.setDatabaseName(path);
        return db.open();
    }

    db.close();
    db.removeDatabase(conn_name);
    db = QSqlDatabase::addDatabase("QSQLITE",conn_name);

    QString path = QDir::toNativeSeparators(file.c_str());
    db.setDatabaseName(path);
    return db.open();
}

bool DatabaseHandler::createTables()
{
    bool ret = true;
    QSqlQuery ssg_query(QSqlDatabase::database(conn_name));
    ret &= ssg_query.exec("create table ssg "
                           "(id integer primary key autoincrement, "
                           "ssg_id integer, "
                           "start integer, "
                           "end integer, "
                           "sample_frame varchar(100), "
                           "coord_x float, "
                           "coord_y float, "
                           "color integer, "
                           "mean_inv blob, "
                           "member_inv blob)");

    QSqlQuery nodesig_query(QSqlDatabase::database(conn_name));
    ret &= nodesig_query.exec("create table nodesig "
                              "(id integer primary key autoincrement, "
                              "ssg_id integer, "
                              "nodesig_id integer, "
                              "count integer, "
                              "center_x integer, "
                              "center_y integer, "
                              "color_r integer, "
                              "color_g integer, "
                              "color_b integer, "
                              "area integer)");


    QSqlQuery frame_bd_query(QSqlDatabase::database(conn_name));
    ret &= frame_bd_query.exec("create table frame_bd "
                              "(id integer primary key autoincrement, "
                              "frame_nr integer, "
                              "filename varchar(100), "
                              "bd blob, "
                              "mean float, "
                              "var float, "
                              "max_dist float)");

    QSqlQuery frame_ns_query(QSqlDatabase::database(conn_name));
    ret &= frame_ns_query.exec("create table frame_ns "
                               "(id integer primary key autoincrement, "
                               "frame_nr integer, "
                               "nodesig_id integer, "
                               "center_x integer, "
                               "center_y integer, "
                               "color_r integer, "
                               "color_g integer, "
                               "color_b integer, "
                               "area integer)");

    return ret;

}

bool DatabaseHandler::insertFrame(FrameDesc frame_desc)
{
    bool ret = true;
    if(db.isOpen())
    {
        QSqlQuery query(QSqlDatabase::database(conn_name));

        query.prepare(QString("replace into frame_bd (frame_nr, filename, bd, mean, var, max_dist) values(?, ?, ?, ?, ?, ?)"));

        QByteArray bd_ = mat2ByteArray(frame_desc.bd);

        query.bindValue("frame_nr", frame_desc.frame_nr);
        query.bindValue("filename", QString(frame_desc.filename.c_str()));
        query.bindValue("bd", bd_);
        query.bindValue("mean", frame_desc.bs.mean);
        query.bindValue("var", frame_desc.bs.variance);
        query.bindValue("max_dist", frame_desc.bs.maxDist);

        ret &= query.exec();

        for(int i = 0; i < frame_desc.ns.size(); i++)
        {
            QSqlQuery ns_query(QSqlDatabase::database(conn_name));

            ns_query.prepare(QString("replace into frame_ns (frame_nr , nodesig_id, center_x, center_y, color_r, color_g, color_b, area) values(?, ?, ?, ?, ?, ?, ?, ?)"));

            ns_query.addBindValue(frame_desc.frame_nr);
            ns_query.addBindValue(frame_desc.ns[i].id);
            ns_query.addBindValue(frame_desc.ns[i].center.x);
            ns_query.addBindValue(frame_desc.ns[i].center.y);
            ns_query.addBindValue(frame_desc.ns[i].colorR);
            ns_query.addBindValue(frame_desc.ns[i].colorG);
            ns_query.addBindValue(frame_desc.ns[i].colorB);
            ns_query.addBindValue(frame_desc.ns[i].area);

            ret &= ns_query.exec();

        }
    }

    return ret;
}

FrameDesc DatabaseHandler::getFrame(int frame_nr)
{
    FrameDesc frame_desc;
    if(db.isOpen())
    {
        QSqlQuery query(QSqlDatabase::database(conn_name));
        query.prepare(QString("select * from frame_bd where frame_nr = :frame_nr"));
        query.bindValue(":frame_nr", frame_nr);
        query.exec();
        while(query.next())
        {
            frame_desc.frame_nr = query.value(1).toInt();
            frame_desc.filename = query.value(2).toString().toStdString();
            frame_desc.bd = byteArray2Mat(query.value(3).toByteArray());
            frame_desc.bs.mean = query.value(4).toFloat();
            frame_desc.bs.variance = query.value(5).toFloat();
            frame_desc.bs.maxDist = query.value(6).toFloat();

            QSqlQuery query(QSqlDatabase::database(conn_name));
            query.prepare(QString("select * from frame_ns where frame_nr = :frame_nr"));
            query.bindValue(":frame_nr", frame_nr);
            query.exec();
            vector<NodeSig> ns_all;
            while(query.next())
            {
                NodeSig ns;
                ns.id = query.value(2).toInt();
                Point center(query.value(3).toInt(), query.value(4).toInt());
                ns.center = center;
                ns.colorR = query.value(5).toInt();
                ns.colorG = query.value(6).toInt();
                ns.colorB = query.value(7).toInt();
                ns.area = query.value(8).toInt();
                ns_all.push_back(ns);
            }
            frame_desc.ns = ns_all;
        }
    }

    return frame_desc;
}

bool DatabaseHandler::insertSSG(SSG& ssg)
{
    if(db.isOpen())
    {
        QSqlQuery query(QSqlDatabase::database(conn_name));

        query.prepare(QString("replace into ssg (ssg_id, start, end, sample_frame, coord_x, coord_y, color, mean_inv, member_inv) values(?, ?, ?, ?, ?, ?, ?, ?, ?)"));

        int ssg_id = ssg.getId();
        int start = ssg.getStartFrame();
        int end = ssg.getEndFrame();
        string sample_frame = ssg.getSampleFrame();
        float coord_x = ssg.getSampleCoord().x;
        float coord_y = ssg.getSampleCoord().y;
        int color = ssg.getColor();
        QByteArray mean_inv = mat2ByteArray(ssg.mean_invariant);
        QByteArray member_inv = mat2ByteArray(ssg.member_invariants);

        query.addBindValue(ssg_id);
        query.addBindValue(start);
        query.addBindValue(end);
        query.addBindValue(QString(sample_frame.c_str()));
        query.addBindValue(coord_x);
        query.addBindValue(coord_y);
        query.addBindValue(color);
        query.addBindValue(mean_inv);
        query.addBindValue(member_inv);

        bool ret = query.exec();

        int last_insert_id = query.lastInsertId().toInt();

        for(int i = 0; i < ssg.nodes.size(); i++)
        {
            int count = ssg.nodes[i].second;
            NodeSig ns = ssg.nodes[i].first;

            QSqlQuery ns_query(QSqlDatabase::database(conn_name));

            ns_query.prepare(QString("replace into nodesig (ssg_id , nodesig_id, count, center_x, center_y, color_r, color_g, color_b, area) values(?, ?, ?, ?, ?, ?, ?, ?, ?)"));

            ns_query.addBindValue(last_insert_id);
            ns_query.addBindValue(ns.id);
            ns_query.addBindValue(count);
            ns_query.addBindValue(ns.center.x);
            ns_query.addBindValue(ns.center.y);
            ns_query.addBindValue(ns.colorR);
            ns_query.addBindValue(ns.colorG);
            ns_query.addBindValue(ns.colorB);
            ns_query.addBindValue(ns.area);

            ns_query.exec();

        }

        return ret;
    }

    return false;
}

vector<SSG> DatabaseHandler::getAllSSGs()
{
    vector<SSG> SSGs;

    if(db.isOpen())
    {
        QSqlQuery query(QString("select* from ssg"), QSqlDatabase::database(conn_name));

        while(query.next())
        {
            int ssg_id = query.value(0).toInt();
            int color = query.value(1).toInt();
            QByteArray mean_inv = query.value(2).toByteArray();
            QByteArray member_inv = query.value(3).toByteArray();

            SSG ssg(ssg_id);
            ssg.setColor(color);
            ssg.mean_invariant = DatabaseHandler::byteArray2Mat(mean_inv);
            ssg.member_invariants = DatabaseHandler::byteArray2Mat(member_inv);

            SSGs.push_back(ssg);
        }
    }

    return SSGs;
}

vector<SSG> DatabaseHandler::getAllSSGsNew()
{
    vector<SSG> SSGs;

    if(db.isOpen())
    {
        QSqlQuery query(QString("select* from ssg"), QSqlDatabase::database(conn_name));

        while(query.next())
        {
            int ssg_uniq_id = query.value(0).toInt();
            int ssg_id = query.value(1).toInt();
            int start  = query.value(2).toInt();
            int end = query.value(3).toInt();
            string sample_frame = query.value(4).toString().toStdString();
            float coord_x = query.value(5).toFloat();
            float coord_y = query.value(6).toFloat();
            int color = query.value(7).toInt();
            QByteArray mean_inv = query.value(8).toByteArray();
            QByteArray member_inv = query.value(9).toByteArray();

            SSG ssg(ssg_id);
            ssg.setStartFrame(start);
            ssg.setEndFrame(end);
            ssg.setSampleFrame(sample_frame);
            ssg.setSampleCoord(Point2f(coord_x, coord_y));
            ssg.setColor(color);
            ssg.mean_invariant = DatabaseHandler::byteArray2Mat(mean_inv);
            ssg.member_invariants = DatabaseHandler::byteArray2Mat(member_inv);

            vector<pair<NodeSig,int> > nodesigs = getNodeSigs(ssg_uniq_id);
            ssg.nodes = nodesigs;

            SSGs.push_back(ssg);
        }
    }

    return SSGs;
}

vector<pair<NodeSig,int> > DatabaseHandler::getNodeSigs(int ssg_id)
{
    vector<pair<NodeSig, int> > nodesigs;

    if(db.isOpen())
    {
        //qDebug() << ssg_id;
        QSqlQuery query(QSqlDatabase::database(conn_name));
        query.prepare(QString("select* from nodesig where ssg_id = :ssg_id"));
        query.bindValue(":ssg_id",ssg_id);
        query.exec();

        while(query.next())
        {
            NodeSig nodesig;
            nodesig.id = query.value(2).toInt();
            nodesig.center = Point(query.value(4).toInt(), query.value(5).toInt());
            nodesig.colorR = query.value(6).toInt();
            nodesig.colorG = query.value(7).toInt();
            nodesig.colorB = query.value(8).toInt();
            nodesig.area = query.value(9).toInt();

            nodesigs.push_back(make_pair(nodesig, query.value(3).toInt()));
        }
    }

    return nodesigs;
}

bool DatabaseHandler::isOpen()
{
    return db.isOpen();
}
void DatabaseHandler::closeDB()
{
    if(db.isOpen()) db.close();
}

QByteArray DatabaseHandler::mat2ByteArray(const cv::Mat &image)
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

cv::Mat DatabaseHandler::byteArray2Mat(const QByteArray & byteArray)
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
