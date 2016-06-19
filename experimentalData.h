#ifndef EXPERIMENTALDATA_H
#define EXPERIMENTALDATA_H

#include <vector>
using namespace std;

#define SITE_FR1 1
#define SITE_SA1 2
#define SITE_LJ1 3
#define SITE_NC1 4
#define SITE_FR2 5
#define SITE_SA2 6
#define SITE_LJ2 7
#define SITE_NC2 8

vector<pair<int, int> > getRevisitMatchesFr();
vector<pair<int, int> > getRevisitMatchesSa();
vector<pair<int, int> > getRevisitMatchesLj();


#endif // EXPERIMENTALDATA_H
