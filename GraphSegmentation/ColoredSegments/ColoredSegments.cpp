#include <stdio.h>
#include <cstdlib>
#include "HEADER.h"
#include "coloredSegments.h"
#include <QDebug>

using namespace std;
using namespace cv;

int labelColors[10000][2]={0};
int myClrs[TOTAL_COLOR_Nr+1][3];
int dummy[10000][3];

vector<Point> contourArray;
vector<vector<Point> > imageContours;
vector<int> segmentClrs;

list<hsvc> col_hash_map;
coor c;

ImData myImData;
time_t tstart, tend, mstart, mend;


stringstream imgstream,mystream,seq_stream,imgstr,segmentstream,filterstream;

//Static array
int label[pyrHeight][pyrWidth]={0};
int I[pyrHeight][pyrWidth]={0};
int Q[pyrHeight][pyrWidth+1]={0};
	
int EQ[MAX_EXP_NrOf_LABELS]={0};
int labelNr;

void colorImg(vector<Mat> channels, Mat ch0, Mat ch1, Mat ch2, int I[pyrHeight][pyrWidth],int imgnr){
    channels.clear();

    mycolors::myClrs;

    for ( int colorindex = 1; colorindex<TOTAL_COLOR_Nr+1; colorindex++)
    {
        for(int i = 0; i < myImData.h; i++) {
            for(int j = 0; j < myImData.w; j++) {
                int colorval= I[i][j];

                if (colorval == colorindex){
                    ch2.at<uchar>(i,j)= mycolors::myClrs[colorindex][0];
                    ch1.at<uchar>(i,j)= mycolors::myClrs[colorindex][1];
                    ch0.at<uchar>(i,j)= mycolors::myClrs[colorindex][2];
                }
            }
        }
    }
    Mat fin_img2;
    channels.push_back(ch0); //b
    channels.push_back(ch1); //g
    channels.push_back(ch2); //r
    merge(channels, fin_img2);

    //	return fin_img2;
    imgstr << "colored_by_respected_colors_"<< imagename1 << quant  << imgnr << imtype;
    string myfilename = imgstr.str();
    imgstr.str("");
    imwrite(myfilename, fin_img2);
    ch0.release();
    ch1.release();
    ch2.release();
    fin_img2.release();
    channels.clear();
}	


void Labeling(int &labelNr,int label[pyrHeight][pyrWidth],int I[pyrHeight][pyrWidth],int Q[pyrHeight][pyrWidth+1],int EQ[MAX_EXP_NrOf_LABELS],ImData &myImData){
    // Label (0,0) start point
    int L = 0;
    labelNr =0;
    ++L; ++labelNr;
    EQ[L]=(L);
    label[0][0]=L; Q[0][1]=L;

    // Label first row
    for(int x=1; x<myImData.w; x++){

        int y=0;
        int n1x=x-1;

        if(I[y][n1x]==I[y][x]){
            label[y][x]=label[y][n1x];
            Q[y][x+1]=label[y][x];
        }
        if(I[y][n1x]!=I[y][x]){
            ++L; ++labelNr;
            EQ[L]=(L);
            label[y][x]= L;
            Q[y][x+1]=L;
        }
    }

    // Label first column starting from second row

    for(int y=1; y<myImData.h; y++){
        if(I[y][0]==I[y-1][0]){
            label[y][0]=label[y-1][0];
            Q[y][1]=label[y][0];
        }

        if(I[y][0]!=I[y-1][0]){
            ++L; ++labelNr;
            EQ[L]=(L);
            label[y][0]=L;
            Q[y][1]=label[y][0];
        }
    }

    //Label the rest of the img

    for(int x=1; x<myImData.w; x++){
        for(int y=1; y<myImData.h; y++){


            int sx= x-1; int sy=y;
            int tx=x;	 int ty=y-1;

            if(I[y][x]==I[sy][sx]	&&	I[y][x]!=I[ty][tx]){
                label[y][x] = label[sy][sx];
            }

            if(I[y][x]!=I[sy][sx]	&&	I[y][x]==I[ty][tx]){
                label[y][x] = label[ty][tx];
            }

            if(I[y][x]!=I[sy][sx]	&&	I[y][x]!=I[ty][tx]){
                ++L; ++labelNr;
                EQ[L]=(L);
                label[y][x] = L;
            }

            if(I[y][x]==I[sy][sx]	&&	I[y][x]==I[ty][tx]	&&	label[sy][sx]==label[ty][tx]){
                label[y][x] = label[ty][tx];
            }

            if(I[y][x]==I[sy][sx]	&&	I[y][x]==I[ty][tx]	&&	label[sy][sx]!=label[ty][tx]){
                int comp = (label[sy][sx]<label[ty][tx]); // Ls < Lt -->  1
                // Ls > Lt -->  0
                int L1,L2; //L1<L2
                comp ? L1 = label[sy][sx] : L1 = label[ty][tx];
                comp ? L2 = label[ty][tx] : L2 = label[sy][sx];

                label[y][x]=L1;
                EQ[L2]=L1;
            }
            Q[y][x+1]=label[y][x];

        }
    }

    for(int i=0; i<myImData.h; i++){
        Q[i][0]=label[i][1];
    }
}

