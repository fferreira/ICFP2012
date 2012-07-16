/************************************

    IFCP Contest 2012

    Authors:
        Francisco Ferreira
        Daniel Gutson

    This file is just the main
    .cpp that is responsible to call
    the metaprogram.

*************************************/
#include <iostream>
using namespace std;

#include "engine.h"
#include "map.h"
#include "robots.h"

int main()
{
    // ALL MAGIC HAPPENS HERE!
    typedef R2D2<TheMap>::Result FinalResult;

#ifdef DEBUG
    showMap<TheMap>();

    showActions<PossibleMoves<TheMap, Wait>::Result>();
    cout << endl;
    cout << endl;

    showMap<FinalResult::map>();

    cout << "Final score: " << FinalResult::score << endl;
#endif
    showActions<FinalResult::ActionList>();
    cout << endl;

    return 0;
}

