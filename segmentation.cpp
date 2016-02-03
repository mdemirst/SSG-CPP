#include "segmentation.h"
#include "utils.h"

Segmentation::Segmentation(QObject *parent) :
    QObject(parent)
{
    //Set segmentation parameters to predefined values
    //These can be changed by calling setSegmentationParameters() method
    sigma = SEG_SIGMA;
    k = SEG_K;
    min_size = SEG_MIN_SIZE;
    scale = SEG_SCALE;
}

void Segmentation::setSegmentationParameters(float sigma, float k, float min_size, float scale)
{
    this->sigma = sigma;
    this->k = k;
    this->min_size = min_size;
    this->scale = scale;
}

void Segmentation::setParSigma(float sigma)
{
    this->sigma = sigma;
}

void Segmentation::setParK(float k)
{
    this->k = k;
}

void Segmentation::setParMinSize(float min_size)
{
    this->min_size = min_size;
}

void Segmentation::setParScale(float scale)
{
    this->scale = scale;
}


float Segmentation::getParSigma()
{
    return this->sigma;
}

float Segmentation::getParK()
{
    return this->k;
}

float Segmentation::getParMinSize()
{
    return this->min_size;
}



vector<NodeSig> Segmentation::segmentImage(const Mat &img_org, Mat &img_seg)
{
    Mat img;
    //If desired, scale it down for performance purposes
    resize(img_org,img,cv::Size(0,0),this->scale,this->scale);

    int width = img.cols;
    int height = img.rows;

    //Convert "Mat" to "image"
    image<rgb> *img_ = new image<rgb>(width,height);
    memcpy((char *)imPtr(img_, 0, 0),img.data,width * height * sizeof(rgb));

    //Segment image
    int nr_segments;
    pair<image<rgb>*, universe*> segments;
    segments = segment_image(img_, this->sigma, this->k, this->min_size, &nr_segments);

    //Calculate statistics of segments
    vector<BlobStats> blobs = calcBlobStats(img, segments.second);

    //Construct node signatures from statistics
    vector<NodeSig> node_signatures;
    node_signatures = constructSegmentsGraph(img, blobs);

    // Save segmented image
    img_seg = drawBlobs(img, blobs);
    //img_seg = convert2Mat(segments.first);
    imwrite("output.jpg", img_seg);

    return node_signatures;
}

//Conversion from "image" to "Mat" structure
template <class T>
Mat Segmentation::convert2Mat(image<T>* img)
{
    Mat imgConverted = Mat::zeros(img->height(), img->width(), CV_8UC3);;

    for (int i = 0; i < img->height(); i++)
    {
        for (int j = 0; j < img->width(); j++)
        {
            imgConverted.at<Vec3b>(i, j)[0] = imRef(img, j, i).b;
            imgConverted.at<Vec3b>(i, j)[1] = imRef(img, j, i).g;
            imgConverted.at<Vec3b>(i, j)[2] = imRef(img, j, i).r;
        }
    }

    return imgConverted;
}

//Diffuse two images
Mat Segmentation::blendImages(Mat img1, Mat img2, float alpha)
{
    float beta = 1 - alpha;
    Mat img_blended;

    addWeighted(img1, alpha, img2, beta, 0.0, img_blended);

    return img_blended;
}

//Construct node signatures from segments statistics
vector<NodeSig> Segmentation::constructSegmentsGraph(Mat img, vector<BlobStats> blobs)
{
    vector<NodeSig> nodeSigs;

    //Construct a node signature for each node
    for (int i = 0; i < blobs.size(); i++)
    {
        NodeSig newNode;
        newNode.id = i+1;
        newNode.colorR = blobs[i].avgR;
        newNode.colorG = blobs[i].avgG;
        newNode.colorB = blobs[i].avgB;
        newNode.center = Point(blobs[i].centerX, blobs[i].centerY);
        newNode.area   = blobs[i].pixelsSize;

        //Find edge attributes for each node
        //Find edges by applying
        //morphological operations. Dilated segments will have
        //overlapping areas if there is a common boundary between
        //two segments.
        for (int j = 0; j < blobs.size(); j++)
        {
            if (i != j)
            {
                Mat img1 = Mat::zeros(img.size(), CV_8UC1);
                Mat img2 = Mat::zeros(img.size(), CV_8UC1);
                Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

                //Dilate ith and jth segments
                for (int i_ = 0; i_ < blobs[i].pixels.size(); i_++)
                {
                    img1.at<uchar>(blobs[i].pixels[i_]) = 255;
                }
                dilate(img1, img1, element);

                for (int i_ = 0; i_ < blobs[j].pixels.size(); i_++)
                {
                    img2.at<uchar>(blobs[j].pixels[i_]) = 255;
                }
                dilate(img2, img2, element);

                //bitwise and two dilated segments
                Mat img3;
                bitwise_and(img1, img2, img3);

                double minVal;
                double maxVal;
                Point minLoc;
                Point maxLoc;

                //if there is at least one 255 value that means
                //two dilated segments overlap
                minMaxLoc(img3, &minVal, &maxVal, &minLoc, &maxLoc);
                if (maxVal == 255)
                {
                    pair<int, float> edge;
                    edge.first = j+1;

                    //For edge attribution different approaches can be used

                    //Pairwise RGB difference
                    /*edge.second = (fabs(blobs[i].avgR - blobs[j].avgR) +
                    fabs(blobs[i].avgG - blobs[j].avgG) +
                    fabs(blobs[i].avgB - blobs[j].avgB)) / 3.0;*/

                    //Segment centroids difference
                    edge.second = sqrt((blobs[i].centerX - blobs[j].centerX)*(blobs[i].centerX - blobs[j].centerX) +
                                       (blobs[i].centerY - blobs[j].centerY)*(blobs[i].centerY - blobs[j].centerY));
                    newNode.edges.push_back(edge);
                }
            }
        }
        //Add new node signature to vector
        nodeSigs.push_back(newNode);
    }

    return nodeSigs;
}

