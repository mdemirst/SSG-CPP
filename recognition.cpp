#include "recognition.h"
#include "utils.h"
#include "utilTypes.h"
#include "segmentation.h"
#include "experimentalData.h"


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

    rec_method = REC_TYPE_SSG_NORMAL;
    showPlaceAsCircle = true;
    showInnerSSGs = false;
}

Recognition::~Recognition()
{
    delete seg;
    delete gm;
}

void Recognition::setRecognitionMethod(int method)
{
    this->rec_method = method;
}

void Recognition::setPlaceDisplayType(int isCircle)
{
    this->showPlaceAsCircle = isCircle;
}

void Recognition::setInnerSSGDisplayType(int isShowInnerSSG)
{
    this->showInnerSSGs = isShowInnerSSG;
}

//Draws SSGs at terminal nodes
void Recognition::drawSSG(Mat& img, SSG ssg, Point coord)
{
    if(coord.y + params->rec_params.ssg_h > img.size().height)
    {
        copyMakeBorder(img, img, 0, params->rec_params.ssg_h, 0, 0, BORDER_CONSTANT, Scalar(255,255,255));
    }

    if(showPlaceAsCircle)
    {
        circle(img,coord,25,  getPlaceColor(ssg.getColor()), -1);

        //Draw Place ids
        stringstream ss;
        ss.str("");
        ss << ssg.getId();
        if(ssg.getId() < 10)
        {
            Point str_coord(coord.x-10, coord.y+8);
            putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255),2);
        }
        else
        {
            Point str_coord(coord.x-17, coord.y+8);
            putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255),2);
        }

        //        Scalar cat_color = getCategoryColor(getPlaceCategory(ssg.getColor(),ssg.getId()));
        //        //Draw place cats
        //        rectangle(img, Point(coord.x-20,40+coord.y-10), Point(coord.x+20,40+coord.y+20),cat_color, CV_FILLED);
        //        ss.str("");
        //        ss << getPlaceCategory(ssg.getColor(),ssg.getId());
        //        Point str_coord(coord.x-10, 40+coord.y+15);
        //        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255),2);


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
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, getPlaceColor(ssg.getColor()),2);

        circle(img,coord,10,  getPlaceColor(ssg.getColor()), -1);
    }

}

//Draws SSGs at inner nodes
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
    putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, getPlaceColor(ssg.getColor()),2);

    //Predefined size
    resize(img_ssg, img_ssg, cv::Size(params->rec_params.ssg_w,params->rec_params.ssg_h));

    rectangle(img_ssg, Rect(0,0,img_ssg.size().width-1, img_ssg.size().height-1),Scalar(0,0,0));

    Rect roi = Rect(coord.x-img_ssg.size().width/2, coord.y-img_ssg.size().height/2, img_ssg.size().width, img_ssg.size().height);

    Mat image_roi = img(roi);

    img_ssg.copyTo(image_roi);
}

//Draws SSG at terminal nodes with images
void Recognition::drawSSGWithImages(Mat& img, SSG ssg, Point coord)
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
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, getPlaceColor(ssg.getColor()),2);
        circle(img,coord,10,  getPlaceColor(ssg.getColor()), -1);
    }
    else
    {
        string place_folder = getPlaceFolder(ssg.getColor());
        vector<string> img_files = getFiles(place_folder);
        int img_index = (ssg.getEndFrame()+ssg.getStartFrame())/2;
        Mat img_real = imread(place_folder + img_files[img_index]);


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
        putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, getPlaceColor(ssg.getColor()),2);

        circle(img,coord,10,  getPlaceColor(ssg.getColor()), -1);
    }

}

//Draws tree
void Recognition::drawBranch(Mat& img, TreeNode* node, int height, double scale_x, double scale_y)
{
    if(!node->isTerminal())
    {
        for(int i = 0; i < node->getChildren().size(); i++)
        {
            Point top(plot_offset+node->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);
            Point middle(plot_offset+node->getChildren()[i]->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);
            Point bottom(plot_offset+node->getChildren()[i]->getXPos()*scale_x, height - plot_offset - 1 - node->getChildren()[i]->getVal()*scale_y);

            line(img, top, middle, Scalar(0,0,0), 2);
            line(img, middle, bottom, Scalar(0,0,0), 2);

            if(showInnerSSGs)
            {
                Point coord(plot_offset+node->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);
                drawInnerSSG(img, node->getDescriptor()->getMember(0), coord);
            }

            drawBranch(img, node->getChildren()[i], height, scale_x, scale_y);
        }
    }
    else
    {        
        Point coord(plot_offset+node->getXPos()*scale_x, height - plot_offset - 1 - node->getVal()*scale_y);


        //Draw Place id
        stringstream ss;
        ss << node->getLabel();
        Point str_coord(coord.x+18, coord.y-18);
        //putText(img, ss.str(), str_coord, FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0),2);

        //Draw SSGs
        for(int i = 0; i < node->getDescriptor()->getCount(); i++)
        {
            Point ssg_coord(coord.x, coord.y + i*params->rec_params.ssg_h);
            if(showPlaceAsCircle)
                drawSSG(img, node->getDescriptor()->getMember(i), ssg_coord);
            else
                drawSSGWithImages(img, node->getDescriptor()->getMember(i), ssg_coord);
        }

    }
}

