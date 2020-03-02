#include "shell.h"
//#include <stdio.h>
#include <unistd.h>
#include <csignal>
//#include <stdlib.h>
//#include <sys/wait.h>
//#include <sys/types.h>
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


// PURPOSE: Kills all current processes
// PARAMETER: int sig
void shell::kill_all_proc(int sig)
{
    if(!PIDList.empty())//If any processes are in the vector list of PIDs than kill.
    {
        cout << "Ending... Killing all Processes from this session" << endl;
        for(int i : PIDList) // Kill all processes that are left running then
            // kill the program.
        {
            kill(i, SIGKILL);
        }
    }
    exit(sig); // Kill the program
}


// PURPOSE: Awaits a users commands, once received, sends to parser
// PARAMETER: none
void shell::listen()
{
    string input; //used to take the user's input
    while(input!="exit")//While the user didn't enter exit.
    {
        cout << "User >> ";//Prompt for the user to enter the command
        getline(cin,input);//grab the user's command
        parse_command(input);//Execute the command through the functions.
    }
};


// PURPOSE: Pushes command to the record of commands
// PARAMETER: None
void shell::add_to_history(string newCmd)
{
    hist_num++; // increment number of commands in history
    history.push_back(newCmd); // insert new command to history vector
}


// PURPOSE: Shoes the last 10 commands
// PARAMETER: None
void shell::display_history()
{
    if (history.empty()) //If nothing in history than display error.
    {
        cout << "Error, no commands in history."<< endl;
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


// PURPOSE: Runs the most recent command again
// PARAMETER: None
void shell::run_last_history()
{
    cout << history.back() << endl;
    parse_command(history.back());
}


// PURPOSE: Allows the user to choose a command to run again
// PARAMETER: n
void shell::run_nth_history(int n)
{
    cout << history[n - 1] << endl;
    parse_command(history[n - 1]);
}


// PURPOSE: To view the user's input, check for any specific commands like !!, !#, exit, history, and if any
// other regular command then execute the command.
// PARAMETER: string input
void shell::parse_command(string cmd_input)
{
    // check for case where empty line is entered as a command
    if (cmd_input == "")
    {
        cout << "Command not found!" << endl;
        return;
    }
    // add commands to history
    if (cmd_input.at(0) != '!')
    {
        add_to_history(cmd_input); // add the command to history only if it is not a !! or !# command
    }
    string key("&"); //To use for the rfind function next line.
    size_t found = cmd_input.rfind(key);//Starts from the rear of the string and
    //finds if there is an '&'
    // check if & is at end of command
    if (found!=std::string::npos) // check if parent should not wait
    {
        cmd_input.replace(found,key.length(),"");
        cout << cmd_input << endl;
        run_in_background = true;
    }

    // Parse out different types of commands
    if (cmd_input == "exit") // if user enters exit command
    {
        kill_all_proc(-1);
    }
    else if (cmd_input == "history") //If the user entered the command "history"
    {
        display_history();
    }
    else if (cmd_input == "!") // exceptin case for when just ! entered
    {
        cout << "Command not found!" << endl;
        return;
    }
    else if (cmd_input == "!!") // if the user enters "!!"
    {
        if (hist_num <= 0) // exception case for no commands in history
        {
            cout << "No commands in history." << endl;
        }
        else //otherwise run this function.
        {
            run_last_history();
        }
    }
    else if (cmd_input.at(0) == '!' && isdigit(cmd_input.at(1))) // if user enters "!#"
    {
        cmd_input.erase(cmd_input.begin()); // remove the ! from the command
        int nthCmd = atoi(cmd_input.c_str());// convert string to int for nth command
        if (nthCmd < 1 || nthCmd > (int)(history.size()))
        {
            cout << "No such command in history." << endl;
        }
        else //run the command that the user wants to run again.
        {
            run_nth_history(nthCmd);
        }
    }
    else // for all other commands, execute with unix shell commands
    {
        //Tokenize the user's command, view the command, parse the first part of it.
        char command [50];
        char* cmd_array[30];
        for(auto & i : cmd_array)//Nullify the array
        {
            i = NULL;
        }
        strcpy(command, cmd_input.c_str());//copy the string into a char array.
        char* p = strtok(command, " ");//tokenize the first word of the command line.
        int x = 0;
        while(p!= NULL) //while there is input til you hit NULL
        {
            cmd_array[x] = p;//insert the word into the command array.
            x++;//On the the next word.
            p = strtok(NULL, " ");//Check the next section if it is NULL. If it is exit while,
            // if not then grab the next data and place it in the cmd_array.

        }//After parsing then execute the command.
        execute_command(cmd_array);
    }
}


// PURPOSE: Read the command, make a fork() system call, and push the command onto the terminal screen and execute it.
// PARAMETER: char* cmd[]
void shell::execute_command(char *cmd_array[])
{
    pid_t pid = fork(); //fork the process
    PIDList.push_back(getpid()); //grab the process id and place it in the vector.
    //DEBUG cout << "After the fork command, pid is: " << pid << endl;
    if(pid < 0)// If the pid is below zero then report a Fork Failed!
    {
        cout << "Fork Failed!" << endl;
        kill_all_proc(-1);
    }
    if(pid == 0)
    {
        // DEBUG cout << "Child Pid: " << pid << endl;
        if(execvp(cmd_array[0], cmd_array) == -1) //If the command doesn't exist then tell the user.
        {
            cout << "This command doesn't exists!" << endl;
            exit(0);
        }
    }
    if(pid > 0)//If the Process is the Parent than wait
    {
        if (!run_in_background)
        {
            if (wait(0) == -1)
                perror("wait");
            // DEBUG cout << "Parent ID: " << pid << endl;
        }
        else
        {
            run_in_background = false;
            cout << endl << endl;
        }
    }
}


