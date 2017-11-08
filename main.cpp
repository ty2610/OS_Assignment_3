#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

//This is a CPP that will be compiled under c++ standard 11
//compilable with g++ -o main main.cpp -std=c++11

using namespace std;

bool isNumber(const string& s);
void takeCommand(int argc, char *argv[]);

typedef struct Process {
    int PID;
    int priority;
    string state;
    int core;
    int turnTime;
    int waitTime;
    int cpuTime;
    int remainTime;
    //MUST ADD A DATATYPE to hold the amount of bursts
    //and an array of ints that counts how long each cpu/io burst takes.
    int ioBursts;
    int *ioBurstTimes;
    int *cpuburstTimes;
    int startTime;
    int cpuBursts;
} Process;

struct CommandInput {
    //1-4
    int cores;
    //1-24
    int processors;
    //0-3
    int algorithm;
    //100-1000
    int contextSwitch;
    //200-2000
    int timeSlice;
}commandInput;

int main(int argc, char *argv[]) {
    commandInput.cores = 0;
    commandInput.processors = 0;
    commandInput.algorithm = -1;
    commandInput.contextSwitch = 0;
    commandInput.timeSlice = 0;

    takeCommand(argc,argv);
    //checks to see if there are arguments, and error checks
    //Look over, I think this works, but it may be flawed

    int firstProcesses = floor(commandInput.processors/3);
    Process *processCollection = new Process[commandInput.processors];
    for(int i=0; i<commandInput.processors; i++){
        Process tempProcess;
        tempProcess.PID = 1024 +i;
        if(i<=firstProcesses){
            tempProcess.state = "Ready";
            tempProcess.startTime = 0;
        } else {
            tempProcess.state = "Not Created";
            tempProcess.startTime = (rand() % 7501) + 500;
        }
        if(commandInput.algorithm==3){
            tempProcess.priority = rand() % 5;
        } else {
            tempProcess.priority = 0;
        }
        tempProcess.ioBursts = (rand() % 9) + 2;
        tempProcess.cpuBursts = tempProcess.ioBursts + 1;
        int *cpuBursts = new int[tempProcess.cpuBursts];
        int *ioBursts = new int[tempProcess.ioBursts];
        for(int j=0; j<tempProcess.cpuBursts; j++) {
            cpuBursts[j] = (rand() % 5001) + 1000;
        }
        for(int j =0; j<tempProcess.ioBursts; j++) {
            ioBursts[j] = (rand() % 5001) + 1000;
        }
        //WILL ADD THIS PROCESS TO VECTOR. WILL EVENTUALLY BE ADDED TO QUEUE
        //WILL BE MOVED TO OWN FUNCTION
    }

    //THE OUTPUT SHOULD BE PUT IN IT'S OWN METHOD
    Process process;
    process.PID = 0;
    process.priority = 0;
    process.state = "Ready";
    process.core = 0;
    process.turnTime = 0;
    process.waitTime = 0;
    process.cpuTime = 0;
    process.remainTime = 0;
    cout << "| PID | Priority | State | Core | Turn Time | Wait Time | CPU Time | Remain Time |" << endl;
    cout << "+-----+----------+-------+------+-----------+-----------+----------+-------------+" << endl;
    string state = "READY";
    for(int i=0; i<5; i++) {
        cout << "| " + to_string(process.PID) + "   | " + to_string(process.priority) + "        | " + state + " | " + to_string(process.core) + "    | "  + to_string(process.turnTime) + "         | " + to_string(process.waitTime)+ "         | "  + to_string(process.cpuTime) + "        | "  + to_string(process.remainTime) + "           |" << endl;
    }
    return 0;
}


//String to int checker to make sure it is valid
//Goes character by character and makes sure it is 0-9
//https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool isNumber(const string& s)
{
    return !s.empty() && find_if(s.begin(),
                                      s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void takeCommand(int argc, char *argv[]) {
    string argumentIndicators[] = {"-c","-p","-s","-o","-t"};
    int amountOfArgumentIndicators = 5;
    bool atArgument = false;
    if(argc > 1) {
        for(int i=1; i<argc; i++) {
            if(find(begin(argumentIndicators), end(argumentIndicators), string(argv[i])) != end(argumentIndicators)){
                if(atArgument == true) {
                    cout << "No argument provided with " << argv[i-1];
                    exit(3);
                } else {
                    atArgument = true;
                }
            } else if (atArgument == true) {
                if(string(argv[i-1]) == "-c"){
                    if(string(argv[i])=="1" || string(argv[i])=="2" || string(argv[i])=="3" || string(argv[i])=="4") {
                        commandInput.cores = stoi(argv[i]);
                    } else {
                        cout << "The given argument for -c must be an integer and 1-4. " << argv[i] << endl;
                        exit(3);
                    }
                } else if(string(argv[i-1]) == "-p"){
                    if(isNumber(string(argv[i]))) {
                        int processorHolder = stoi(string(argv[i]));
                        if(processorHolder<1 || processorHolder>24){
                            cout << "The given number for argument -p must be greater than 0 but smaller than 25: " << argv[i] << endl;
                            exit(3);
                        } else {
                            commandInput.processors = processorHolder;
                        }
                    } else {
                        cout << "The given command for argument -p is not an integer " << argv[i] << endl;
                        exit(3);
                    }
                } else if(string(argv[i-1]) == "-s"){
                    if(isNumber(string(argv[i]))) {
                        int algorithmHolder = stoi(string(argv[i]));
                        if(algorithmHolder<0 || algorithmHolder>3){
                            cout << "The given number for argument -s must be smaller than 4: " << argv[i] << endl;
                            exit(3);
                        } else {
                            commandInput.algorithm = algorithmHolder;
                        }
                    } else {
                        cout << "The given command for argument -s is not an integer " << argv[i] << endl;
                        exit(3);
                    }
                } else if(string(argv[i-1]) == "-o"){
                    if(isNumber(string(argv[i]))) {
                        int contextSwitchHolder = stoi(string(argv[i]));
                        if(contextSwitchHolder<100 || contextSwitchHolder>1000){
                            cout << "The given number for argument -o must be smaller than 1001 and greater than 99: " << argv[i] << endl;
                            exit(3);
                        } else {
                            commandInput.contextSwitch = contextSwitchHolder;
                        }
                    } else {
                        cout << "The given command for argument -o is not an integer " << argv[i] << endl;
                        exit(3);
                    }
                } else if(string(argv[i-1]) == "-t"){
                    //may need if check to see if the given algorithm is round robin
                    //if it is, then don't do any error checking on the given value
                    //because we wont be using it anyways
                    if(isNumber(string(argv[i]))) {
                        int timeSliceHolder = stoi(string(argv[i]));
                        if(timeSliceHolder<200 || timeSliceHolder>2000){
                            cout << "The given number for argument -t must be smaller than 2001 and greater than 199: " << argv[i] << endl;
                            exit(3);
                        } else {
                            commandInput.timeSlice = timeSliceHolder;
                        }
                    } else {
                        cout << "The given command for argument -t is not an integer " << argv[i] << endl;
                        exit(3);
                    }
                }
                atArgument = false;
            } else {
                cout << "Incorrect command line argument " << i << endl;
                exit(3);
            }
        }
    } else {
        cout << "You must have arguments" << endl;
        exit(3);
    }
    if(commandInput.cores==0 || commandInput.algorithm==0 || commandInput.processors==0 || commandInput.contextSwitch==0 || (commandInput.timeSlice==0 && commandInput.algorithm!=-1)) {
        cout << "The correct amount of arguments was not provided" << endl;
        exit(3);
    }
}