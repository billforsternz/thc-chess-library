/*

    Rebuild and Test THC Chess Library

    Does two jobs;

    1) Concatentate the .cpp files into a single thc.cpp and the .h files into a single thc.h
    2) Do some basic testing of the library

    Note that 1) generates thc-regen.cpp and thc-regen.h and 2) relies upon thc.h being
    included and thc.cpp being linked in.
    
    After some rework (refactoring, synchronising, harmonising) on 2 December 2020 the
    thc and thc-regen files do match (and a test is now included to check for this match)

 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include "util.h"
#include "thc.h"

// Prototypes
bool regenerate_h_file();
bool regenerate_cpp_file();
bool files_match( const std::string &file1, const std::string &file2 );

int main()
{
    // Step 1)
    bool ok=false;
    if( regenerate_h_file() )
        if( regenerate_cpp_file() )
            ok = true;

    // Step 2)
    if( ok )
    {
        bool match = files_match( "../src/thc.cpp", "../src/thc-regen.cpp" );
        printf( "File ../src/thc.cpp does %s regenerated file ../src/thc-regen.cpp as intended\n", match ? "match":"not match" );
        match = files_match( "../src/thc.h", "../src/thc-regen.h" );
        printf( "File ../src/thc.h does %s regenerated file ../src/thc-regen.h as intended\n", match ? "match":"not match" );
    }

    // Step 3)
    if( ok )
    {
        thc::ChessRules cr;
        bool ok = cr.TestInternals( printf );
        printf( "Internal THC library tests %s\n", ok ? "pass":"fail" );
    }
    return -1;
}

#define nbrof(array) (sizeof(array)/sizeof((array)[0]))

bool regenerate_h_file()
{
    const char *fixed_header[]=
    {
        "/****************************************************************************",
        " * Triple Happy Chess library = thc library",
        " *  This is thc rendered as a single thc.h header + thc.cpp source file to",
        " *  avoid the complications of libraries - Inspired by sqlite.c",
        " *",
        " *  Author:  Bill Forster",
        " *  License: MIT license. Full text of license is in associated file LICENSE",
        " *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>",
        " ****************************************************************************/",
        "",
        "/*" ,
        "    thc.h The basic idea is to concatenate the following into one .h file;",
        "",
        "        ChessDefs.h",
        "        Move.h",
        "        ChessPositionRaw.h",
        "        ChessPosition.h",
        "        ChessRules.h",
        "        ChessEvaluation.h",
        "",
        " */",
        "",
        "#include <stddef.h>",
        "#include <string>",
        "#include <vector>"
    };

    const char *hdr_files[]=
    {
        "../src/ChessDefs.h",
        "../src/Move.h",
        "../src/ChessPositionRaw.h",
        "../src/ChessPosition.h",
        "../src/ChessRules.h",
        "../src/ChessEvaluation.h"
    };

    std::ofstream out("../src/thc-regen.h");
    if( !out )
    {
        printf( "Cannot open src/thc-regen.h\n" );
        return false;
    }

    for( unsigned int i=0; i<nbrof(hdr_files); i++ )
    {
        // First time only, prepend a fixed header
        if( i == 0 )
        {
            for( unsigned int j=0; j<nbrof(fixed_header); j++ )
            {
                std::string line(fixed_header[j]);
                util::putline(out,line);
            }
        }

        // Concatenate next file
        std::ifstream in(hdr_files[i]);
        if( !in )
        {
            printf( "Cannot open %s\n", hdr_files[i] );
            return false;
        }
        for(;;)
        {
            std::string line;
            if( !std::getline(in,line) )
                break;
            if( !util::prefix(line,"#include") )
                util::putline(out,line);
        }
    }
    return true;
}


