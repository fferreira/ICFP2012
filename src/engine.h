/************************************

    IFCP Contest 2012

    Authors:
        Francisco Ferreira
        Daniel Gutson

    This file contains the
    MAIN metaprogram code for the
    game engine. Actually, contains
    the definitions and some library
    metafunctions that the robots
    will use to implement their
    algorithms.

*************************************/

class NIL {};

enum GameStatus
{
    GoOn,
    RobotWon,
    RobotAborted,
    GameOver
};

enum CellTypes
{
    Robot,
    Rock,
    ClosedLift,
    Earth,
    Wall,
    Lambda,
    OpenLift,
    Empty
};

template <CellTypes _Head, class _Tail>
struct Cons
{
    static const CellTypes Head = _Head;
    typedef _Tail Tail;
};

enum Actions
{
    Left,
    Right,
    Up,
    Down,
    Wait,
    Abort
};

template <Actions action>
struct RevertAction
{
    static const Actions value =
        action == Left ? Right :
        action == Right ? Left :
        action == Up ? Down :
        action == Down ? Up :
        action;
};

template <Actions _Head, class _Tail>
struct ConsActions
{
    static const Actions Head = _Head;
    typedef _Tail Tail;
};

template <unsigned int _RowSize,
          unsigned int _Rows,
          unsigned int _RobotX,
          unsigned int _RobotY,
          unsigned int _LiftX,
          unsigned int _LiftY,
          unsigned int _InitialLambdas,
          unsigned int _RemainingLambdas = _InitialLambdas,
          unsigned int _Tick = 0u,
          unsigned int _Water = 0u
>
struct MapInfo
{
    enum
    {
        RowSize          = _RowSize,
        Rows             = _Rows,
        RobotX           = _RobotX,
        RobotY           = _RobotY,
        LiftX            = _LiftX,
        LiftY            = _LiftY,
        InitialLambdas   = _InitialLambdas,
        RemainingLambdas = _RemainingLambdas,
        TotalPositions   = _RowSize * _Rows,
        Tick             = _Tick,
        Water            = _Water
    };
};

template <class _MapInfo, class _Cons>
struct Map
{
    typedef _MapInfo MapInfo;
    typedef _Cons Cons;
};

///////////////////////////////////////////

template <unsigned int T>
inline void showCell()
{
    static const char CellChars[] = "R*L.#\\O ";
    const char cc[2] = { CellChars[T], 0 };
    cout << cc;
}

template <unsigned int RowSize, unsigned int ColumnsLeft, class Row>
struct ShowRow
{
    static void print()
    {
        showCell<Row::Head>();
        ShowRow<RowSize, ColumnsLeft - 1, typename Row::Tail>::print();
    }
};

template <unsigned int RowSize, class Row>
struct ShowRow<RowSize, 0u, Row>
{
    static void print()
    {
        cout << endl;
        ShowRow<RowSize, RowSize, Row>::print();
    }
};

template<unsigned int RowSize>
struct ShowRow<RowSize, 0u, NIL>
{
    static void print()
    {
        cout << endl;
    }
};

template <class MInfo>
void showMapInfo()
{
    cout
        << "RowSize:       " << MInfo::RowSize << endl
        << "Rows:          " << MInfo::Rows << endl
        << "RobotX:        " << MInfo::RobotX << endl
        << "RobotY:        " << MInfo::RobotY << endl
        << "LiftX:         " << MInfo::LiftX << endl
        << "LiftY:         " << MInfo::LiftY << endl
        << "Init. Lambdas: " << MInfo::InitialLambdas << endl
        << "Rem. Lambdas:  " << MInfo::RemainingLambdas << endl
        << "Tick:          " << MInfo::Tick << endl
        << endl;
}

template <class Map>
inline void showMap()
{
    ShowRow<Map::MapInfo::RowSize, Map::MapInfo::RowSize, typename Map::Cons>::print();
    showMapInfo<typename Map::MapInfo>();
    //cout << "Remaining Lambdas: " << Map::MapInfo::RemainingLambdas << endl;
}

template <class Acts>
inline void showActions()
{
    static const char c[] = "LRUDWA";
    const char cc[2] = { c[Acts::Head], 0 };
    cout << cc;
    showActions<typename Acts::Tail>();
}

template <>
inline void showActions<NIL>()
{
//    cout << endl;
}

template <class _Map, unsigned int X, unsigned int Y>
struct Coords2Pos
{
    static const unsigned int pos = _Map::MapInfo::RowSize * Y + X;
};

