#include <iostream>
#include "shell.h"

using namespace std;

int main()
{
    cout << "====================================================\n"
         << "Michael Tobin and Travis Vensel\n"
         << "CS433 - Operating Systems\n"
         << "Assignment 2\n"
         << "Simulate a unix shell\n"
         << "====================================================" << endl;

    shell shell; // Create a new shell program
    shell.list_commands();
    shell.listen(); // Start listening for inputs
    return 0;
}