/****************************************************************************
 * ChessRules.cpp Chess classes - Rules of chess
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <algorithm>
#include "ChessRules.h"
#include "PrivateChessDefs.h"
using namespace std;
using namespace thc;

// Table indexed by Square, gives mask for DETAIL_CASTLING, such that a move
//  to (or from) that square results in castling being prohibited, eg a move
//  to e8 means that subsequently black kingside and black queenside castling
//  is prohibited
static unsigned char castling_prohibited_table[] =
{
    (unsigned char)(~BQUEEN), 0xff, 0xff, 0xff,                             // a8-d8
    (unsigned char)(~(BQUEEN+BKING)), 0xff, 0xff, (unsigned char)(~BKING),  // e8-h8
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a7-h7
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a6-h6
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a5-h5
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a4-h4
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a3-h3
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a2-h2
    (unsigned char)(~WQUEEN), 0xff, 0xff, 0xff,                             // a1-d1
    (unsigned char)(~(WQUEEN+WKING)),  0xff, 0xff, (unsigned char)(~WKING)  // e1-h1
};

/****************************************************************************
 * Test internals, for porting to new environments etc
 *   For the moment at least, this is best used by stepping through it
 *   thoughtfully with a debugger. It is not set up to automatically
 *   check whether THC is going to work in the new environment
 ****************************************************************************/
