#include "segmenttrack.h"

SegmentTrack::SegmentTrack(QObject *parent) :
    QObject(parent)
{
    //Find image dimensions
    string img_path = getFilePath(DATASET_FOLDER, DATASET_NO, IMG_FILE_PREFIX, START_IDX);
    Mat img = imread(img_path);
    img_height = img.size().height*IMG_RESCALE_RAT;
    img_width = img.size().width*IMG_RESCALE_RAT;

    //Initialize new graph match and segmentation object
    gm = new GraphMatch(0, img_width, img_height);
    seg = new Segmentation();

    // Parameter initialization
    tau_w = INIT_TAU_W;
    tau_m = INIT_TAU_M;
}


void SegmentTrack::drawCursor(Mat& img)
{
    img.at<cv::Vec3b>(cursor)[0] = 0;
    img.at<cv::Vec3b>(cursor)[1] = 0;
    img.at<cv::Vec3b>(cursor)[2] = 255;
}

void SegmentTrack::drawMap()
{
    Mat img, img_scaled;
    // For drawing purposes convert positive values to 255, zero to 0.
    // Resulting drawing will be black and white existence map drawing
    this->M.convertTo(img, CV_8U, 255, 0);

    cvtColor(img,img,CV_GRAY2BGR);

    drawCursor(img);

    scaleUpMap(img, img_scaled, mapScaleFactor, mapScaleFactor);

    emit showTrackMap(mat2QImage(img_scaled));
}

