/************************************

    IFCP Contest 2012

    Authors:
        Francisco Ferreira
        Daniel Gutson

    This file contains the
    metaprograms of some robots.
    Actually, Wall_e became obsolete,
    and only R2D2 survived.

*************************************/

///////////// Tools ////////////////
template <class ActionList, Actions action, bool valid>
struct AppendAction;

template <class ActionList, Actions action>
struct AppendAction<ActionList, action, true>
{
    typedef ConsActions< action, ActionList > Result;
};

template <class ActionList, Actions action>
struct AppendAction<ActionList, action, false>
{
    typedef ActionList Result;
};

template <Actions Prev, Actions Next>
struct CompatibleConsecutive
{
    static const bool value =
        ( Prev != RevertAction<Next>::value ) ||
        Prev == Abort ||
        Prev == Wait
    ;
};

template <class _Map, Actions Prev, class RemainingActs, class ValidActs >
struct _PossibleMoves
{
    typedef typename AppendAction<
        typename _PossibleMoves<_Map, Prev, typename RemainingActs::Tail, ValidActs >::Result,
        RemainingActs::Head,
        ValidateAction<_Map, RemainingActs::Head>::value && CompatibleConsecutive<Prev, RemainingActs::Head>::value
    >::Result Result;
};

template <class _Map, Actions Prev, class ValidActs >
struct _PossibleMoves<_Map, Prev, NIL, ValidActs>
{
    typedef ValidActs Result;
};


template <class _Map, Actions Prev>
struct PossibleMoves
{
    typedef typename _PossibleMoves< _Map, Prev, AllActions, NIL >::Result Result;
};

///////////////////////// Robots ////////////////////
template <class _Map>
struct Wall_e
{
    static const Actions action = Abort;//Right;
};


///////////////// R2D2 //////////////////////////

template <class _Map, class _ActionList, int _score>
struct Choice
{
    typedef _Map map;
    typedef _ActionList ActionList;
    static const int score = _score;
};

template <class Choice1, class Choice2, bool firstIsBetter>
struct SelectChoice;

template <class Choice1, class Choice2>
struct SelectChoice<Choice1, Choice2, true>
{
    typedef Choice1 Result;
};

template <class Choice1, class Choice2>
struct SelectChoice<Choice1, Choice2, false>
{
    typedef Choice2 Result;
};

template <class Choice1, class Choice2>
struct BestChoice
{
    typedef typename SelectChoice<Choice1, Choice2, (Choice1::score > Choice2::score)>::Result Result;
};

template <class Choice1>
struct BestChoice<Choice1, NIL>
{
    typedef Choice1 Result;
};

template <class Choice2>
struct BestChoice<NIL, Choice2>
{
    typedef Choice2 Result;
};

template <class _Map, GameStatus status = GoOn, Actions Prev = Wait>
struct R2D2;

template <class _Map, Actions action>
struct Evaluate1Action
{
    typedef EvolveWorld<_Map, action> EvolvedWorld;

    typedef R2D2<typename EvolvedWorld::ResWorld, EvolvedWorld::newStatus, action> ChildResult;

    typedef Choice<
        typename ChildResult::Result::map,
        ConsActions< action, typename ChildResult::Result::ActionList >,
        ChildResult::Result::score
    > Result;
};

template <class _Map, class ActionList >
struct EvaluateActions
{
    typedef Evaluate1Action<_Map, ActionList::Head> EvaluatedHead;

    typedef typename BestChoice<
        typename EvaluatedHead::Result,
        typename EvaluateActions<
            _Map,
            typename ActionList::Tail
        >::Result
    >::Result Result;
};

template <class _Map >
struct EvaluateActions<_Map, NIL>
{
    typedef NIL Result;
};

// This is for all except GoOn: FINAL RECURSION
template <class _Map, Actions Prev, GameStatus status>
struct _R2D2
{
    typedef Choice<
        _Map,
        NIL,
        CalcScore< typename _Map::MapInfo, status>::value
    > Result;
};

template <class _Map, Actions Prev>
struct _R2D2<_Map, Prev, GoOn>
{
    typedef typename PossibleMoves<_Map, Prev>::Result possibilities;

    typedef typename EvaluateActions<_Map, possibilities>::Result Result;
};

template <class _Map, GameStatus status, Actions Prev>
struct R2D2
{
    static const unsigned int Fuel = 6; // :-)

    typedef typename _R2D2<
        _Map,
        Prev,
        ( (_Map::MapInfo::Tick < Fuel) ? status : RobotAborted)
    >::Result Result;
};