void LabelEqualization(int EQ[MAX_EXP_NrOf_LABELS],int label[pyrHeight][pyrWidth],ImData &myImData, int labelColors[10000][2]){
    //Equalization of labels
    for(int k =1; k<MAX_EXP_NrOf_LABELS; k++){

    if (EQ[k]==0){break;}

    if(EQ[k]!=k){
        EQ[EQ[k]] == EQ[k] ? 1 :  EQ[k]=EQ[EQ[EQ[k]]];
    }

    for(int i = 0; i < myImData.h; i++) {
        for(int j = 0; j < myImData.w; j++) {
            if(label[i][j] == k)
                label[i][j] = EQ[k];
                Q[i][j+1]=label[i][j];

                //labelColors[label[i][j]][0] = I[i][j];
            }
        }
    }
}

void createHash(string dy){

    string line;
    ifstream myfile (dy.c_str());

    while ( getline (myfile,line) )
    {
        stringstream   linestream(line);
        string         data;
        int hl,hh,sl,sh,vl,vh,color_name;// HueLow, HueHigh, SaturationLow, SaturationHigh, ValueLow, ValueHigh

        getline(linestream, data, '\t');

        linestream >> hh >> sl >> sh >> vl >> vh >> color_name;
        hsvc new_hsvc;
        new_hsvc.hlow = atoi(data.c_str());
        new_hsvc.hhigh = hh;
        new_hsvc.slow = sl;
        new_hsvc.shigh = sh;
        new_hsvc.vlow = vl;
        new_hsvc.vhigh = vh;
        new_hsvc.col_name = color_name;

        col_hash_map.push_back(new_hsvc);

    }
    myfile.close();
}

void keepcolors(string clrs){
    int cidx = 0;
    string line;
    ifstream file (clrs.c_str());

    while ( getline (file,line) )
    {
        stringstream   linestream(line);
        string        data;
        int r,g,b;

        getline(linestream, data, '\t');
        linestream  >> g >> b;

        /* Array implementation*/
        dummy[cidx][0]= atoi(line.c_str());
        dummy[cidx][1]= g;
        dummy[cidx][2]= b;

        ++cidx;

    }
    file.close();
}



