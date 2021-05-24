/*

    Simple demo of THC Chess library

    This is a simple "hello world" exercise to get started with the THC chess library
    Just compile and link with thc.cpp. You only need thc.cpp and thc.h to use the
    THC library (see README.MD for more information)

 */

#include <stdio.h>
#include <string>
#include <vector>
#include "thc.h"

void display_position( thc::ChessRules &cr, const std::string &description )
{
    std::string fen = cr.ForsythPublish();
    std::string s = cr.ToDebugStr();
    printf( "%s\n", description.c_str() );
    printf( "FEN (Forsyth Edwards Notation) = %s\n", fen.c_str() );
    printf( "Position = %s\n", s.c_str() );
}

int main()
{
    // Example 1, Play a few good moves from the initial position
    thc::ChessRules cr;
    display_position( cr, "Initial position" );
    thc::Move mv;
    mv.NaturalIn( &cr, "e4" );
    cr.PlayMove(mv);
    mv.NaturalIn( &cr, "e5" );
    cr.PlayMove(mv);
    mv.NaturalIn( &cr, "Nf3" );
    cr.PlayMove(mv);
    mv.NaturalIn( &cr, "Nc6" );
    cr.PlayMove(mv);
    mv.NaturalIn( &cr, "Bc4" );
    cr.PlayMove(mv);
    mv.NaturalIn( &cr, "Bc5" );
    cr.PlayMove(mv);
    display_position( cr, "Starting position of Italian opening, after 1.e4 e5 2.Nf3 Nc6 3.Bc4 Bc5" );

    printf( "List of all legal moves in the current position\n" );
    std::vector<thc::Move> moves;
    std::vector<bool> check;
    std::vector<bool> mate;
    std::vector<bool> stalemate;
    cr.GenLegalMoveList(  moves, check, mate, stalemate );
    unsigned int len = moves.size();
    for( unsigned int i=0; i<len; i++ )
    {
        mv = moves[i];
        std::string mv_txt = mv.NaturalOut(&cr);
        const char *suffix="";
        if( check[i] )
            suffix = " (note '+' indicates check)";
        else if( mate[i] )
            suffix = " (note '#' indicates mate)";
        printf( "4.%s%s\n", mv_txt.c_str(), suffix );
    }

    // Example 2, The shortest game leading to mate
    printf("\n");
    thc::ChessRules cr2;
    cr = cr2;
    display_position( cr, "Initial position" );
    mv.TerseIn( &cr, "g2g4" );
    std::string description = "Position after 1." + mv.NaturalOut(&cr);
    cr.PlayMove(mv);
    display_position( cr, description );
    mv.TerseIn( &cr, "e7e5" );
    description = "Position after 1..." + mv.NaturalOut(&cr);
    cr.PlayMove(mv);
    display_position( cr, description );
    mv.TerseIn( &cr, "f2f4" );
    description = "Position after 2." + mv.NaturalOut(&cr);
    cr.PlayMove(mv);
    thc::TERMINAL eval_penultimate_position;
    bool legal1 = cr.Evaluate( eval_penultimate_position );
    bool normal1 = (eval_penultimate_position == thc::NOT_TERMINAL);
    display_position( cr, description );
    mv.TerseIn( &cr, "d8h4" );
    description = "Position after 2..." + mv.NaturalOut(&cr);
    cr.PlayMove(mv);
    display_position( cr, description );
    thc::TERMINAL eval_final_position;
    bool legal2 = cr.Evaluate( eval_final_position );
    bool mate2 = (eval_final_position == thc::TERMINAL_WCHECKMATE);
    printf( "legal1=%s, normal1=%s, legal2=%s, mate2=%s\n", 
        legal1?"true":"false",
        normal1?"true":"false",
        legal2?"true":"false",
        mate2?"true":"false");
    if( legal1 && normal1 && legal2 && mate2 )
        printf( "As expected, all flags true, so both penultimate and final positions are legal, in the final position White is mated\n" );
    else
        printf( "Strange(???!), we expected all flags true, meaning both penultimate and final positions are legal, in the final position White is mated\n" );
}