bool SegmentTrack::eventFilter( QObject* watched, QEvent* event ) {
    if ( event->type() == QEvent::MouseButtonPress )
    {
        const QMouseEvent* const me = static_cast<const QMouseEvent*>( event );

        //might want to check the buttons here
        const QPoint p = me->pos();

        this->cursor.x = p.x()/mapScaleFactor;
        this->cursor.y = p.y()/mapScaleFactor;
    }
    else if( event->type() == QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);

        if ( key->key() == Qt::Key_Left )
        {
            this->cursor.x = max(0, this->cursor.x-1);
        }
        else if ( key->key() == Qt::Key_Right )
        {
            this->cursor.x = min(this->M.size().width-1, this->cursor.x+1);
        }
        else if ( key->key() == Qt::Key_Up )
        {
            this->cursor.y = max(0, this->cursor.y-1);
        }
        else if ( key->key() == Qt::Key_Down )
        {
            this->cursor.y = min(this->M.size().height-1, this->cursor.y+1);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    qDebug() << "x:" << this->cursor.x << "y:" << this->cursor.y << "id:" << this->M.at<int>(this->cursor.y, this->cursor.x);

    drawMap();

    int segment_id = this->M.at<int>(this->cursor.y, this->cursor.x);
    string img_path = getFilePath(DATASET_FOLDER, DATASET_NO, IMG_FILE_PREFIX, START_IDX + this->cursor.x);
    Mat img = imread(img_path);
    resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
    Mat img_seg;

    vector<int> ids;
    ids.push_back(segment_id);
    this->seg->getSegmentByIds(img, img_seg, ids);

    emit showTrackSegment(mat2QImage(img_seg));

    return true;
}

void SegmentTrack::processImage(const Mat cur_img, vector<vector<NodeSig> > &ns_vec)
{
    static Mat prev_img, prev_img_seg;
    Mat cur_img_seg;


    //If we are processing the first tau_w images
    if(ns_vec.size() == 0)
    {
        qDebug() << "a";
        vector<NodeSig> ns = seg->segmentImage(cur_img, cur_img_seg);

        ns_vec.push_back(ns);

        //Initialize M matrix (Existence "M"ap)
        M = Mat(ns.size(), 1, CV_32S, -1);

        for(int i = 0; i < ns.size(); i++)
        {
            M.at<int>(i,0) = i; //Nodes are placed in order in the first column of M

            //Initialize avg node signatures list
            pair<NodeSig, int> new_node(ns[i], 1);

            M_ns.push_back(new_node);
        }

        prev_img = cur_img;
        prev_img_seg = cur_img_seg;
    }
    else
    {
        qDebug() << "b";
        ns_vec.push_back(seg->segmentImage(cur_img,cur_img_seg));

        if(ns_vec.size() > tau_w)
            ns_vec.erase(ns_vec.begin());

        //Show original images on the window
        emit showImg1(mat2QImage(prev_img));
        emit showImg2(mat2QImage(cur_img));

        //Drawing purposes only
        gm->drawMatches(ns_vec[ns_vec.size()-2], ns_vec.back(), prev_img_seg, cur_img_seg);

        //Fill node existence map
        fillNodeMap(M, M_ns, ns_vec);

        //Plot connectivity map
        plotMap(M);

        prev_img = cur_img;
        prev_img_seg = cur_img_seg;
    }
}

void SegmentTrack::processImagesOnline()
{
    vector<vector<NodeSig> > ns_vec;  //Stores last tau_w node signatures

    //Process all images
    for(int frame_no = START_IDX; frame_no < END_IDX-1; frame_no++)
    {
        //getFilePath returns the path of the file given frame number
        string img_path = getFilePath(DATASET_FOLDER, DATASET_NO, IMG_FILE_PREFIX, frame_no);

        Mat img = imread(img_path);
        resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        processImage(img, ns_vec);

        Mat img_seg;
        getCoherentSegments(this->M, img, 0.9, img_seg);
        if(img_seg.empty() == 0)
        {
            imshow("Coherent segments", img_seg);
            waitKey(0);
        }
        else
            waitKey(1);
    }
}

bool pairCompare(const std::pair<int, float>& firstElem, const std::pair<int, float>& secondElem) {
  return firstElem.second > secondElem.second;
}

vector<pair<int, float> > SegmentTrack::getCoherentSegments(const Mat map, const Mat img, float thres, Mat& img_seg)
{
    vector<pair<int, float> > coherent_segments;
    if(map.size().width >= tau_w)
    {
        for(int i = 0; i < map.size().height; i++)
        {
            int nr_appear = 0;
            for(int j = map.size().width - tau_w; j < map.size().width; j++)
            {
                if(map.at<int>(i, j) > 0)
                    nr_appear++;
            }

            float score = ((float)nr_appear/tau_w);

            if( score > thres )
            {
                pair<int, float> segment(map.at<int>(i,map.size().width-1), score);
                coherent_segments.push_back(segment);
            }
        }
        std::sort(coherent_segments.begin(), coherent_segments.end(), pairCompare);

        vector<int> ids;
        for(int j = 0; j < coherent_segments.size(); j++)
            ids.push_back(coherent_segments[j].first);
        if(coherent_segments.size() > 0)
        {
            this->seg->getSegmentByIds(img, img_seg, ids);
        }
    }
    return coherent_segments;
}

// Plots node existence map
void SegmentTrack::plotMap(Mat& M)
{
    Mat img;
    // For drawing purposes convert positive values to 255, zero to 0.
    // Resulting drawing will be black and white existence map drawing
    M.convertTo(img, CV_8U, 255, 0);

    if(img.size().height < EXISTENCE_MAP_H)
    {
        copyMakeBorder(img,img,0,EXISTENCE_MAP_H-img.size().height,0,0,BORDER_CONSTANT,0);
    }

    resize(img, img, Size(EXISTENCE_MAP_W, EXISTENCE_MAP_H),INTER_LINEAR);

    emit showMap(mat2QImage(img));
}

// Fill the new column of node existence matrix using the last tau_w permutation and
// cost matrices. Starting from the last permutation matrix, each new segment is connected
// to one of the previous segments where pairwise segment matching cost is below the tau_m.
// For each new segment, optimum match with previous segments defined by permutation matrix
// and new segment is connected to the latest segment for which matching cost is below tau_m
float SegmentTrack::fillNodeMap(Mat& M, vector<pair<NodeSig, int> >& M_ns, const vector<vector<NodeSig> > ns_vec)
{
    int N = ns_vec.size(); //Number of the permutation matrices
    float matching_cost = 0;
    float smallest_matching_cost = 9999999;
    vector<Mat> P(N-1), C(N-1);

    //Create pairwise P and C matrices
    //Last frame is matched with each of last tau_w frames
    //and associated P and C matrices inserted into a vector
    //This step is requied for matching any nonmatched nodes
    for(int i = 2; i <= N; i++)
    {
        gm->matchTwoImages(ns_vec[N-i], ns_vec.back(), P[N-i], C[N-i]);
    }


    //Expand M(node existence matrix) to fit new column
    copyMakeBorder(M,M,0,0,0,1,BORDER_CONSTANT,-1);

    //Place each segment into M matrix
    for(int s = 0; s < ns_vec.back().size(); s++)
    {
        int node_id = -1; //Node id represents new segment's id in existence map

        //Check last N frames(associated permutation matrices)
        //Find if any of previous segments matches with segment(i.e. matching cost
        // is below threshold)
        for(int i = 2; i <= N; i++)
        {
            int j = getPermuted(P[N-i],s);

            //Check matching cost
            if(j != -1 && C[N-i].at<float>(j,s) < tau_m)
            {
                //return index of jth node in node existence map
                node_id = getIndexByCol(M, M.size().width-i, j);
                matching_cost = matching_cost + C[N-i].at<float>(j,s);
                break;
            }

            //If no matches are found, add smallest matching cost to total
            //matching cost
            if(j != -1 && smallest_matching_cost > C[N-i].at<float>(j,s))
            {
                smallest_matching_cost = C[N-i].at<float>(j,s);
            }
        }

        // If there is no match
        if(node_id == -1)
        {
            //Add new row to M matrix and set last element with new id
            copyMakeBorder(M,M,0,1,0,0,BORDER_CONSTANT,-1);
            M.at<int>(M.size().height-1, M.size().width-1) = s;
            matching_cost = matching_cost + smallest_matching_cost;

            //Add new empty node to M_ns
            pair<NodeSig, int> new_node(ns_vec.back()[s], 1);
            M_ns.push_back(new_node);
        }
        else
        {
            M.at<int>(node_id, M.size().width-1) = s;

            //Update average node signatures
            SSGProc::updateNodeSig(M_ns[node_id], ns_vec.back()[s]);
        }
    }

    return matching_cost;
}
