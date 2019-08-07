#include <stdio.h>
#include <stdlib.h>
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
#define nbrof(array) (sizeof(array)/sizeof((array)[0]))

bool regenerate_h_file();
bool regenerate_cpp_file();

int main()
{
    if( regenerate_h_file() )
        if( regenerate_cpp_file() )
            return 0;
    return -1;
}

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
        " *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>",
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
        "        ChessEngine.h",
        "",
        " */",
        "",
        "#include \"Portability.h\"",
        "#include <stddef.h>",
        "#include <string>",
        "#include <vector>"
    };

    const char *hdr_files[]=
    {
        "src/ChessDefs.h",
        "src/Move.h",
        "src/ChessPositionRaw.h",
        "src/ChessPosition.h",
        "src/ChessRules.h",
        "src/ChessEvaluation.h",
        "src/ChessEngine.h"
    };

    std::ofstream out("src/thc-regen.h");
    if( !out )
    {
        printf( "Cannt open src/thc-regen.h\n" );
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
        " *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>",
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
        "        ChessEngine.cpp",
        "        Move.cpp",
        "        PrivateChessDefs.cpp",
        "         nested inline expansion of -> GeneratedLookupTables.h",
        " */",
        "",
        "// Don't reproduce this section",
        "#include <stdio.h>",
        "#include <stdlib.h>",
        "#include <string.h>",
        "#include <ctype.h>",
        "#include <assert.h>",
        "#include <algorithm>",
        "#include \"Portability.h\"",
        "#include \"DebugPrintf.h\"",
        "#include \"thc.h\"",
        "using namespace std;",
        "using namespace thc;"
    };

    const char *cpp_files[]=
    {
        "src/Portability.cpp",
        "src/PrivateChessDefs.h",
        "src/HashLookup.h",
        "src/ChessPosition.cpp",
        "src/ChessRules.cpp",
        "src/ChessEvaluation.cpp",
        "src/ChessEngine.cpp",
        "src/Move.cpp",
        "src/PrivateChessDefs.cpp"
    };

    std::ofstream out("src/thc-regen.cpp");
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
                std::ifstream in2("src/GeneratedLookupTables.h");
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

