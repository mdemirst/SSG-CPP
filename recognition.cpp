#include "recognition.h"
#include "utils.h"
#include "utilTypes.h"
#include "segmentation.h"


Recognition::Recognition(Parameters* params,
                         Dataset* dataset,
                         GraphMatch* gm,
                         Segmentation* seg)
{
    this->params = params;
    this->dataset = dataset;

    plot_offset = 150;

    //Initialize new graph match and segmentation object
    this->gm = gm;
    this->seg = seg;

    next = false;

    rec_method = REC_TYPE_SSG_NORMAL;
    norm_type = NORM_L2;
}

Recognition::~Recognition()
{
    delete seg;
    delete gm;
}

void Recognition::setNormType(int method)
{
    this->norm_type = method;
}

void Recognition::setRecognitionMethod(int method)
{
    this->rec_method = method;
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

cv::Scalar getColor(int color)
{
    switch(color)
    {
        case 1:
            return Scalar(255,0,0);     //blue      -- fr
        case 2:
            return Scalar(0,0,0);       //black     -- sa
        case 3:
            return Scalar(0,140,255);   //orange    -- lj
        case 4:
            return Scalar(0,255,0);     //green     -- nc
        case 5:
            return Scalar(255,255,0);   //aqua      -- fr2
        case 6:
            return Scalar(103,101,98);  //gray      -- sa2
        case 7:
            return Scalar(159,231,249); //yellow    -- lj2
        case 8:
            return Scalar(191,223,169); //l. green  -- nc2
        default:
            return Scalar(255,255,255);
    }
}

void Recognition::drawSSG(Mat& img, SSG ssg, Point coord)
{
    if(coord.y + params->rec_params.ssg_h > img.size().height)
    {
        copyMakeBorder(img, img, 0, params->rec_params.ssg_h, 0, 0, BORDER_CONSTANT, Scalar(255,255,255));
    }

    if(params->rec_params.ssg_w < 10)
    {
        stringstream ss;
        ss.str("");
        ss << ssg.getId();
        Point str_coord(coord.x-5, coord.y+25);
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 0.5, getColor(ssg.getColor()),2);
        circle(img,coord,10,  getColor(ssg.getColor()), -1);
    }
    else
    {
        Mat img_real = imread(ssg.getSampleFrame());


        Mat img_ssg(params->ssg_params.img_height, params->ssg_params.img_width, CV_8UC3, Scalar(255,255,255));

        for(int i = 0; i < ssg.nodes.size(); i++)
        {
            Point p = ssg.nodes[i].first.center;
            double r = sqrt(ssg.nodes[i].first.area)/4.0;
            r = max(r,1.0);

            circle(img_ssg,p,r,Scalar(ssg.nodes[i].first.colorB, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorR), -1);
        }

        //Predefined size
        resize(img_real, img_real, cv::Size(params->rec_params.ssg_w,params->rec_params.ssg_h));
        resize(img_ssg, img_ssg, cv::Size(params->rec_params.ssg_w,params->rec_params.ssg_h));

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
        Point str_coord(coord.x+params->rec_params.ssg_w, coord.y+params->rec_params.ssg_h/2.0);
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 0.5, getColor(ssg.getColor()),2);

        circle(img,coord,10,  getColor(ssg.getColor()), -1);
    }

}

void Recognition::drawInnerSSG(Mat& img, SSG ssg, Point coord)
{
    Mat img_ssg(params->ssg_params.img_height, params->ssg_params.img_width, CV_8UC3, Scalar(255,255,255));

    for(int i = 0; i < ssg.nodes.size(); i++)
    {
        Point p = ssg.nodes[i].first.center;
        double r = sqrt(ssg.nodes[i].first.area)/4.0;
        r = max(r,1.0);

        circle(img_ssg,p,r,Scalar(ssg.nodes[i].first.colorB, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorR), -1);
    }

    stringstream ss;
    ss.str("");
    ss << ssg.getId();
    Point str_coord(coord.x+params->rec_params.ssg_w/2, coord.y-3);
    putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 0.5, getColor(ssg.getColor()),2);

    //Predefined size
    resize(img_ssg, img_ssg, cv::Size(params->rec_params.ssg_w,params->rec_params.ssg_h));

    rectangle(img_ssg, Rect(0,0,img_ssg.size().width-1, img_ssg.size().height-1),Scalar(0,0,0));

    Rect roi = Rect(coord.x-img_ssg.size().width/2, coord.y-img_ssg.size().height/2, img_ssg.size().width, img_ssg.size().height);

    Mat image_roi = img(roi);

    img_ssg.copyTo(image_roi);
}