//Finds blobs directly from segments datastruct
vector<BlobStats> Segmentation::calcBlobStats(Mat img, universe* segments)
{
    int w = img.size().width;
    int h = img.size().height;

    Mat imgHsv;
    cvtColor(img, imgHsv, CV_BGR2HSV);

    vector<BlobStats> blobs(segments->num_sets());

    vector<int> rootPixels;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int comp = segments->find(y * w + x);

            int blobIdx = -1;
            for (int i = 0; i < rootPixels.size(); i++)
            {
                if (rootPixels[i] == comp)
                blobIdx = i;
            }
            if (blobIdx == -1)
            {
                blobIdx = rootPixels.size();
                rootPixels.push_back(comp);
            }

            Vec3b pixBGR = img.at<Vec3b>(y, x);
            Vec3b pixHSV = imgHsv.at<Vec3b>(y, x);

            //Set blob properties/attributes
            blobs[blobIdx].avgHue = (pixHSV.val[0] + blobs[blobIdx].avgHue*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].avgSat = 125;// (pixHSV.val[1] + blobs[blobIdx].avgSat*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].avgVal = 125;// (pixHSV.val[2] + blobs[blobIdx].avgVal*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].avgB = (pixBGR.val[0] + blobs[blobIdx].avgB*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].avgG = (pixBGR.val[1] + blobs[blobIdx].avgG*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].avgR = (pixBGR.val[2] + blobs[blobIdx].avgR*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].centerX = (x + blobs[blobIdx].centerX*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].centerY = (y + blobs[blobIdx].centerY*blobs[blobIdx].pixelsSize) / (float)(blobs[blobIdx].pixelsSize + 1);
            blobs[blobIdx].pixels.push_back(Point(x, y));
            blobs[blobIdx].pixelsSize++;
        }
    }

    //drawBlobs(img, blobsClus); //Draws clustered blobs

    return blobs;
}

Mat Segmentation::drawBlobs(Mat img, vector<BlobStats> blobs)
{
    Mat imgSeg = Mat::zeros(img.size(), CV_8UC3);

    /*HUE Drawing*/
    /*
    for (int i = 0; i < blobs.size(); i++)
    {
        for (int j = 0; j < blobs[i].pixels.size(); j++)
        {
            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[0] = (int)blobs[i].avgHue;
            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[1] = (int)150;
            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[2] = (int)150;
        }
    }

    vector<Mat> hsv_planes(3);
    split(imgSeg, hsv_planes);
    hsv_planes[0]; // H channel
    hsv_planes[1]; // S channel
    hsv_planes[2]; // V channel

    cvtColor(imgSeg, imgSeg, CV_HSV2BGR);
    imshow("Blobs", imgSeg);
    waitKey(0);*/

    /*RGB Drawing*/

    for (int i = 0; i < blobs.size(); i++)
    {
        for (int j = 0; j < blobs[i].pixels.size(); j++)
        {
            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[0] = (int)blobs[i].avgB;
            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[1] = (int)blobs[i].avgG;
            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[2] = (int)blobs[i].avgR;
        }
    }

    /*Random RGB Drawing*/
//    for (int i = 0; i < blobs.size(); i++)
//    {
//        int R = (int)rand()*255;
//        int G = (int)rand()*255;
//        int B = (int)rand()*255;

//        for (int j = 0; j < blobs[i].pixels.size(); j++)
//        {
//            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[0] = R;
//            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[1] = G;
//            imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[2] = B;
//        }
//    }

    //imshow("Blobs", imgSeg);
    //waitKey(0);

    return imgSeg;
}
