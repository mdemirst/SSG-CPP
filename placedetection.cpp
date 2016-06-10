#include "placedetection.h"
#include "utils.h"
#include "unistd.h"
#include <dirent.h>

#include "rag.h"



//void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
//  cv::Mat &output, cv::Point2i location)
//{
//  background.copyTo(output);


//  // start at the row indicated by location, or at row 0 if location.y is negative.
//  for(int y = std::max(location.y , 0); y < background.rows; ++y)
//  {
//    int fY = y - location.y; // because of the translation

//    // we are done of we have processed all rows of the foreground image.
//    if(fY >= foreground.rows)
//      break;

//    // start at the column indicated by location,

//    // or at column 0 if location.x is negative.
//    for(int x = std::max(location.x, 0); x < background.cols; ++x)
//    {
//      int fX = x - location.x; // because of the translation.

//      // we are done with this row if the column is outside of the foreground image.
//      if(fX >= foreground.cols)
//        break;

//      // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
//      double opacity =
//        ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

//        / 255.;


//      // and now combine the background and foreground pixel, using the opacity,

//      // but only if opacity > 0.
//      for(int c = 0; opacity > 0 && c < output.channels(); ++c)
//      {
//        unsigned char foregroundPx =
//          foreground.data[fY * foreground.step + fX * foreground.channels() + c];
//        unsigned char backgroundPx =
//          background.data[y * background.step + x * background.channels() + c];
//        output.data[y*output.step + output.channels()*x + c] =
//          backgroundPx * (1.-opacity) + foregroundPx * opacity;
//      }
//    }
//  }
//}

//Constructs the average appearance of the scene by averaging specified
//segments.
//void PlaceDetection::constructSceneGist(Mat& M, vector<pair<NodeSig, int> > M_ns)
//{
//    vector<int> segment_ids;

//    int thres = 10;

//    //First find coherent segments
//    for(int i = 0; i < M_ns.size(); i++)
//    {
//        if( M_ns[i].second > thres )
//        {
//            segment_ids.push_back(i);
//        }
//    }

//    //Stores average appearances of coherent segments
//    vector< pair<Mat, int> > average_segments(M.size().height, make_pair(Mat::zeros(0,0,CV_8UC3), 0));

//    //Search segments in M starting from first frame to last frame
//    for(int i = 0; i < M.size().width; i++)
//    {
//        //Check if any of specified segments appear in this frame
//        bool do_segmentation = false;
//        for(int j = 0; j < segment_ids.size(); j++)
//        {
//            if(M.at<int>(segment_ids[j],i) != -1)
//            {
//                do_segmentation = true;
//                break;
//            }
//        }
//        //This frame does not contain interested segment, so continue to next
//        if(do_segmentation == false)
//            continue;

//        Mat img = imread(DATASET_FOLDER + img_files[START_IDX + i]);
//        resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);

//        vector<Mat> segments = seg_track->seg->segmentImage(img);

//        //Get interested segments in the frame and average them
//        //with matched previous segments
//        for(int j = 0; j < segment_ids.size(); j++)
//        {
//            int node_id = segment_ids[j];
//            int segment_id = M.at<int>(node_id,i);

//            if(segment_id != -1)
//            {
//                Mat img_gray;
//                vector<vector<Point> > contours;
//                vector<Vec4i> hierarchy;

//                Mat img_seg = segments[segment_id];

//                cvtColor(img_seg, img_gray, COLOR_BGR2GRAY);
//                threshold(img_gray, img_gray, 1, 255, CV_THRESH_BINARY);
//                findContours(img_gray.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

//                //Adding alpha channel
//                Mat rgb[3];
//                split(img_seg,rgb);

//                Mat rgba[4]={rgb[0],rgb[1],rgb[2],img_gray};
//                merge(rgba,4,img_seg);

//                Rect rect = boundingRect(contours[0]);

//                Mat img_crop = img_seg(rect);

