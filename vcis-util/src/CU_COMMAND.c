
#include <argp.h>
#include <stdbool.h>
#include "CU.h"
#include <string.h>
#include <stdlib.h> 
#include "version.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define DEFAULT_PROGRAM_ID 2
#define DEFAULT_SERVER_NODE_ID 0x49
#define DEFAULT_SOCKET_CAN_INTERFACE "can0"

const char *argp_program_version = "programname programversion";
const char *argp_program_bug_address = "<your@email.address>";
static char doc[] = "Your program description.";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = { 
    { "version", 'v', 0, 0, "Application Version Information"},
    { "program", 'p', "PROGRAM", 0, "Select Program Number to Download (Default: " STR(DEFAULT_PROGRAM_ID) ")"},
    { "file", 'f', "FILE", 0, "File to Program Device"},
    { "interface", 'i', "INTERFACE", 0, "SocketCAN Interface (Default: " DEFAULT_SOCKET_CAN_INTERFACE ")"},
    { "node", 'n', "NODE", 0, "Select CANopen Node (Default: " STR(DEFAULT_SERVER_NODE_ID) ")"},
    { "details", 'd', 0, 0, "Print CANopen Node Information"},
    { "reset", 'r', 0, 0, "Reset Adapter"},
    { "state", 's', "STATE", 0, "Set Node into Specified State. 0x01: operational, 0x02: Stopped, 0x80: pre-operational, 0x81: reset, 0x82: reset comms"},
    { "test", 't', "TEST", 0, "Send a Test Command"},    
    { 0 } 
};

struct arguments {
    enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode;
    bool isCaseInsensitive;
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    CU_TaskDetails *cmd = state->input;

    switch (key) {
    case 'p':
        if((strlen(arg) < 3) || (strcmp(arg, "0x") != 0))
        {
            cmd->programID = strtol(&arg[0], NULL, 10);
        }
        else
        {
            cmd->programID = strtol(&arg[2], NULL, 16);
        }   
        break;
    case 'f': 
        cmd->programFilename = arg; 
        // add program task
        CU_TASK_addTask("program", 0);
        break;
    case 'i': cmd->interfaceName = arg; break;
    case 'n': 
        if((strlen(arg) < 3) || (strncmp(arg, "0x", 2) != 0))
        {
            cmd->nodeID = strtol(&arg[0], NULL, 10);
        }
        else
        {
            cmd->nodeID = strtol(&arg[2], NULL, 16);
        }   
        break;
    case 'd': 
        // add task info task
        CU_TASK_addTask("info", 0);
        break;
    case 'r': 
        // add task reset
        CU_TASK_addTask("reset", 0);
        break;
    case 's':
        if((strlen(arg) < 3) || (strncmp(arg, "0x", 2) != 0))
        {
            cmd->options = strtol(&arg[0], NULL, 10);
        }
        else
        {
            cmd->options  = strtol(&arg[2], NULL, 16);
        }  
        CU_TASK_addTask("state", 0);
        break;
    case  'v':  // version information
        printf("%s - %s - %s\r\n", VERSION_GIT_HASH, VERSION_GIT_TAG, VERSION_GIT_DATE_LOCAL);
        exit(0);        
    case 't': 
        // add task reset
        CU_TASK_addTask("test", 0);
        break;        
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void CU_COMMAND_parseArgs(CU_TaskDetails *cmd, int argc, char *argv[])
{
    cmd->nodeID = DEFAULT_SERVER_NODE_ID;
    cmd->interfaceName = DEFAULT_SOCKET_CAN_INTERFACE;
    cmd->programID = DEFAULT_PROGRAM_ID;
 
    argp_parse(&argp, argc, argv, 0, 0, cmd);
}