//Draws resulting tree into image file
void Recognition::drawTree(TreeNode* root_node, int nrPlaces, int height, int width)
{
    Mat img(height, width, CV_8UC3, Scalar(255,255,255));

    double scale_x = (double) (width - 2*plot_offset) / nrPlaces;
    double scale_y = (double) (height - 2*plot_offset) / root_node->getVal();

    drawBranch(img, root_node, height, scale_x, scale_y);

    emit showTree(mat2QImage(img));

    imwrite(string(OUTPUT_FOLDER) + "tree.jpg",img);
    waitKey(0);
}

//Returns all terminal nodes originated from given root node
void Recognition::getTerminalNodes(TreeNode* node, vector<TreeNode*>& terminal_nodes)
{
    if(node->isTerminal())
    {
        terminal_nodes.push_back(node);
    }

    for(int i = 0; i < node->getChildren().size(); i++)
        getTerminalNodes(node->getChildren()[i], terminal_nodes);
}

//Sort terminal nodes based on depth-first traversal
//Used for cosmetic purposes of tree drawing
void Recognition::sortTerminalNodes(TreeNode* node, int* last_pos)
{
    if(node->isTerminal())
    {
        node->setXPos((*last_pos)++);
    }

    for(int i = 0; i < node->getChildren().size(); i++)
        sortTerminalNodes(node->getChildren()[i], last_pos);
}

//Experimental
//Calculates merged SSGs at inner nodes using SSGs at children nodes
//Caution! This function will produce memory leak unless places deallocated manually!
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

//Converts tree (Node) produced by SLINK algorithm into tree format(TreeNode) used in this work
//Caution! This may produce memory leaks.. Deallocate tree manually later
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

//Returns tree node as specified by its place label
//There may be more than one SSG in a place
TreeNode* Recognition::findNodeWithPlaceLabel(int label, TreeNode* root)
{
    if(label == root->getLabel())
        return root;

    //Depth first traversal
    for(int i = 0; i < root->getChildren().size(); i++){
        TreeNode* p = findNodeWithPlaceLabel(label, root->getChildren()[i]);
        if(p != NULL) return p;
    }
    return NULL;
}

//Returns tree node as specified by its SSG label
//There may be more than one SSG in a place
TreeNode* Recognition::findNodeWithSSGLabel( int site, int label, TreeNode* root)
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
        TreeNode* p = findNodeWithSSGLabel(site, label, root->getChildren()[i]);
        if(p != NULL) return p;
    }
    return NULL;
}

//SSG Voting based distance between two SSGs
double Recognition::calculateDistanceSSGVoting(SSG& old_place, SSG& detected_place)
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

//Calculates BD-Based distance between two SSG
//Calculation methods can be changed from GUI
double Recognition::calculateDistanceTSC(SSG& old_place, SSG& detected_place)
{
    double total_distance = 0;

    //Color is not used
    if(rec_method == REC_TYPE_BD_NORMAL )
    {
        int bd_vec_color_idx_start = 0;
        int bd_vec_filter_idx_start = 100;
        int bd_vec_filter_idx_end = 599;

        Mat oldp = old_place.mean_invariant.rowRange(bd_vec_filter_idx_start,bd_vec_filter_idx_end);
        Mat newp = detected_place.mean_invariant.rowRange(bd_vec_filter_idx_start,bd_vec_filter_idx_end);
        total_distance = norm(oldp, newp);
    }
    //Color information is used
    else if(rec_method == REC_TYPE_BD_COLOR)
    {
        total_distance = norm(old_place.mean_invariant,detected_place.mean_invariant,NORM_L2);
    }
    //Hakan calculated log of bd vector -- this condition is used to reverse taking log process
    else if(rec_method == REC_TYPE_BD_COLOR_LOG)
    {
        Mat a = old_place.mean_invariant*25;
        float const_eps = 2.71;
        cv::pow(a,const_eps,a);

        Mat b = detected_place.mean_invariant*25;
        cv::pow(b,const_eps,b);
        total_distance = norm(a,b,NORM_L2);
    }
    //voting based
    else if(rec_method == REC_TYPE_BD_VOTING)
    {
        float votes = 0;
        for(int i = 0; i < detected_place.member_invariants.size().width; i++)
        {
            if(norm(old_place.mean_invariant,detected_place.member_invariants.col(i),NORM_L2) < params->rec_params.tau_v)
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

//Returns distance between two places
//Calculation method is chosen by the user from GUI
double Recognition::getDistance(PlaceSSG& p1, PlaceSSG& p2)
{
    int count = 0;
    double distance = 0;
    for(int i = 0; i < p1.getCount(); i++)
    {
        for(int j = 0; j < p2.getCount(); j++)
        {
            if(rec_method == REC_TYPE_SSG_NORMAL)
            {
                Mat P, C;
                distance += gm->matchTwoImages(p1.getMember(i),p2.getMember(j),P,C);
            }
            else if(rec_method == REC_TYPE_SSG_VOTING)
            {
                distance += calculateDistanceSSGVoting(p1.getMember(i),p2.getMember(j));
            }
            else if(rec_method == REC_TYPE_BD_NORMAL || rec_method == REC_TYPE_BD_VOTING || rec_method == REC_TYPE_BD_COLOR)
            {
                distance += calculateDistanceTSC(p1.getMember(i),p2.getMember(j));
            }
            count++;
        }
    }

    return distance / count;

}

//Calculates pairwise place distances and return distance matrix
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

            dist_matrix[r][c] = distance;
            dist_matrix[c][r] = distance;
        }
    }

    return dist_matrix;
}

