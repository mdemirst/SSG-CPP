#include "tsc.h"
#include <math.h>
#include "defs.h"
#include "utils.h"



TSC::TSC(Dataset* dataset)
{
    std::vector<std::string> img_files = getFiles(dataset->location);
    Mat img = imread(dataset->location + img_files[dataset->start_idx]);
    int img_height = img.size().height;
    int img_width = img.size().width;

    //deneme
//    detector.tau_w = 3;//20;//5;
//    detector.tau_n = 3;//15;
//    detector.tau_p = 10;//3//20;
//    detector.tau_avgdiff = 0.4;//0.00;//0.15;//0.4;
//    detector.focalLengthPixels = 550;//350;//550;
//    detector.satLower = 10;
//    detector.satUpper = 255;
//    detector.valLower = 10;
//    detector.valUpper = 255;
//    detector.noHarmonics = 10;
//    detector.image_counter = dataset->start_idx;
//    detector.shouldProcess = false;
//    detector.debugMode = true;
//    detector.debugFilePath = "/home/isl-mahmut/Datasets/New-College/";
//    detector.debugFilePrefix = "rgb_";
//    detector.debugFileNo = dataset->end_idx-dataset->start_idx;
//    detector.usePreviousMemory = false;
//    detector.previousMemoryPath = "";
//    detector.dbmanager = &dbmanager;


    //Benim degerler
    detector.tau_w = 2;//20;//5;
    detector.tau_n = 20;//15;
    detector.tau_p = 5;//3//20;
    detector.tau_avgdiff = 0.15;//0.00;//0.15;//0.4;
    detector.focalLengthPixels = 550;//350;//550;
    detector.satLower = 10;
    detector.satUpper = 255;
    detector.valLower = 10;
    detector.valUpper = 255;
    detector.noHarmonics = 10;
    detector.image_counter = dataset->start_idx;
    detector.shouldProcess = false;
    detector.debugMode = true;
    detector.debugFilePath = "/home/isl-mahmut/Datasets/New-College/";
    detector.debugFilePrefix = "rgb_";
    detector.debugFileNo = dataset->end_idx-dataset->start_idx;
    detector.usePreviousMemory = false;
    detector.previousMemoryPath = "";
    detector.dbmanager = &dbmanager;

    //hakanin
//    detector.tau_w = 5;//20;//5;
//    detector.tau_n = 10;//15;
//    detector.tau_p = 20;//5;//3//20;
//    detector.tau_avgdiff = 0.4;//0.15;//0.00;//0.15;//0.4;
//    detector.focalLengthPixels = 550;//350;//550;
//    detector.satLower = 10;
//    detector.satUpper = 255;
//    detector.valLower = 10;
//    detector.valUpper = 255;
//    detector.noHarmonics = 10;
//    detector.image_counter = dataset->start_idx;
//    detector.shouldProcess = false;
//    detector.debugMode = true;
//    detector.debugFilePath = "/home/isl-mahmut/Datasets/New-College/";
//    detector.debugFilePrefix = "rgb_";
//    detector.debugFileNo = dataset->end_idx-dataset->start_idx;
//    detector.usePreviousMemory = false;
//    detector.previousMemoryPath = "";
//    detector.dbmanager = &dbmanager;


    detector.tau_inv = 0.4;//0.4;//0.1;//0.4;        //Incoherency threshold
    detector.tau_val_mean = 0.1;//0.2;//0.2;   //tau_mu
    detector.tau_val_var = 0.003;//0.01;//0.01;    //tau_sigma

    // This should be done before starting the process
    bubbleProcess::calculateImagePanAngles(detector.focalLengthPixels,img_width,img_height);
    bubbleProcess::calculateImageTiltAngles(detector.focalLengthPixels,img_width,img_height);

    QString basepath = QDir::homePath().append("/TSC");
    basepath.append("/visual_filters");

    QString path(basepath);

    path.append("/filtre0.txt");
    ImageProcess::readFilter(path,29,false,false,false);
    path.clear();
    path = basepath;

    path.append("/filtre6.txt");
    ImageProcess::readFilter(path,29,false,false,false);
    path.clear();
    path = basepath;

    path.append("/filtre12.txt");
    ImageProcess::readFilter(path,29,false,false,false);
    path.clear();
    path = basepath;

    path.append("/filtre18.txt");
    ImageProcess::readFilter(path,29,false,false,false);
    path.clear();
    path = basepath;

    path.append("/filtre36.txt");
    ImageProcess::readFilter(path,29,false,false,false);
    createDirectories("");
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

float TSC::processImage(const Mat& cur_img, bool isLastImage)
{
    detector.currentImage =  cur_img;

    float score = detector.processImage();

    if(isLastImage)
    {
        if(detector.currentPlace && detector.currentPlace->id > 0 && detector.currentPlace->members.size() > 0)
        {
            detector.currentPlace->calculateMeanInvariant();

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

    return score;
}