void Recognition::drawSSG2(Mat& img, SSG ssg, Point coord)
{
    if(coord.y + params->rec_params.ssg_h > img.size().height)
    {
        copyMakeBorder(img, img, 0, params->rec_params.ssg_h, 0, 0, BORDER_CONSTANT, Scalar(255,255,255));
    }

    if(params->rec_params.ssg_w < 10)
    {
        stringstream ss;
        ss.str("");
        ss << ssg.getId();
        Point str_coord(coord.x-5, coord.y+25);
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 0.5, getColor(ssg.getColor()),2);
        circle(img,coord,10,  getColor(ssg.getColor()), -1);
    }
    else
    {
        vector<string> img_files = getFiles("/home/isl-mahmut/Datasets/fr_seq2_cloudy1/std_cam/");
        int img_index = (ssg.getEndFrame()+ssg.getStartFrame())/2;
        Mat img_real = imread(string("/home/isl-mahmut/Datasets/fr_seq2_cloudy1/std_cam/") + img_files[img_index]);


        Mat img_ssg(params->ssg_params.img_height, params->ssg_params.img_width, CV_8UC3, Scalar(255,255,255));

        for(int i = 0; i < ssg.nodes.size(); i++)
        {
            Point p = ssg.nodes[i].first.center;
            double r = sqrt(ssg.nodes[i].first.area)/4.0;
            r = max(r,1.0);

            circle(img_ssg,p,r,Scalar(ssg.nodes[i].first.colorB, ssg.nodes[i].first.colorG, ssg.nodes[i].first.colorR), -1);
        }

        //Predefined size
        resize(img_real, img_real, cv::Size(params->rec_params.ssg_w,params->rec_params.ssg_h));
        resize(img_ssg, img_ssg, cv::Size(params->rec_params.ssg_w,params->rec_params.ssg_h));

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
        Point str_coord(coord.x+params->rec_params.ssg_w, coord.y+params->rec_params.ssg_h/2.0);
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 0.5, getColor(ssg.getColor()),2);

        circle(img,coord,10,  getColor(ssg.getColor()), -1);
    }

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

            //Point coord(plot_offset+node->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);
            //drawInnerSSG(img, node->getDescriptor()->getMember(0), coord);

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
            Point ssg_coord(coord.x, coord.y + i*params->rec_params.ssg_h);
            drawSSG2(img, node->getDescriptor()->getMember(i), ssg_coord);
        }

    }
}

void Recognition::drawTree(TreeNode* root_node, int nrPlaces, int height, int width)
{
    Mat img(height, width, CV_8UC3, Scalar(255,255,255));

    double scale_x = (double) (width - 2*plot_offset) / nrPlaces;
    double scale_y = (double) (height - 2*plot_offset) / root_node->getVal();

    drawBranch(img, root_node, height, scale_x, scale_y);

    emit showTree(mat2QImage(img));
    //imshow("Tree with images", img);
    imwrite(string(OUTPUT_FOLDER) + "tree.jpg",img);
    waitKey(0);
}