//Performs recognition check for new detected place..
//If new place is recognized, it's merged with the recognized one and tree is not changed(it should be changed actually)
//If new place is not recognized, it's placed into tree using SLINK algorithm
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

    //qDebug() << "New recognition calculation...";
    double** dist_matrix = calculateDistanceMatrix(places);


    //Find hierarchical tree using SLINK algorithm
    Node* tree = solveSLink(nrPlaces, nrPlaces, dist_matrix);

    *hierarchy = *convert2Tree(tree, nrPlaces-1, nrPlaces, places);

    //Draw tree
    drawTree(*hierarchy, nrPlaces, params->rec_params.plot_h, params->rec_params.plot_w);

    //Get pointer to position of new detected place
    TreeNode* new_place_node = findNodeWithPlaceLabel(nrPlaces-1, *hierarchy);

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
            qDebug() << "Recognized!" << closest_node->getDescriptor()->getMember(0).getId() << "->" << new_place_node->getDescriptor()->getMember(0).getId();

            //qDebug() << "Recognized place! " << closest_node->getLabel() << "<-" << new_place_node->getLabel();
            for(int i = 0; i < new_place_node->getDescriptor()->getCount(); i++)
                closest_node->getDescriptor()->addMember(new_place_node->getDescriptor()->getMember(i));

            places.erase(places.end());

            recognition_status = RECOGNIZED;
        }
    }

    //Remove dist_matrix
    for (int i = 0; i < nrPlaces; i++) delete[] dist_matrix[i];
    delete[] dist_matrix;

    return recognition_status;
}

