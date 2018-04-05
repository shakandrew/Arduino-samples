#include <string.h>

#define ON_STR "on"
#define OFF_STR "off"
#define UP_STR "up"
#define DOWN_STR "down"
#define PAUSE_STR "pause"
#define RESET_STR "reset"
#define PERIOD_STR "period"
#define HELP_STR "help"
#define SPLIT_STR " "

namespace CMD{
    typedef enum {
        PAUSE, // 0 = off | ~0 = on
        RESET,
        UP,
        DOWN,
        PERIOD, // up to many values
        HELP,
        ERROR
    } Command_type;

    const byte upper_lower_diff = 'a' - 'A';

    typedef struct {
        int value;
        Command_type cmd;
    } COMMAND;

    void init(COMMAND& cmd) {
        value = 0;
        cmd = PAUSE;
    }

    void checkCommand(COMMAND& cmd, char *str, byte n) {
        char *token;
        token = strtok(str, SPLIT_STR);
        to_lower(token);

        // cases where is a second param
        if (!strcmp(token, PAUSE_STR)) {
            token = strtok(NULL, SPLIT_STR);
            to_lower(token);
            pause_cmd(cmd, token);
        }
        else if (!strcmp(token, PERIOD_STR)) {
            token = strtok(NULL, SPLIT_STR);
            period_cmd(cmd, token);
        }

        if (strtok(NULL, SPLIT_STR)==NULL) error_cmd(cmd);

        if (!strcmp(token, DOWN_STR)) down_cmd(cmd);
        else if (!strcmp(token, HELP_STR)) help_cmd(cmd);
        else if (!strcmp(token, RESET_STR)) reset_cmd(cmd);
        else if (!strcmp(token, PAUSE_STR)) pause_cmd(cmd);
        else error_cmd(cmd);
    }

    void up_cmd(COMMAND& cmd) {
        cmd.cmd = UP;
    }

    void dowm_cmd(COMMAND& cmd) {
        cmd.cmd = DOWN;
    }

    void help_cmd(COMMAND& cmd) {
        cmd.cmd = HELP;
    }

    void reset_cmd(COMMAND& cmd) {
        cmd.cmd = RESET;
    }

    void pause_cmd(COMMAND& cmd, char * token) {
        cmd.cmd = PAUSE;

        if (!strcmp(token, ON_STR)) cmd.value = 1;
        else if (!strcmp(token, OFF_STR)) cmd.value = 0;
        else cmd.cmd = ERROR;
    }

    void period_cmd(COMMAND& cmd, char * token) {
        int temp = 0;
        cmd.cmd = PERIOD;

        for (byte i=0;token[i]!='\0';i++)
            if (token[i]<'0' || token[i]>'9') {
                cmd.cmd = ERROR;
                return;
            }
            temp = temp*10 + token[i]-'0';
    }

    void error_cmd(COMMAND& cmd) {
        cmd.cmd = ERROR;
    }

    // String functions
    void to_lower(char *str){
        for (byte i=0; str[i]!='\0';i++)
            if (str[i] >= 'A' || str[i] <= 'Z')
                  str[i] = str[i] + upper_lower_diff;
    }
}
