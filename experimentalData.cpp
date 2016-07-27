#include "experimentalData.h"

vector<pair<int, int> > getRevisitMatchesFr()
{
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

    return matches;
}

vector<pair<int, int> > getRevisitMatchesSa()
{
    vector<pair<int, int> > matches;
    matches.push_back(make_pair(0, 0 ));
    matches.push_back(make_pair(1, 1 ));
    matches.push_back(make_pair(1, 2 ));
    matches.push_back(make_pair(2, 3 ));
    matches.push_back(make_pair(3, 4 ));
    matches.push_back(make_pair(4, 4 ));
    matches.push_back(make_pair(5, 5 ));
    matches.push_back(make_pair(6, 6 ));
    matches.push_back(make_pair(7, 7));
    matches.push_back(make_pair(8, 8));
    matches.push_back(make_pair(9, 10));
    matches.push_back(make_pair(10, 11));
    return matches;
}

vector<pair<int, int> > getRevisitMatchesLj()
{
    vector<pair<int, int> > matches;
    matches.push_back(make_pair(0, 0));
    matches.push_back(make_pair(1, 1));
    matches.push_back(make_pair(2, 2));
    matches.push_back(make_pair(3, 3));
    matches.push_back(make_pair(4, 4));
    matches.push_back(make_pair(5, 4));
    matches.push_back(make_pair(7, 5));
    matches.push_back(make_pair(8, 7));
    matches.push_back(make_pair(8, 8));
    matches.push_back(make_pair(9, 9));
    matches.push_back(make_pair(9, 10));
    matches.push_back(make_pair(10, 12));
    matches.push_back(make_pair(11, 12));
    matches.push_back(make_pair(12, 12));
    matches.push_back(make_pair(13, 12));
    matches.push_back(make_pair(14, 12));
    matches.push_back(make_pair(16, 13));
    matches.push_back(make_pair(17, 15));
    matches.push_back(make_pair(19, 17));
    matches.push_back(make_pair(20, 17));
    matches.push_back(make_pair(21, 17));
    matches.push_back(make_pair(23, 19));
    matches.push_back(make_pair(23, 20));
    matches.push_back(make_pair(24, 21));
    matches.push_back(make_pair(25, 22));
    matches.push_back(make_pair(27, 23));
    matches.push_back(make_pair(28, 24));
    return matches;
}

char getPlaceCategory(int site, int place)
{
    vector<char> fr1_cats;
    fr1_cats.push_back('C');
    fr1_cats.push_back('R');
    fr1_cats.push_back('R');
    fr1_cats.push_back('C');
    fr1_cats.push_back('R');
    fr1_cats.push_back('C');
    fr1_cats.push_back('R');
    fr1_cats.push_back('R');
    fr1_cats.push_back('C');
    fr1_cats.push_back('C');
    fr1_cats.push_back('C');
    fr1_cats.push_back('C');
    fr1_cats.push_back('B');
    fr1_cats.push_back('B');
    fr1_cats.push_back('B');
    fr1_cats.push_back('B');
    fr1_cats.push_back('C');
    fr1_cats.push_back('C');
    fr1_cats.push_back('C');

    vector<char> sa1_cats;
    sa1_cats.push_back('H');
    sa1_cats.push_back('H');
    sa1_cats.push_back('H');
    sa1_cats.push_back('C');
    sa1_cats.push_back('C');
    sa1_cats.push_back('R');
    sa1_cats.push_back('R');
    sa1_cats.push_back('C');
    sa1_cats.push_back('R');
    sa1_cats.push_back('R');
    sa1_cats.push_back('C');

    vector<char> lj1_cats;
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('R');
    lj1_cats.push_back('R');
    lj1_cats.push_back('R');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('C');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');
    lj1_cats.push_back('H');

    vector<char> nc1_cats;

    vector<char> fr2_cats;
    fr2_cats.push_back('C');
    fr2_cats.push_back('R');
    fr2_cats.push_back('C');
    fr2_cats.push_back('R');
    fr2_cats.push_back('C');
    fr2_cats.push_back('R');
    fr2_cats.push_back('R');
    fr2_cats.push_back('R');
    fr2_cats.push_back('C');
    fr2_cats.push_back('R');
    fr2_cats.push_back('C');
    fr2_cats.push_back('R');
    fr2_cats.push_back('R');
    fr2_cats.push_back('C');
    fr2_cats.push_back('C');
    fr2_cats.push_back('B');
    fr2_cats.push_back('B');
    fr2_cats.push_back('B');
    fr2_cats.push_back('C');
    fr2_cats.push_back('C');
    fr2_cats.push_back('C');
    fr2_cats.push_back('C');
    fr2_cats.push_back('C');

    vector<char> sa2_cats;
    sa2_cats.push_back('H');
    sa2_cats.push_back('H');
    sa2_cats.push_back('H');
    sa2_cats.push_back('H');
    sa2_cats.push_back('C');
    sa2_cats.push_back('C');
    sa2_cats.push_back('C');
    sa2_cats.push_back('R');
    sa2_cats.push_back('R');
    sa2_cats.push_back('R');
    sa2_cats.push_back('R');
    sa2_cats.push_back('C');
    sa2_cats.push_back('R');
    sa2_cats.push_back('R');
    sa2_cats.push_back('R');
    sa2_cats.push_back('R');
    sa2_cats.push_back('C');

    vector<char> lj2_cats;
    lj2_cats.push_back('H');
    lj2_cats.push_back('H');
    lj2_cats.push_back('H');
    lj2_cats.push_back('H');
    lj2_cats.push_back('C');
    lj2_cats.push_back('R');
    lj2_cats.push_back('R');
    lj2_cats.push_back('R');
    lj2_cats.push_back('R');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('H');
    lj2_cats.push_back('H');
    lj2_cats.push_back('H');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('C');
    lj2_cats.push_back('H');
    lj2_cats.push_back('H');
    lj2_cats.push_back('H');

    vector<char> nc2_cats;

    vector<vector<char> > place_cats;
    place_cats.push_back(fr1_cats);
    place_cats.push_back(sa1_cats);
    place_cats.push_back(lj1_cats);
    place_cats.push_back(nc1_cats);
    place_cats.push_back(fr2_cats);
    place_cats.push_back(sa2_cats);
    place_cats.push_back(lj2_cats);
    place_cats.push_back(nc2_cats);


    return place_cats[site-1][place];
}

