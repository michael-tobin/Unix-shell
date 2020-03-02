#ifndef SHELL_H
#define SHELL_H

#include <cstring>
#include <vector>
using namespace std;

class shell
{
private:
    bool run_in_background; // Can this run in the background
    int hist_num; // A counter of commands in history
    vector<int> PIDList; // Vector list of processes.
    vector<string> history; // vector of commands entered.

public:
    shell(); // Default constructor
    ~shell(); // Default destructor
    void list_commands();
    void listen(); // Wait for user input
    void kill_all_proc(int sig);
    void add_to_history(string newCmd);
    void display_history();
    void run_last_history();
    void run_nth_history(int n);
    void parse_command(string cmd_input);
    void execute_command (char *cmd_array[]);
};

#endif