//Performs recognition check for new detected place..
//If new place is recognized, it's merged with the recognized one and tree is not changed(it should be changed actually)
//If new place is not recognized, it's placed into tree using SLINK algorithm
int Recognition::performRecognitionHybridOld(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy)
{
    int recognition_status = NOT_RECOGNIZED;
    //TODO: Incremental distance matrix creation
    //First create distance matrix (We don't need to calculate distance matrix from scratch)

    //We'll push new place into places vector, then erase at the end
    places.push_back(new_place);

    //Hybrid method
    //Calculate best candidates based on SSG method
    //BD based recognition will be applied only on these candidates
    vector<vector<int> > best_ssg_candidates = calculateBestSSGCandidates(places);

    //If there is not enough place for recognition
    if(places.size() < 2)
        return RECOGNITION_ERROR;

    int nrPlaces = places.size();

    //qDebug() << "New recognition calculation...";
    double** dist_matrix = calculateDistanceMatrix(places);


    //Find hierarchical tree using SLINK algorithm
    Node* tree = solveSLink(nrPlaces, nrPlaces, dist_matrix);

    *hierarchy = *convert2Tree(tree, nrPlaces-1, nrPlaces, places);

    //Draw tree
    drawTree(*hierarchy, nrPlaces, params->rec_params.plot_h, params->rec_params.plot_w);

    //Get pointer to position of new detected place
    TreeNode* new_place_node = findNodeWithPlaceLabel(nrPlaces-1, *hierarchy);

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
            vector<int> candidates = best_ssg_candidates[new_place_node->getLabel()];

            //Check if any of candidates is equal recognized place based on BD method
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


//Performs recognition check for new detected place..
//If new place is recognized, it's merged with the recognized one and tree is not changed(it should be changed actually)
//If new place is not recognized, it's placed into tree using SLINK algorithm
int Recognition::performRecognitionHybrid(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy, vector<vector<vector<pair<int, int> > > >& familiarities)
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

    //qDebug() << "New recognition calculation...";
    double** dist_matrix = calculateDistanceMatrix(places);

    //Find hierarchical tree using SLINK algorithm
    Node* tree = solveSLink(nrPlaces, nrPlaces, dist_matrix);

    *hierarchy = *convert2Tree(tree, nrPlaces-1, nrPlaces, places);

    //Draw tree
    drawTree(*hierarchy, nrPlaces, params->rec_params.plot_h, params->rec_params.plot_w);

    //Get pointer to position of new detected place
    TreeNode* new_place_node = findNodeWithPlaceLabel(nrPlaces-1, *hierarchy);


    //Find the ancestor of the detected place which has the highest height but lower than tau_f
    TreeNode* highest_ancestor = new_place_node;

    while(highest_ancestor->getParent() != NULL && highest_ancestor->getParent()->getVal() < params->rec_params.tau_r)
    {
        highest_ancestor = highest_ancestor->getParent();
    }

    //bool containsPair = false;

    vector<vector<pair<int,int> > > familiar_nodes;
    //If highest_ancestor is not NULL then check its terminal offsprings for recognition
    //The familiarity between detected place and terminal offspring nodes is greater than threshold
    //Therefore these are the candidate nodes
    if(highest_ancestor != NULL && highest_ancestor->getLabel() != new_place_node->getLabel())
    {
        //Get terminal offspring nodes of highest ancestor
        vector<TreeNode*> terminal_nodes;
        getTerminalNodes(highest_ancestor, terminal_nodes);

        float best_dissimilarity = -1;
        TreeNode* closest_node = NULL;

        for(int i = 0; i < terminal_nodes.size(); i++)
        {
            TreeNode* node = terminal_nodes[i];

            vector<pair<int, int> > familiar_node_ssgs;
            for(int j = 0; j < node->getDescriptor()->getCount(); j++)
            {
                int site = node->getDescriptor()->getMember(j).getColor();
                int id = node->getDescriptor()->getMember(j).getId();
                familiar_node_ssgs.push_back(make_pair(site,id));
//                bool status = getMatchStatus(node->getDescriptor()->getMember(j).getColor(),
//                                             node->getDescriptor()->getMember(j).getId(),
//                                             new_place_node->getDescriptor()->getMember(0).getColor(),
//                                             new_place_node->getDescriptor()->getMember(0).getId());

//                containsPair |= status;
            }
            familiar_nodes.push_back(familiar_node_ssgs);

            if(node->getLabel() != new_place_node->getLabel())
            {
                //Check at-least-one-common-ssg-node criteria
                if(getNumberMatchedSSGNodes(*new_place_node->getDescriptor(), *node->getDescriptor(), params->rec_params.tau_v))
                {
                    float dissimilarity = getDistance(*new_place_node->getDescriptor(), *terminal_nodes[i]->getDescriptor());
                    if(best_dissimilarity == -1 || dissimilarity < best_dissimilarity )
                    {
                        best_dissimilarity = dissimilarity;
                        closest_node = node;
                    }
                }
            }
        }

        //If closest node is not null, then place is recognized
        if(closest_node != NULL)
        {
            for(int i = 0; i < new_place_node->getDescriptor()->getCount(); i++)
            {
                closest_node->getDescriptor()->addMember(new_place_node->getDescriptor()->getMember(i));
            }

            places.erase(places.end());

            recognition_status = RECOGNIZED;
        }

    }

    if(new_place_node->getDescriptor()->getMember(0).getColor() == SITE_LJ2)
        familiarities.push_back(familiar_nodes);

    //Remove dist_matrix
    for (int i = 0; i < nrPlaces; i++) delete[] dist_matrix[i];
    delete[] dist_matrix;

    return recognition_status;
}