//                Mat img_old = average_segments[node_id].first;
//                float count = average_segments[node_id].second;
//                int area1 = img_old.size().width * img_old.size().height;
//                int area2 = img_crop.size().width * img_crop.size().height;

//                if(area1 == 0)
//                {
//                    img_old = img_crop;
//                    average_segments[node_id].first = img_old;
//                    average_segments[node_id].second = count + 1;
//                }
//                else
//                {
//                    if(area1 > area2)
//                    {
//                        resize(img_crop, img_crop, img_old.size());
//                    }
//                    else
//                    {
//                        resize(img_old, img_old, img_crop.size());
//                    }

//                    float alpha = count / (count + 1);

//                    cout << img_old.channels() << img_crop.channels() << endl;

//                    addWeighted(img_old, alpha, img_crop, 1-alpha, 0.0, img_old, CV_8UC4);
//                    average_segments[node_id].first = img_old;
//                    average_segments[node_id].second = count + 1;
//                }
//            }
//        }
//    }

//    //Draw segments together
//    Mat img = imread(DATASET_FOLDER + img_files[START_IDX]);
//    resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);

//    Mat img_gist = Mat::zeros(img.size(),CV_8UC3);
//    for(int i = 0; i < average_segments.size(); i++)
//    {
//        if(average_segments[i].second > 0)
//        {
//            Point center = M_ns[i].first.center;
//            Mat img = average_segments[i].first;
//            int x1 = center.x - img.size().width/2;
//            int x2 = x1 + img.size().width;
//            int y1 = center.y - img.size().height/2;
//            int y2 = y1 + img.size().height;

//            if(x1 < 0)
//            {
//                img = img(Rect(-1*x1,0,img.size().width+x1,img.size().height));
//                x1 = 0;
//            }
//            if(x2 > img_gist.size().width)
//            {
//                img = img(Rect(0,0,img.size().width-(x2-img_gist.size().width),img.size().height));
//                x2 = img_gist.size().width;
//            }
//            if(y1 < 0)
//            {
//                img = img(Rect(0,-1*y1,img.size().width,img.size().height+y1));
//                y1 = 0;
//            }
//            if(y2 > img_gist.size().height)
//            {
//                img = img(Rect(0,0,img.size().width,img.size().height-(y2-img_gist.size().height)));
//                y2 = img_gist.size().height;
//            }

//            Mat img_rect = img_gist(Rect(x1,y1,img.size().width,img.size().height));
//            overlayImage(img_rect, img, img_rect, cv::Point(0,0));
//        }
//    }

//    imshow("Gist", img_gist);
//    cvWaitKey(0);
//}

//bool PlaceDetection::eventFilter( QObject* watched, QEvent* event ) {
//    int max_frames = END_IDX-START_IDX;

//    if ( event->type() == QEvent::MouseButtonPress )
//    {
//        const QMouseEvent* const me = static_cast<const QMouseEvent*>( event );

//        //might want to check the buttons here
//        const QPoint p = me->pos();

//        cursor = (p.x() / (float)COH_PLOT_W)*max_frames;
//    }
//    else if( event->type() == QEvent::KeyPress)
//    {
//        QKeyEvent* key = static_cast<QKeyEvent*>(event);

//        if ( key->key() == Qt::Key_Left )
//        {
//            cursor = max(0, cursor-1);
//        }
//        else if ( key->key() == Qt::Key_Right )
//        {
//            cursor = min(max_frames-1, cursor+1);
//        }
//        else
//        {
//            return false;
//        }
//    }
//    else
//    {
//        return false;
//    }

//    //Show cursor
//    coherency_plot->graph(PLOT_TRACK_IDX)->clearData();
//    coherency_plot->graph(PLOT_TRACK_IDX)->addData(cursor,0);
//    coherency_plot->rescaleAxes();
//    coherency_plot->replot();

//    string filepath = DATASET_FOLDER + img_files[START_IDX + cursor];

//    Mat img = imread(filepath);
//    resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);

//    //Show original images on the window
//    emit showImg2(mat2QImage(img));


//    return true;
//}

