#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

//This is a CPP that will be compiled under c++ standard 11
//compilable with g++ -o main main.cpp -std=c++11

using namespace std;

bool isNumber(const string& s);

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
    int cpuBursts;

} Process;

int main(int argc, char *argv[]) {
    string argumentIndicators[] = {"-c","-p","-s","-o","-t"};
    int amountOfArgumentIndicators = 5;
    int cores = 0;
    int processors = 0;
    int algorithm = -1;
    int contextSwitch = 0;
    int timeSlice = 0;
    bool atArgument = false;
    //checks to see if there are arguments, and error checks
    //Look over, I think this works, but it may be flawed
    if(argc > 1) {
        for(int i=1; i<argc; i++) {
            if(find(begin(argumentIndicators), end(argumentIndicators), string(argv[i])) != end(argumentIndicators)){
                if(atArgument == true) {
                    cout << "No argument provided with " << argv[i-1];
                    return 0;
                } else {
                    atArgument = true;
                }
            } else if (atArgument == true) {
                if(string(argv[i-1]) == "-c"){
                    if(string(argv[i])=="1" || string(argv[i])=="2" || string(argv[i])=="3" || string(argv[i])=="4") {
                        cores = stoi(argv[i]);
                    } else {
                        cout << "The given argument for -c must be an integer and 1-4. " << argv[i] << endl;
                        return 0;
                    }
                } else if(string(argv[i-1]) == "-p"){
                    if(isNumber(string(argv[i]))) {
                        int processorHolder = stoi(string(argv[i]));
                        if(processorHolder<1 || processorHolder>24){
                            cout << "The given number for argument -p must be greater than 0 but smaller than 25: " << argv[i] << endl;
                            return 0;
                        } else {
                            processors = processorHolder;
                        }
                    } else {
                        cout << "The given command for argument -p is not an integer " << argv[i] << endl;
                        return 0;
                    }
                } else if(string(argv[i-1]) == "-s"){
                    if(isNumber(string(argv[i]))) {
                        int algorithmHolder = stoi(string(argv[i]));
                        if(algorithmHolder<0 || algorithmHolder>3){
                            cout << "The given number for argument -s must be smaller than 4: " << argv[i] << endl;
                            return 0;
                        } else {
                            algorithm = algorithmHolder;
                        }
                    } else {
                        cout << "The given command for argument -s is not an integer " << argv[i] << endl;
                        return 0;
                    }
                } else if(string(argv[i-1]) == "-o"){
                    if(isNumber(string(argv[i]))) {
                        int contextSwitchHolder = stoi(string(argv[i]));
                        if(contextSwitchHolder<100 || contextSwitchHolder>1000){
                            cout << "The given number for argument -o must be smaller than 1001 and greater than 99: " << argv[i] << endl;
                            return 0;
                        } else {
                            contextSwitch = contextSwitchHolder;
                        }
                    } else {
                        cout << "The given command for argument -o is not an integer " << argv[i] << endl;
                        return 0;
                    }
                } else if(string(argv[i-1]) == "-t"){
                    //may need if check to see if the given algorithm is round robin
                    //if it is, then don't do any error checking on the given value
                    //because we wont be using it anyways
                    if(isNumber(string(argv[i]))) {
                        int timeSliceHolder = stoi(string(argv[i]));
                        if(timeSliceHolder<200 || timeSliceHolder>2000){
                            cout << "The given number for argument -t must be smaller than 2001 and greater than 199: " << argv[i] << endl;
                            return 0;
                        } else {
                            timeSlice = timeSliceHolder;
                        }
                    } else {
                        cout << "The given command for argument -t is not an integer " << argv[i] << endl;
                        return 0;
                    }
                }
                atArgument = false;
            } else {
                cout << "Incorrect command line argument " << i << endl;
            }
        }
    }
    int firstProcesses = floor(processors/3);
    Process *processCollection = new Process[processors];
    for(int i=0; i<processors; i++){
        Process tempProcess;
        tempProcess.PID = 1024 +i;
        if(i<=firstProcesses){
            tempProcess.state = "Ready";
        } else {
            tempProcess.state = "Not Created";
        }
        if(algorithm==3){
            tempProcess.priority = rand() % 5;
        } else {
            tempProcess.priority = 0;
        }
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