//Performs recognition check for new detected place..
//If new place is recognized, it's merged with the recognized one and tree is not changed(it should be changed actually)
//If new place is not recognized, it's placed into tree using SLINK algorithm
int Recognition::performRecognitionHybridNoRec(vector<PlaceSSG>& places, PlaceSSG new_place, TreeNode** hierarchy, vector<vector<vector<pair<int, int> > > >& familiarities,
                                               vector<pair<pair<int,int>, pair<int,int> > >& recognized_places)
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

    //qDebug() << "New recognition calculation...";
    double** dist_matrix = calculateDistanceMatrix(places);

    //Find hierarchical tree using SLINK algorithm
    Node* tree = solveSLink(nrPlaces, nrPlaces, dist_matrix);

    *hierarchy = *convert2Tree(tree, nrPlaces-1, nrPlaces, places);

    //Draw tree
    drawTree(*hierarchy, nrPlaces, params->rec_params.plot_h, params->rec_params.plot_w);

    //Get pointer to position of new detected place
    TreeNode* new_place_node = findNodeWithPlaceLabel(nrPlaces-1, *hierarchy);


    //Find the ancestor of the detected place which has the highest height but lower than tau_f
    TreeNode* highest_ancestor = new_place_node;

    while(highest_ancestor->getParent() != NULL && highest_ancestor->getParent()->getVal() < params->rec_params.tau_r)
    {
        highest_ancestor = highest_ancestor->getParent();
    }

    //bool containsPair = false;

    vector<vector<pair<int,int> > > familiar_nodes;
    //If highest_ancestor is not NULL then check its terminal offsprings for recognition
    //The familiarity between detected place and terminal offspring nodes is greater than threshold
    //Therefore these are the candidate nodes
    if(highest_ancestor != NULL && highest_ancestor->getLabel() != new_place_node->getLabel())
    {
        //Get terminal offspring nodes of highest ancestor
        vector<TreeNode*> terminal_nodes;
        getTerminalNodes(highest_ancestor, terminal_nodes);

        float best_dissimilarity = -1;
        TreeNode* closest_node = NULL;

        for(int i = 0; i < terminal_nodes.size(); i++)
        {
            TreeNode* node = terminal_nodes[i];

            vector<pair<int, int> > familiar_node_ssgs;
            for(int j = 0; j < node->getDescriptor()->getCount(); j++)
            {
                int site = node->getDescriptor()->getMember(j).getColor();
                int id = node->getDescriptor()->getMember(j).getId();
                familiar_node_ssgs.push_back(make_pair(site,id));
//                bool status = getMatchStatus(node->getDescriptor()->getMember(j).getColor(),
//                                             node->getDescriptor()->getMember(j).getId(),
//                                             new_place_node->getDescriptor()->getMember(0).getColor(),
//                                             new_place_node->getDescriptor()->getMember(0).getId());

//                containsPair |= status;
            }
            familiar_nodes.push_back(familiar_node_ssgs);


            if(node->getLabel() != new_place_node->getLabel())
            {
                //Check at-least-one-common-ssg-node criteria
                if(getNumberMatchedSSGNodes(*new_place_node->getDescriptor(), *node->getDescriptor(), params->rec_params.tau_v))
                {
                    float dissimilarity = getDistance(*new_place_node->getDescriptor(), *terminal_nodes[i]->getDescriptor());
                    if(best_dissimilarity == -1 || dissimilarity < best_dissimilarity )
                    {
                        best_dissimilarity = dissimilarity;
                        closest_node = node;
                    }
                }
            }
        }

        //If closest node is not null, then place is recognized
        if(closest_node != NULL)
        {
            for(int i = 0; i < new_place_node->getDescriptor()->getCount(); i++)
            {
                recognized_places.push_back(make_pair(make_pair(closest_node->getDescriptor()->getMember(0).getColor(), closest_node->getDescriptor()->getMember(0).getId()),
                                                      make_pair(new_place_node->getDescriptor()->getMember(0).getColor(), new_place_node->getDescriptor()->getMember(0).getId())));
            }

            recognition_status = RECOGNIZED;
        }

    }

//    if(new_place_node->getDescriptor()->getMember(0).getColor() == SITE_FR2)
//        familiarities.push_back(familiar_nodes);

    //Remove dist_matrix
    for (int i = 0; i < nrPlaces; i++) delete[] dist_matrix[i];
    delete[] dist_matrix;

    return recognition_status;
}

//Returns the tree
Node* Recognition::solveSLink(int nrows, int ncols, double** data)
{
    Node* tree;

    tree = treecluster(nrows, ncols, 0, 0, 0, 0, 'e', 'a', data);

    //Pre-process tree
    for(int i = 0; i < nrows-1; i++)
    {
        if(tree[i].left < 0)
            tree[i].left = -1*tree[i].left + nrows-1;
        if(tree[i].right < 0)
            tree[i].right = -1*tree[i].right + nrows-1;
    }

    const int nnodes = nrows-1;

    if (!tree)
    {
        qDebug() << "treecluster routine failed due to insufficient memory";
        return NULL;
    }
    else
    {
        //Print out the tree
        //for(int i = 0; i < nnodes; i++)
            //qDebug()<<-i-1<<tree[i].left<<tree[i].right<<tree[i].distance;
        return tree;
    }

    return NULL;
}

//Experimental
//Calculates recognition performance based on BD method
//Prints out the order of correct match for each place
//For example; place 1 is similar to place 9
//and ordered distances of places to place 1 is 1 : 1(self),3,2,9,4...
//then order of correct match is 3 because place 3 and 2 calculated
//to be more similar to place 1(in reality, they are not!)
void Recognition::calculateRecPerformanceDist2Match(vector<PlaceSSG>& places, TreeNode* root)
{
    //Fr
    int site1 = SITE_FR1;
    int site2 = SITE_FR2;
    vector<pair<int, int> > matches = getRevisitMatchesFr();

    //Sa
//    int site1 = SITE_SA1;
//    int site2 = SITE_SA2;
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
//    int site1 = SITE_LJ1;
//    int site2 = SITE_LJ2;
//    vector<pair<int, int> > matches = getRevisitMatchesLj();

    double** dist_matrix = calculateDistanceMatrix(places);

    qDebug() << "Distances to match";
    for(int i = 0; i < matches.size(); i++)
    {
        TreeNode* n1 = findNodeWithSSGLabel(site1, matches[i].first, root);
        TreeNode* n2 = findNodeWithSSGLabel(site2, matches[i].second, root);
        int n1_label = n1->getLabel();
        int n2_label = n2->getLabel();

        float n1n2_dist = dist_matrix[n1_label][n2_label];
        int count = 0;
        for(int j = 0; j < places.size(); j++)
        {
            if(j != n1_label && dist_matrix[n1_label][j] < n1n2_dist)
            {
                count++;
            }
        }
        qDebug() << matches[i].first << "->" << matches[i].second << ":" << count;
    }
}

