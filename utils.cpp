#include "utils.h"

//Convert Mat to QImage
QImage mat2QImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

//Convert 2 dim-vector to unsigned char Mat
Mat vec2mat(vector< vector<int> > vec_matrix)
{
    Mat matrix = Mat::zeros(vec_matrix.size(),vec_matrix[0].size(), CV_8U);

    for(int i = 0; i < vec_matrix.size(); i++)
    {
        for(int j = 0; j < vec_matrix[0].size(); j++)
        {
            matrix.at<unsigned char>(i,j) = vec_matrix[i][j];
        }
    }

    return matrix;
}

//Convert 2 dim-vector to integer Mat
Mat vec2matInt(vector< vector<int> > vec_matrix)
{
    Mat matrix = Mat::zeros(vec_matrix.size(),vec_matrix[0].size(), CV_32F);

    for(int i = 0; i < vec_matrix.size(); i++)
    {
        for(int j = 0; j < vec_matrix[0].size(); j++)
        {
            matrix.at<float>(i,j) = vec_matrix[i][j];
        }
    }

    return matrix;
}



//Convert 2 dim array to floating Mat
Mat array2Mat32F(int** C, int rows, int cols){
    Mat matrix = Mat::zeros(rows,cols, CV_32F);
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            matrix.at<float>(i,j) =  C[i][j];
        }
    }
    return matrix;
}

//Convert 2 dim array to unsigned char Mat
Mat array2Mat8U(int** C, int rows, int cols){
    Mat matrix = Mat::zeros(rows,cols, CV_8U);
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            matrix.at<unsigned char>(i,j) =  C[i][j];
        }
    }
    return matrix;
}

//Returns permuted index of j given permutation matrix
//Give column index -> Returns row index
int getPermuted(Mat& P, int c)
{
    for(int r = 0; r < P.col(c).size().height; r++)
    {
        if(P.at<unsigned char>(r,c) == 1)
            return r;
    }
    return -1;
}

//Get index of value at particular colum col
int getIndexByCol(Mat& M, int col, int val)
{
    //size() returns in [col,row] index order

    for(int i = 0; i < M.col(col).size().height; i++)
    {
        if(M.at<int>(i,col) == val)
            return i;
    }
    return -1;
}

// Given frame number returns associated file path of that frame
std::string getFilePath(std::string dir, std::string folder, std::string prefix, int frame_no)
{
    std::stringstream path;

    if(frame_no < 10)
        path << dir << "/" << folder << "/" << prefix << "000" << frame_no << ".jpg";
    else if(frame_no < 100)
        path << dir << "/" << folder << "/" << prefix << "00" << frame_no << ".jpg";
    else if(frame_no < 1000)
        path << dir << "/" << folder << "/" << prefix << "0" << frame_no << ".jpg";
    else
        path << dir << "/" << folder << "/" << prefix << frame_no << ".jpg";

    return path.str();
}

// Scales up the given image by given factor in vertical axis only
// Sample-and-hold interpolation is used
void scaleUpMap(Mat &img, Mat &img_scaled, int factor_x, int factor_y)
{
    img_scaled = Mat::zeros(img.size().height*factor_y, img.size().width*factor_x, CV_8UC3);
    for(int i = 0; i < img.size().height; i++)
    {
        for(int j = 0; j < img.size().width; j++)
        {
            for(int a = 0; a < factor_x; a++)
                for(int b = 0; b < factor_y; b++)
                    img_scaled.at<Vec3b>(i*factor_y+b, j*factor_x+a) = img.at<Vec3b>(i,j);
        }
    }
}

//Gets list of files inside directory
std::vector<std::string> getFiles(std::string dir)
{
    std::vector<std::string> files_list;
    struct dirent **namelist;
    int n;

    n = scandir(dir.c_str(), &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        for(int i = 0; i < n; i++)
        {
            if(namelist[i]->d_name[0] != '.')
                files_list.push_back(namelist[i]->d_name);
        }
    }

    for(int i = 0; i < n; i++){
        free(namelist[i]);
    }
    free(namelist);

    return files_list;
}

cv::Point2f getCoordCold(std::string filename)
{
    cv::Point2f coord;

    int start = filename.find("_x") + 2;
    int end = filename.find("_y");

    std::stringstream ss_x(filename.substr(start,end));
    ss_x >> coord.y;

    start = filename.find("_y") + 2;
    end = filename.find("_a");

    std::stringstream ss_y(filename.substr(start,end));
    ss_y >> coord.x;

    coord.x = -1*coord.x;

    return coord;
}

