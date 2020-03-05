#include "shell.h"
#include <cstdio>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <sys/wait.h>
#include <iostream>
using namespace std;


// PURPOSE: Default constructor
// PARAMETER: None
shell::shell()
{
    hist_num = 0;
    run_in_background = false;
}


// PURPOSE: Default destructor
// PARAMETER: None
shell::~shell()
{
    kill_all_proc(0);
}

// PURPOSE: Display some usable command suggestions to the user
// PARAMETER: None
void shell::list_commands()
{
    cout << "====================================================\n"
    << "Some usable commands include:\n"
    << "ls      Lists files in current directory\n"
    << "cd      Move back one directory\n"
    << "mkdir   Make a directory\n"
    << "rmdir   Remove direcory\n"
    << "rm      Remove file\n"
    << "mv      Move or rename files\n"
    << "history View the past 10 commands\n"
    << "!!      Run the last command again\n"
    << "!n      Run the nth command again\n"
    << "====================================================\n";
}


// PURPOSE: Awaits a users commands, once received, sends to parser
// PARAMETER: none
void shell::listen()
{
    string input;
    while(input!="exit") // As long as the user hasn't chosen to exit.
    {
        cout << "User >> "; // Prompt for the user to enter a command
        getline(cin,input); // Wait for input
        parse_command(input); // Send the input to the parser
    }
};


// PURPOSE: To view the user's input, check for any specific commands like !!, !#, exit, history, and if any
// other regular command then execute the command.
// PARAMETER: string input
void shell::parse_command(string cmd_input)
{
    // Case where user did not type a command, and pressed enter
   if (cmd_input.empty())
    {
        return;
    }

    // Case where the user entered only spaces as a command
    for (char i : cmd_input)
    {
        if (!isspace(i))
        {
            break;
        }
    }

    // **********************************************************************
    // Any command that has made it this far has at least SOMETHING typed in
    // **********************************************************************

    // Case where command is NOT preceded with !, used to indicate previous commands
    if (cmd_input.at(0) != '!')
    {
        add_to_history(cmd_input); // Add the command to history only if it is a new command
    }

    string key("&"); // Key for rfind
    size_t found = cmd_input.rfind(key); // Starts from the rear of the string and checks for the key
    if (found!=string::npos)
    {
        cmd_input.replace(found,key.length(),""); // remove the '&' from the end
        cout << cmd_input << endl;
        run_in_background = true; // Allows the process to run and the user to input more commands
    }

    // **********************************************************************
    // Any command that has made it this far is not empty, has been checked for
    // whether it should be run in the background, and (if not a repeat command
    // !! or !n) has been added to the history.
    // **********************************************************************

    // **********************************************************************
    // The next section checks for a few reserved special commands.
    // **********************************************************************
    // Case where the command is 'exit', kill all active processes and exit
    else if (cmd_input == "exit")
    {
        kill_all_proc(-1);
    }

    // Case where the command is 'history', display the last 10 commands
    else if (cmd_input == "history")
    {
        display_history();
    }

    // Case where the command is a single '!'; this saves the shell from an error
    else if (cmd_input == "!")
    {
        cout << "Command not found, enter a new command.\n";
    }

    // Case where the command is '!!', rerun the most recent command
    else if (cmd_input == "!!") // if the user enters "!!"
    {
        run_last_command();
    }

    // Case where user enters '!n", rerun the nth command
    else if (cmd_input.at(0) == '!' && isdigit(cmd_input.at(1)))
    {
        // Strip out the '!' and convert n to an int
        cmd_input.erase(cmd_input.begin());
        int n = atoi(cmd_input.c_str());

        // Case where the user has chosen an n value that is out of scope
        if (n < 1 || n > (int)(history.size()))
        {
            cout << "No results, try another command.\n";
        }

        // Case where n is valid, rerun the nth command
        else
        {
            run_nth_command(n);
        }
    }

    // **********************************************************************
    // Any other commands run through this function. The function takes the
    // command string and splits it into individual tokenized words, then
    // fills an array with pointers to these tokens. That array is then sent
    // to the execute command.
    // **********************************************************************

    else
    {

        // Create an array of character pointers and make them all point to null.
        char* cmd_array[30];
        for(auto & i : cmd_array)
        {
            i = nullptr;
        }

        // Copy the users input to an array of chars, this allows it to be tokenized
        char command [50]; // Create a blank array of chars
        strcpy(command, cmd_input.c_str()); // Copy the command string to the array.
        char* command_first_word = strtok(command, " "); // Tokenize it

        int x = 0; // Counter for position in the array
        while(command_first_word != nullptr)
            {
            cmd_array[x] = command_first_word; // insert the word into the command array.
            x++; // Move to the next position in the array.
            command_first_word = strtok(nullptr, " "); // Check the next section if it is NULL.
        }

        execute_command(cmd_array);
    }
}


// PURPOSE: Read the command, make a fork() system call, print the command, then
//          execute it.
// PARAMETER: The array of pointers to the command words
void shell::execute_command(char *cmd_array[])
{
    pid_t pid = fork(); // Fork the parent process
    PIDList.push_back(getpid()); // Store the PID into the vector of Process ID's

    // Case where the pid < zero, report a failure and kill all processes
    if(pid < 0)
    {
        cout << "Fork Failed, killing processes\n";
        kill_all_proc(-1);
    }

    // Case where PID == 0, execute the command. If failure is returned, tell the user else, continue.
    if(pid == 0)
    {
        if(execvp(cmd_array[0], cmd_array) == -1)
        {
            cout << "That is not a valid command.\n";
            exit(0);
        }
    }

    // Case where PID > 0, try to run the command in the background
    if(pid > 0)//If the Process is the Parent than wait
    {
        if (!run_in_background)
        {
            if (wait(0) == -1)
                perror("wait");
        }

        else
        {
            run_in_background = false;
            cout << "\n\n";
        }
    }
}

// PURPOSE: Pushes command to the record of commands
// PARAMETER: None
void shell::add_to_history(string newCmd)
{
    hist_num++; // increment number of commands in history
    history.push_back(newCmd); // insert new command to history vector
}


// PURPOSE: Runs the most recent command again
// PARAMETER: None
void shell::run_last_command()
{
    if (history.empty())
        cout << "Command not found.\n";

    else
    {
        cout << history.back() << endl;
        parse_command(history.back());
    }
}


// PURPOSE: Allows the user to choose a command to run again
// PARAMETER: n
void shell::run_nth_command(int n)
{
    cout << history[n - 1] << endl;
    parse_command(history[n - 1]);
}


// PURPOSE: Shoes the last 10 commands
// PARAMETER: None
void shell::display_history()
{
    if (history.empty()) //If nothing in history than display error.
    {
        cout << "Error, no commands in history.\n";
    }
    else //otherwise display the last ten commands used
    {
        int tenthCmd; // index for where history should stop displaying
        if (hist_num > 10) // if hist_num is greater than 10.
        {
            tenthCmd = hist_num - 10;// subtract 10 from hist_num.
        }
        else
        {
            tenthCmd = 0;
        }
        for (int nthCmd = hist_num - 1; nthCmd >= tenthCmd; nthCmd--) // will only display the last 10 items in history
        {
            cout << nthCmd+1 << " " << history[nthCmd] << endl;
        }
    }
}


// PURPOSE: Kills all current processes
// PARAMETER: int sig
void shell::kill_all_proc(int sig)
{
    if(!PIDList.empty())
    {
        cout << "Ending... Killing all Processes from this session.\n";
        for(int i : PIDList)
        {
            kill(i, SIGKILL);
        }
    }
    exit(sig); // Kill the program
}