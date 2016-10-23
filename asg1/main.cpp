#include <iostream>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#include "auxlib.h"
#include "string_set.h"



using namespace std;

const string CPP = "/usr/bin/cpp";

bool verifyOC(char* filename){
    string test = filename;
    bool check = false;
    if (test.substr(test.find_last_of("."), 0) == ".oc"){
        check = true;
    }
    return check;
}

void ReadPipe(const char* ComLine, string_set &line){
    FILE* pipe = popen(ComLine, "r");
    if(pipe == nullptr){
        fprintf(stderr, "%s is not a valid command.", ComLine);
        exec::exit_status = EXIT_FAILURE;
        return;
    }
    int LineSize = 1024;
    char buffer[LineSize], *token, *saveptr;

    while (fgets(buffer, LineSize, pipe)){
        token = strtok_r(buffer, " \t\n", &saveptr);

        while(token){
            if (strcmp("#", token)==0) break;
            line.intern(token);
            token = strtok_r(nullptr, " \t\n", &saveptr);
        }
    }
    pclose(pipe);

}

/*Don't forget: you are using a struct instead of class,
so you need to edit ReadPipe accordingly.*/


int main(int argc, char *argv[]) {
    char* filename;

    string command = CPP + " ";



    int c;
     while ((c = getopt (argc, argv, "@:D:ly"))!= -1){
        switch (c)
        {
        case '@':
        /*Call set_debugflags, and use DEBUGF and DEBUGSTMT for debugging.
        The details of the flags are at the implementor’s discretion, and are
        not documented here. */
        set_debugflags(optarg);
        break;

        case 'D':
        /*Pass this option and its argument to cpp. This is mostly useful as
        -D__OCLIB_OH__ to suppress inclusion of the code from oclib.oh
        when testing a program.*/
        command = CPP + " -D" + optarg + " ";
        break;

        case 'l':
        /*Debug yylex() with yy_flex_debug = 1 can be ignored for asg1*/
        cout << "'l' was used as an option";
        break;

        case 'y':
        /*Debug yyparse() with yydebug = 1 can be ignored for asg1*/
        cout << "'y' was used as an option";
        break;

        case '?':
        if (optopt == '@'){
            fprintf(stderr, "'@' option requires flags");
        }
        else if(optopt == 'D'){
            fprintf(stderr, "'D' option requires string");
        }

        }
    }

    if (argc > optind){
        filename = argv[optind];
    }
    else{
        fprintf(stderr, ".oc file not provided!");
        exec::exit_status = EXIT_FAILURE;
        return exec::exit_status;
    }


    if (verifyOC(filename) == true){
        command.append(filename);
    }
    else{
        fprintf(stderr, "filename did not have .oc extention");
        exec::exit_status = EXIT_FAILURE;
        return exec::exit_status;
    }

    string_set line;
    exec::execname = basename(argv[0]);

    //inputs command into the shell
    ReadPipe(command.c_str(), line);
    //WriteFile


	return exec::exit_status;
}
