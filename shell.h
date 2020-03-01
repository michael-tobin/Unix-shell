#ifndef SHELL_H
#define SHELL_H

#include <cstring>
#include <vector>
using namespace std;

class shell
{
private:
    bool runBackground; // Can this run in the background
    int hist_num; // number of commands in history
    vector<int> PIDList; // Vector list of processes.
    vector<string> history; // vector of commands entered.

public:
    //Default constructor and destructor
    shell();
    ~shell();
    void listen();
    void killAllProcesses(int);
    void addHistory(string);
    void displayHistory();
    void runRecentHistory();
    void runNthHistory(int);
    void executeCmd (char* []);
    void parseCmd(string);
};

#endif