//Experimental
//Calculate recognition performance if no tree is used
//Only pairwise BD based place distances are used -- Hybrid method(SSG correction is used)
void Recognition::calculateRecPerformanceHybridWithoutTree(vector<PlaceSSG>& places)
{
    //Fr
    int nr_places_site1 = 19;
    vector<pair<int, int> > matches = getRevisitMatchesFr();

    //Sa
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
//    vector<pair<int, int> > matches = getRevisitMatchesLj();

    vector<vector<int> > candidates_mat = calculateBestSSGCandidates(places);

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

        if(best_score < params->rec_params.tau_r)
        {
            if(best_id == matches[i].second+nr_places_site1)
                qDebug() << 1;  //Correct recognition
            else
                qDebug() << -1; //False recognition
        }
        else
        {
            qDebug() << 0;      //No recognition
        }

    }
}

//Experimental
//Calculate N2N values and corresponding h(distance) values of SSG and BD method
void Recognition::calculateRecPerformanceWithHValues(TreeNode* root)
{
    //Fr
    int site1 = SITE_FR1;
    int site2 = SITE_FR2;
    vector<pair<int, int> > matches = getRevisitMatchesFr();

    //Sa
//    int site1 = SITE_SA1;
//    int site2 = SITE_SA2;
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
//    int site1 = SITE_LJ1;
//    int site2 = SITE_LJ2;
//    vector<pair<int, int> > matches = getRevisitMatchesLj();


    qDebug() << "N2N Distances";
    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;

        qDebug() << calculateN2NTreeDistance(findNodeWithSSGLabel(site1, place1, root), findNodeWithSSGLabel(site2, place2, root));
    }

    qDebug() << "SSG H values";
    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;

        Mat P, C;
        qDebug() << gm->matchTwoImages(findNodeWithSSGLabel(site1, place1, root)->getDescriptor()->getMember(0),findNodeWithSSGLabel(site2, place2, root)->getDescriptor()->getMember(0),P,C);
    }

    qDebug() << "BD H Values";

    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;

        qDebug() << calculateDistanceTSC(findNodeWithSSGLabel(site1, place1, root)->getDescriptor()->getMember(0),findNodeWithSSGLabel(site2, place2, root)->getDescriptor()->getMember(0));
    }

}

//Experimental
//Calculate N2N values
void Recognition::calculateN2NRecPerformance(TreeNode* root)
{
    //Fr
    int site1 = SITE_FR1;
    int site2 = SITE_FR2;
    vector<pair<int, int> > matches = getRevisitMatchesFr();

    //Sa
//    int site1 = SITE_SA1;
//    int site2 = SITE_SA2;
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
//    int site1 = SITE_LJ1;
//    int site2 = SITE_LJ2;
//    vector<pair<int, int> > matches = getRevisitMatchesLj();

    qDebug() << "N2N Distances";
    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;

        int dist = calculateN2NTreeDistance(findNodeWithSSGLabel(site1, place1, root), findNodeWithSSGLabel(site2, place2, root));
        qDebug() << (int)(dist == 0);
    }
}

//Experimental
//Calculate N2N values
void Recognition::calculateRecPerformance(TreeNode* root)
{
    //Fr
    int site1 = SITE_FR1;
    int site2 = SITE_FR2;
    vector<pair<int, int> > matches = getRevisitMatchesFr();

    //Sa
//    int site1 = SITE_SA1;
//    int site2 = SITE_SA2;
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
//    int site1 = SITE_LJ1;
//    int site2 = SITE_LJ2;
//    vector<pair<int, int> > matches = getRevisitMatchesLj();

    qDebug() << "N2N Distances";
    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;

        TreeNode* place1_node = findNodeWithSSGLabel(site1, place1, root);
        TreeNode* place2_node = findNodeWithSSGLabel(site2, place2, root);

        if(place1_node->getLabel() == place2_node->getLabel())
        {
            qDebug() << 1;
        }
        else
        {
            if(place2_node->getDescriptor()->getCount() > 1)
                qDebug() << -1;
            else
                qDebug() << 0;
        }
    }
}