template <class _Map, unsigned int Pos>
struct Pos2Coords
{
    static const unsigned int X = Pos % _Map::MapInfo::RowSize;
    static const unsigned int Y = Pos / _Map::MapInfo::RowSize;
};

template <class Cons, unsigned int Pos>
struct At1
{
    static const CellTypes value = At1<typename Cons::Tail, Pos - 1>::value;
};

template <class Cons>
struct At1<Cons, 0u>
{
    //enum { value = Cons::Head };
    static const CellTypes value = Cons::Head;
};

template <class Cons, unsigned int Pos, bool isInside>
struct _At1
{
    static const CellTypes value = At1<Cons, Pos>::value;
};

template <class Cons, unsigned int Pos>
struct _At1<Cons, Pos, false>
{
    static const CellTypes value = Wall;
};

template <class _Map, int X, int Y>
struct WithinMap
{
    enum { isInside = X >= 0 && Y >= 0 && X < _Map::MapInfo::RowSize && Y < _Map::MapInfo::Rows };
};

template <class _Map, int X, int Y>
struct At
{
    static const CellTypes value =
        _At1<
            typename _Map::Cons,
            Coords2Pos<_Map, static_cast<unsigned int>(X), static_cast<unsigned int>(Y)>::pos,
            WithinMap<_Map, X, Y>::isInside
        >::value;
};

template <Actions Act>
struct Steps
{
};

template <>
struct Steps<Left>
{
    enum { stepX = -1, stepY = 0 };
};

template <>
struct Steps<Right>
{
    enum { stepX = 1, stepY = 0 };
};

template <>
struct Steps<Up>
{
    enum { stepX = 0, stepY = -1 };
};

template <>
struct Steps<Down>
{
    enum { stepX = 0, stepY = 1 };
};

template <class SourceCons, unsigned int Pos, CellTypes T>
struct Replace
{
    typedef Cons< SourceCons::Head, typename Replace< typename SourceCons::Tail, Pos - 1, T>::Result > Result;
};

template <class SourceCons, CellTypes T>
struct Replace<SourceCons, 0u, T>
{
    typedef Cons< T, typename SourceCons::Tail> Result;
};

template <class SourceCons, unsigned int Pos, CellTypes T, bool doReplace>
struct ConditionalReplace
{
    typedef typename Replace<SourceCons, Pos, T>::Result Result;
};

template <class SourceCons, unsigned int Pos, CellTypes T>
struct ConditionalReplace<SourceCons, Pos, T, false>
{
    typedef SourceCons Result;
};


template <class SourceCons, unsigned int Pos0, unsigned int Pos1>
struct Move
{
    typedef typename Replace<typename Replace<SourceCons, Pos0, Empty>::Result, Pos1, At1<SourceCons, Pos0>::value >::Result Result;
};

template <class _Map, Actions Act>
struct MoveRobot
{
    typedef Steps<Act> S;
    static const int newX = _Map::MapInfo::RobotX + S::stepX;
    static const int newY = _Map::MapInfo::RobotY + S::stepY;

    static const CellTypes dest = At<_Map, newX, newY>::value;
    typedef Map<
        MapInfo<
            _Map::MapInfo::RowSize,
            _Map::MapInfo::Rows,
            newX,
            newY,
            _Map::MapInfo::LiftX,
            _Map::MapInfo::LiftY,
            _Map::MapInfo::InitialLambdas,
            dest != Lambda ? _Map::MapInfo::RemainingLambdas : _Map::MapInfo::RemainingLambdas - 1,
            _Map::MapInfo::Tick
        >,
        typename ConditionalReplace<
            typename Replace<typename _Map::Cons, Coords2Pos<_Map, _Map::MapInfo::RobotX, _Map::MapInfo::RobotY>::pos, Empty>::Result,
            Coords2Pos<_Map, newX, newY>::pos,
            Robot,
            dest != OpenLift    // if the robot moves to the OpenLift, the robot vanishes inside (and the OpenLift remains)
        >::Result
    > Result;
};

template <class _Map>
struct MoveRobot<_Map, Wait>
{
    typedef _Map Result;
};

template <class _Map>
struct MoveRobot<_Map, Abort>
{
    typedef _Map Result;
};

template <class _Map, unsigned int X0, unsigned int Y0, Actions Act>
struct MoveObject
{
    typedef Map<
        typename _Map::MapInfo,
        typename Move<
            typename _Map::Cons,
            Coords2Pos<_Map, X0, Y0>::pos,
            Coords2Pos<_Map, X0 + Steps<Act>::stepX, Y0 + Steps<Act>::stepY>::pos
        >::Result
    > Result;
};


