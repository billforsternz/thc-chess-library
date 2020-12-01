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

#if 0
void ChessRules::TestInternals()
{
    Init();
    Move mv;
    printf( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"g1f3");
    PlayMove(mv);
    mv.TerseIn(this,"g8f6");
    PlayMove(mv);
    printf( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"h1g1");
    PlayMove(mv);
    mv.TerseIn(this,"h8g8");
    PlayMove(mv);
    mv.TerseIn(this,"g1h1");
    PlayMove(mv);
    printf( "WKING castling not allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"g8h8");
    PlayMove(mv);
    printf( "WKING BKING castling not allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"b1c3");
    PlayMove(mv);
    mv.TerseIn(this,"b8c6");
    PlayMove(mv);
    mv.TerseIn(this,"a1b1");
    PlayMove(mv);
    mv.TerseIn(this,"a8b8");
    PlayMove(mv);
    mv.TerseIn(this,"b1a1");
    PlayMove(mv);
    printf( "WKING BKING WQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"b8a8");
    PlayMove(mv);
    printf( "WKING BKING WQUEEN BQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    ChessPosition::Init();
    printf( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"e2e3");
    PlayMove(mv);
    mv.TerseIn(this,"e7e6");
    PlayMove(mv);
    printf( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"e1e2");
    PlayMove(mv);
    mv.TerseIn(this,"e8e7");
    PlayMove(mv);
    mv.TerseIn(this,"e2e1");
    PlayMove(mv);
    printf( "WKING WQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"e7e8");
    PlayMove(mv);
    printf( "WKING WQUEEN BKING BQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    const char *fen = "b3k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
    Move move;
    Forsyth(fen);
    printf( "Addresses etc.;\n" );
    printf( " this = 0x%p\n",                         this );
    printf( " (void *)this = 0x%p\n",                 (void *)this );
    printf( " &white = 0x%p\n",                       &white );
    printf( " &squares[0] = 0x%p\n",                  &squares[0] );
    printf( " &half_move_clock = 0x%p\n",             &half_move_clock );
    printf( " &full_move_count = 0x%p\n",             &full_move_count );
    printf( " size to end of full_move_count = %lu", ((char *)&full_move_count - (char *)this) + sizeof(full_move_count) );
    printf( " sizeof(ChessPosition) = %lu (should be 4 more than size to end of full_move_count)\n",
           sizeof(ChessPosition) );
    printf( " sizeof(Move) = %lu\n",                  sizeof(Move) );
    
    printf( " sizeof(ChessPositionRaw) = %lu\n", sizeof(ChessPositionRaw) );
    printf( " (offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL) =");
    printf( " %lu + %lu + %lu = %lu\n",
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
        printf( " DETAIL_ADDR = 0x%p\n",  p );
        printf( " DETAIL_ADDR[0] = %02x\n",  p[0] );
        printf( " DETAIL_ADDR[1] = %02x\n",  p[1] );
        printf( " DETAIL_ADDR[2] = %02x\n",  p[2] );
        printf( " DETAIL_ADDR[3] = %02x\n",  p[3] );
        printf( "Before %s: enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
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
        printf( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PopMove(move);
        printf( "After PopMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PushMove(move);
        printf( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
    }
}





void ChessEngine::TestInternals()
{
    const char *fen = "b3k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
    Move move;
    Forsyth(fen);
    printf( "Addresses etc.;\n" );
    printf( " this = 0x%p\n",                         this );
    printf( " &white = 0x%p\n",                       &white );
    printf( " &squares[0] = 0x%p\n",                  &squares[0] );
    printf( " &half_move_clock = 0x%p\n",             &half_move_clock );
    printf( " &full_move_count = 0x%p\n",             &full_move_count );
    printf( " size to end of full_move_count = 0x%x\n", (unsigned int)( ((char *)&full_move_count - (char *)this) + sizeof(full_move_count) )  );
    printf( " sizeof(ChessPosition) = 0x%x (should be 4 more than size to end of full_move_count)\n",
                                                        (unsigned int)sizeof(ChessPosition) );
    printf( " sizeof(Move) = 0x%x\n",                  (unsigned int)sizeof(Move) );
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
        printf( " DETAIL_ADDR = 0x%p\n",  p );
        printf( " DETAIL_ADDR[0] = %02x\n",  p[0] );
        printf( " DETAIL_ADDR[1] = %02x\n",  p[1] );
        printf( " DETAIL_ADDR[2] = %02x\n",  p[2] );
        printf( " DETAIL_ADDR[3] = %02x\n",  p[3] );
        printf( "Before %s: enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
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
        printf( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
                " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
            enpassant_target,
            wking_square,
            bking_square,
            wking ?"true":"false",
            wqueen?"true":"false",
            bking ?"true":"false",
            bqueen?"true":"false" );
        PopMove(move);        
        printf( "After PopMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
                " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
            enpassant_target,
            wking_square,
            bking_square,
            wking ?"true":"false",
            wqueen?"true":"false",
            bking ?"true":"false",
            bqueen?"true":"false" );
        PushMove(move);        
        printf( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
                " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
            enpassant_target,
            wking_square,
            bking_square,
            wking ?"true":"false",
            wqueen?"true":"false",
            bking ?"true":"false",
            bqueen?"true":"false" );
    }
}

#endif
