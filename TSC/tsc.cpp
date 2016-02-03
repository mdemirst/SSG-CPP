#include "tsc.h"

TSC::TSC(QObject *parent) :
    QObject(parent)
{
}

bool TSC::createDirectories(QString previousMemoryPath)
{
    QDir dir(QDir::home());

    QString mainDirectoryName = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss");

    if(!dir.mkdir(mainDirectoryName)) return false;

    dir.cd(mainDirectoryName);

    mainDirectoryPath = dir.path();
    qDebug() << "Main Directory Path" << mainDirectoryPath;


    QDir mainDir(QDir::homePath().append("/").append(mainDirectoryName));

    QString imageDirectory = "images";

    if(!mainDir.mkdir(imageDirectory)) return false;

    mainDir.cd(imageDirectory);

    imagesPath = mainDir.path();
    detector.imagesPath = imagesPath;

    qDebug() << "Image directory path" << imagesPath;


    QString databasepath = QDir::homePath();

    databasepath.append("/emptydb");

    QString detecplacesdbpath = databasepath;
    detecplacesdbpath.append("/detected_places.db");

    QFile file(detecplacesdbpath);

    if(file.exists())
    {
        QString newdir = mainDirectoryPath;
        newdir.append("/detected_places.db");
        QFile::copy(detecplacesdbpath,newdir);

        if(!dbmanager.openDB(newdir))
            return false;
    }
    else
        return false;

    // If we don't have a previous memory than create an empty memory
    if(previousMemoryPath.size() <= 1 || previousMemoryPath.isNull())
    {
        QString knowledgedbpath = databasepath;
        knowledgedbpath.append("/knowledge.db");

        QFile file2(knowledgedbpath);

        if(file2.exists())
        {
            QString newdir = mainDirectoryPath;
            QFile::copy(knowledgedbpath,newdir.append("/knowledge.db"));
        }
        else
            return false;

    }
    // If we have supplied a previous memory path, then open that db
    else
    {
        QString knowledgedbpath = previousMemoryPath;
        knowledgedbpath.append("/knowledge.db");

        QFile file2(knowledgedbpath);

        if(file2.exists())
        {
            QString newdir = mainDirectoryPath;
            QFile::copy(knowledgedbpath,newdir.append("/knowledge.db"));
        }
        else
            return false;
    }
    return true;
}

bool TSC::saveParameters(QString filepath)
{
    QString fullpath = filepath;

    fullpath.append("/PDparams.txt");

    QFile file(fullpath);

    if(file.open(QFile::WriteOnly))
    {
        QTextStream str(&file);

        str<<"tau_w "<<detector.tau_w<<"\n";
        str<<"tau_n "<<detector.tau_n<<"\n";
        str<<"tau_p "<<detector.tau_p<<"\n";
        str<<"tau_inv "<<detector.tau_inv<<"\n";
        str<<"tau_avgdiff "<<detector.tau_avgdiff<<"\n";
        str<<"focal_length_pixels "<<detector.focalLengthPixels<<"\n";
        str<<"tau_val_mean "<<detector.tau_val_mean<<"\n";
        str<<"tau_val_var "<<detector.tau_val_var<<"\n";
        str<<"sat_lower "<<detector.tau_avgdiff<<"\n";
        str<<"sat_upper "<<detector.focalLengthPixels<<"\n";
        str<<"val_lower "<<detector.tau_val_mean<<"\n";
        str<<"val_upper "<<detector.tau_val_var<<"\n";
        str<<"debug_mode "<<detector.debugMode<<"\n";
        str<<"debug_filePath "<<QString::fromStdString(detector.debugFilePath)<<"\n";
        str<<"debug_fileNo "<<detector.debugFileNo<<"\n";

        file.close();
    }
    else
    {
        qDebug()<<"Param File could not be opened for writing!!";
        return false;
    }

    return true;
}

double TSC::compareHistHK( InputArray _H1, InputArray _H2, int method )
{
    Mat H1 = _H1.getMat(), H2 = _H2.getMat();
    const Mat* arrays[] = {&H1, &H2, 0};
    Mat planes[2];
    NAryMatIterator it(arrays, planes);
    double result = 0;
    int j, len = (int)it.size;

    CV_Assert( H1.type() == H2.type() && H1.depth() == CV_32F );

    CV_Assert( it.planes[0].isContinuous() && it.planes[1].isContinuous() );

    for( size_t i = 0; i < it.nplanes; i++, ++it )
    {
        const float* h1 = (const float*)it.planes[0].data;
        const float* h2 = (const float*)it.planes[1].data;
        len = it.planes[0].rows*it.planes[0].cols*H1.channels();


        for( j = 0; j < len; j++ )
        {
            double a = h1[j] - h2[j];
            double b =  h1[j] + h2[j];
            if( fabs(b) > DBL_EPSILON )
                result += a*a/b;
        }

    }

    return result;

}

