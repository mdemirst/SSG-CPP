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
    matches.push_back(make_pair(4, 5 ));
    matches.push_back(make_pair(6, 5 ));
    matches.push_back(make_pair(7, 6 ));
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
    matches.push_back(make_pair(6, 5));
    matches.push_back(make_pair(7, 6));
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
    matches.push_back(make_pair(22, 19));
    matches.push_back(make_pair(23, 20));
    matches.push_back(make_pair(24, 21));
    matches.push_back(make_pair(25, 22));
    matches.push_back(make_pair(27, 23));
    matches.push_back(make_pair(28, 24));
    return matches;
}