//Experimental
void Recognition::calculateRecPerformance(vector<pair<pair<int,int>, pair<int,int> > >& recognized_places)
{
    //Fr
//    int site1 = SITE_FR1;
//    int site2 = SITE_FR2;
//    vector<pair<int, int> > matches = getRevisitMatchesFr();

    //Sa
//    int site1 = SITE_SA1;
//    int site2 = SITE_SA2;
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
    int site1 = SITE_LJ1;
    int site2 = SITE_LJ2;
    vector<pair<int, int> > matches = getRevisitMatchesLj();


    qDebug() << "Rec performance with no rec";

    qDebug() << params->rec_params.tau_r;
    qDebug() << params->rec_params.tau_v;
    qDebug() << params->ssg_params.tau_p;
    qDebug() << "-";

    for(int i = 0; i < matches.size(); i++)
    {
        int isRec = 0;
        for(int j = 0; j < recognized_places.size(); j++)
        {
            if(recognized_places[j].second.first == site2 && recognized_places[j].second.second == matches[i].second)
            {
                if(recognized_places[j].first.first == site1 && recognized_places[j].first.second == matches[i].first)
                {
                    isRec = 1;
                }
                else
                {
                    isRec = -1;
                }
            }
        }
        qDebug() << isRec;
    }
}

//Experimental
void Recognition::calculateRecPerformanceCombined(vector<pair<pair<int,int>, pair<int,int> > >& recognized_places)
{
    int site1;
    int site2;
    vector<pair<int, int> > matches;
    vector<int> sites;
    sites.push_back(SITE_FR2);
    sites.push_back(SITE_SA2);
    sites.push_back(SITE_LJ2);

    qDebug() << "Perf check starts >>>>>";

    for(int s = 0; s < sites.size(); s++)
    {
        int site = sites[s];

        if(site == SITE_FR2)
        {
            site1 = SITE_FR1;
            site2 = SITE_FR2;
            matches = getRevisitMatchesFr();
        }
        else if(site == SITE_SA2)
        {
            site1 = SITE_SA1;
            site2 = SITE_SA2;
            matches = getRevisitMatchesSa();
        }
        else if(site == SITE_LJ2)
        {
            site1 = SITE_LJ1;
            site2 = SITE_LJ2;
            matches = getRevisitMatchesLj();
        }

        qDebug() << "Rec performance with no rec";

        qDebug() << params->rec_params.tau_r;
        qDebug() << params->rec_params.tau_v;
        qDebug() << params->ssg_params.tau_p;
        qDebug() << "-";

        for(int i = 0; i < matches.size(); i++)
        {
            int isRec = 0;
            for(int j = 0; j < recognized_places.size(); j++)
            {
                if(recognized_places[j].second.first == site2 && recognized_places[j].second.second == matches[i].second)
                {
                    if(recognized_places[j].first.first == site1 && recognized_places[j].first.second == matches[i].first)
                    {
                        isRec = 1;
                    }
                    else
                    {
                        isRec = -1;
                    }
                }
            }
            qDebug() << isRec;
        }
    }
}

//Experimental
//Calculate familiarity performance
void Recognition::calculateFamiliarityPerformance(vector<vector<vector<pair<int,int> > > >& familiarities)
{
//    //Fr
//    int site1 = SITE_FR1;
//    int site2 = SITE_FR2;
//    vector<pair<int, int> > matches = getRevisitMatchesFr();

   // Sa
//    int site1 = SITE_SA1;
//    int site2 = SITE_SA2;
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
    int site1 = SITE_LJ1;
    int site2 = SITE_LJ2;
    vector<pair<int, int> > matches = getRevisitMatchesLj();

    qDebug() << "Familiarity performance";

    qDebug() << params->rec_params.tau_r;
    qDebug() << params->rec_params.tau_v;
    qDebug() << params->ssg_params.tau_p;
    qDebug() << "-";
    for(int i = 0; i < matches.size(); i++)
    {
        int place1 = matches[i].first;
        int place2 = matches[i].second;

        bool isFound = false;
        for(int j = 0; j < familiarities[place2].size(); j++)
        {
            for(int k = 0; k < familiarities[place2][j].size(); k++)
            {
                if(familiarities[place2][j][k].second == place1)
                {
                    isFound |= true;
                }
            }

        }

        qDebug() << (int)isFound << "\t" << familiarities[place2].size();
    }
}

//Experimental
//Returns true if two places are same in real
bool Recognition::getMatchStatus(int psite1, int place1, int psite2, int place2)
{
    //Fr
    int site1 = SITE_FR1;
    int site2 = SITE_FR2;
    vector<pair<int, int> > matches = getRevisitMatchesFr();

    //Sa
//    int site1 = SITE_SA1;
//    int site2 = SITE_SA2;
//    vector<pair<int, int> > matches = getRevisitMatchesSa();

    //Lj
//    int site1 = SITE_LJ1;
//    int site2 = SITE_LJ2;
//    vector<pair<int, int> > matches = getRevisitMatchesLj();

    for(int i = 0; i < matches.size(); i++)
    {
        if(psite1 == site1 && psite2 == site2)
        {
            if(matches[i].first == place1 && matches[i].second == place2)
                return true;
        }
        else if(psite1 == site2 && psite2 == site1)
        {
            if(matches[i].first == place2 && matches[i].second == place1)
                return true;
        }
    }

    return false;
}