void TSC::writeInvariant(cv::Mat inv, int count)
{
    QString pathh = QDir::homePath();
    pathh.append("/invariants_").append(QString::number(count)).append(".txt");
    QFile file(pathh);

    if(file.open(QFile::WriteOnly))
    {
        QTextStream str(&file);

        for(int i = 0; i < inv.rows; i++)
        {
            str<<inv.at<float>(i,0)<<"\n";
        }
        file.close();
    }
}

void TSC::processImages()
{
    int img_width = 640;
    int img_height = 480;

    detector.tau_w = 3;
    detector.tau_n = 3;
    detector.tau_p = 10;
    detector.tau_avgdiff = 0.4;
    detector.focalLengthPixels = 550;
    detector.satLower = 10;
    detector.satUpper = 255;
    detector.valLower = 10;
    detector.valUpper = 255;
    detector.noHarmonics = 10;
    detector.image_counter = 1;
    detector.shouldProcess = false;
    detector.debugMode = true;
    detector.debugFilePath = "/home/isl-mahmut/Datasets/Cold/";
    detector.debugFileNo = 1500;
    detector.usePreviousMemory = false;
    detector.previousMemoryPath = "";
    detector.dbmanager = &dbmanager;


    detector.tau_inv = 0.2;
    detector.tau_val_mean = 0.1;
    detector.tau_val_var = 0.003;



    /***********************************************************/
    qDebug()<<"Saturation and Value thresholds"<<detector.satLower<<detector.satUpper<<detector.valLower<<detector.valUpper<<detector.tau_avgdiff;

    if(detector.debugMode)
    {
        qDebug()<<"Debug mode is on!! File Path"<<QString::fromStdString(detector.debugFilePath)<<"No of files to be processed"<<detector.debugFileNo;
    }
    /********************************************/

    // This should be done before starting the process
    bubbleProcess::calculateImagePanAngles(detector.focalLengthPixels,img_width,img_height);
    bubbleProcess::calculateImageTiltAngles(detector.focalLengthPixels,img_width,img_height);

    QString basepath = QDir::homePath().append("/TSC");
    basepath.append("/visual_filters");

    QString path(basepath);

    path.append("/filtre0.txt");
    qDebug()<<path;

    ImageProcess::readFilter(path,29,false,false,false);

    path.clear();
    path = basepath;

    path.append("/filtre6.txt");
    qDebug()<<path;

    ImageProcess::readFilter(path,29,false,false,false);

    path.clear();
    path = basepath;

    path.append("/filtre12.txt");
    qDebug()<<path;


    ImageProcess::readFilter(path,29,false,false,false);

    path.clear();
    path = basepath;

    path.append("/filtre18.txt");
    qDebug()<<path;


    ImageProcess::readFilter(path,29,false,false,false);

    path.clear();
    path = basepath;


    path.append("/filtre36.txt");
    qDebug()<<path;

    ImageProcess::readFilter(path,29,false,false,false);

    createDirectories("");
    saveParameters(mainDirectoryPath);


    QFile scores_file( mainDirectoryPath.append("/").append(QString("dissimilarity_scores.txt")) );
    scores_file.open(QIODevice::ReadWrite);
    QTextStream scores_stream( &scores_file );

    for(int i = 1; i <= detector.debugFileNo; i++)
    {
        QString path = QString::fromStdString(detector.debugFilePath);

        path.append("Cold-").append(QString::number(i).rightJustified(4,'0')).append(".jpg");

        Mat imm = imread(path.toStdString().data(),CV_LOAD_IMAGE_COLOR);

        qint64 starttime = QDateTime::currentMSecsSinceEpoch();

        cv::Rect rect(0,0,imm.cols,(imm.rows));

        detector.currentImage = imm(rect);

        scores_stream << detector.processImage() << endl;

        qint64 stoptime = QDateTime::currentMSecsSinceEpoch();

        qDebug()<<(float)(stoptime-starttime);

    }

    scores_file.close();


    if(detector.currentPlace && detector.currentPlace->id > 0 && detector.currentPlace->members.size() > 0)
    {
        detector.currentPlace->calculateMeanInvariant();

        qDebug()<<"Current place mean invariant: "<<detector.currentPlace->meanInvariant.rows<<detector.currentPlace->meanInvariant.cols<<detector.currentPlace->members.size();

        if(detector.currentPlace->memberIds.rows >= detector.tau_p)
        {

            dbmanager.insertPlace(*detector.currentPlace);

            detector.detectedPlaces.push_back(*detector.currentPlace);

            detector.placeID++;
        }

        delete detector.currentPlace;
        detector.currentPlace = 0;
        detector.shouldProcess = false;

    }

    // Delete the current place
    if(detector.currentPlace)
    {
        delete detector.currentPlace;
        detector.currentPlace = 0;
    }

    // Insert basepoints to the database
    if(detector.wholebasepoints.size()>0)
        dbmanager.insertBasePoints(detector.wholebasepoints);

    dbmanager.closeDB();

}
