#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

#include "auxlib.h"
#include "string_set.h"
#include "astree.h"
#include "lyutils.h"

constexpr size_t LINESIZE = 1024;
extern FILE * tok_file;




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


int main(int argc, char *argv[]) {
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

    //opens token file
    string outtok = basename(filename);
    outtok = outtok.substr(0,outtok.find_last_of("."));
    outtok = outtok + ".tok";
    tok_file = fopen(outtok.c_str(), "w");

    //reads file and puts through cpp
    yyin = popen(command.c_str(), "r");
    if (yyin == nullptr){
        exec::exit_status = EXIT_FAILURE;
        fprintf(stderr, "%s: %s: %s\n", exec::execname.c_str(),
                        command.c_str(), strerror(errno));
    }

    while(true){
        int token = yylex();
        if (yy_flex_debug) fflush (nullptr);
        if (token == YYEOF) break;

        line.intern(yytext);
    }
    pclose(yyin);


    //WriteFile basname doesn't work for whatever reason
    string outputname = basename(filename);
    outputname = outputname.substr(0,outputname.find_last_of("."));
    outputname = outputname + ".str";
    FILE* output = fopen(outputname.c_str(), "w");

    line.dump(output);
    fclose(output);

    fclose(tok_file);



    return exec::exit_status;
}
