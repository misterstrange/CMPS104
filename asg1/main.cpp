#include <iostream>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

#include "auxlib.h"
#include "string_set.h"

constexpr size_t LINESIZE = 1024;




using namespace std;

const string CPP = "/usr/bin/cpp";

/* Chomp the last character from a buffer
    if it is delim. from cppstrtok.cpp*/
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

bool verifyOC(char* filename){
    string test = filename;
    bool check = false;
    if (test.substr(test.find_last_of(".")) == ".oc"){
        check = true;
    }
    return check;
}

//Run cpp against the lines of the file, from cppstrtok.cpp
void cpplines (FILE* pipe, const char* filename, string_set &line){
    int linenr = 1;
    char inputname[LINESIZE];
    strcpy(inputname, filename);
    for(;;){
        char buffer[LINESIZE];
        char* fgets_rc = fgets (buffer, LINESIZE,pipe);
        if (fgets_rc == nullptr) break;
        chomp(buffer, '\n');
        int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                                    &linenr, inputname);

        if (sscanf_rc==2){
            continue;
        }

        char* savepos = nullptr;
        char* bufptr = buffer;
        for (int tokenct = 1 ;; ++tokenct){
            char* token = strtok_r (bufptr, " \t\n", &savepos);
            bufptr = nullptr;
            if (token == nullptr) break;
            line.intern(token);
        }
        ++linenr;
    }
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
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr){
        exec::exit_status = EXIT_FAILURE;
        fprintf(stderr, "%s: %s: %s\n", exec::execname.c_str(),
                        command.c_str(), strerror(errno));
    }
    else{
        cpplines (pipe, filename, line);
        int pclose_rc = pclose (pipe);
        if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;

    }



    //WriteFile basname doesn't work for whatever reason
    string outputname = basename(filename);
    outputname = outputname.substr(0,outputname.find_last_of("."));
    outputname = outputname + ".str";
    FILE* output = fopen(outputname.c_str(), "w");

    line.dump(output);
    fclose(output);



    return exec::exit_status;
}
