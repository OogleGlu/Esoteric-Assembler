/* Version: 2021.3.10
 * Description: Another assembly simulator
 */

#include "headers.h"
#include "global.c"
#include "input.c"
#include "output.c"
#include "interpreter.c"

#define VER "v2021.3.10.3"

void initialize() {
    tab = NULL;
    tabIndex = 0;
    console = false;
    input = false;
    printLbl = false;
    dev = false;
    file = NULL;
    FLAG = false;
    a = b = c = d = 0;
    dataPtr = 0;
    intBuffer = 0;
    lineNo = 1;
    steps = 0;
}

void evalOptions(int argsc, char **args, int indx) {
    // If indx argument is help
    if (!strcmp(args[indx], "--help") || !strcmp(args[indx], "-h")) {
        if (argsc > 2) {
            E1b: fprintf (stderr, RED "ERR> " RST "Too many arguments\n");
            quit(1);
        }
        printHelp(true);
        quit(0);
    }
    // if indx arg is console mode
    else if (!strcmp(args[indx], "--console") || !strcmp(args[indx], "-c")) {
        if (argsc > 2) {
            E1c: fprintf (stderr, RED "ERR> " RST "Too many arguments\n");
            quit(1);
        }
        printf ("Esoteric Assembler " VER " Console\n");
        printf ("NOTE: Function calls and jumps have been disabled.\n\n");
        console = true;
    }
    // if indx argument is version
    else if (!strcmp(args[indx], "--version") || !strcmp(args[indx], "-v")) {
        if (argsc > 2) {
            E1d: fprintf (stderr, RED "ERR> " RST "Too many arguments\n");
            quit(1);
        }
        printf ("Esoteric Assembler\n");
        printf ("Version: " VER "\n");
        quit(0);
    }
    // if it is dev
    else if (!strcmp(args[indx], "--dev") || !strcmp(args[indx], "-d")) {
        if (argsc > 3) {
            E1e: fprintf (stderr, RED "ERR> " RST "Too many arguments\n");
            quit(1);
        }
        /* incase dev is 1st argument, ie indx is 1, indx 2 may be left empty
         * in that case this error msg is printed
         */
        if (args[2] == NULL) {
            E2a: fprintf (stderr, RED "ERR> " RST "No file path entered\n");
            quit(2);
        }
        // make sure dev isn't enabled accidentally
        W1: printf (YEL "WRN> DEVELOPER (DEBUG) MODE\n" RST);
        printf ("  - This mode is for debugging the interpreter and not the asm\n    script.\n");
        printf ("  - This mode prints every token the interpreter reads in.\n");
        printf ("  - Label table is printed in this mode.\n");
        printf ("  - I/O prompts of asm script will look ugly so removing them is\n    recommended.\n");
        printf (YEL "Enter 'n' for normal execution. Enable debugger? (y/n) " RST);
        char s[8];
        scanStr(stdin, s, 8);
        if (s[0] == 'y' || s[0] == 'Y') {
            printLbl = true;
            dev = true;
        }
    }
    // if indx arg is labels
    else if (!strcmp(args[indx], "--labels") || !strcmp(args[indx], "-l")) {
        if (argsc > 3) {
            E1f: fprintf (stderr, RED "ERR> " RST "Too many arguments\n");
            quit(1);
        }
        // check if file path is not empty
        if (args[2] == NULL) {
            E2b: fprintf (stderr, RED "ERR> " RST "No file path entered\n");
            quit(2);
        }
        // set label table printing flag on
        printLbl = true;
    }
    else {
        // if arg is invalid
        E3: fprintf (stderr, RED "ERR> " RST "Invalid option: '%s'\n", unEscape(args[indx]));
        fprintf (stderr, RED "ERR> " RST "Use --help or -h for help text\n");
        quit(3);
    }
}

/* args[] is used to pass command line arguments to program
 | For eg:
 | ~/$ ass /path/to/file.ass
 | This is an execution of binary file ass with file path as 
 | an argument. Automatically, the shell counts no. of arguments, 
 | and assigns "ass"  to args[0] and "/path/to/file.c"  to args[1].
 | *args[] is a DDA of chars or SDA of strings. (Note how *a is same 
 | as a[0], so *(*a) or *a[0] is same as (a[0])[0].
 */
int main(int argsc, char *args[]) {
    initialize();                            // initialize variables
    int arg = 1;                             // initialize arg with 1. arg refers to the filepath argument
    /* If 1st argument (a string) is empty.
     | 0th argument is the executive command itself.
     */
    if(argsc == 1) {
        printf ("Usage:\n");
        printf ("  asm [filepath]\n  asm [OPTION]\n  asm [OPTION] [filepath]\n\n");
        printf ("Options:\n");
        printf ("  -h, --help       Display extended help text\n");
        printf ("  -l, --labels     Display declared labels in tabular form\n");
        printf ("  -c, --console    Console mode to execute codes from stdin\n");
        printf ("  -v, --version    Display version information\n");
        printf ("  -d, --dev        Developer mode to debug interpreter I/O\n");
        quit(0);
    }
    else if (argsc == 2) {            
        if (args[1][0] == '-') {
            evalOptions(argsc, args, 1);
            arg = 2;
        }
    }
    else if (argsc == 3) {
        if (args[2][0] == '-') {
            evalOptions(argsc, args, 2);
        }
        else if (args[1][0] == '-') {
            evalOptions(argsc, args, 1);
            arg = 2;
        }
    }
    else {
        E1a: fprintf (stderr, RED "ERR> " RST "Too many arguments\n");
        quit(1);
    }
    if (!console) openFile(args[arg]);                                     // if not console mode, open specified file path
    else file = stdin;                                                     // file set to stdin for console mode
    if (!console) genJmpTable();                                           // generate jump table
    fseek(file, 0, SEEK_SET);                                              // goto file beginning
    lineNo = 1;                                                            // set lineNo to line1
    if (console) printf(GRN "asm> " RST);                                  // prompt for console mode 
    else if (dev) printf (YEL "SCAN FILE FOR CODES\n" RST);                // heading for dev mode
    interpret();                                                           // scan the file for codes and interpret them
    if (dev) printf (GRN "\nEXECUTION COMPLETE | %lu Steps\n" RST, steps);
    quit(0);                                                               // closes file if it isn't null, exits with code 0
}