bool regenerate_cpp_file()
{
    const char *fixed_header[]=
    {
        "/****************************************************************************",
        " * Triple Happy Chess library = thc library",
        " *  This is thc rendered as a single thc.h header + thc.cpp source file to",
        " *  avoid the complications of libraries - Inspired by sqlite.c",
        " *",
        " *  Author:  Bill Forster",
        " *  License: MIT license. Full text of license is in associated file LICENSE",
        " *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>",
        " ****************************************************************************/",
        "",
        "/*",
        "    thc.cpp The basic idea is to concatenate the following into one .cpp file;",
        "",
        "        Portability.cpp",
        "        PrivateChessDefs.h",
        "        HashLookup.h",
        "        ChessPosition.cpp",
        "        ChessRules.cpp",
        "        ChessEvaluation.cpp",
        "        Move.cpp",
        "        PrivateChessDefs.cpp",
        "         nested inline expansion of -> GeneratedLookupTables.h",
        " */",
        "",
        "#define _CRT_SECURE_NO_WARNINGS",
        "#include <stdio.h>",
        "#include <stdlib.h>",
        "#include <string.h>",
        "#include <ctype.h>",
        "#include <assert.h>",
        "#include <algorithm>",
        "#include \"thc.h\"",
        "using namespace std;",
        "using namespace thc;"
    };

    const char *cpp_files[]=
    {
        "../src/Portability.cpp",
        "../src/PrivateChessDefs.h",
        "../src/HashLookup.h",
        "../src/ChessPosition.cpp",
        "../src/ChessRules.cpp",
        "../src/ChessEvaluation.cpp",
        "../src/Move.cpp",
        "../src/PrivateChessDefs.cpp"
    };

    std::ofstream out("../src/thc-regen.cpp");
    if( !out )
    {
        printf( "Cannot open src/thc-regen.cpp\n" );
        return false;
    }

    for( unsigned int i=0; i<nbrof(cpp_files); i++ )
    {
        // First time only, prepend a fixed header
        if( i == 0 )
        {
            for( unsigned int j=0; j<nbrof(fixed_header); j++ )
            {
                std::string line(fixed_header[j]);
                util::putline(out,line);
            }
        }

        // Concatenate next file
        std::ifstream in(cpp_files[i]);
        if( !in )
        {
            printf( "Cannot open %s\n", cpp_files[i] );
            return false;
        }
        for(;;)
        {
            std::string line;
            if( !std::getline(in,line) )
                break;
            bool keep=true;
            if( util::prefix(line,"#include \"GeneratedLookupTables.h\"") )
            {
                util::putline(out,"// " + line);
                keep = false;
                std::ifstream in2("../src/GeneratedLookupTables.h");
                if( !in2 )
                {
                    printf( "Cannot open src/GeneratedLookupTables.h\n" );
                    return false;
                }
                for(;;)
                {
                    std::string line;
                    if( !std::getline(in2,line) )
                        break;
                    util::putline(out,line);
                }

            }
            if( util::prefix(line,"#include") )
                keep = false;
            if( util::prefix(line,"#define _CRT_SECURE_NO_DEPRECATE") )
                keep = false;
            if( util::prefix(line,"#define _CRT_SECURE_NO_WARNINGS") )
                keep = false;
            if( util::prefix(line,"using namespace std") )
                keep = false;
            if( util::prefix(line,"using namespace thc") )
                keep = false;
            if( keep )
                util::putline(out,line);
        }
    }
    util::putline(out,"");
    util::putline(out,"");
    return true;
}

bool files_match( const std::string &file1, const std::string &file2 )
{
    bool match = false;
    std::ifstream in1(file1);
    if( !in1 )
    {
        printf( "Cannot open %s\n", file1.c_str() );
        return false;
    }
    std::ifstream in2(file2);
    if( !in2 )
    {
        printf( "Cannot open %s\n", file2.c_str() );
        return false;
    }
    for(;;)
    {
        std::string line1;
        std::string line2;
        if( !std::getline(in1,line1) )
        {
            if( !std::getline(in2,line2) )
                match = true; // every line has matched, and files end at same point
            break;
        }
        if( !std::getline(in2,line2) )
            break;
        if( line1 != line2 )
            break;
    }
    return match;
}