////////////////////////////////////

template <class _Map, unsigned int Pos>
struct Neighbors
{
    typedef Pos2Coords<_Map, Pos> Coords;

    static const CellTypes
        Up    = At<_Map, Coords::X, Coords::Y-1>::value,
        Down  = At<_Map, Coords::X, Coords::Y+1>::value,
        Left  = At<_Map, Coords::X-1, Coords::Y>::value,
        Right = At<_Map, Coords::X+1, Coords::Y>::value,

        UpLeft    = At<_Map, Coords::X-1, Coords::Y-1>::value,
        UpRight   = At<_Map, Coords::X+1, Coords::Y-1>::value,
        DownLeft  = At<_Map, Coords::X-1, Coords::Y+1>::value,
        DownRight = At<_Map, Coords::X+1, Coords::Y+1>::value
    ;
};

template <class _Map, unsigned int Pos, CellTypes T>
struct _UpdateOneCell
{
    static const CellTypes value = T;
};

template <class _Map, unsigned int Pos>
struct _UpdateOneCell<_Map, Pos, Rock>
{
    typedef Neighbors<_Map, Pos> N;

    static const CellTypes value =
        ( N::Down == Empty ||
            ( N::Down == Rock &&
              ( N::DownLeft == Empty || N::DownRight == Empty )
            )
        )
        ? Empty
        : // Now check if rock will roll over a lambda
            ( N::Down == Lambda &&  N::DownRight == Empty )
            ? Empty
            : Rock;
};


template <class _Map, unsigned int Pos>
struct _UpdateOneCell<_Map, Pos, Empty>
{
    typedef Neighbors<_Map, Pos> N;

    static const CellTypes value =
        N::Up == Rock ||  // up is Rock
        (
          ( N::Left  == Rock && N::UpLeft  == Rock ) ||
          ( N::Right == Rock && N::UpRight == Rock )
        )
        ? Rock
        : // not check if we get a rolling rock
            ( N::UpLeft == Rock && N::Left == Lambda )
            ? Rock
            : Empty;
};

template <class _Map, unsigned int Pos>
struct _UpdateOneCell<_Map, Pos, ClosedLift>
{
    static const CellTypes value =
        ( _Map::MapInfo::RemainingLambdas == 0u )
        ? OpenLift
        : ClosedLift;
};

template <class _Map, class _PartialCons, unsigned int RemainingPositions>
struct _UpdateCells
{
    typedef Cons< _UpdateOneCell<_Map, _Map::MapInfo::TotalPositions - RemainingPositions, _PartialCons::Head>::value,
                  typename _UpdateCells<_Map, typename _PartialCons::Tail, RemainingPositions-1>::Result >
            Result;
};

template <class _Map, class _PartialCons>
struct _UpdateCells<_Map, _PartialCons, 0u>
{
    typedef NIL Result;
};

template <class _Map>
struct UpdateCells
{
    typedef Map<
                typename _Map::MapInfo,
                typename _UpdateCells<_Map, typename _Map::Cons, _Map::MapInfo::TotalPositions>::Result
    > Result;
};

template <class _Map>
struct UpdateWater
{
    typedef _Map Result;
};

typedef ConsActions< Left,
        ConsActions< Up,
        ConsActions< Right,
        ConsActions< Down,
        ConsActions< Wait,
        ConsActions< Abort, NIL
        > > > > > > AllActions;

template <class _Map, Actions action>
struct ValidateAction
{
    typedef Steps<action> S;
    static const CellTypes dest = At<_Map, _Map::MapInfo::RobotX + S::stepX, _Map::MapInfo::RobotY + S::stepY>::value;

    enum
    {
        value = dest == Earth || dest == Lambda || dest == Empty || dest == OpenLift ||
        // check if the robot can move a rock:
        (
            dest == Rock && (
                ( action == Left  && At<_Map, _Map::MapInfo::RobotX - 2, _Map::MapInfo::RobotY>::value == Empty ) ||
                ( action == Right && At<_Map, _Map::MapInfo::RobotX + 2, _Map::MapInfo::RobotY>::value == Empty )
            )
        )
    };
};

template <class _Map>
struct ValidateAction<_Map, Wait>
{
    enum { value = true };
};

template <class _Map>
struct ValidateAction<_Map, Abort>
{
    enum { value = true };
};

