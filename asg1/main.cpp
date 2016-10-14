#include <iostream>
#include <unistd.h>

#include "auxlib.h"
#include "string_set.h"
using namespace std;

void readopt (int argc, char  *argv[], const char *options){
    int c;

    while ((c = getopt (argc, argv, options))!= -1){
        switch (c)
        {
        case '@':
        /*Call set_debugflags, and use DEBUGF and DEBUGSTMT for debugging.
        The details of the flags are at the implementor’s discretion, and are
        not documented here. */
        break;

        case 'D':
        /*Pass this option and its argument to cpp. This is mostly useful as
        -D__OCLIB_OH__ to suppress inclusion of the code from oclib.oh
        when testing a program.*/
        break;

        case 'l':
        /*Debug yylex() with yy_flex_debug = 1*/
        break;

        case 'y':
        /*Debug yyparse() with yydebug = 1*/
        break;

        case '?':
        /*error checking*/
        return;
        }

    }
}

int main(int argc, char *argv[]) {
    readopt(argc, argv, "@Dly:");
	return 0;
}
