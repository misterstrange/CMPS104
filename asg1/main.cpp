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
    if (test.substr(test.find_last_of(".")) == ".oc"){
        check = true;
    }
    return check;
}

void ReadPipe(const char* ComLine, string_set &line){
    FILE* pipe = popen(ComLine, "r");
    if(pipe == nullptr){
        fprintf(stderr, "%s is not a valid command.\n", ComLine);
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


void WriteFile(char* direct, string ext, string_set line){
    string fn = direct;
    fn = fn.substr(0, fn.find_last_of('.'));

    fn.append(ext);

    FILE* output = fopen(fn.c_str(), "w");
    line.dump(output);
    fclose(output);
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
        /*Debug yylex() with yy_flex_debug = 1 can be ignored for asg1*/
        cout << "'l' was used as an option";
        break;

        case 'y':
        /*Debug yyparse() with yydebug = 1 can be ignored for asg1*/
        cout << "'y' was used as an option";
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

    //inputs command into the shell
    ReadPipe(command.c_str(), line);
    WriteFile(filename, ".str", line);


	return exec::exit_status;
}
