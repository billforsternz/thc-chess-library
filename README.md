*Please note* This repository has been neglected. It basically serves as
the genesis of the files src/thc.cpp and src/thc.h in the peer repository
tarrasch-chess-gui. Those files serve as a bedrock chess API for the program
Tarrasch Chess GUI. Those two files are basically a concatenation of the files
present in this library. The idea for that was inspired by SQLite, which is
similarly distributed as single concatenated .c and .h files to avoid the
pain point of integrating a library into a project.

My recommendation  is to use files src/thc.cpp and src/thc.h from peer
repository tarrasch-chess-gui in preference to this repository, simply
because they are being actively used. Of course it is easier to understand
the more granular presentation of the code in this repository. One day it
would be nice to add a python script or similar to this repository to
automatically regenerate the concatenated two file version of the library.

Background
==========

This is a general purpose chess library for C++. It provides clients with
simple facilities for creating and manipulating chess positions and moves
in accordance with the rules of standard chess. 

Features
========

* Import/export positions using FEN (Forsyth Edwards Notation)
* Import/export moves using SAN (Standard algebraic notation)
* Generate legal move list, understands all chess rules
* Make moves in positions, push and pop moves in positions
* Evaluate positions for mate and all draw rules
* Compress positions
* Generate fast position hash codes move by move.
* Fast operation using lookup tables, efficient data structures

Status
======

This code was written as part of the Tarrasch Chess GUI [see peer repository/(ies)]. At a certain
point it seemed sensible to break the Chess API out into a library. After some experience
with using a thc.lib / thc.a static library with Visual Studio/Windows and XCode/Mac I have
reluctantly concluded that libraries can be very annoying and I now use a slightly different
mechanism; I concatenate the modules together and provide a single thc.cpp and thc.h file
to include in any project in the simplest possible way. This idea was inspired by SQLite3
which does the same thing.  The thc.cpp and thc.h files are in the source directory, and they
are the only files you need EXCEPT, unfortunately DebugPrintf.h and Portability.h, which are
Tarrasch Chess GUI files and are also needed just at the moment. The presence of DebugPrintf.h
requires that the user contribute a DebugPrintfInner() or similar function.

To Do
=====

Firstly I need to get rid of DebugPrintf.h and Portability.h cross contamination from Tarrasch
(see above). Next priority is adding more and better tests and documentation. At the moment there
are just a few gratuitous tests in ChessEngine.cpp - dating from the time I was having trouble
porting to Mac.

I would like to do a bit of a renaming exercise - I am moving away from
"using" and prefer explicit use of namespaces. All the thc library code
is in namespace thc, so thc::position for example would be more logical
than thc::ChessPosition (note thc = triple happy chess, where triple
happy comes from triplehappy.com, a domain I use).

A Python or similar script to generate thc.cpp and thc.h from the constituent parts would be a
Good Thing (TM).

An annoying thing is that all the code in this repository seems to have reverted to Windows
CR/LF endings despite me carefully standardising on LF only. I need to sort this out. I blame
(possibly unfairly) GitHub.

Although you won't find much test code (yet), the code is battle proven from years of use and
incremental improvement inside the Tarrasch Chess GUI project.

Bill Forster <billforsternz@gmail.com> 25Mar2014