//Returns color of place
cv::Scalar getPlaceColor(int place)
{
    switch(place)
    {
        case SITE_FR1:
            return cv::Scalar(255,0,0);     //blue      -- fr
        case SITE_SA1:
            return cv::Scalar(0,0,0);       //black     -- sa
        case SITE_LJ1:
            return cv::Scalar(0,140,255);   //orange    -- lj
        case SITE_NC1:
            return cv::Scalar(0,255,0);     //green     -- nc
        case SITE_FR2:
            return cv::Scalar(255,255,0);   //aqua      -- fr2
        case SITE_SA2:
            return cv::Scalar(103,101,98);  //gray      -- sa2
        case SITE_LJ2:
            return cv::Scalar(159,231,249); //yellow    -- lj2
        case SITE_NC2:
            return cv::Scalar(191,223,169); //l. green  -- nc2
        case SITE_CV:
            return cv::Scalar(0,0,255); //red camvid
        case SITE_J1:
            return cv::Scalar(0,0,0); //jag 1
        case SITE_J2:
            return cv::Scalar(103,101,98); //jag 2
        case SITE_JM:
            return cv::Scalar(0,0,0); //jag mahmut
        case SITE_JM2:
            return cv::Scalar(0,0,255); //jag mahmut 2
        default:
            return cv::Scalar(255,255,255);
    }
}

string getPlaceFolder(int place)
{
    switch(place)
    {
        case SITE_FR1:
            return "/home/isl-mahmut/Datasets/fr_seq2_cloudy1/std_cam/";
        case SITE_SA1:
            return "/home/isl-mahmut/Datasets/sa_seq1_cloudy1/std_cam/";
        case SITE_LJ1:
            return "/home/isl-mahmut/Datasets/lj_seq1_cloudy1/std_cam/";
        case SITE_NC1:
            return "/home/isl-mahmut/Datasets/nc/";
        case SITE_FR2:
            return "/home/isl-mahmut/Datasets/fr_seq2_cloudy2/std_cam/";
        case SITE_SA2:
            return "/home/isl-mahmut/Datasets/sa_seq1_cloudy2/std_cam/";
        case SITE_LJ2:
            return "/home/isl-mahmut/Datasets/lj_seq1_cloudy2/std_cam/";
        case SITE_NC2:
            return "/home/isl-mahmut/Datasets/nc/";
        case SITE_CV:
            return "/home/isl-mahmut/Datasets/CamVid/";
        case SITE_J1:
            return "/home/isl-mahmut/Datasets/jag_tour1/";
        case SITE_J2:
            return "/home/isl-mahmut/Datasets/jag_tour2/";
        case SITE_JM:
            return "/home/isl-mahmut/Datasets/jaguar/";
        case SITE_JM2:
            return "/home/isl-mahmut/Datasets/jaguar2/";
        default:
            return "";
    }
}

cv::Scalar getCategoryColor(char cat)
{
    switch(cat)
    {
        case 'C':
            return cv::Scalar(0, 102, 153); //brown
        case 'R':
            return cv::Scalar(0, 0, 128); //red
        case 'H':
            return cv::Scalar(255, 102, 51); //blueish
        case 'B':
            return cv::Scalar(153, 153, 102); //greenish
        default:
            return cv::Scalar(255,255,255);
    }
}