int getMedian(vector<int> v)
{
    std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
    return v[v.size()/2];
}

// returns true iff all the elements of v are identical
bool getRegionStatus(vector<int> v)
{
    for(int i = 0; i < v.size()-1; i++)
    {
        if(v[i] != v[i+1])
        {
            return false;
        }
    }

    return true;
}

string getOutputFolder(bool reset)
{
    static string str_folder = OUTPUT_FOLDER+QDate::currentDate().toString().toStdString()+"-"+QTime::currentTime().toString().toStdString()+"/";

    if(reset)
        str_folder = OUTPUT_FOLDER+QDate::currentDate().toString().toStdString()+"-"+QTime::currentTime().toString().toStdString()+"/";

    QString path(QString::fromStdString(str_folder));
    QDir dir(path);
    if (!dir.exists()){
      dir.mkpath(path);
    }

    return str_folder;
}

void savePlacesFrameInfo(vector<PlaceSSG>& places)
{

    string filename = getOutputFolder()+"/places-frames.txt";

    ofstream file;
    file.open (filename.c_str());
    for(int i = 0; i < places.size(); i++)
    {
        //qDebug() << places[i].getCount();
        for(int j = 0; j < places[i].getCount(); j++)
        {
            file << "P: " << i << "\t" << "SSG: " << places[i].getMember(j).getId() << "\t" << places[i].getMember(j).getStartFrame() << "-" << places[i].getMember(j).getEndFrame() << "\n";
        }
    }
    file.close();
}


/////////////////////////////////////
////PlaceDistances Class Definition//
/////////////////////////////////////

//PlaceDistances::PlaceDistances(GraphMatch* gm)
//{
//    this->gm = gm;
//    max_id = 0;
//}

////Calculates pairwise distace between new detected place and old detected places
////Stores pairwise distances in the distances map structure.
////This is used in order to prevent to re calculate previously calculated distances at each step
////Only new pairwise distances will be calculated.
////Please note that in a place there may be multiple members. In this case, we will take the average
////of the distance
//void PlaceDistances::updatePlace(PlaceSSG& place, vector<PlaceSSG>& old_places)
//{
//    if(find(place_ids.begin(), place_ids.end(),place.getId()) == place_ids.end())
//    {
//        place_ids.push_back(place.getId());
//        if(place.getId() > max_id)  max_id = place.getId();
//    }

//    for(int i = 0; i < old_places.size(); i++)
//    {
//        setDistance(place, old_places[i], calculateDistance(place, old_places[i]));
//    }
//}


//void PlaceDistances::deletePlace(PlaceSSG& place)
//{
//    int max_id = 0;
//    typedef map<pair<int,int>,double> DistanceMap;

//    for( DistanceMap::const_iterator it = distances.begin(); it != distances.end(); it++ )
//    {
//        pair<int,int> coord = it->first;

//        if(coord.first == place.getId() || coord.second == place.getId())
//        {
//            distances.erase(coord);
//        }
//        else
//        {
//            if(coord.first > max_id)
//                max_id = coord.first;
//            else if(coord.second > max_id)
//                max_id = coord.second;
//        }
//    }

//    this->max_id = max_id;
//}


////Calculate distance between two places.
////Note that one place can have multiple members
//double PlaceDistances::calculateDistance(PlaceSSG& place1, PlaceSSG& place2)
//{
//    double distance = 0;
//    int count = 0;
//    for(int i = 0; i < place1.getCount(); i++)
//    {
//        for(int j = 0; j < place2.getCount(); j++)
//        {
//            Mat P, C;
//            distance += gm->matchTwoImages(place1.getMembers()[i].getRAG(),place2.getMembers()[j].getRAG(),P,C);
//            count++;
//        }
//    }

//    //qDebug() << count;
//    return distance/count;
//}

////Adds new distance value associated with two provided places into distances map structure
//void PlaceDistances::setDistance(PlaceSSG& place1, PlaceSSG& place2, double distance)
//{
//    int id1 = min(place1.getId(), place2.getId());
//    int id2 = max(place1.getId(), place2.getId());

//    distances[make_pair(id1, id2)] = distance;
//}

