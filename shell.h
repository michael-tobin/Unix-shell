#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <cstring>
#include <vector>
using namespace std;

class shell
{
private:
    bool run_in_background; // Whether or not the process can run in the background.
    int hist_num; // A counter of commands in history.
    vector<int> PIDList; // Vector list of process IDs.
    vector<string> history; // Vector of commands entered.

public:
    shell();
    ~shell();
    void list_commands();
    void listen();
    void parse_command(string cmd_input);
    void execute_command (char *cmd_array[]);
    void add_to_history(string newCmd);
    void run_last_command();
    void run_nth_command(int n);
    void display_history();
    void kill_all_proc(int sig);
};

#endif