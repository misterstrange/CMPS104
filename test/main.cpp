/* Juan Gonzalez
   1497521
    CMPS104a Assignment 1

    DEBUGGING
    -@
    @ - Enable all debug flags
    f - Debug filename
    o - Debug command line to pipe
*/

#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#include "auxlib.h"
#include "lyutils.h"
#include "astree.h"
#include "stringset.h"

struct file_op {

    char * file_name;
    string commands = "cpp ";
};
FILE * tok_file;

// Scans through command line options using optget(3)
void options( int argc, char * argv[],
    const char * options, file_op &ops ) {

    yy_flex_debug = yydebug = 0;

    int op;
    while ( (op = getopt( argc, argv, options )) != -1 ) {

        switch ( op ) {

            case 'l':
                yy_flex_debug = 1;
                break;

            case 'y':
                yydebug = 1;
                break;

            case '@':
                set_debugflags(optarg);
                break;

            case 'D':
                ops.commands.append("-D ");
                ops.commands.append(optarg );
                ops.commands.append(" ");
                break;

            case '?':

                if ( optopt == '@' || optopt == 'D' )
                    fprintf(stderr, "-%c needs an argument\n", optopt);
                else
                    fprintf(stderr, "Invalid option: -%c\n", optopt);
        }
    }

    if ( argc > optind ) {

        DEBUGF( 'f', "argv[optind] = %s\n", argv[optind] );
        ops.file_name = &*argv[optind];
    }
    else
        ops.file_name = nullptr;
}

// Used to verify whether the file is a proper .oc
// Returns a bool true if valid - false if invalid
bool verify( char * file_name ) {

    char * pos = strrchr(file_name, '.');

    bool check = pos && (strcmp(pos, ".oc") == 0);
    DEBUGF( 'f', "File Verification: %d\n", check );

    return check;
}

void read_pipe( const char * cmd_line) {

    yyin = popen(cmd_line, "r");
    if ( !yyin ) {

        fprintf(stderr, "Error running cpp!");
        set_exitstatus( EXIT_FAILURE );
        return;
    }

    while (true) {

        int token = yylex();
        if (token == YYEOF) break;

        stringset::intern_stringset (yytext);
    }

    pclose(yyin);
}

void write_file( char * path, string extension) {

    string file_name = path;
    file_name = file_name.substr(0, file_name.find_last_of('.'));
    DEBUGF('f', "Basename: %s\n", file_name.c_str());

    file_name.append(extension);
    DEBUGF('f', "Writing to file -> %s\n", file_name.c_str());

    FILE * output = fopen(file_name.c_str(), "w");
    stringset::dump_stringset(output);
    fclose(output);
}

void open_tok( char * path, string extension) {

    string file_name = path;
    file_name = file_name.substr(0, file_name.find_last_of('.'));
    file_name.append(extension);
    tok_file = fopen(file_name.c_str(), "w");
}

void close_tok() {
    fclose(tok_file);
}

int main( int argc, char * argv[] ) {

    char execname[] = "Hello";
    set_execname(execname);

    file_op ops;
    options(argc, argv, "ly@:D:", ops);
    DEBUGF( 'f', "File Name: %s\n", ops.file_name );
    if ( !ops.file_name || !verify(ops.file_name) ) {

        fprintf(stderr, "Invalid .oc file or was not provided!");
        set_exitstatus( EXIT_FAILURE );
        return get_exitstatus();
    }

    ops.commands.append(ops.file_name);
    DEBUGF( 'o', "CPP command line: %s\n", ops.commands.c_str() );

    stringset set;

    open_tok(ops.file_name, ".tok");
    read_pipe(ops.commands.c_str());
    write_file(ops.file_name, ".str");
    close_tok();

    return get_exitstatus();
}