int Recognition::getNumberMatchedSSGNodes(PlaceSSG place1, PlaceSSG place2, float threshold)
{
    Mat P, C;
    int max_nr_matched_nodes = -1;
    for(int i = 0; i < place1.getCount(); i++)
    {
        for(int j = 0; j < place2.getCount(); j++)
        {
            gm->matchTwoImages(place1.getMember(i),place2.getMember(j),P,C);

            vector<Point> nonzero_locs;
            findNonZero(P,nonzero_locs);

            int nr_matched_nodes = 0;
            for(int k = 0; k < nonzero_locs.size(); k++)
            {
                float cost = C.at<float>(nonzero_locs[k].y, nonzero_locs[k].x);
                if(cost < threshold)
                {
                    nr_matched_nodes++;
                }
            }

            //Keep the maximum number of matches
            if(max_nr_matched_nodes == -1 || max_nr_matched_nodes < nr_matched_nodes )
            {
                max_nr_matched_nodes = nr_matched_nodes;
            }
        }
    }

    return max_nr_matched_nodes;
}

//Returns most similar N places to each place
//Calculation is based on SSG distance
//Candidates are used to verify BD-based recognition is correct
//In general BD based recognition outperforms SSG based recognition, however
//SSG provides useful local information. In this function, if two places share
//a common SSG node, then place is chosen as candidate.
vector<vector<int> > Recognition::calculateBestSSGCandidates(vector<PlaceSSG>& places)
{
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

            int nr_matched_nodes = 0;
            for(int i = 0; i < nonzero_locs.size(); i++)
            {
                float cost = C.at<float>(nonzero_locs[i].y, nonzero_locs[i].x);
                if(cost < params->rec_params.tau_v)
                {
                    nr_matched_nodes++;
                }
            }


            if(nr_matched_nodes > 0)
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

//Experimental
//Prints out N2N Distances for each pair of places(terminal nodes of tree)
void Recognition::calculateN2NTreeDistanceMatrix(TreeNode* root_node)
{
    vector<TreeNode*> all_terminal_nodes;
    getTerminalNodes(root_node, all_terminal_nodes);

    int size = all_terminal_nodes.size();

    Mat distance_matrix = Mat::zeros(size, size, CV_32F);
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            float dist = calculateN2NTreeDistance(all_terminal_nodes[i], all_terminal_nodes[j]);
            distance_matrix.at<float>(all_terminal_nodes[i]->getLabel(),all_terminal_nodes[j]->getLabel()) = dist;
        }
    }

    cerr << distance_matrix << endl;
}

//Experimental
void printCVMat(Mat matrix)
{
    for(int i = 0; i < matrix.size().height; i++)
    {
        for(int j = 0; j < matrix.size().width; j++)
        {
            cout << std::setprecision(2) << std::fixed << matrix.at<float>(i,j) << " ";
        }
        cout << ";" << endl;
    }
    cout << endl;
}


//Experimental
//Prints out SSG Distances for each pair of places(terminal nodes of tree)
void Recognition::calculateSSGDistanceMatrix(TreeNode* root_node)
{
    vector<TreeNode*> all_terminal_nodes;
    getTerminalNodes(root_node, all_terminal_nodes);

    int size = all_terminal_nodes.size();

    qDebug() << "SSG Distance";
    Mat distance_matrix = Mat::zeros(size, size, CV_32F);
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            Mat P, C;
            float dist = gm->matchTwoImages(all_terminal_nodes[i]->getDescriptor()->getMember(0),all_terminal_nodes[j]->getDescriptor()->getMember(0),P,C);
            distance_matrix.at<float>(all_terminal_nodes[i]->getLabel(),all_terminal_nodes[j]->getLabel()) = dist;
        }
    }

    printCVMat(distance_matrix);
    //cerr << distance_matrix << endl;
}


//Experimental
//Prints out BD Distances for each pair of places(terminal nodes of tree)
void Recognition::calculateBDDistanceMatrix(TreeNode* root_node)
{
    vector<TreeNode*> all_terminal_nodes;
    getTerminalNodes(root_node, all_terminal_nodes);

    int size = all_terminal_nodes.size();

    qDebug() << "BD Distance";
    Mat distance_matrix = Mat::zeros(size, size, CV_32F);
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            float dist = calculateDistanceTSC(all_terminal_nodes[i]->getDescriptor()->getMember(0),all_terminal_nodes[j]->getDescriptor()->getMember(0));
            distance_matrix.at<float>(all_terminal_nodes[i]->getLabel(),all_terminal_nodes[j]->getLabel()) = dist;
        }
    }

    printCVMat(distance_matrix);
    //cerr << distance_matrix << endl;
}

//Calculate Node-to-Node tree distance
//Distance is equal to the distance of shortest path from one node to another.
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