////Constructs distance matrix in C style
//double** PlaceDistances::getDistanceMatrix()
//{
//    double** dist_matrix = new double*[max_id+1];

//    for (int i = 0; i < max_id+1; i++)
//    {
//        dist_matrix[i] = new double[max_id+1];
//    }

//    typedef map<pair<int,int>,double> DistanceMap;

//    for( DistanceMap::const_iterator it = distances.begin(); it != distances.end(); it++ )
//    {
//        pair<int,int> coord = it->first;
//        double dist = it->second;
//        dist_matrix[coord.first][coord.second] = dist;
//        dist_matrix[coord.second][coord.first] = dist;
//    }

//    return dist_matrix;
//}

//void PlaceDistances::printDistanceMatrix()
//{
//    double** dist_matrix = getDistanceMatrix();

//    setprecision(2);

//    for(int i = 0; i < max_id+1; i++)
//    {
//        for(int j = 0; j < max_id+1; j++)
//        {
//            cout << dist_matrix[i][j] << "\t";
//        }
//        cout << endl;
//    }
//}


////class PlaceDistances
////{
////    Mat distances;
////public:
////    PlaceDistances();
////    void addPlace(const PlaceSSG& place);
////    void addPlaces(const vector<PlaceSSG> places);
////    void updatePlace(const PlaceSSG& place);
////    double getDistance(const PlaceSSG& place1, const PlaceSSG& place2);
////};

void saveParameters(string filename,
                    Parameters* params)
{
    ofstream file;
    file.open (filename.c_str());

    file << "IMG_ORG_W" << "\t" << params->ssg_params.img_org_width << endl;
    file << "IMG_ORG_H" << "\t" << params->ssg_params.img_org_height << endl;
    file << "IMG_W" << "\t" << params->ssg_params.img_width << endl;
    file << "IMG_H" << "\t" << params->ssg_params.img_height << endl;
    file << "TAU_N" << "\t" << params->ssg_params.tau_n << endl;
    file << "TAU_C" << "\t" << params->ssg_params.tau_c << endl;
    file << "TAU_F" << "\t" << params->ssg_params.tau_f << endl;
    file << "TAU_D" << "\t" << params->ssg_params.tau_d << endl;
    file << "TAU_P" << "\t" << params->ssg_params.tau_p << endl;
    file << "COEFF_NODE_DISAPPEAR1" << "\t" << params->ssg_params.coeff_node_disappear1 << endl;
    file << "COEFF_NODE_DISAPPEAR2" << "\t" << params->ssg_params.coeff_node_disappear2 << endl;
    file << "COEFF_NODE_APPEAR" << "\t" << params->ssg_params.coeff_node_appear << endl;
    file << "COEFF_COH_EXP_BASE" << "\t" << params->ssg_params.coeff_coh_exp_base << endl;
    file << "COEFF_COH_APPEAR_THRES" << "\t" << params->ssg_params.coeff_coh_appear_thres << endl;

    file << "TAU_W" << "\t" << params->seg_track_params.tau_w << endl;
    file << "TAU_M" << "\t" << params->seg_track_params.tau_m << endl;

    file << "SIGMA" << "\t" << params->seg_params.sigma << endl;
    file << "K" << "\t" << params->seg_params.k << endl;
    file << "MIN_SIZE" << "\t" << params->seg_params.min_size << endl;

    file << "POS_WEIGHT" << "\t" << params->gm_params.pos_weight << endl;
    file << "COLOR_WEIGHT" << "\t" << params->gm_params.color_weight << endl;
    file << "AREA_WEIGHT" << "\t" << params->gm_params.area_weight << endl;
    file << "BOW_WEIGHT" << "\t" << params->gm_params.bow_weight << endl;

    file << "TAU_R" << "\t" << params->rec_params.tau_r << endl;
    file << "PLOT_H" << "\t" << params->rec_params.plot_h << endl;
    file << "PLOT_W" << "\t" << params->rec_params.plot_w << endl;
    file << "SSG_H" << "\t" << params->rec_params.ssg_h << endl;
    file << "SSG_W" << "\t" << params->rec_params.ssg_w << endl;
    file << "TAU_V" << "\t" << params->rec_params.tau_v << endl;

    file << "TAU_MU" << "\t" << params->tsc_params.tau_mu << endl;
    file << "TAU_SIGMA" << "\t" << params->tsc_params.tau_sigma << endl;

//    file << "DATASET_COUNT" << "\t" << datasets.size() << endl;

//    for(int i = 0; i < datasets.size(); i++)
//    {
//        file << "DATASET_" << i+1 << "_LOC "<< "\t" << datasets[i].location << endl;
//        file << "DATASET_" << i+1 << "_START "<< "\t" << datasets[i].start_idx << endl;
//        file << "DATASET_" << i+1 << "_END "<< "\t" << datasets[i].end_idx << endl;
//    }

    file.close();

}

