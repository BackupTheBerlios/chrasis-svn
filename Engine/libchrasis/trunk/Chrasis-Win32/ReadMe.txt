libchrasis-win32
Compilation and Installation Guide

Building Environment
  - Windows XP
  - Microsoft Visual Studio 2005
  - sqlite3
    You'll need two files:
      - sqlitedll-<version>.zip
      - sqlite-amalgamation-<version>.zip
    Both them can be downloaded from sqlite homepage.
    Unzip them in one same directory, so that the directory
    contains 4 files:
      - sqlite3.def
      - sqlite3.dll
      - sqlite3.h
      - sqlite3ext.h
    The file sqlite3.c isn't that important, you can recycle
    it, delete it, do whatever with it, or just leave it there.
    Open up a VS 2005 Command Line Prompt, cd to that directory,
    type in
      - lib /def:sqlite3.def /MACHINE:x86
    This will generate two files,
      - sqlite3.exp
      - sqlite3.lib
    Great! You're done!
  - libxml2 (for tools)
    download from GnuWin32
  - iconv (for libxml2)
    download from GnuWin32