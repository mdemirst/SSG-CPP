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


