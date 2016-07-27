#ifndef EXPERIMENTALDATA_H
#define EXPERIMENTALDATA_H

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
using namespace std;

#define SITE_FR1 1
#define SITE_SA1 2
#define SITE_LJ1 3
#define SITE_NC1 4
#define SITE_FR2 5
#define SITE_SA2 6
#define SITE_LJ2 7
#define SITE_NC2 8
#define SITE_CV 9
#define SITE_J1 10
#define SITE_J2 11
#define SITE_JM 13
#define SITE_JM2 14

vector<pair<int, int> > getRevisitMatchesFr();
vector<pair<int, int> > getRevisitMatchesSa();
vector<pair<int, int> > getRevisitMatchesLj();
char getPlaceCategory(int site, int place);
cv::Scalar getPlaceColor(int place);
string getPlaceFolder(int place);
cv::Scalar getCategoryColor(char cat);


#endif // EXPERIMENTALDATA_H