void Recognition::getTerminalNodes(TreeNode* node, vector<TreeNode*>& terminal_nodes)
{
    if(node->isTerminal())
    {
        terminal_nodes.push_back(node);
    }

    for(int i = 0; i < node->getChildren().size(); i++)
        getTerminalNodes(node->getChildren()[i], terminal_nodes);
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

//Burada memory leak olacak
PlaceSSG* Recognition::mergeSSGs(PlaceSSG* p1, PlaceSSG* p2, int id)
{
    SSG ssg(id);

    if(p1->getMember(0).nodes.size() == 0 || p2->getMember(0).nodes.size() == 0)
    {
        PlaceSSG* place_ssg = new PlaceSSG(id,ssg);
        return place_ssg;
    }


    Mat C, P;
    SSG ssg1 = p1->getMember(0);
    SSG ssg2 = p2->getMember(0);
    vector<pair<NodeSig,int> > ns1 = ssg1.nodes;
    vector<pair<NodeSig,int> > ns2 = ssg2.nodes;


    gm->matchTwoImages(ssg1, ssg2, P, C);

    vector<Point> nonzero_locs;
    findNonZero(P,nonzero_locs);



    for(int i = 0; i < nonzero_locs.size(); i++)
    {
        float cost = C.at<float>(nonzero_locs[i].y, nonzero_locs[i].x);
        if(cost < params->rec_params.tau_v)
        {
            NodeSig ns;
            cv::Point p =(ns1[nonzero_locs[i].y].first.center+ns2[nonzero_locs[i].x].first.center);
            ns.center = cv::Point(p.x/2,p.y/2);
            ns.area = (ns1[nonzero_locs[i].y].first.area + ns2[nonzero_locs[i].x].first.area) / 2;
            ns.colorR = (ns1[nonzero_locs[i].y].first.colorR + ns2[nonzero_locs[i].x].first.colorR) / 2;
            ns.colorG = (ns1[nonzero_locs[i].y].first.colorG + ns2[nonzero_locs[i].x].first.colorG) / 2;
            ns.colorB = (ns1[nonzero_locs[i].y].first.colorB + ns2[nonzero_locs[i].x].first.colorB) / 2;
            ssg.nodes.push_back(make_pair(ns,1));
        }
    }


    ssg.setColor(ssg1.getColor());

    PlaceSSG* place_ssg = new PlaceSSG(0,ssg);

    return place_ssg;
}

TreeNode** Recognition::convert2Tree(Node* tree, int nrNodes, int nrPlaces, vector<PlaceSSG>& places)
{
    TreeNode* nodes = new TreeNode[nrPlaces+nrNodes];

    for(int i = 0; i < nrPlaces; i++)
    {
        nodes[i].setLabel(i);
        nodes[i].setXPos(i);
        nodes[i].setDescriptor(&places[i]);
        nodes[i].setVal(0);
    }

    for(int i = 0; i < nrNodes; i++)
    {
        nodes[i+nrPlaces].setLabel(i+nrPlaces);
        nodes[i+nrPlaces].setVal(tree[i].distance);

        //qDebug() << tree[i].distance;
        nodes[i+nrPlaces].addChild(&nodes[tree[i].left]);
        nodes[i+nrPlaces].addChild(&nodes[tree[i].right]);

        //set merged ssg for inner nodes
        nodes[i+nrPlaces].setDescriptor(mergeSSGs(nodes[tree[i].left].getDescriptor(), nodes[tree[i].right].getDescriptor(), i+nrPlaces));
    }

    TreeNode* root_node = &nodes[nrPlaces+nrNodes-1];

    int x_pos = 0;
    sortTerminalNodes(root_node, &x_pos);

    for(int i = 0; i < nrNodes; i++)
    {
        nodes[i+nrPlaces].setXPos((nodes[tree[i].left].getXPos() + nodes[tree[i].right].getXPos() ) / 2.0);
    }

    return &root_node;
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

TreeNode* Recognition::findNode( int site, int label, TreeNode* root)
{
    if(root->isTerminal())
    {
        for(int i = 0; i < root->getDescriptor()->getCount(); i++)
        {
            if(root->getDescriptor()->getMember(i).getColor() == site && root->getDescriptor()->getMember(i).getId() == label)
            {
                return root;
            }
        }
    }
    //Depth first traversal
    for(int i = 0; i < root->getChildren().size(); i++){
        TreeNode* p = findNode(site, label, root->getChildren()[i]);
        if(p != NULL) return p;
    }
    return NULL;
}

double Recognition::calculateDistance(SSG& old_place, SSG& detected_place)
{
    vector<NodeSig> ns;
    double vote = 0;
    int count = 0;

    //First option
    for(int i = 0; i < old_place.nodes.size(); i++)
        ns.push_back(old_place.nodes[i].first);


    for(int i = 0; i < detected_place.basepoints.size(); i++)
    {
        Mat P, C;
        //voting system
        if(gm->matchTwoImages(detected_place.basepoints[i],ns,P,C) < params->rec_params.tau_v)
            vote += 1;
        count++;
    }
    vote /= count;

    return 1-vote;
}

static inline float computeSquare (float x) { return x*x; }

double Recognition::calculateDistanceTSC(SSG& old_place, SSG& detected_place)
{
    double total_distance = 0;

    if(rec_method == REC_TYPE_BD_NORMAL )
    {
        Mat oldp = old_place.mean_invariant.rowRange(100,599);
        Mat newp = detected_place.mean_invariant.rowRange(100,599);
        total_distance = norm(oldp, newp);
    }
    else if(rec_method == REC_TYPE_BD_COLOR)
    {
        total_distance = norm(old_place.mean_invariant,detected_place.mean_invariant,norm_type);
    }
    else if(rec_method == REC_TYPE_BD_COLOR_LOG)
    {
        Mat a = old_place.mean_invariant*25;
        cv::pow(a,2.71,a);

        Mat b = detected_place.mean_invariant*25;
        cv::pow(b,2.71,b);
        total_distance = norm(a,b,norm_type);
    }
    else if(rec_method == REC_TYPE_BD_VOTING)
    {
        float votes = 0;
        for(int i = 0; i < detected_place.member_invariants.size().width; i++)
        {
            if(norm(old_place.mean_invariant,detected_place.member_invariants.col(i),norm_type) < params->rec_params.tau_v)
            {
                votes += 1;
            }
        }
        if(votes == 0)
            total_distance = 1;
        else
        {
            total_distance = votes / detected_place.member_invariants.size().width;
            total_distance = 1 - total_distance;
        }

    }

    return total_distance;
}

double Recognition::getDistance(PlaceSSG& p1, PlaceSSG& p2)
{
    int count = 0;
    double distance = 0;
    for(int i = 0; i < p1.getCount(); i++)
    {
        for(int j = 0; j < p2.getCount(); j++)
        {
            Mat P, C;
            if(rec_method == REC_TYPE_SSG_NORMAL)
            {
                //Graph distance calculation -- method #1
                distance += gm->matchTwoImages(p1.getMember(i),p2.getMember(j),P,C);
            }
            else if(rec_method == REC_TYPE_SSG_VOTING)
            {
                //Graph distance calculation -- method #2
                distance += calculateDistance(p1.getMember(i),p2.getMember(j));
            }
            else if(rec_method == REC_TYPE_BD_NORMAL || rec_method == REC_TYPE_BD_VOTING || rec_method == REC_TYPE_BD_COLOR)
            {
                //Bubble descriptor distance -- method #3
                distance += calculateDistanceTSC(p1.getMember(i),p2.getMember(j));
            }
            else if(rec_method == REC_TYPE_HYBRID)
            {

            }
            count++;
        }
    }

    return distance / count;

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
        dist_matrix[r][r] = 0;
        for(int c = r+1; c < nrPlaces; c++)
        {
            double distance = 0;

            distance = getDistance(places[r], places[c]);
            //qDebug() << "Rec. score between" << r << c << "is " << distance;

            dist_matrix[r][c] = distance;
            dist_matrix[c][r] = distance;

        }
    }

    return dist_matrix;
}

int Recognition::performRecognition(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy)
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

    //qDebug() << "New recognition calculation...";
    double** dist_matrix = calculateDistanceMatrix(places);


    //Find hierarchical tree using SLINK algorithm
    Node* tree = solveSLink(nrPlaces, nrPlaces, dist_matrix);

    *hierarchy = *convert2Tree(tree, nrPlaces-1, nrPlaces, places);

    //Draw tree
    drawTree(*hierarchy, nrPlaces, params->rec_params.plot_h, params->rec_params.plot_w);

    //Get pointer to position of new detected place
    TreeNode* new_place_node = findNode(nrPlaces-1, *hierarchy);


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
                if(dist < params->rec_params.tau_r)
                {
                    TreeNode* recognized_node = new_place_parent->getChildren()[i];
                    PlaceSSG* detected_place = new_place_node->getDescriptor();
                    PlaceSSG* recognized_place = recognized_node->getDescriptor();

                    for(int i = 0; i < detected_place->getCount(); i++)
                        recognized_place->addMember(detected_place->getMember(i));

                    places.erase(places.end());

                    //qDebug() << "New place is recognized!" << "Recognized id's:" << sibling_label << new_place_label;

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

int Recognition::performRecognition2(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy)
{
    int recognition_status = NOT_RECOGNIZED;
    //TODO: Incremental distance matrix creation
    //First create distance matrix (We don't need to calculate distance matrix from scratch)

    //We'll push new place into places vector, then erase at the end
    places.push_back(new_place);

    //calculcate place candidates
    vector<vector<int> > candidates_mat = calculatePlaceCandidates(places);

    //If there is not enough place for recognition
    if(places.size() < 2)
        return RECOGNITION_ERROR;

    int nrPlaces = places.size();
    thumbnail_scale = 1.0 / (nrPlaces+1);

    //qDebug() << "New recognition calculation...";
    double** dist_matrix = calculateDistanceMatrix(places);


    //Find hierarchical tree using SLINK algorithm
    Node* tree = solveSLink(nrPlaces, nrPlaces, dist_matrix);

    *hierarchy = *convert2Tree(tree, nrPlaces-1, nrPlaces, places);

    //Draw tree
    drawTree(*hierarchy, nrPlaces, params->rec_params.plot_h, params->rec_params.plot_w);

    //Get pointer to position of new detected place
    TreeNode* new_place_node = findNode(nrPlaces-1, *hierarchy);

    TreeNode* new_place_parent = new_place_node->getParent();

    //If new detected place's h is below tau_r perform rec
    //Assign it to the most closes terminal node..
    if(new_place_parent->getVal() < params->rec_params.tau_r)
    {
        vector<TreeNode*> terminal_nodes;
        getTerminalNodes(new_place_parent, terminal_nodes);

        double best_dist = 999;
        TreeNode* closest_node = NULL;

        for(int i = 0; i < terminal_nodes.size(); i++)
        {
            if(new_place_node->getLabel() != terminal_nodes[i]->getLabel())
            {
                double dist = getDistance(*new_place_node->getDescriptor(), *terminal_nodes[i]->getDescriptor());

                if(dist < best_dist)
                {
                    best_dist = dist;
                    closest_node = terminal_nodes[i];
                }
            }
        }

        if(closest_node != NULL)
        {
            vector<int> candidates = candidates_mat[new_place_node->getLabel()];
            //Check candidates contains recognized place
            if(find(candidates.begin(), candidates.end(),closest_node->getLabel()) != candidates.end())
            {
                qDebug() << "Recognized!" << closest_node->getDescriptor()->getMember(0).getId() << "->" << new_place_node->getDescriptor()->getMember(0).getId();

                //qDebug() << "Recognized place! " << closest_node->getLabel() << "<-" << new_place_node->getLabel();
                for(int i = 0; i < new_place_node->getDescriptor()->getCount(); i++)
                    closest_node->getDescriptor()->addMember(new_place_node->getDescriptor()->getMember(i));

                places.erase(places.end());

                recognition_status = RECOGNIZED;

            }
            else
            {
                qDebug() << "Couldn't pass SSG candidates test" << closest_node->getDescriptor()->getMember(0).getId() << "->" << new_place_node->getDescriptor()->getMember(0).getId();
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
    TreeNode** hierarchy_tree = NULL;
    vector<PlaceSSG> places;
    //Read all images extract node signatures and store into vector
    for(int i = 0; i < img_files.size(); i++)
    {
        Mat img = imread(dataset->location + img_files[i]);
        images.push_back(img);
        resize(img, img, cv::Size(0,0), IMG_RESCALE_RAT, IMG_RESCALE_RAT);
        Mat img_seg;

        vector<NodeSig> ns = seg->segmentImage(img, img_seg);
        SSG new_ssg(i+1,ns);
        PlaceSSG new_place(i, new_ssg);

        performRecognition(places, new_place, hierarchy_tree);

        while(next == false) cvWaitKey(1);
        next = false;
    }
}


Node* Recognition::solveSLink(int nrows, int ncols, double** data)
{
    Node* tree;

    tree = treecluster(nrows, ncols, 0, 0, 0, 0, 'e', 'a', data);

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
    Mat img(params->ssg_params.img_height, params->ssg_params.img_width, CV_8UC3, Scalar(255,255,255));

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
                if( r != -1 && C.at<float>(r,c) < params->seg_track_params.tau_m)
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

//bool Recognition::isRevisited(int site1, int site2, int place1, int place2)
//{
//    if(site1 == site2)
//        return false;
//    else
//    {
//        if(site1 > site2)
//        {
//            int dum = place2;
//            place2 = place1;
//            place1 = dum;

//            dum = site2;
//            site2 = site1;
//            site1 = dum;
//        }
//    }
//    if(site1 == 1 && site2 == 5)
//    {
//        if(place1 == 0 && place2 == 0)
//            return true;
//        else if(place1 == 0 && place2 == 0)
//            return true;
//        else if(place1 == 1 && place2 == 1)
//            return true;
//        else if(place1 == 2 && place2 == 3)
//            return true;
//        else if(place1 == 3 && place2 == 4)
//            return true;
//        else if(place1 == 5 && place2 == 7)
//            return true;
//        else if(place1 == 6 && place2 == 8)
//            return true;
//        else if(place1 == 7 && place2 == 9)
//            return true;
//        else if(place1 == 8 && place2 == 10)
//            return true;
//        else if(place1 == 9 && place2 == 14)
//            return true;
//        else if(place1 == 10 && place2 == 14)
//            return true;
//        else if(place1 == 11 && place2 == 14)
//            return true;
//        else if(place1 == 13 && place2 == 15)
//            return true;
//        else if(place1 == 14 && place2 == 16)
//            return true;
//        else if(place1 == 15 && place2 == 17)
//            return true;
//        else if(place1 == 16 && place2 == 18)
//            return true;
//        else if(place1 == 17 && place2 == 19)
//            return true;
//        else if(place1 == 18 && place2 == 21)
//            return true;
//    }
//    else if(site1 == 2 && site2 == 6)
//    {
//        if(place1 == 0 && place2 == 0)
//            return true;
//        else if(place1 == 0 && place2 == 0)
//            return true;
//        else if(place1 == 1 && place2 == 1)
//            return true;
//        else if(place1 == 1 && place2 == 2)
//            return true;
//        else if(place1 == 2 && place2 == 3)
//            return true;
//        else if(place1 == 3 && place2 == 4)
//            return true;
//        else if(place1 == 4 && place2 == 5)
//            return true;
//        else if(place1 == 6 && place2 == 5)
//            return true;
//        else if(place1 == 7 && place2 == 6)
//            return true;
//        else if(place1 == 7 && place2 == 7)
//            return true;
//        else if(place1 == 8 && place2 == 8)
//            return true;
//        else if(place1 == 9 && place2 == 10)
//            return true;
//        else if(place1 == 10 && place2 == 11)
//            return true;
//    }
//    else if(site1 == 3 && site2 == 7)
//    {
//        if(place1 == 0 && place2 == 0)
//            return true;
//        else if(place1 == 0 && place2 == 0)
//            return true;
//        else if(place1 == 1 && place2 == 1)
//            return true;
//        else if(place1 == 2 && place2 == 2)
//            return true;
//        else if(place1 == 3 && place2 == 3)
//            return true;
//        else if(place1 == 4 && place2 == 4)
//            return true;
//        else if(place1 == 5 && place2 == 4)
//            return true;
//        else if(place1 == 6 && place2 == 5)
//            return true;
//        else if(place1 == 7 && place2 == 6)
//            return true;
//        else if(place1 == 8 && place2 == 7)
//            return true;
//        else if(place1 == 8 && place2 == 8)
//            return true;
//        else if(place1 == 9 && place2 == 9)
//            return true;
//        else if(place1 == 9 && place2 == 10)
//            return true;
//        else if(place1 == 10 && place2 == 12)
//            return true;
//        else if(place1 == 11 && place2 == 12)
//            return true;
//        else if(place1 == 12 && place2 == 12)
//            return true;
//        else if(place1 == 13 && place2 == 12)
//            return true;
//        else if(place1 == 14 && place2 == 22)
//            return true;
//        else if(place1 == 16 && place2 == 13)
//            return true;
//        else if(place1 == 17 && place2 == 15)
//            return true;
//        else if(place1 == 19 && place2 == 17)
//            return true;
//        else if(place1 == 20 && place2 == 17)
//            return true;
//        else if(place1 == 21 && place2 == 17)
//            return true;
//        else if(place1 == 22 && place2 == 19)
//            return true;
//        else if(place1 == 23 && place2 == 20)
//            return true;
//        else if(place1 == 24 && place2 == 21)
//            return true;
//        else if(place1 == 25 && place2 == 22)
//            return true;
//        else if(place1 == 27 && place2 == 23)
//            return true;
//        else if(place1 == 28 && place2 == 24)
//            return true;
//    }
//    return false;
//}


void Recognition::calculateRecPerformance2(int nrPlaces, double** dist_matrix, TreeNode* root)
{
    int site1 = 1;
    int site2 = 5;
    vector<pair<int, int> > matches;
    matches.push_back(make_pair(0, 0 ));
    matches.push_back(make_pair(1, 1 ));
    matches.push_back(make_pair(2, 3 ));
    matches.push_back(make_pair(3, 4 ));
    matches.push_back(make_pair(5, 7 ));
    matches.push_back(make_pair(6, 8 ));
    matches.push_back(make_pair(7, 9 ));
    matches.push_back(make_pair(8, 10));
    matches.push_back(make_pair(9, 14));
    matches.push_back(make_pair(10, 14));
    matches.push_back(make_pair(11, 14));
    matches.push_back(make_pair(13, 15));
    matches.push_back(make_pair(14, 16));
    matches.push_back(make_pair(15, 17));
    matches.push_back(make_pair(16, 18));
    matches.push_back(make_pair(17, 19));
    matches.push_back(make_pair(18, 21));

    qDebug() << "Distances to match";
    for(int i = 0; i < matches.size(); i++)
    {
        TreeNode* n1 = findNode(site1, matches[i].first, root);
        TreeNode* n2 = findNode(site2, matches[i].second, root);
        int n1_label = n1->getLabel();
        int n2_label = n2->getLabel();

        float n1n2_dist = dist_matrix[n1_label][n2_label];
        int count = 0;
        for(int j = 0; j < nrPlaces; j++)
        {
            if(j != n1_label && dist_matrix[n1_label][j] < n1n2_dist)
            {
                count++;
            }
        }
        qDebug() << matches[i].first << "->" << matches[i].second << ":" << count;
    }
}

void Recognition::calculateRecPerformance3(vector<PlaceSSG> places)
{
    int site1 = 1;
    int site2 = 5;
    vector<pair<int, int> > matches;
    matches.push_back(make_pair(0, 0 ));
    matches.push_back(make_pair(1, 1 ));
    matches.push_back(make_pair(2, 3 ));
    matches.push_back(make_pair(3, 4 ));
    matches.push_back(make_pair(5, 7 ));
    matches.push_back(make_pair(6, 8 ));
    matches.push_back(make_pair(7, 9 ));
    matches.push_back(make_pair(8, 10));
    matches.push_back(make_pair(9, 14));
    matches.push_back(make_pair(10, 14));
    matches.push_back(make_pair(11, 14));
    matches.push_back(make_pair(13, 15));
    matches.push_back(make_pair(14, 16));
    matches.push_back(make_pair(15, 17));
    matches.push_back(make_pair(16, 18));
    matches.push_back(make_pair(17, 19));
    matches.push_back(make_pair(18, 21));
    int nrPlaces1 = 19;

    vector<vector<int> > candidates_mat = calculatePlaceCandidates(places);

    double** dist_matrix = calculateDistanceMatrix(places);

    for(int i = 0; i < matches.size(); i++)
    {
        int n1 = matches[i].first;

        float best_score = 1000;
        float best_id = -1;
        for(int j = 0; j < candidates_mat[n1].size(); j++)
        {
            int n2 = candidates_mat[n1][j];
            if(n1 != n2 && dist_matrix[n1][n2] < best_score)
            {
                best_score = dist_matrix[n1][n2];
                best_id = n2;
            }
        }

        //qDebug() << n1 << "->" << best_id-nrPlaces1;
        if(best_score < params->rec_params.tau_r)
        {
            if(best_id == matches[i].second+nrPlaces1)
                qDebug() << 1;
            else
                qDebug() << -1;
        }
        else
        {
            qDebug() << 0;
        }

    }
}

void Recognition::calculateRecPerformance(TreeNode* root)
{
    int site1 = 1;
    int site2 = 5;
    vector<pair<int, int> > matches;
    matches.push_back(make_pair(0, 0 ));
    matches.push_back(make_pair(1, 1 ));
    matches.push_back(make_pair(2, 3 ));
    matches.push_back(make_pair(3, 4 ));
    matches.push_back(make_pair(5, 7 ));
    matches.push_back(make_pair(6, 8 ));
    matches.push_back(make_pair(7, 9 ));
    matches.push_back(make_pair(8, 10));
    matches.push_back(make_pair(9, 14));
    matches.push_back(make_pair(10, 14));
    matches.push_back(make_pair(11, 14));
    matches.push_back(make_pair(13, 15));
    matches.push_back(make_pair(14, 16));
    matches.push_back(make_pair(15, 17));
    matches.push_back(make_pair(16, 18));
    matches.push_back(make_pair(17, 19));
    matches.push_back(make_pair(18, 21));


//    int site1 = 2;
//    int site2 = 6;
//    vector<pair<int, int> > matches;
//    matches.push_back(make_pair(0, 0 ));
//    matches.push_back(make_pair(1, 1 ));
//    matches.push_back(make_pair(1, 2 ));
//    matches.push_back(make_pair(2, 3 ));
//    matches.push_back(make_pair(3, 4 ));
//    matches.push_back(make_pair(4, 5 ));
//    matches.push_back(make_pair(6, 5 ));
//    matches.push_back(make_pair(7, 6 ));
//    matches.push_back(make_pair(7, 7));
//    matches.push_back(make_pair(8, 8));
//    matches.push_back(make_pair(9, 10));
//    matches.push_back(make_pair(10, 11));


//    int site1 = 3;
//    int site2 = 7;
//    vector<pair<int, int> > matches;
//    matches.push_back(make_pair(0, 0));
//    matches.push_back(make_pair(1, 1));
//    matches.push_back(make_pair(2, 2));
//    matches.push_back(make_pair(3, 3));
//    matches.push_back(make_pair(4, 4));
//    matches.push_back(make_pair(5, 4));
//    matches.push_back(make_pair(6, 5));
//    matches.push_back(make_pair(7, 6));
//    matches.push_back(make_pair(8, 7));
//    matches.push_back(make_pair(8, 8));
//    matches.push_back(make_pair(9, 9));
//    matches.push_back(make_pair(9, 10));
//    matches.push_back(make_pair(10, 12));
//    matches.push_back(make_pair(11, 12));
//    matches.push_back(make_pair(12, 12));
//    matches.push_back(make_pair(13, 12));
//    matches.push_back(make_pair(14, 12));
//    matches.push_back(make_pair(16, 13));
//    matches.push_back(make_pair(17, 15));
//    matches.push_back(make_pair(19, 17));
//    matches.push_back(make_pair(20, 17));
//    matches.push_back(make_pair(21, 17));
//    matches.push_back(make_pair(22, 19));
//    matches.push_back(make_pair(23, 20));
//    matches.push_back(make_pair(24, 21));
//    matches.push_back(make_pair(25, 22));
//    matches.push_back(make_pair(27, 23));
//    matches.push_back(make_pair(28, 24));


    qDebug() << "N2N";
    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;


        TreeNode* n1 = findNode(site1, place1, root);
        TreeNode* n2 = findNode(site2, place2, root);

        //qDebug() << n1->getLabel()<< n2->getLabel();

        //qDebug() << place1 << "\t" << place2 << "\t" << calculateN2NTreeDistance(findNode(site1, place1, root), findNode(site2, place2, root));
        Mat P, C;

        //qDebug() << findNode(site1, place1, root)->getDescriptor()->getCount() << findNode(site2, place2, root)->getDescriptor()->getCount();
        qDebug() << calculateN2NTreeDistance(findNode(site1, place1, root), findNode(site2, place2, root));
    }

    qDebug() << "SSG";
    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;


        TreeNode* n1 = findNode(site1, place1, root);
        TreeNode* n2 = findNode(site2, place2, root);

        //qDebug() << n1->getLabel()<< n2->getLabel();

        //qDebug() << place1 << "\t" << place2 << "\t" << calculateN2NTreeDistance(findNode(site1, place1, root), findNode(site2, place2, root));
        Mat P, C;

        //qDebug() << findNode(site1, place1, root)->getDescriptor()->getCount() << findNode(site2, place2, root)->getDescriptor()->getCount();
        qDebug() << gm->matchTwoImages(findNode(site1, place1, root)->getDescriptor()->getMember(0),findNode(site2, place2, root)->getDescriptor()->getMember(0),P,C);
    }

    qDebug() << "BD";

    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;


        TreeNode* n1 = findNode(site1, place1, root);
        TreeNode* n2 = findNode(site2, place2, root);

        //qDebug() << n1->getLabel()<< n2->getLabel();

        //qDebug() << place1 << "\t" << place2 << "\t" << calculateN2NTreeDistance(findNode(site1, place1, root), findNode(site2, place2, root));
        Mat P, C;

        //qDebug() << findNode(site1, place1, root)->getDescriptor()->getCount() << findNode(site2, place2, root)->getDescriptor()->getCount();
        qDebug() << calculateDistanceTSC(findNode(site1, place1, root)->getDescriptor()->getMember(0),findNode(site2, place2, root)->getDescriptor()->getMember(0));
    }

}


void printMat(Mat mat, int prec)
{
    for(int i=0; i<mat.size().height; i++)
    {
        cout << "[";
        for(int j=0; j<mat.size().width; j++)
        {
            cout << setprecision(prec) << mat.at<double>(i,j);
            if(j != mat.size().width-1)
                cout << ", ";
            else
                cout << "]" << endl;
        }
    }
}

vector<vector<int> > Recognition::calculatePlaceCandidates(vector<PlaceSSG>& places)
{
    Mat cue_matrix = Mat::zeros(places.size(), places.size(), CV_32F);
    vector<vector<int> > candidates_mat;

    for(int i = 0; i < places.size(); i++)
    {
        vector<int> candidates_vec;
        for(int j = 0; j < places.size(); j++)
        {
            Mat P, C;
            gm->matchTwoImages(places[i].getMember(0),places[j].getMember(0),P,C);

            vector<Point> nonzero_locs;
            findNonZero(P,nonzero_locs);

            int nr_cue = 0;
            for(int i = 0; i < nonzero_locs.size(); i++)
            {
                float cost = C.at<float>(nonzero_locs[i].y, nonzero_locs[i].x);
                if(cost < params->rec_params.tau_v)
                {
                    nr_cue++;
                }
            }

            cue_matrix.at<float>(i,j) = nr_cue;

            if(nr_cue > 0)
            {
                candidates_vec.push_back(j);
                //if(places[i].getMember(0).getColor() == 1 && places[j].getMember(0).getColor() == 5)
                    //qDebug() << places[i].getMember(0).getId() << "->" << places[j].getMember(0).getId() << ":" << nr_cue;
            }
        }
        candidates_mat.push_back(candidates_vec);
    }

    return candidates_mat;
}

void Recognition::calculateN2NDistanceMatrix(TreeNode* root_node)
{
    vector<TreeNode*> all_terminal_nodes;
    getTerminalNodes(root_node, all_terminal_nodes);

    int size = all_terminal_nodes.size();

    Mat N2N_distance_matrix = Mat::zeros(size, size, CV_32F);
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            //float dist = calculateN2NTreeDistance(all_terminal_nodes[i], all_terminal_nodes[j]);

            Mat P, C;
            //float dist = gm->matchTwoImages(all_terminal_nodes[i]->getDescriptor()->getMember(0),all_terminal_nodes[j]->getDescriptor()->getMember(0),P,C);
            float dist = calculateDistanceTSC(all_terminal_nodes[i]->getDescriptor()->getMember(0),all_terminal_nodes[j]->getDescriptor()->getMember(0));
            N2N_distance_matrix.at<float>(all_terminal_nodes[i]->getLabel(),all_terminal_nodes[j]->getLabel()) = dist;
        }
    }
    setprecision(3);

    cerr << N2N_distance_matrix << endl;
}

int Recognition::calculateN2NTreeDistance(TreeNode* node1, TreeNode* node2)
{
    if(node1->getLabel() == node2->getLabel())
        return 0;
    else
    {
        vector<int> labels1, labels2;
        while(node1->getParent() != NULL)
        {
            labels1.push_back(node1->getParent()->getLabel());
            node1 = node1->getParent();
        }
        while(node2->getParent() != NULL)
        {
            labels2.push_back(node2->getParent()->getLabel());
            node2 = node2->getParent();
        }

        for(int i = 0; i < labels1.size(); i++)
        {
            for(int j = 0; j < labels2.size(); j++)
            {
                if(labels1[i] == labels2[j])
                {
                    return i+j+1;
                }
            }
        }
        return -1;
    }
}