void readDatasets(string filename,
                  vector<Dataset>& datasets)
{
    ifstream file;
    file.open (filename.c_str());

    string str;
    vector<Dataset> datasets_all;
    int dataset_count;
    file >> str >> dataset_count;
    for(int i = 0; i < dataset_count; i++)
    {
        string location;
        int start_idx, end_idx;
        file >> str >> location;
        file >> str >> start_idx;
        file >> str >> end_idx;
        Dataset new_dataset(location, start_idx, end_idx, i+1);
        datasets_all.push_back(new_dataset);
    }
    int active_dataset_count = 0;
    file >> str >> active_dataset_count;
    file >> str;
    for(int i = 0; i < active_dataset_count; i++)
    {
        int dataset_id;
        file >> dataset_id;
        datasets.push_back(datasets_all[dataset_id-1]);
    }


    file.close();
}

void readParameters(string filename,
                    Parameters* params)
{
    ifstream file;
    file.open (filename.c_str());

    string str;
    file >> str >> params->ssg_params.img_org_width ;
    file >> str >> params->ssg_params.img_org_height ;
    file >> str >> params->ssg_params.img_width ;
    file >> str >> params->ssg_params.img_height ;
    file >> str >> params->ssg_params.tau_n ;
    file >> str >> params->ssg_params.tau_c ;
    file >> str >> params->ssg_params.tau_f ;
    file >> str >> params->ssg_params.tau_d ;
    file >> str >> params->ssg_params.tau_p ;
    file >> str >> params->ssg_params.coeff_node_disappear1 ;
    file >> str >> params->ssg_params.coeff_node_disappear2 ;
    file >> str >> params->ssg_params.coeff_node_appear     ;
    file >> str >> params->ssg_params.coeff_coh_exp_base    ;
    file >> str >> params->ssg_params.coeff_coh_appear_thres;

    file >> str >> params->seg_track_params.tau_w ;
    file >> str >> params->seg_track_params.tau_m ;

    file >> str >> params->seg_params.sigma   ;
    file >> str >> params->seg_params.k       ;
    file >> str >> params->seg_params.min_size;

    file >> str >> params->gm_params.pos_weight  ;
    file >> str >> params->gm_params.color_weight;
    file >> str >> params->gm_params.area_weight ;
    file >> str >> params->gm_params.bow_weight ;

    file >> str >> params->rec_params.tau_r ;
    file >> str >> params->rec_params.plot_h;
    file >> str >> params->rec_params.plot_w;
    file >> str >> params->rec_params.ssg_h ;
    file >> str >> params->rec_params.ssg_w ;
    file >> str >> params->rec_params.tau_v ;

    file >> str >> params->tsc_params.tau_mu ;
    file >> str >> params->tsc_params.tau_sigma ;

//    int dataset_count;
//    file >> str >> dataset_count;
//    for(int i = 0; i < dataset_count; i++)
//    {
//        string location;
//        int start_idx, end_idx;
//        file >> str >> location;
//        file >> str >> start_idx;
//        file >> str >> end_idx;
//        Dataset new_dataset(location, start_idx, end_idx);
//        datasets.push_back(new_dataset);
//    }


    file.close();


}

int getMostCoherentFrame(vector<float> coh_scores, int start_frame, int end_frame, int start_idx)
{
    int most_coh_id = start_frame;
    float most_coh_score = coh_scores[start_frame-start_idx];

    end_frame = (end_frame - start_idx) < coh_scores.size() ? end_frame : (coh_scores.size() + start_idx);
    for(int i = start_frame; i < end_frame; i++)
    {
        if(coh_scores[i-start_idx] > most_coh_score)
        {
            most_coh_score = coh_scores[i-start_idx];
            most_coh_id = i;
        }
    }

    return most_coh_id;
}
