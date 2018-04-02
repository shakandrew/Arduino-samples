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

typedef enum {
    PAUSE, // 0 = off | ~0 = on
    RESET,
    UP,
    DOWN,
    PERIOD, // up to many values
    HELP,
    ERROR
} COMMAND;

const byte upper_lower_diff = 'a' - 'A';

typedef struct {
    int value;
    COMMAND cmd;
} CMD;

void initCMD(CMD& cmd) {
    value = 0;
    cmd = PAUSE;
    Serial.begin(9600);
}

void checkForNewCommand(CMD& cmd) {
    if (Serial.available() > 0) {
        String str = Serial.readStringUntil('\n');
        Serial.println("Serial monitor accepted command: " + str);
        initCommand(cmd, (byte) str.length(), str.c_str());
    }
}

void initCommand(CMD& cmd, char *str, byte n) {
    char *token;
    token = strtok(str, SPLIT_STR);
    to_lower(token);

    if (!strcmp(token, UP_STR)) up_cmd(cmd);
    else if (!strcmp(token, DOWN_STR)) down_cmd(cmd);
    else if (!strcmp(token, HELP_STR)) help_cmd(cmd);
    else if (!strcmp(token, RESET_STR)) reset_cmd(cmd);
    else if (!strcmp(token, PAUSE_STR)) pause_cmd(cmd);
    else if (!strcmp(token, PERIOD_STR)) period_cmd(cmd);
    else error_cmd(cmd);
}

// TODO fix messages
void help() {
    Serial.print("You can use next commands:\n");
    Serial.print("\"pause on\" - to pause(freeze) the display, but let counter count\n");
    Serial.print("\"pause off\" - to start the display, it will show current counter numeral\n");
    Serial.print("\"reset\" - resets counter and numeral show on the display\n");
    Serial.print("\"help\" - show help\n");
}


void up_cmd(CMD& cmd) {
    if (strtok(NULL, SPLIT_STR)==null)
        cmd.cmd = UP;
    else
        cmd.cmd = ERROR;
}
void dowm_cmd(CMD& cmd) {
  if (strtok(NULL, SPLIT_STR)==null)
      cmd.cmd = DOWN;
  else
      cmd.cmd = ERROR;
}
void help_cmd(CMD& cmd) {
  if (strtok(NULL, SPLIT_STR)==null)
      cmd.cmd = HELP;
  else
      cmd.cmd = ERROR;
}
void reset_cmd(CMD& cmd) {
  if (strtok(NULL, SPLIT_STR)==null)
      cmd.cmd = RESET;
  else
      cmd.cmd = ERROR;
}
void pause_cmd(CMD& cmd) {
    char* token = strtok(NULL, SPLIT_STR);
    to_lower(token);
    cmd.cmd = PAUSE;
    if (!strcmp(token, ON_STR)) cmd.value = 1;
    else if (!strcmp(token, OFF_STR)) cmd.value = 0;
    else cmd.cmd = ERROR;
    if (strtok(NULL, SPLIT_STR)==null) cmd.cmd = ERROR;
}
void period_cmd(CMD& cmd) {
    token = strtok(NULL, SPLIT_STR);
    int temp = 0;
    for (byte i=0;token[i]!='\0';i++)
        if (token[i]<'0' || token[i]>'9') {
            cmd.cmd = ERROR;
            return;
        }
        temp = temp*10 + token[i]-'0';
    cmd.cmd = PERIOD;
}
void error_cmd(CMD& cmd) {
    cmd.cmd = ERROR;
}

// String functions
void to_lower(char *str){
    for (byte i=0; str[i]!='\0';i++)
        if (str[i] >= 'A' || str[i] <= 'Z')
              str[i] = str[i] + upper_lower_diff;
}