template <class _MapInfo>
struct UpdateMapInfo
{
    typedef MapInfo<
        _MapInfo::RowSize,
        _MapInfo::Rows,
        _MapInfo::RobotX,
        _MapInfo::RobotY,
        _MapInfo::LiftX,
        _MapInfo::LiftY,
        _MapInfo::InitialLambdas,
        _MapInfo::RemainingLambdas,
        _MapInfo::Tick + 1u,
        _MapInfo::Water
    > Result;
};

template <class _MapBefore, class _MapAfter, Actions TransitioningAction>
struct CheckGameStatus
{
    static const GameStatus value = 
        (TransitioningAction == Abort)
        ? RobotAborted
        : ( At<_MapAfter,  _MapAfter::MapInfo::RobotX, _MapAfter::MapInfo::RobotY>::value == OpenLift )
          ? RobotWon
          : ( At<_MapAfter,  _MapAfter::MapInfo::RobotX, _MapAfter::MapInfo::RobotY-1>::value == Rock &&
              At<_MapBefore, _MapAfter::MapInfo::RobotX, _MapAfter::MapInfo::RobotY-1>::value != Rock )
            ? GameOver
            : GoOn
    ;
};

template <class _Map, Actions action>
struct EvolveWorld
{
    //static const Actions robotAction = action;

    typedef typename MoveRobot<_Map, action>::Result AfterRobot;
    typedef typename UpdateCells<AfterRobot>::Result AfterUpdate;
    //typedef typename UpdateWater<AfterUpdate>::Result AfterWater;
    //typedef ConsActions< action, _ActsHistory > ActionList;
    //typedef AfterWater ResWorld;
    typedef Map<
        typename UpdateMapInfo<typename AfterUpdate::MapInfo>::Result,
        typename AfterUpdate::Cons
    > ResWorld;

    static const GameStatus newStatus = CheckGameStatus<_Map, ResWorld, action>::value;
};

#if 0
// TODO DFG CHECK!!!!!!!!!!
template <class _Map, template <class> class AI, class _ActsHistory = NIL, GameStatus status = GoOn>
struct PlayGame{}
/*
{
    typedef _Map Result;
    typedef _ActsHistory ActionList;
    static const GameStatus finalStatus = GoOn;
};
*/
template <class _Map, template <class> class AI, class _ActsHistory>
struct PlayGame<_Map, AI, _ActsHistory, RobotWon>
{
    typedef _Map Result;
    typedef _ActsHistory ActionList;
    static const GameStatus finalStatus = RobotWon;
};

template <class _Map, template <class> class AI, class _ActsHistory>
struct PlayGame<_Map, AI, _ActsHistory, GameOver>
{
    typedef _Map Result;
    typedef _ActsHistory ActionList;
    static const GameStatus finalStatus = GameOver;
};

template <class _Map, template <class> class AI, class _ActsHistory>
struct PlayGame<_Map, AI, _ActsHistory, RobotAborted>
{
    typedef _Map Result;
    typedef _ActsHistory ActionList;
    static const GameStatus finalStatus = RobotAborted;
};

template <class _Map, template <class> class AI, class _ActsHistory>
struct PlayGame<_Map, AI, _ActsHistory, GoOn>
{
    typedef EvolveWorld<_Map, AI<_Map>::action, _ActsHistory> EvolvedWorld;
    typedef PlayGame<typename EvolvedWorld::ResWorld, AI, typename EvolvedWorld::ActionList, newStatus> ResGame;
    typedef typename ResGame::ActionList ActionList;

    typedef typename ResGame::Result Result;

    static const GameStatus finalStatus = ResGame::finalStatus;
};
#endif

template <GameStatus status>
struct LambdaMultiplier {};

template <>
struct LambdaMultiplier<RobotWon>
{
    static const int value = 75; //25 per lambda + 50 per lambda for wining
};

template <>
struct LambdaMultiplier<RobotAborted>
{
    static const int value = 50; //25 per lambda + 25 per lambda for aborting
};

template <>
struct LambdaMultiplier<GameOver>
{
    static const int value = 25; //25 per lambda for loosing
};

template <class _MapInfo, GameStatus finalStatus>
struct CalcScore
{

    static const int value =
        ( _MapInfo::InitialLambdas - _MapInfo::RemainingLambdas ) * LambdaMultiplier<finalStatus>::value
        - _MapInfo::Tick
        ;//+ ( finalStatus == RobotAborted ? 1 : 0);
};

