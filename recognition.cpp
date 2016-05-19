#include "recognition.h"
#include "utils.h"
#include "utilTypes.h"
#include "segmentation.h"


Recognition::Recognition(RecognitionParams* rec_params,
                         SSGParams* ssg_params,
                         SegmentTrackParams* seg_track_params,
                         SegmentationParams* seg_params,
                         GraphMatchParams* gm_params)
{
    this->rec_params = rec_params;
    this->ssg_params = ssg_params;
    this->seg_track_params = seg_track_params;
    this->seg_params = seg_params;
    this->gm_params = gm_params;

    img_files = getFiles(DATASET_FOLDER);

    Mat img = imread(DATASET_FOLDER + img_files[START_IDX]);
    img_height = img.size().height*IMG_RESCALE_RAT;
    img_width = img.size().width*IMG_RESCALE_RAT;

    plot_offset = 150;

    //Initialize new graph match and segmentation object
    gm = new GraphMatch(img_width, img_height, gm_params);
    seg = new Segmentation(seg_params);

    next = false;
}

Recognition::~Recognition()
{
    delete seg;
    delete gm;
}



void Recognition::processTree(Node* tree, int size)
{
    for(int i = 0; i < size; i++)
    {
        if(tree[i].left < 0)
            tree[i].left = -1*tree[i].left + size;
        if(tree[i].right < 0)
            tree[i].right = -1*tree[i].right + size;
    }
}

void Recognition::drawSSG(Mat& img, SSG ssg, Point coord)
{
    if(coord.y + rec_params->ssg_h > img.size().height)
    {
        copyMakeBorder(img, img, 0, rec_params->ssg_h, 0, 0, BORDER_CONSTANT, Scalar(255,255,255));
    }
    qDebug() << "g"<< img_files.size() << ssg.getSampleFrame()-START_IDX;

    Mat img_real = imread(DATASET_FOLDER + img_files[ssg.getSampleFrame()-START_IDX]);


    Mat img_ssg(img_height, img_width, CV_8UC3, Scalar(255,255,255));

    for(int i = 0; i < ssg.nodes.size(); i++)
    {
        Point p = ssg.nodes[i].first.center;
        double r = sqrt(ssg.nodes[i].first.area)/4.0;
        r = max(r,1.0);

        circle(img_ssg,p,r,Scalar(ssg.nodes[i].first.colorB, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorR), -1);
    }

    //Predefined size
    resize(img_real, img_real, cv::Size(rec_params->ssg_w,rec_params->ssg_h));
    resize(img_ssg, img_ssg, cv::Size(rec_params->ssg_w,rec_params->ssg_h));

    rectangle(img_real, Rect(0,0,img_real.size().width-1, img_real.size().height-1),Scalar(0,0,0));
    rectangle(img_ssg, Rect(0,0,img_ssg.size().width-1, img_ssg.size().height-1),Scalar(0,0,0));

    Rect roi = Rect(coord.x-img_real.size().width, coord.y, img_real.size().width, img_real.size().height);

    Mat image_roi = img(roi);

    img_real.copyTo(image_roi);

    Rect roi2 = Rect(coord.x, coord.y, img_ssg.size().width, img_ssg.size().height);

    Mat image_roi2 = img(roi2);

    img_ssg.copyTo(image_roi2);

    stringstream ss;
    ss.str("");
    ss << ssg.getId();
    Point str_coord(coord.x+rec_params->ssg_w, coord.y+rec_params->ssg_h/2.0);
    putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255),2);

}

void Recognition::drawBranch(Mat& img, TreeNode* node, int height, double scale_x, double scale_y)
{
    if(!node->isTerminal())
    {
        for(int i = 0; i < node->getChildren().size(); i++)
        {
            //qDebug() <<node->x_pos << node->children[i]->x_pos;
            Point top(plot_offset+node->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);
            Point middle(plot_offset+node->getChildren()[i]->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);
            Point bottom(plot_offset+node->getChildren()[i]->getXPos()*scale_x, height - plot_offset - 1 - node->getChildren()[i]->getVal()*scale_y);

            line(img, top, middle, Scalar(0,0,0), 2);
            line(img, middle, bottom, Scalar(0,0,0), 2);



            drawBranch(img, node->getChildren()[i], height, scale_x, scale_y);
        }
    }
    else
    {        
        Point coord(plot_offset+node->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);


        //Draw Place id
        stringstream ss;
        ss << node->getLabel();
        Point str_coord(coord.x+10, coord.y-10);
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0),2);

        //Draw SSGs
        for(int i = 0; i < node->getDescriptor()->getCount(); i++)
        {
            Point ssg_coord(coord.x, coord.y + i*rec_params->ssg_h);
            drawSSG(img, node->getDescriptor()->getMember(i), ssg_coord);
        }

    }
}