static int log_discard( const char *, ... ) { return 0; }
bool ChessRules::TestInternals( int (*log)(const char *,...) )
{
    if( log == NULL )
        log = log_discard;
    Init();
    Move mv;
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"g1f3");
    PlayMove(mv);
    mv.TerseIn(this,"g8f6");
    PlayMove(mv);
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    bool KQkq_allowed   = wking && bking && wqueen && bqueen;
    bool KQkq_allowed_f = wking_allowed() && bking_allowed() && wqueen_allowed() && bqueen_allowed();
    mv.TerseIn(this,"h1g1");
    PlayMove(mv);
    mv.TerseIn(this,"h8g8");
    PlayMove(mv);
    mv.TerseIn(this,"g1h1");  // return of rook to h1 clears wking
    PlayMove(mv);
    log( "WKING castling not allowed %08x\n", *DETAIL_ADDR );
    bool Qkq_allowed  = !wking && bking && wqueen && bqueen;
    bool Qkq_allowed_f = !wking_allowed() && !bking_allowed() && wqueen_allowed() && bqueen_allowed();
    // The _f version is different, !bking_allowed() because it checks and finds no rook on h8
    mv.TerseIn(this,"g8h8");  // return of rook to h8 clears bking
    PlayMove(mv);
    log( "WKING BKING castling not allowed %08x\n", *DETAIL_ADDR );
    bool Qq_allowed   = !wking && !bking && wqueen && bqueen;
    bool Qq_allowed_f = !wking_allowed() && !bking_allowed() && wqueen_allowed() && bqueen_allowed();
    mv.TerseIn(this,"b1c3");
    PlayMove(mv);
    mv.TerseIn(this,"b8c6");
    PlayMove(mv);
    mv.TerseIn(this,"a1b1");
    PlayMove(mv);
    mv.TerseIn(this,"a8b8");
    PlayMove(mv);
    mv.TerseIn(this,"b1a1");  // return of rook to a1 clears wqueen
    PlayMove(mv);
    log( "WKING BKING WQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool q_allowed   = !wking && !bking && !wqueen && bqueen;
    bool q_allowed_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    // The _f version is different, !bqueen_allowed() because it checks and finds no rook on a8
    mv.TerseIn(this,"b8a8");  // return of rook to a8 clears bqueen
    PlayMove(mv);
    log( "WKING BKING WQUEEN BQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool none_allowed   = !wking && !bking && !wqueen && !bqueen;
    bool none_allowed_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    ChessPosition::Init();
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"e2e3");
    PlayMove(mv);
    mv.TerseIn(this,"e7e6");
    PlayMove(mv);
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"e1e2");
    PlayMove(mv);
    mv.TerseIn(this,"e8e7");
    PlayMove(mv);
    mv.TerseIn(this,"e2e1");  // return of king to e1 clears wking, wqueen
    PlayMove(mv);
    log( "WKING WQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool kq_allowed   = !wking && bking && !wqueen && bqueen;
    bool kq_allowed_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    // The _f version is different, !bking_allowed() and !bqueen_allowed() because they check and finds no king on e8
    mv.TerseIn(this,"e7e8");
    PlayMove(mv);
    log( "WKING WQUEEN BKING BQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool none_allowed2   = !wking && !bking && !wqueen && !bqueen;
    bool none_allowed2_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    const char *fen = "b3k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
    Move move;
    Forsyth(fen);
    log( "Addresses etc.;\n" );
    log( " this = 0x%p\n",                         this );
    log( " (void *)this = 0x%p\n",                 (void *)this );
    log( " &white = 0x%p\n",                       &white );
    log( " &squares[0] = 0x%p\n",                  &squares[0] );
    log( " &half_move_clock = 0x%p\n",             &half_move_clock );
    log( " &full_move_count = 0x%p\n",             &full_move_count );
    log( " size to end of full_move_count = %lu", ((char *)&full_move_count - (char *)this) + sizeof(full_move_count) );
    log( " sizeof(ChessPosition) = %lu (should be 4 more than size to end of full_move_count)\n",
           sizeof(ChessPosition) );
    log( " sizeof(Move) = %lu\n",                  sizeof(Move) );

    log( " sizeof(ChessPositionRaw) = %lu\n", sizeof(ChessPositionRaw) );
    log( " (offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL) =");
    log( " %lu + %lu + %lu = %lu\n",
           offsetof(ChessPositionRaw,full_move_count), sizeof(full_move_count), sizeof(DETAIL),
           offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL)
           );
    for( int i=0; i<6; i++ )
    {
        switch(i)
        {
            case 0: move.TerseIn(this,"h1h2");    break;
            case 1: move.TerseIn(this,"a8h1");    break;
            case 2: move.TerseIn(this,"e1c1");    break;
            case 3: move.TerseIn(this,"h1a8");    break;
            case 4: move.TerseIn(this,"c1b1");    break;
            case 5: move.TerseIn(this,"e8g8");    break;
        }
        unsigned char *p = (unsigned char *)DETAIL_ADDR;
        log( " DETAIL_ADDR = 0x%p\n",  p );
        log( " DETAIL_ADDR[0] = %02x\n",  p[0] );
        log( " DETAIL_ADDR[1] = %02x\n",  p[1] );
        log( " DETAIL_ADDR[2] = %02x\n",  p[2] );
        log( " DETAIL_ADDR[3] = %02x\n",  p[3] );
        log( "Before %s: enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               move.TerseOut().c_str(),
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PushMove(move);
        log( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PopMove(move);
        log( "After PopMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PushMove(move);
        log( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
    }

    // Later, extend this to check addresses etc
    return KQkq_allowed && Qkq_allowed && Qq_allowed && q_allowed && none_allowed && kq_allowed && none_allowed2 &&
           KQkq_allowed_f && Qkq_allowed_f && Qq_allowed_f && q_allowed_f && none_allowed_f && kq_allowed_f && none_allowed2_f;
}

/****************************************************************************
 * Play a move
 ****************************************************************************/
void ChessRules::PlayMove( Move imove )
{
    // Legal move - save it in history
    history[history_idx++] = imove; // ring array

    // Update full move count
    if( !white )
        full_move_count++;

    // Update half move clock
    if( squares[imove.src] == 'P' || squares[imove.src] == 'p' )
        half_move_clock=0;   // pawn move
    else if( !IsEmptySquare(imove.capture) )
        half_move_clock=0;   // capture
    else
        half_move_clock++;   // neither pawn move or capture

    // Actually play the move
    PushMove( imove );
}


/****************************************************************************
 * Create a list of all legal moves in this position
 ****************************************************************************/
void ChessRules::GenLegalMoveList( vector<Move> &moves )
{
    MOVELIST movelist;
    GenLegalMoveList( &movelist );
    for( int i=0; i<movelist.count; i++ )
        moves.push_back( movelist.moves[i] );
}

/****************************************************************************
 * Create a list of all legal moves in this position, with extra info
 ****************************************************************************/
void ChessRules::GenLegalMoveList( vector<Move>  &moves,
                                   vector<bool>  &check,
                                   vector<bool>  &mate,
                                   vector<bool>  &stalemate )
{
    MOVELIST movelist;
    bool bcheck[MAXMOVES];
    bool bmate[MAXMOVES];
    bool bstalemate[MAXMOVES];
    GenLegalMoveList( &movelist, bcheck, bmate, bstalemate  );
    for( int i=0; i<movelist.count; i++ )
    {
        moves.push_back    ( movelist.moves[i] );
        check.push_back    ( bcheck[i] );
        mate.push_back     ( bmate[i] );
        stalemate.push_back( bstalemate[i] );
    }
}

/****************************************************************************
 * Create a list of all legal moves in this position
 ****************************************************************************/
void ChessRules::GenLegalMoveList( MOVELIST *list )
{
    int i, j;
    bool okay;
    MOVELIST list2;

    // Generate all moves, including illegal (eg put king in check) moves
    GenMoveList( &list2 );

    // Loop copying the proven good ones
    for( i=j=0; i<list2.count; i++ )
    {
        PushMove( list2.moves[i] );
        okay = Evaluate();
        PopMove( list2.moves[i] );
        if( okay )
            list->moves[j++] = list2.moves[i];
    }
    list->count  = j;
}

/****************************************************************************
 * Create a list of all legal moves in this position, with extra info
 ****************************************************************************/
void ChessRules::GenLegalMoveList( MOVELIST *list, bool check[MAXMOVES],
                                                    bool mate[MAXMOVES],
                                                    bool stalemate[MAXMOVES] )
{
    int i, j;
    bool okay;
    TERMINAL terminal_score;
    MOVELIST list2;

    // Generate all moves, including illegal (eg put king in check) moves
    GenMoveList( &list2 );

    // Loop copying the proven good ones
    for( i=j=0; i<list2.count; i++ )
    {
        PushMove( list2.moves[i] );
        okay = Evaluate(terminal_score);
        Square king_to_move = (Square)(white ? wking_square : bking_square );
        bool bcheck = false;
        if( AttackedPiece(king_to_move) )
            bcheck = true;
        PopMove( list2.moves[i] );
        if( okay )
        {
            stalemate[j] = (terminal_score==TERMINAL_WSTALEMATE ||
                            terminal_score==TERMINAL_BSTALEMATE);
            mate[j]      = (terminal_score==TERMINAL_WCHECKMATE ||
                            terminal_score==TERMINAL_BCHECKMATE);
            check[j]     = mate[j] ? false : bcheck;
            list->moves[j++] = list2.moves[i];
        }
    }
    list->count  = j;
}

/****************************************************************************
 * Check draw rules (50 move rule etc.)
 ****************************************************************************/
bool ChessRules::IsDraw( bool white_asks, DRAWTYPE &result )
{
    bool   draw=false;

    // Insufficient mating material
    draw =  IsInsufficientDraw( white_asks, result );

    // 50 move rule
    if( !draw && half_move_clock>=100 )
    {
        result = DRAWTYPE_50MOVE;
        draw = true;
    }

    // 3 times repetition,
    if( !draw && GetRepetitionCount()>=3 )
    {
        result = DRAWTYPE_REPITITION;
        draw = true;
    }

    if( !draw )
        result = NOT_DRAW;
    return( draw );
}

/****************************************************************************
 * Get number of times position has been repeated
 ****************************************************************************/
int ChessRules::GetRepetitionCount()
{
    int matches=0;

    //  Save those aspects of current position that are changed by multiple
    //  PopMove() calls as we search backwards (i.e. squares, white,
    //  detail, detail_idx)
    char save_squares[sizeof(squares)];
    memcpy( save_squares, squares, sizeof(save_squares) );
    unsigned char save_detail_idx = detail_idx;  // must be unsigned char
    bool          save_white      = white;
    unsigned char idx             = history_idx; // must be unsigned char
    DETAIL_SAVE;

    // Search backwards ....
    int nbr_half_moves = (full_move_count-1)*2 + (!white?1:0);
    if( nbr_half_moves > nbrof(history)-1 )
        nbr_half_moves = nbrof(history)-1;
    if( nbr_half_moves > nbrof(detail_stack)-1 )
        nbr_half_moves = nbrof(detail_stack)-1;
    for( int i=0; i<nbr_half_moves; i++ )
    {
        Move m = history[--idx];
        if( m.src == m.dst )
            break;  // unused history is set to zeroed memory
        PopMove(m);

        // ... looking for matching positions
        if( white    == save_white      && // quick ones first!
            DETAIL_EQ_KING_POSITIONS    &&
            0 == memcmp(squares,save_squares,sizeof(squares) )
            )
        {
            matches++;
            if( !DETAIL_EQ_ALL )    // Castling flags and/or enpassant target different?
            {
                // It might not be a match (but it could be - we have to unpack what the differences
                //  really mean)
                bool revoke_match = false;

                // Revoke match if different value of en-passant target square means different
                //  en passant possibilities
                if( !DETAIL_EQ_EN_PASSANT )
                {
                    int ep_saved = (int)(tmp&0xff);
                    int ep_now   = (int)(*DETAIL_ADDR&0xff);

                    // Work out whether each en_passant is a real one, i.e. is there an opposition
                    //  pawn in place to capture (if not it's just a double pawn advance with no
                    //  actual enpassant consequences)
                    bool real=false;
                    int ep = ep_saved;
                    char const *squ = save_squares;
                    for( int j=0; j<2; j++ )
                    {
                        if( ep == 0x10 )    // 0x10 = a6
                        {
                             real = (squ[SE(ep)] == 'P');
                        }
                        else if( 0x10<ep && ep<0x17 )   // 0x10 = h6
                        {
                             real = (squ[SW(ep)] == 'P' || squ[SE(ep)] == 'P');
                        }
                        else if( ep==0x17 )
                        {
                             real = (squ[SW(ep)] == 'P');
                        }
                        else if( 0x28==ep )   // 0x28 = a3
                        {
                             real = (squ[NE(ep)] == 'p');
                        }
                        else if( 0x28<ep && ep<0x2f )   // 0x2f = h3
                        {
                             real = (squ[NE(ep)] == 'p' || squ[NW(ep)] == 'p');
                        }
                        else if( ep==0x2f )
                        {
                             real = (squ[NW(ep)] == 'p' );
                        }
                        if( j > 0 )
                            ep_now = real?ep:0x40;      // evaluate second time through
                        else
                        {
                            ep_saved = real?ep:0x40;    // evaluate first time through
                            ep = ep_now;                // setup second time through
                            squ = squares;
                            real = false;
                        }
                    }

                    // If for example one en_passant is real and the other not, it's not a real match
                    if( ep_saved != ep_now )
                        revoke_match = true;
                }

                // Revoke match if different value of castling flags means different
                //  castling possibilities
                if( !revoke_match && !DETAIL_EQ_CASTLING )
                {
                    bool wking_saved  = save_squares[e1]=='K' && save_squares[h1]=='R' && (int)(tmp&(WKING<<24));
                    bool wking_now    = squares[e1]=='K' && squares[h1]=='R' && (int)(*DETAIL_ADDR&(WKING<<24));
                    bool bking_saved  = save_squares[e8]=='k' && save_squares[h8]=='r' && (int)(tmp&(BKING<<24));
                    bool bking_now    = squares[e8]=='k' && squares[h8]=='r' && (int)(*DETAIL_ADDR&(BKING<<24));
                    bool wqueen_saved = save_squares[e1]=='K' && save_squares[a1]=='R' && (int)(tmp&(WQUEEN<<24));
                    bool wqueen_now   = squares[e1]=='K' && squares[a1]=='R' && (int)(*DETAIL_ADDR&(WQUEEN<<24));
                    bool bqueen_saved = save_squares[e8]=='k' && save_squares[a8]=='r' && (int)(tmp&(BQUEEN<<24));
                    bool bqueen_now   = squares[e8]=='k' && squares[a8]=='r' && (int)(*DETAIL_ADDR&(BQUEEN<<24));
                    revoke_match = ( wking_saved != wking_now ||
                                     bking_saved != bking_now ||
                                     wqueen_saved != wqueen_now ||
                                     bqueen_saved != bqueen_now );
                }

                // If the real castling or enpassant possibilities differ, it's not a match
                //  At one stage we just did a naive binary match of the details - not good enough. For example
                //  a rook moving away from h1 doesn't affect the WKING flag, but does disallow white king side
                //  castling
                if( revoke_match )
                     matches--;
            }
        }

        // For performance reasons, abandon search early if pawn move
        //  or capture
        if( squares[m.src]=='P' || squares[m.src]=='p' || !IsEmptySquare(m.capture) )
            break;
    }

    // Restore current position
    memcpy( squares, save_squares, sizeof(squares) );
    white      = save_white;
    detail_idx = save_detail_idx;
    DETAIL_RESTORE;
    return( matches+1 );  // +1 counts original position
}

/****************************************************************************
 * Check insufficient material draw rule
 ****************************************************************************/
bool ChessRules::IsInsufficientDraw( bool white_asks, DRAWTYPE &result )
{
    char   piece;
    int    piece_count=0;
    bool   bishop_or_knight=false, lone_wking=true, lone_bking=true;
    bool   draw=false;

    // Loop through the board
    for( Square square=a8; square<=h1; ++square )
    {
        piece = squares[square];
        switch( piece )
        {
            case 'B':
            case 'b':
            case 'N':
            case 'n':       bishop_or_knight=true;  // and fall through
            case 'Q':
            case 'q':
            case 'R':
            case 'r':
            case 'P':
            case 'p':       piece_count++;
                            if( isupper(piece) )
                                lone_wking = false;
                            else
                                lone_bking = false;
                            break;
        }
        if( !lone_wking && !lone_bking )
            break;  // quit early for performance
    }

    // Automatic draw if K v K or K v K+N or K v K+B
    //  (note that K+B v K+N etc. is not auto granted due to
    //   selfmates in the corner)
    if( piece_count==0 ||
        (piece_count==1 && bishop_or_knight)
      )
    {
        draw = true;
        result = DRAWTYPE_INSUFFICIENT_AUTO;
    }
    else
    {

        // Otherwise side playing against lone K can claim a draw
        if( white_asks && lone_bking )
        {
            draw   = true;
            result = DRAWTYPE_INSUFFICIENT;
        }
        else if( !white_asks && lone_wking )
        {
            draw   = true;
            result = DRAWTYPE_INSUFFICIENT;
        }
    }
    return( draw );
}

/****************************************************************************
 * Generate a list of all possible moves in a position
 ****************************************************************************/
void ChessRules::GenMoveList( MOVELIST *l )
{
    Square square;

    // Convenient spot for some asserts
    //  Have a look at TestInternals() for this,
    //   A ChessPositionRaw should finish with 32 bits of detail information
    //   (see DETAIL macros, this assert() checks that)
    assert( sizeof(ChessPositionRaw) ==
               (offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL))
          );

    // We also rely on Moves being 32 bits for the implementation of Move
    //  bitwise == and != operators
    assert( sizeof(Move) == sizeof(int32_t) );

    // Clear move list
    l->count  = 0;   // set each field for each move

    // Loop through all squares
    for( square=a8; square<=h1; ++square )
    {

        // If square occupied by a piece of the right colour
        char piece=squares[square];
        if( (white&&IsWhite(piece)) || (!white&&IsBlack(piece)) )
        {

            // Generate moves according to the occupying piece
            switch( piece )
            {
                case 'P':
                {
                    WhitePawnMoves( l, square );
                    break;
                }
                case 'p':
                {
                    BlackPawnMoves( l, square );
                    break;
                }
                case 'N':
                case 'n':
                {
                    const lte *ptr = knight_lookup[square];
                    ShortMoves( l, square, ptr, NOT_SPECIAL );
                    break;
                }
                case 'B':
                case 'b':
                {
                    const lte *ptr = bishop_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'R':
                case 'r':
                {
                    const lte *ptr = rook_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'Q':
                case 'q':
                {
                    const lte *ptr = queen_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'K':
                case 'k':
                {
                    KingMoves( l, square );
                    break;
                }
            }
        }
    }
}

/****************************************************************************
 * Generate moves for pieces that move along multi-move rays (B,R,Q)
 ****************************************************************************/
void ChessRules::LongMoves( MOVELIST *l, Square square, const lte *ptr )
{
    Move *m=&l->moves[l->count];
    Square dst;
    lte nbr_rays = *ptr++;
    while( nbr_rays-- )
    {
        lte ray_len = *ptr++;
        while( ray_len-- )
        {
            dst = (Square)*ptr++;
            char piece=squares[dst];

            // If square not occupied (empty), add move to list
            if( IsEmptySquare(piece) )
            {
                m->src     = square;
                m->dst     = dst;
                m->capture = ' ';
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }

            // Else must move to end of ray
            else
            {
                ptr += ray_len;
                ray_len = 0;

                // If not occupied by our man add a capture
                if( (white&&IsBlack(piece)) || (!white&&IsWhite(piece)) )
                {
                    m->src     = square;
                    m->dst     = dst;
                    m->special = NOT_SPECIAL;
                    m->capture = piece;
                    l->count++;
                    m++;
                }
            }
        }
    }
}

/****************************************************************************
 * Generate moves for pieces that move along single move rays (N,K)
 ****************************************************************************/
void ChessRules::ShortMoves( MOVELIST *l, Square square,
                                         const lte *ptr, SPECIAL special  )
{
    Move *m=&l->moves[l->count];
    Square dst;
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        dst = (Square)*ptr++;
        char piece = squares[dst];

        // If square not occupied (empty), add move to list
        if( IsEmptySquare(piece) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Else if occupied by enemy man, add move to list as a capture
        else if( (white&&IsBlack(piece)) || (!white&&IsWhite(piece)) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = piece;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of king moves
 ****************************************************************************/
void ChessRules::KingMoves( MOVELIST *l, Square square )
{
    const lte *ptr = king_lookup[square];
    ShortMoves( l, square, ptr, SPECIAL_KING_MOVE );

    // Generate castling king moves
    Move *m;
    m = &l->moves[l->count];

    // White castling
    if( square == e1 )   // king on e1 ?
    {

        // King side castling
        if(
            squares[g1] == ' '   &&
            squares[f1] == ' '   &&
            squares[h1] == 'R'   &&
            (wking)            &&
            !AttackedSquare(e1,false) &&
            !AttackedSquare(f1,false) &&
            !AttackedSquare(g1,false)
          )
        {
            m->src     = e1;
            m->dst     = g1;
            m->special = SPECIAL_WK_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Queen side castling
        if(
            squares[b1] == ' '         &&
            squares[c1] == ' '         &&
            squares[d1] == ' '         &&
            squares[a1] == 'R'         &&
            (wqueen)                 &&
            !AttackedSquare(e1,false)  &&
            !AttackedSquare(d1,false)  &&
            !AttackedSquare(c1,false)
          )
        {
            m->src     = e1;
            m->dst     = c1;
            m->special = SPECIAL_WQ_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }
    }

    // Black castling
    if( square == e8 )   // king on e8 ?
    {

        // King side castling
        if(
            squares[g8] == ' '         &&
            squares[f8] == ' '         &&
            squares[h8] == 'r'         &&
            (bking)                  &&
            !AttackedSquare(e8,true) &&
            !AttackedSquare(f8,true) &&
            !AttackedSquare(g8,true)
          )
        {
            m->src     = e8;
            m->dst     = g8;
            m->special = SPECIAL_BK_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Queen side castling
        if(
            squares[b8] == ' '         &&
            squares[c8] == ' '         &&
            squares[d8] == ' '         &&
            squares[a8] == 'r'         &&
            (bqueen)                 &&
            !AttackedSquare(e8,true) &&
            !AttackedSquare(d8,true) &&
            !AttackedSquare(c8,true)
          )
        {
            m->src     = e8;
            m->dst     = c8;
            m->special = SPECIAL_BQ_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of white pawn moves
 ****************************************************************************/
void ChessRules::WhitePawnMoves( MOVELIST *l,  Square square )
{
    Move *m = &l->moves[l->count];
    const lte *ptr = pawn_white_lookup[square];
    bool promotion = (RANK(square) == '7');

    // Capture ray
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        Square dst = (Square)*ptr++;
        if( dst == enpassant_target )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = SPECIAL_WEN_PASSANT;
            m->capture = 'p';
            m++;
            l->count++;
        }
        else if( IsBlack(squares[dst]) )
        {
            m->src    = square;
            m->dst    = dst;
            m->capture = squares[dst];
            if( !promotion )
            {
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }
            else
            {

                // Generate (under)promotions in the order (Q),N,B,R
                //  but we no longer rely on this elsewhere as it
                //  stops us reordering moves
                m->special   = SPECIAL_PROMOTION_QUEEN;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_KNIGHT;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_BISHOP;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_ROOK;
                m++;
                l->count++;
            }
        }
    }

    // Advance ray
    nbr_moves = *ptr++;
    for( lte i=0; i<nbr_moves; i++ )
    {
        Square dst = (Square)*ptr++;

        // If square occupied, end now
        if( !IsEmptySquare(squares[dst]) )
            break;
        m->src     = square;
        m->dst     = dst;
        m->capture = ' ';
        if( !promotion )
        {
            m->special  =  (i==0 ? NOT_SPECIAL : SPECIAL_WPAWN_2SQUARES);
            m++;
            l->count++;
        }
        else
        {

            // Generate (under)promotions in the order (Q),N,B,R
            //  but we no longer rely on this elsewhere as it
            //  stops us reordering moves
            m->special   = SPECIAL_PROMOTION_QUEEN;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_KNIGHT;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_BISHOP;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_ROOK;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of black pawn moves
 ****************************************************************************/
void ChessRules::BlackPawnMoves( MOVELIST *l, Square square )
{
    Move *m = &l->moves[l->count];
    const lte *ptr = pawn_black_lookup[square];
    bool promotion = (RANK(square) == '2');

    // Capture ray
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        Square dst = (Square)*ptr++;
        if( dst == enpassant_target )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = SPECIAL_BEN_PASSANT;
            m->capture = 'P';
            m++;
            l->count++;
        }
        else if( IsWhite(squares[dst]) )
        {
            m->src  = square;
            m->dst    = dst;
            m->capture = squares[dst];
            if( !promotion )
            {
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }
            else
            {

                // Generate (under)promotions in the order (Q),N,B,R
                //  but we no longer rely on this elsewhere as it
                //  stops us reordering moves
                m->special   = SPECIAL_PROMOTION_QUEEN;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_KNIGHT;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_BISHOP;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_ROOK;
                m++;
                l->count++;
            }
        }
    }

    // Advance ray
    nbr_moves = *ptr++;
    for( int i=0; i<nbr_moves; i++ )
    {
        Square dst = (Square)*ptr++;

        // If square occupied, end now
        if( !IsEmptySquare(squares[dst]) )
            break;
        m->src  = square;
        m->dst  = dst;
        m->capture = ' ';
        if( !promotion )
        {
            m->special  =  (i==0 ? NOT_SPECIAL : SPECIAL_BPAWN_2SQUARES);
            m++;
            l->count++;
        }
        else
        {

            // Generate (under)promotions in the order (Q),N,B,R
            //  but we no longer rely on this elsewhere as it
            //  stops us reordering moves
            m->special   = SPECIAL_PROMOTION_QUEEN;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_KNIGHT;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_BISHOP;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_ROOK;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Make a move (with the potential to undo)
 ****************************************************************************/
void ChessRules::PushMove( Move& m )
{
    // Push old details onto stack
    DETAIL_PUSH;

    // Update castling prohibited flags for destination square, eg h8 -> bking
    DETAIL_CASTLING(m.dst);
                    // IMPORTANT - only dst is required since we also qualify
                    //  castling with presence of rook and king on right squares.
                    //  (I.E. if a rook or king leaves its original square, the
                    //  castling prohibited flag is unaffected, but it doesn't
                    //  matter since we won't castle unless rook and king are
                    //  present on the right squares. If subsequently a king or
                    //  rook returns, that's okay too because the  castling flag
                    //  is cleared by its arrival on the m.dst square, so
                    //  castling remains prohibited).
    enpassant_target = SQUARE_INVALID;

    // Special handling might be required
    switch( m.special )
    {
        default:
        squares[m.dst] = squares[m.src];
        squares[m.src] = ' ';
        break;

        // King move updates king position in details field
        case SPECIAL_KING_MOVE:
        squares[m.dst] = squares[m.src];
        squares[m.src] = ' ';
        if( white )
            wking_square = m.dst;
        else
            bking_square = m.dst;
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_QUEEN:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'Q':'q');
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_ROOK:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'R':'r');
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_BISHOP:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'B':'b');
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_KNIGHT:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'N':'n');
        break;

        // White enpassant removes pawn south of destination
        case SPECIAL_WEN_PASSANT:
        squares[m.src] = ' ';
        squares[m.dst] = 'P';
        squares[ SOUTH(m.dst) ] = ' ';
        break;

        // Black enpassant removes pawn north of destination
        case SPECIAL_BEN_PASSANT:
        squares[m.src] = ' ';
        squares[m.dst] = 'p';
        squares[ NORTH(m.dst) ] = ' ';
        break;

        // White pawn advances 2 squares sets an enpassant target
        case SPECIAL_WPAWN_2SQUARES:
        squares[m.src] = ' ';
        squares[m.dst] = 'P';
        enpassant_target = SOUTH(m.dst);
        break;

        // Black pawn advances 2 squares sets an enpassant target
        case SPECIAL_BPAWN_2SQUARES:
        squares[m.src] = ' ';
        squares[m.dst] = 'p';
        enpassant_target = NORTH(m.dst);
        break;

        // Castling moves update 4 squares each
        case SPECIAL_WK_CASTLING:
        squares[e1] = ' ';
        squares[f1] = 'R';
        squares[g1] = 'K';
        squares[h1] = ' ';
        wking_square = g1;
        break;
        case SPECIAL_WQ_CASTLING:
        squares[e1] = ' ';
        squares[d1] = 'R';
        squares[c1] = 'K';
        squares[a1] = ' ';
        wking_square = c1;
        break;
        case SPECIAL_BK_CASTLING:
        squares[e8] = ' ';
        squares[f8] = 'r';
        squares[g8] = 'k';
        squares[h8] = ' ';
        bking_square = g8;
        break;
        case SPECIAL_BQ_CASTLING:
        squares[e8] = ' ';
        squares[d8] = 'r';
        squares[c8] = 'k';
        squares[a8] = ' ';
        bking_square = c8;
        break;
    }

    // Toggle who-to-move
    Toggle();
}

/****************************************************************************
 * Undo a move
 ****************************************************************************/
void ChessRules::PopMove( Move& m )
{
    // Previous detail field
    DETAIL_POP;

    // Toggle who-to-move
    Toggle();

    // Special handling might be required
    switch( m.special )
    {
        default:
        squares[m.src] = squares[m.dst];
        squares[m.dst] = m.capture;
        break;

        // For promotion, src piece was a pawn
        case SPECIAL_PROMOTION_QUEEN:
        case SPECIAL_PROMOTION_ROOK:
        case SPECIAL_PROMOTION_BISHOP:
        case SPECIAL_PROMOTION_KNIGHT:
        if( white )
            squares[m.src] = 'P';
        else
            squares[m.src] = 'p';
        squares[m.dst] = m.capture;
        break;

        // White enpassant re-insert black pawn south of destination
        case SPECIAL_WEN_PASSANT:
        squares[m.src] = 'P';
        squares[m.dst] = ' ';
        squares[SOUTH(m.dst)] = 'p';
        break;

        // Black enpassant re-insert white pawn north of destination
        case SPECIAL_BEN_PASSANT:
        squares[m.src] = 'p';
        squares[m.dst] = ' ';
        squares[NORTH(m.dst)] = 'P';
        break;

        // Castling moves update 4 squares each
        case SPECIAL_WK_CASTLING:
        squares[e1] = 'K';
        squares[f1] = ' ';
        squares[g1] = ' ';
        squares[h1] = 'R';
        break;
        case SPECIAL_WQ_CASTLING:
        squares[e1] = 'K';
        squares[d1] = ' ';
        squares[c1] = ' ';
        squares[a1] = 'R';
        break;
        case SPECIAL_BK_CASTLING:
        squares[e8] = 'k';
        squares[f8] = ' ';
        squares[g8] = ' ';
        squares[h8] = 'r';
        break;
        case SPECIAL_BQ_CASTLING:
        squares[e8] = 'k';
        squares[d8] = ' ';
        squares[c8] = ' ';
        squares[a8] = 'r';
        break;
    }
}


/****************************************************************************
 * Determine if an occupied square is attacked
 ****************************************************************************/
bool ChessRules::AttackedPiece( Square square )
{
    bool enemy_is_white  =  IsBlack(squares[square]);
    return( AttackedSquare(square,enemy_is_white) );
}

/****************************************************************************
 * Is a square is attacked by enemy ?
 ****************************************************************************/
bool ChessRules::AttackedSquare( Square square, bool enemy_is_white )
{
    Square dst;
    const lte *ptr = (enemy_is_white ? attacks_black_lookup[square] : attacks_white_lookup[square] );
    lte nbr_rays = *ptr++;
    while( nbr_rays-- )
    {
        lte ray_len = *ptr++;
        while( ray_len-- )
        {
            dst = (Square)*ptr++;
            char piece=squares[dst];

            // If square not occupied (empty), continue
            if( IsEmptySquare(piece) )
                ptr++;  // skip mask

            // Else if occupied
            else
            {
                lte mask = *ptr++;

                // White attacker ?
                if( IsWhite(piece) && enemy_is_white )
                {
                    if( to_mask[piece] & mask )
                        return true;
                }

                // Black attacker ?
                else if( IsBlack(piece) && !enemy_is_white )
                {
                    if( to_mask[piece] & mask )
                        return true;
                }

                // Goto end of ray
                ptr += (2*ray_len);
                ray_len = 0;
            }
        }
    }

    ptr = knight_lookup[square];
    lte nbr_squares = *ptr++;
    while( nbr_squares-- )
    {
        dst = (Square)*ptr++;
        char piece=squares[dst];

        // If occupied by an enemy knight, we have found an attacker
        if( (enemy_is_white&&piece=='N') || (!enemy_is_white&&piece=='n') )
            return true;
    }
    return false;
}

/****************************************************************************
 * Evaluate a position, returns bool okay (not okay means illegal position)
 ****************************************************************************/
bool ChessRules::Evaluate()
{
    Square enemy_king = (Square)(white ? bking_square : wking_square);
    // Enemy king is attacked and our move, position is illegal
    return !AttackedPiece(enemy_king);
}

bool ChessRules::Evaluate( TERMINAL &score_terminal )
{
    return( Evaluate(NULL,score_terminal) );
}

bool ChessRules::Evaluate( MOVELIST *p, TERMINAL &score_terminal )
{
    /* static ;remove for thread safety */ MOVELIST local_list;
    MOVELIST &list = p?*p:local_list;
    int i, any;
    Square my_king, enemy_king;
    bool okay;
    score_terminal=NOT_TERMINAL;

    //DIAG_evaluate_count++;

    // Enemy king is attacked and our move, position is illegal
    enemy_king = (Square)(white ? bking_square : wking_square);
    if( AttackedPiece(enemy_king) )
        okay = false;

    // Else legal position
    else
    {
        okay = true;

        // Work out if the game is over by checking for any legal moves
        GenMoveList( &list );
        for( any=i=0 ; i<list.count && any==0 ; i++ )
        {
            PushMove( list.moves[i] );
            my_king = (Square)(white ? bking_square : wking_square);
            if( !AttackedPiece(my_king) )
                any++;
            PopMove( list.moves[i] );
        }

        // If no legal moves, position is either checkmate or stalemate
        if( any == 0 )
        {
            my_king = (Square)(white ? wking_square : bking_square);
            if( AttackedPiece(my_king) )
                score_terminal = (white ? TERMINAL_WCHECKMATE
                                        : TERMINAL_BCHECKMATE);
            else
                score_terminal = (white ? TERMINAL_WSTALEMATE
                                        : TERMINAL_BSTALEMATE);
        }
    }
    return(okay);
}

// Transform a position with W to move into an equivalent with B to move and vice-versa
void ChessRules::Transform()
{
    Toggle();
    Square wking_square_ = (Square)0;
    Square bking_square_ = (Square)0;
    Square enpassant_target_ = (Square)this->enpassant_target;

    // swap wking <-> bking
    int tmp = wking;
    wking  = bking;
    bking  = tmp;

    // swap wqueen <-> bqueen
    tmp    = wqueen;
    wqueen = bqueen;
    bqueen = tmp;
    Square src, dst;
    char file, r1, r2;

    // Loop through the board
    for( file='a'; file<='h'; file++ )
    {
        for( r1='1',r2='8'; r1<='8'; r1++,r2-- )
        {
            src = SQ(file,r1);
            dst = SQ(file,r2);
            if( wking_square == src )
                bking_square_ = dst;
            if( bking_square == src )
                wking_square_ = dst;
            if( enpassant_target == src )
                enpassant_target_ = dst;
        }
    }
    this->wking_square      = wking_square_;
    this->bking_square      = bking_square_;
    this->enpassant_target  = enpassant_target_;

    // Loop through half the board
    for( file='a'; file<='h'; file++ )
    {
        for( r1='1',r2='8'; r1<='4'; r1++,r2-- )
        {
            src = SQ(file ,r1);
            dst = SQ(file ,r2);
            char from = squares[src];
            char tmpc = squares[dst];
            for( int i=0; i<2; i++ )
            {
                switch( from )
                {
                    case 'K':   squares[dst] = 'k';   break;
                    case 'Q':   squares[dst] = 'q';   break;
                    case 'R':   squares[dst] = 'r';   break;
                    case 'B':   squares[dst] = 'b';   break;
                    case 'N':   squares[dst] = 'n';   break;
                    case 'P':   squares[dst] = 'p';   break;
                    case 'k':   squares[dst] = 'K';   break;
                    case 'q':   squares[dst] = 'Q';   break;
                    case 'r':   squares[dst] = 'R';   break;
                    case 'b':   squares[dst] = 'B';   break;
                    case 'n':   squares[dst] = 'N';   break;
                    case 'p':   squares[dst] = 'P';   break;
                    default:    squares[dst] = from;
                }
                from = tmpc;
                dst  = src;
            }
        }
    }
}


// Transform a W move in a transformed position to a B one and vice-versa
Move ChessRules::Transform( Move move )
{
    Move ret;
    Square src=a8, dst=a8, from, to;
    char file, r1, r2;

    // Loop through the board (this is a very lazy and slow way to do it!)
    for( file='a'; file<='h'; file++ )
    {
        for( r1='1', r2='8'; r1<='8'; r1++,r2-- )
        {
            from = SQ(file,r1);
            to = SQ(file,r2);
            if( move.src == from )
                src = to;
            if( move.dst == from )
                dst = to;
        }
    }
    ret.src = src;
    ret.dst = dst;

    // Special moves
    switch( move.special )
    {
        case SPECIAL_WK_CASTLING:       ret.special = SPECIAL_BK_CASTLING;          break;
        case SPECIAL_BK_CASTLING:       ret.special = SPECIAL_WK_CASTLING;          break;
        case SPECIAL_WQ_CASTLING:       ret.special = SPECIAL_BQ_CASTLING;          break;
        case SPECIAL_BQ_CASTLING:       ret.special = SPECIAL_WQ_CASTLING;          break;
        case SPECIAL_WPAWN_2SQUARES:    ret.special = SPECIAL_BPAWN_2SQUARES;       break;
        case SPECIAL_BPAWN_2SQUARES:    ret.special = SPECIAL_WPAWN_2SQUARES;       break;
        case SPECIAL_WEN_PASSANT:       ret.special = SPECIAL_BEN_PASSANT;          break;
        case SPECIAL_BEN_PASSANT:       ret.special = SPECIAL_WEN_PASSANT;          break;
        default: break;
    }

    // Captures
    if( islower(move.capture) )
        ret.capture = toupper(move.capture);
    else if( isupper(move.capture) )
        ret.capture = tolower(move.capture);
    return ret;
}

/****************************************************************************
 *  Test for legal position, sets reason to a mask of possibly multiple reasons
 ****************************************************************************/
bool ChessRules::IsLegal( ILLEGAL_REASON& reason )
{
    int  ireason = 0;
    int  wkings=0, bkings=0, wpawns=0, bpawns=0, wpieces=0, bpieces=0;
    bool legal = true;
    int  file, rank;
    char p;
    Square opposition_king_location = SQUARE_INVALID;

    // Loop through the board
    file=0;     // go from a8,b8..h8,a7,b7..h1
    rank=7;
    for( ;;)
    {
        Square sq = SQ('a'+file, '1'+rank);
        p = squares[sq];
        if( (p=='P'||p=='p') && (rank==0||rank==7) )
        {
            legal = false;
            ireason |= IR_PAWN_POSITION;
        }
        if( IsWhite(p) )
        {
            if( p == 'P' )
                wpawns++;
            else
            {
                wpieces++;
                if( p == 'K' )
                {
                    wkings++;
                    if( !white )
                        opposition_king_location = sq;
                }
            }
        }
        else if( IsBlack(p) )
        {
            if( p == 'p' )
                bpawns++;
            else
            {
                bpieces++;
                if( p == 'k' )
                {
                    bkings++;
                    if( white )
                        opposition_king_location = sq;
                }
            }
        }
        if( sq == h1 )
            break;
        else
        {
            file++;
            if( file == 8 )
            {
                file = 0;
                rank--;
            }
        }
    }
    if( wkings!=1 || bkings!=1 )
    {
        legal = false;
        ireason |= IR_NOT_ONE_KING_EACH;
    }
    if( opposition_king_location!=SQUARE_INVALID && AttackedPiece(opposition_king_location) )
    {
        legal = false;
        ireason |= IR_CAN_TAKE_KING;
    }
    if( wpieces>8 && (wpieces+wpawns)>16 )
    {
        legal = false;
        ireason |= IR_WHITE_TOO_MANY_PIECES;
    }
    if( bpieces>8 && (bpieces+bpawns)>16 )
    {
        legal = false;
        ireason |= IR_BLACK_TOO_MANY_PIECES;
    }
    if( wpawns > 8 )
    {
        legal = false;
        ireason |= IR_WHITE_TOO_MANY_PAWNS;
    }
    if( bpawns > 8 )
    {
        legal = false;
        ireason |= IR_BLACK_TOO_MANY_PAWNS;
    }
    reason = (ILLEGAL_REASON)ireason;
    return( legal );
}

