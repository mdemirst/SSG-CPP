#include "segmentation.h"
#include "utils.h"


Segmentation::Segmentation(SegmentationParams* params)
{
    this->params = params;
}

Segmentation::Segmentation(SegmentationParams* params, Mat dict)
{
    this->params = params;
    this->dict = dict;
}

vector<NodeSig> Segmentation::segmentImage(const Mat &img, Mat &img_seg)
{

    int width = img.cols;
    int height = img.rows;

    //Convert "Mat" to "image"
    image<rgb> *img_ = new image<rgb>(width,height);
    memcpy((char *)imPtr(img_, 0, 0),img.data,width * height * sizeof(rgb));

    //Segment image
    int nr_segments;
    pair<image<rgb>*, universe*> segments;

    segments = segment_image(img_, params->sigma, params->k, params->min_size, &nr_segments);

    //Calculate statistics of segments
    vector<BlobStats> blobs = calcBlobStats(img, segments.second);

    //free unused segment variables
    delete segments.first;
    delete segments.second;
    delete img_;

    //Construct node signatures from statistics
    vector<NodeSig> node_signatures;
    node_signatures = constructSegmentsGraph(img, blobs);

    // Save segmented image
    img_seg = drawBlobs(img, blobs);
    //img_seg = convert2Mat(segments.first);
    //imwrite("output.jpg", img_seg);
    img_seg = blendImages(img, img_seg, 0.0);

    return node_signatures;
}

vector<Mat> Segmentation::segmentImage(const Mat &img)
{

    int width = img.cols;
    int height = img.rows;

    //Convert "Mat" to "image"
    image<rgb> *img_ = new image<rgb>(width,height);
    memcpy((char *)imPtr(img_, 0, 0),img.data,width * height * sizeof(rgb));

    //Segment image
    int nr_segments;
    pair<image<rgb>*, universe*> segments;
    segments = segment_image(img_, this->params->sigma, this->params->k, this->params->min_size, &nr_segments);

    //Calculate statistics of segments
    vector<BlobStats> blobs = calcBlobStats(img, segments.second);

    vector<Mat> segment_images;
    for(int i = 0; i < blobs.size(); i++)
        segment_images.push_back(drawBlobs(img,blobs,i));

    return segment_images;
}

void Segmentation::getSegmentByIds(const Mat &img, Mat &img_seg, vector<int> ids)
{
    img_seg = Mat::zeros(img.size(), CV_8UC3);
    if(ids.size() == 0)
    {
        return;
    }

    int width = img.cols;
    int height = img.rows;

    //Convert "Mat" to "image"
    image<rgb> *img_ = new image<rgb>(width,height);
    memcpy((char *)imPtr(img_, 0, 0),img.data,width * height * sizeof(rgb));

    //Segment image
    int nr_segments;
    pair<image<rgb>*, universe*> segments;
    segments = segment_image(img_, this->params->sigma, this->params->k, this->params->min_size, &nr_segments);

    //Calculate statistics of segments
    vector<BlobStats> blobs = calcBlobStats(img, segments.second);

    //Construct node signatures from statistics
    vector<NodeSig> node_signatures;
    node_signatures = constructSegmentsGraph(img, blobs);

    for(int i = 0; i < ids.size(); i++)
    {
        if(ids[i] >= 0 && ids[i] < blobs.size())
        {
            // Save segmented image
            img_seg += drawBlobs(img, blobs, ids[i]);
            //img_seg = convert2Mat(segments.first);
            //imwrite("output.jpg", img_seg);
        }
    }
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
        newNode.bow_hist = blobs[i].bow_hist;

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

#ifdef BOW_APPROACH_USED
    Ptr<DescriptorExtractor> desc_extractor;
    #if BOW_DESC_TYPE == SIFT
        desc_extractor = DescriptorExtractor::create("SIFT");
    #elif BOW_DESC_TYPE == SURF
        desc_extractor = DescriptorExtractor::create("SURF");
    #elif BOW_DESC_TYPE == MSER
        desc_extractor = DescriptorExtractor::create("MSER");
    #endif

    Ptr<DescriptorMatcher> desc_matcher = DescriptorMatcher::create(BOW_MATCHER_TYPE);

    BOW_DESC_TYPE feature_detector;

    BOWImgDescriptorExtractor bow_desc_extractor(desc_extractor, desc_matcher);
    bow_desc_extractor.setVocabulary(this->dict);


    for(int i = 0; i < blobs.size(); i++)
    {
        Mat blob_img = drawBlobs(img, blobs, i);


        Mat mask_img;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        cvtColor(blob_img, blob_img, COLOR_BGR2GRAY);
        threshold(blob_img, mask_img, 1, 255, CV_THRESH_BINARY);
        findContours(mask_img.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        Rect rect = boundingRect(contours[0]);

        blob_img = img(rect);
        blobs[i].img = blob_img;

        //Calculate BOW descriptor
        vector<KeyPoint> keypoints;
        Mat hist;
        feature_detector.detect(blob_img, keypoints);
        bow_desc_extractor.compute(blob_img, keypoints, hist);



        if(keypoints.size() == 0)
            hist = Mat::zeros(1,BOW_DICT_SIZE,CV_32F);

        //qDebug() << blob_img.size().width << blob_img.size().height << keypoints.size() << hist.size().width << hist.size().height;
        blobs[i].bow_hist = hist;

        //cv::imshow("ds",blob_img);
        //cv::waitKey(0);
    }
#endif
    //drawBlobs(img, blobsClus); //Draws clustered blobs

    return blobs;
}

Mat Segmentation::drawBlobs(Mat img, vector<BlobStats> blobs, int id)
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

    // If no blob is is specified or id exceed total number of blobs
    // then draw all blobs
    if(id < 0 || id > blobs.size())
    {
        for (int i = 0; i < blobs.size(); i++)
        {
            for (int j = 0; j < blobs[i].pixels.size(); j++)
            {
                imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[0] = (int)blobs[i].avgB;
                imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[1] = (int)blobs[i].avgG;
                imgSeg.at<Vec3b>(blobs[i].pixels[j]).val[2] = (int)blobs[i].avgR;
            }
        }
    }
    //Draw only one segment
    else
    {
        for (int j = 0; j < blobs[id].pixels.size(); j++)
        {
            imgSeg.at<Vec3b>(blobs[id].pixels[j]) = img.at<Vec3b>(blobs[id].pixels[j]);
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