void Recognition::drawTree(TreeNode* root_node, int nrPlaces, int height, int width)
{
    Mat img(height, width, CV_8UC3, Scalar(255,255,255));

    double scale_x = (double) (width - 2*plot_offset) / nrPlaces;
    double scale_y = (double) (height - 2*plot_offset) / root_node->getVal();

    drawBranch(img, root_node, height, scale_x, scale_y);
    int border_size = 50;
    //copyMakeBorder(img, img, border_size, border_size, border_size, border_size, BORDER_CONSTANT, Scalar(255, 255, 255));

    emit showTree(mat2QImage(img));
    //imshow("Tree with images", img);
    imwrite(getOutputFolder() + "tree.jpg",img);
    waitKey(0);
}

void Recognition::sortTerminalNodes(TreeNode* node, int* last_pos)
{
    if(node->isTerminal())
    {
        //qDebug() << node->id+1;
        node->setXPos((*last_pos)++);
    }

    for(int i = 0; i < node->getChildren().size(); i++)
        sortTerminalNodes(node->getChildren()[i], last_pos);
}

TreeNode* Recognition::convert2Tree(Node* tree, int nrNodes, int nrPlaces, vector<PlaceSSG>& places)
{
    TreeNode* nodes = new TreeNode[nrPlaces+nrNodes];

    for(int i = 0; i < nrPlaces; i++)
    {
        nodes[i].setLabel(i);
        nodes[i].setXPos(i);
        nodes[i].setDescriptor(&places[i]);
    }

    for(int i = 0; i < nrNodes; i++)
    {
        nodes[i+nrPlaces].setLabel(i+nrPlaces);
        nodes[i+nrPlaces].setVal(tree[i].distance);

        //qDebug() << tree[i].distance;
        nodes[i+nrPlaces].addChild(&nodes[tree[i].left]);
        nodes[i+nrPlaces].addChild(&nodes[tree[i].right]);
    }

    TreeNode* root_node = &nodes[nrPlaces+nrNodes-1];

    int x_pos = 0;
    sortTerminalNodes(root_node, &x_pos);

    for(int i = 0; i < nrNodes; i++)
    {
        nodes[i+nrPlaces].setXPos((nodes[tree[i].left].getXPos() + nodes[tree[i].right].getXPos() ) / 2.0);
    }

    return root_node;
}

TreeNode* Recognition::findNode(int label, TreeNode* root)
{
    if(label == root->getLabel())
        return root;

    //Depth first traversal
    for(int i = 0; i < root->getChildren().size(); i++){
        TreeNode* p = findNode(label, root->getChildren()[i]);
        if(p != NULL) return p;
    }
    return NULL;
}


double** Recognition::calculateDistanceMatrix(vector<PlaceSSG>& places)
{
    int nrPlaces = places.size();
    double** dist_matrix = new double*[nrPlaces];

    for (int i = 0; i < nrPlaces; i++)
    {
        dist_matrix[i] = new double[nrPlaces];
    }

    for(int r = 0; r < nrPlaces; r++)
    {
        for(int c = 0; c < nrPlaces; c++)
        {
            double distance = 0;
            int count = 0;
            for(int i = 0; i < places[r].getCount(); i++)
            {
                for(int j = 0; j < places[c].getCount(); j++)
                {
                    Mat P, C;
                    distance += gm->matchTwoImages(places[r].getMember(i),places[c].getMember(j),P,C);
                    count++;
                }
            }

            dist_matrix[r][c] = distance/count;
        }
    }

    return dist_matrix;
}

int Recognition::performRecognition(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode* hierarchy)
{
    int recognition_status = NOT_RECOGNIZED;
    //TODO: Incremental distance matrix creation
    //First create distance matrix (We don't need to calculate distance matrix from scratch)

    //We'll push new place into places vector, then erase at the end
    places.push_back(new_place);

    //If there is not enough place for recognition
    if(places.size() < 2)
        return RECOGNITION_ERROR;

    int nrPlaces = places.size();
    thumbnail_scale = 1.0 / (nrPlaces+1);

    double** dist_matrix = calculateDistanceMatrix(places);


    //Find hierarchical tree using SLINK algorithm
    Node* tree = solveSLink(nrPlaces, nrPlaces, dist_matrix);

    hierarchy = convert2Tree(tree, nrPlaces-1, nrPlaces, places);

    //Draw tree
    drawTree(hierarchy, nrPlaces, rec_params->plot_h, rec_params->plot_w);

    //Get pointer to position of new detected place
    TreeNode* new_place_node = findNode(nrPlaces-1, hierarchy);

    int number = new_place_node->getDescriptor()->getMember(0).nodes.size();

    //Check if there is any sibling of new place
    //And check if they are similar
    TreeNode* new_place_parent = new_place_node->getParent();
    if(new_place_parent != NULL && new_place_parent->getChildren().size() > 1)
    {
        //Search siblings
        for(int i = 0; i < new_place_parent->getChildren().size(); i++)
        {
            //If any of the sibling is terminal node then check for recognition.
            if(new_place_parent->getChildren()[i]->getLabel() != new_place_node->getLabel() && new_place_parent->getChildren()[i]->isTerminal())
            {
                int sibling_label = new_place_parent->getChildren()[i]->getLabel();
                int new_place_label = new_place_node->getLabel();

                double dist = dist_matrix[sibling_label][new_place_label];
                if(dist < rec_params->tau_r)
                {
                    TreeNode* recognized_node = new_place_parent->getChildren()[i];
                    PlaceSSG* detected_place = new_place_node->getDescriptor();
                    PlaceSSG* recognized_place = recognized_node->getDescriptor();

                    for(int i = 0; i < detected_place->getCount(); i++)
                        recognized_place->addMember(detected_place->getMember(i));

                    places.erase(places.end());

                    qDebug() << "New place is recognized!" << "Recognized id's:" << sibling_label+1 << new_place_label+1;

                    emit printMessage("Rec: " + QString::number(sibling_label+1) + " " +  QString::number(new_place_label+1));
                    recognition_status = RECOGNIZED;
                    break;
                }
            }
        }

    }

    //Remove dist_matrix
    for (int i = 0; i < nrPlaces; i++) delete[] dist_matrix[i];
    delete[] dist_matrix;

    return recognition_status;


}

