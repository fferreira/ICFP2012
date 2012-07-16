#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

/*
template <unsigned int _RowSize,
          unsigned int _Rows,
          unsigned int _RobotX,
          unsigned int _RobotY,
          unsigned int _LiftX,
          unsigned int _LiftY,
          unsigned int _Tick = 0u,
*/
struct MapInfo
{
    size_t rowSize;
    size_t rows;
    size_t robotX;
    size_t robotY;
    size_t liftX;
    size_t liftY;
    size_t initialLambdas;

    bool robotFound;
    bool liftFound;

    MapInfo() : rowSize( 0 ), rows( 0 ), initialLambdas( 0 ), robotFound( false ), liftFound( false ) {}
    int dimensionsFromLine( const string& line )
    {
        int ret = 0;
        rows++;
        if ( rowSize == 0 )
            rowSize = line.length();
        else if ( rowSize < line.length() )
        {
            ret = rowSize - line.length();
            rowSize = line.length();
        }
        else if ( rowSize > line.length() )
            ret = rowSize - line.length();

        return ret;
    }

    void setRobot( size_t x )
    {
        if ( robotFound )
            throw "Robot appears more than once.";

        robotFound = true;
        robotX = x;
        robotY = rows-1;
    }

    void setLift( size_t x )
    {
        if ( liftFound )
            throw "Lift appears more than once.";

        liftFound = true;
        liftX = x;
        liftY = rows-1;
    }

    void incLambdas()
    {
        initialLambdas++;
    }
};

static void doOutput( const MapInfo& minfo, const list<string>& output )
{
    cout << "typedef" << endl;
    cout << "Map<" << endl;
    cout << "MapInfo< "
        << minfo.rowSize
        << "u, " << minfo.rows
        << "u, " << minfo.robotX
        << "u, " << minfo.robotY
        << "u, " << minfo.liftX
        << "u, " << minfo.liftY
        << "u, " << minfo.initialLambdas
        << "u >," << endl;
    for ( list<string>::const_iterator it = output.begin(); it != output.end(); ++it )
        cout << *it << endl;

    cout << "NIL" << endl;
    const size_t closingBrackets = minfo.rowSize * minfo.rows;
    for ( size_t i = 0; i < closingBrackets; i++ )
        cout << " >";
    cout << endl << "> TheMap;" << endl;
}

static void expandOutput( list<string>& output, size_t amount )
{
    string toExpand;
    for ( size_t i=0; i < amount; i++ )
        toExpand += "Cons< Wall, ";

    for ( list<string>::iterator it = output.begin(); it != output.end(); ++it )
        *it += toExpand;
}

int main()
{
    MapInfo minfo;
    string line;
    int ret = EXIT_SUCCESS;
    list<string> output;
    const char* toAdd;
    size_t currentX;
    string outputLine;
    bool finished = false;

    try
    {
        while( getline( cin, line ) && !finished)
        {
            if ( !line.empty() )
            {
                outputLine.clear();
                const int expand = minfo.dimensionsFromLine( line );
                if ( expand < 0 )
                    expandOutput( output, size_t(-expand) );
                else if ( expand > 0 )
                    line += string(size_t(expand), '#');

                toAdd = NULL;
                currentX = 0;
                for ( string::const_iterator it = line.begin(); it != line.end(); ++it )
                {
                    switch (*it)
                    {
                        case '#': toAdd = "Wall"; break;
                        case '.': toAdd = "Earth"; break;
                        case ' ': toAdd = "Empty"; break;
                        case '@':
                        case '*': toAdd = "Rock"; break;
                        case '\\':
                            toAdd = "Lambda";
                            minfo.incLambdas();
                            break;

                        case 'R':
                            toAdd = "Robot";
                            minfo.setRobot( currentX );
                            break;

                        case 'L':
                            toAdd = "ClosedLift";
                            minfo.setLift( currentX );
                            break;

                        default:
                            toAdd = "Wall";
                    }

                    if ( toAdd != NULL )
                    {
                        outputLine += "Cons< ";
                        outputLine += toAdd;
                        outputLine += ", ";
                    }
                    currentX++;
                }
                output.push_back( outputLine );
            }
            else
                finished = !output.empty(); // if we already read anything, then we're done.
        }

        doOutput( minfo, output );
    }
    catch( const char* err )
    {
        cerr << err << endl;
        ret = EXIT_FAILURE;
    }

    return ret;
}