Mat segmentImageGokce(Mat src)
{
    LUT ::setArray();

    keepcolors("SegmentColors.txt");
    createHash("ColorQuantas.txt");


    Mat dst = src;

    bilateralFilter (dst,myImData.original, 9, 30, 30 );



    vector<Mat> rgb_channels;
    split(myImData.original,rgb_channels);

    cvtColor(myImData.original, myImData.intensity, CV_BGR2GRAY);
    cvtColor(myImData.original, myImData.hsvImg,  CV_BGR2HSV);

    myImData.h = myImData.original.rows;
    myImData.w = myImData.original.cols;

    vector<Mat> hsvchannels;
    split(myImData.hsvImg,hsvchannels);

    myImData.hsv_filter.push_back(hsvchannels[0]);
    myImData.hsv_filter.push_back(hsvchannels[1]);
    myImData.hsv_filter.push_back(hsvchannels[2]);


    hsvchannels.clear();

    for(int k = 0; k < ( myImData.h * myImData.w); k++){
        int x = k %  (myImData.w);
        int y = (k - x) % (myImData.w - 1);
        bool flag=false;

        for (list<hsvc>::iterator it=col_hash_map.begin(); it != col_hash_map.end(); ++it){
            int val_h = myImData.hsv_filter.at(0).at<uchar>(y,x);
            int val_s = myImData.hsv_filter.at(1).at<uchar>(y,x);
            int val_v = myImData.hsv_filter.at(2).at<uchar>(y,x);


            if(val_h >= it->hlow && val_h <= it->hhigh && val_s >= it->slow &&
               val_s <= it->shigh && val_v >= it->vlow && val_v <= it->vhigh){

                I[y][x]=it->col_name;

                flag=true; break;
            }

        }
    }

    Labeling(labelNr,label, I, Q ,EQ, myImData);

    LabelEqualization(EQ, label, myImData, labelColors);

    /*Merge small components with their nearest component*/
    std::map<int, int> occurrences;

    for (int i = 0; i < myImData.h; ++i){
        for(int j = 0; j < myImData.w; ++j){
            ++occurrences[label[i][j]];
        }
    }

    for (int i = 0; i < myImData.h; ++i){
        for(int j = 0; j < myImData.w; ++j){
            if(occurrences[label[i][j]] < MAX_PxNr_SMALL_AREA) {
                EQ[label[i][j]] = Q[i][j];
                Q[i][j+1] = Q[i][j];
            }
        }
    }
    occurrences.clear();

    // LabelEqualization(EQ, label, myImData,labelColors);
    vector<int> nIndx;
    int indx=1;

    while (indx!=labelNr+1){
        contourArray.clear();
        for(int i = 0; i < myImData.h; i++) {
            for(int j = 0; j < myImData.w; j++) {

                int val=label[i][j];
                if (val == indx){
                    contourArray.push_back(Point(j,i));
                }
            }
        }
        if(contourArray.empty() == false){

            myImData.connComp.push_back(contourArray);
            int clrv;

            // int clrv = I[contourArray.at(0).y][contourArray.at(0).x]; //yanlýþ renk birleþtirme olmasýn diye deðiþtiriyorum burayý 07.10.2015
            if(contourArray.size() == 1){
                clrv = I[contourArray.at(0).y][contourArray.at(0).x];
            }
            if(contourArray.size()>1){
                clrv = I[contourArray.at(contourArray.size()-1).y][contourArray.at(contourArray.size()-1).x];
            }

            segmentClrs.push_back(clrv);

            if(contourArray.size()> MIN_PxNr_BIG_AREA){
                nIndx.push_back(myImData.connComp.size()-1);
            }

        }
        ++indx;
    }

    for (int nfc = 0; nfc< nIndx.size(); nfc++ ){

        int numberofcomponents = nIndx.at(nfc);
        Mat component_Img = Mat::zeros(myImData.h,myImData.w,CV_8UC1);
        Mat dilated_component_Img,dst;
        Mat eroded_;
        // Create binary image of big segment
        for(int comp =0; comp < myImData.connComp.at(numberofcomponents).size(); comp++){
            Point component = myImData.connComp.at(numberofcomponents).at(comp);
            component_Img.at<uchar>(component.y,component.x)=255;
        }

        dilate(component_Img,dilated_component_Img,dilation_element);

        // Obtain adjacent parts
        cv::bitwise_xor(component_Img,dilated_component_Img,dst);
        //imshow("dst", dst); //adjacent img
        //	waitKey(0);


        vector<Point> nonZeroCoordinates;		//keep adjacent pixels in here

        findNonZero(dst, nonZeroCoordinates);

        int	ColorNr1 = segmentClrs.at(numberofcomponents);
        int ColorNr2;
        int newLabel = label[myImData.connComp.at(numberofcomponents).at(0).y][myImData.connComp.at(numberofcomponents).at(0).x];

        for(int g = 0; g<nonZeroCoordinates.size(); g++){
            Point AdjPoint	= nonZeroCoordinates.at(g);
            ColorNr2	= I[AdjPoint.y][AdjPoint.x];
            if(LUT::relationLUT[ColorNr1][ColorNr2] == 1){
                EQ[label[AdjPoint.y][AdjPoint.x]] = newLabel;
            }

        }

    }

    LabelEqualization(EQ, label, myImData, labelColors);


    /*	To see the most current components again push back components and visualise */
    //vector<Mat> mychannels;
    //Mat fin_img;

    //Mat ch0 =  Mat::zeros(myImData.h,myImData.w,CV_8UC1); //B
    //Mat ch1 =  Mat::zeros(myImData.h,myImData.w,CV_8UC1); //G
    //Mat ch2 =  Mat::zeros(myImData.h,myImData.w,CV_8UC1); //R

    myImData.connComp.clear();

    vector<vector<double> > segAvgRGB;
    vector<double> AvgRGB;



    int nindx=1;
    while (nindx!=labelNr+1){

        int  AvgR = 0, AvgG = 0, AvgB = 0;
        double sumR = 0, sumG = 0, sumB = 0; //dumValR,	dumValG, dumValB,

        contourArray.clear();
        for(int i = 0; i < myImData.h; i++) {
            for(int j = 0; j < myImData.w; j++) {
                int nval=label[i][j];
                if (nval == nindx){
                    contourArray.push_back(Point(j,i));

                    double dumValR = rgb_channels.at(2).at<uchar>(i,j);
                    sumR = sumR + dumValR;

                    double dumValG = rgb_channels.at(1).at<uchar>(i,j);
                    sumG = sumG + dumValG;

                    double dumValB = rgb_channels.at(0).at<uchar>(i,j);
                    sumB = sumB + dumValB;

                    //ch2.at<uchar>(i,j)= dummy[(nindx)][0];
                    //ch1.at<uchar>(i,j)= dummy[(nindx)][1];
                    //ch0.at<uchar>(i,j)= dummy[(nindx)][2];
                }
            }
        }
        if(contourArray.empty() == false){
            AvgRGB.clear();
            myImData.connComp.push_back(contourArray);

                    AvgRGB.push_back(sumR / contourArray.size());
                    AvgRGB.push_back(sumG / contourArray.size());
                    AvgRGB.push_back(sumB / contourArray.size());

                    segAvgRGB.push_back(AvgRGB);


                    //AvgR = sumR / contourArray.size();
                    //AvgG = sumG / contourArray.size();
                    //AvgB = sumB / contourArray.size();
        }
        ++nindx;
    }

	vector<Mat> AvgImageLayers;
	Mat AvgImage;
	Mat layerR	=	Mat::zeros(myImData.h,myImData.w,CV_8UC1);
	Mat layerG	=	Mat::zeros(myImData.h,myImData.w,CV_8UC1);
	Mat layerB	=	Mat::zeros(myImData.h,myImData.w,CV_8UC1);

	for(int segment_no= 0; segment_no < myImData.connComp.size(); segment_no++)
	{
		for (int d =0; d<myImData.connComp.at(segment_no).size(); d++)
		{
			layerR.at<uchar>(myImData.connComp.at(segment_no).at(d).y,myImData.connComp.at(segment_no).at(d).x) = segAvgRGB.at(segment_no).at(0);
            layerG.at<uchar>(myImData.connComp.at(segment_no).at(d).y,myImData.connComp.at(segment_no).at(d).x) = segAvgRGB.at(segment_no).at(1);
            layerB.at<uchar>(myImData.connComp.at(segment_no).at(d).y,myImData.connComp.at(segment_no).at(d).x) = segAvgRGB.at(segment_no).at(2);
		}

	}
	AvgImageLayers.push_back(layerB);
    AvgImageLayers.push_back(layerG);
    AvgImageLayers.push_back(layerR);

	merge(AvgImageLayers,AvgImage);

    //mychannels.push_back(ch0); //b
    //mychannels.push_back(ch1); //g
    //mychannels.push_back(ch2); //r
    //merge(mychannels, fin_img);

    //Write out segmented image
    imwrite("img_out_gokce.jpg", AvgImage);


    //colorImg(mychannels,  ch0, ch1,  ch2,  I,0);


    nindx = 0;
    nIndx.clear();
    for (int ph=0; ph<pyrHeight; ph++){
        for(int pw=0; pw<pyrWidth; pw++){
            label[ph][pw]=0;
            I[ph][pw]=0;
        }
    }
    for (int ph=0; ph<pyrHeight; ph++){
        for(int pw=0; pw<pyrWidth+1; pw++){
            Q[ph][pw]=0;
        }
    }

    for(int eqn=0; eqn<MAX_EXP_NrOf_LABELS; eqn++){
        EQ[eqn]=0;
    }

    segmentClrs.clear();
    contourArray.clear();

    AvgImageLayers.clear();
    layerR.release();
    layerG.release();
    layerB.release();

    segAvgRGB.clear();

    //mychannels.clear();

    //ch0.release();
    //ch1.release();
    //ch2.release();


    myImData.hsv_filter.clear();

    myImData.connComp.clear();
    myImData.original.release();
    myImData.intensity.release();
    myImData.hsvImg.release();
    labelNr=0;
    return AvgImage;
}
