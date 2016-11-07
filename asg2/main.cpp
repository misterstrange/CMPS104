#include <iostream>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <stdio.h>


#include "auxlib.h"
#include "string_set.h"
#include "lyutils.h"
#include "astree.h"

constexpr size_t LINESIZE = 1024;




using namespace std;

const string CPP = "/usr/bin/cpp";

bool verifyOC(char* filename){
    string test = filename;
    bool check = false;
    if (test.substr(test.find_last_of(".")) == ".oc"){
        check = true;
    }
    return check;
}

void cpp_pclose(string command){
    int pclose_rc = pclose(yyin);
    eprint_status (command.c_str(), pclose_rc);
    if (pclose_rc !=0) exit(exec::exit_status);
}

void scan(string test, string command, string_set &line){
    string basefile(basename(const_cast<char*> (test.c_str())));

    char* tokenfile = strdup((basefile.substr(0, basefile.length()-3)
                            + ".tok").c_str());

    FILE* tok_file = fopen (tokenfile, "w");
    if (tok_file == nullptr){
        fprintf (stderr, "There was a problem opening the .tok file");
    }
    else{
        for(;;){
            int token = yylex();

            if (yy_flex_debug) fflush(nullptr);
            if(token == YYEOF) break;

            const string* tok = line.intern(yytext);
            (void) tok;
        }
    }
    fprintf(tok_file, "test\n");
    fclose (tok_file);
    cpp_pclose(command);
}

void cpp_popen(char* filename, string command, string_set &line){
    string test = filename;
    yyin = popen (command.c_str(), "r");
    if (yyin == nullptr) syserrprintf(command.c_str());
    else{
        if(yy_flex_debug){
            fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                    command.c_str(), fileno(yyin));
        }
        lexer::newfilename(command.c_str());
        scan(test, command, line);
    }
}


int main(int argc, char *argv[]) {
    vector<string> args (&argv[1], &argv[argc]);
    char* filename;

    string command = CPP + " ";



    int c;
     while ((c = getopt (argc, argv, "@:D:ly"))!= -1){
        switch (c)
        {
        case '@':
        set_debugflags(optarg);
        break;

        case 'D':
        command = CPP + " -D" + optarg + " ";
        break;

        case 'l':
        yy_flex_debug = 1;
        break;

        case 'y':
        yydebug = 1;
        break;

        case '?':
        if (optopt == '@'){
            fprintf(stderr, "'@' option requires flags\n");
        }
        else if(optopt == 'D'){
            fprintf(stderr, "'D' option requires string\n");
        }

        }
    }

    if (argc > optind){
        filename = argv[optind];
    }
    else{
        fprintf(stderr, ".oc file not provided!\n");
        exec::exit_status = EXIT_FAILURE;
        return exec::exit_status;
    }


    if (verifyOC(filename) == true){
        command.append(filename);
    }
    else{
        fprintf(stderr, "filename did not have .oc extention\n");
        exec::exit_status = EXIT_FAILURE;
        return exec::exit_status;
    }

    string_set line;
    exec::execname = basename(argv[0]);

    cpp_popen(filename, command, line);

    string outputname = basename(filename);
    outputname = outputname.substr(0,outputname.find_last_of("."));
    outputname = outputname + ".str";
    FILE* output = fopen(outputname.c_str(), "w");

    line.dump(output);
    fclose(output);


    return exec::exit_status;
}