void Recognition::testRecognition()
{
    vector<Mat> images;
    TreeNode* hierarchy_tree;
    vector<PlaceSSG> places;
    //Read all images extract node signatures and store into vector
    for(int i = 0; i < img_files.size(); i++)
    {
        Mat img = imread(DATASET_FOLDER + img_files[i]);
        images.push_back(img);
        resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        Mat img_seg;

        vector<NodeSig> ns = seg->segmentImage(img, img_seg);
        SSG new_ssg(i+1,ns);
        PlaceSSG new_place(i, new_ssg);


        double tau_r = 0.01;
        performRecognition(places, new_place, hierarchy_tree);

        while(next == false) cvWaitKey(1);
        next = false;
    }
}


Node* Recognition::solveSLink(int nrows, int ncols, double** data)
{
    Node* tree;

    tree = treecluster(nrows, ncols, 0, 0, 0, 0, 'e', 's', data);

    processTree(tree, nrows-1);

    const int nnodes = nrows-1;

    if (!tree)
    {
        qDebug()<<"treecluster routine failed due to insufficient memory";
        return NULL;
    }
    else
    {
        for(int i = 0; i < nnodes; i++)
            //qDebug()<<-i-1<<tree[i].left<<tree[i].right<<tree[i].distance;
        return tree;
    }

    return NULL;
}

Mat Recognition::saveRAG(const vector<NodeSig> ns, string name)
{
    Mat img(img_height, img_width, CV_8UC3, Scalar(255,255,255));

    for(int i = 0; i < ns.size(); i++)
    {
        Point p = ns[i].center;
        double r = sqrt(ns[i].area)/4.0;
        r = max(r,1.0);

        circle(img,p,r,Scalar(ns[i].colorB, ns[i].colorG, ns[i].colorR), -1);
    }

    string outName = getOutputFolder()+name+".jpg";
    imwrite(outName, img);

    return img;

}

void Recognition::generateRAGs(const Node* tree, int nTree, vector<vector<NodeSig> >& rags, vector<Mat>& images)
{
    //Rags for detected places are already placed into rags vector
    //We need to create rags related to higher nodes
    int nPlaces = rags.size();

    //Create new RAGs
    for(int i = 0; i < nTree; i++)
    {
        Mat P, C;
        vector<NodeSig> new_rag;
        float cost = gm->matchTwoImages(rags[tree[i].left], rags[tree[i].right], P, C);

        if(cost != -1)
        {
            //If any of matched nodes have below threshold cost,
            //insert that node to new_rag
            for(int c = 0; c < P.size().width; c++)
            {
                int r = getPermuted(P, c);
                if( r != -1 && C.at<float>(r,c) < seg_track_params->tau_m)
                {
                    NodeSig ns;
                    NodeSig nsL = rags[tree[i].left][r];
                    NodeSig nsR = rags[tree[i].right][c];

                    ns.area =  (nsL.area + nsR.area) / 2;
                    ns.center.x =  (nsL.center.x + nsR.center.x) / 2;
                    ns.center.y =  (nsL.center.y + nsR.center.y) / 2;
                    ns.colorR =  (nsL.colorR + nsR.colorR) / 2;
                    ns.colorG =  (nsL.colorG + nsR.colorG) / 2;
                    ns.colorB =  (nsL.colorB + nsR.colorB) / 2;

                    new_rag.push_back(ns);
                }
            }
        }

        rags.push_back(new_rag);
    }

    for(int i = 0; i < rags.size(); i++)
    {
        stringstream ss;
        ss << i;
        Mat img = saveRAG(rags[i], ss.str());
        images.push_back(img);
    }
}

