#include <iostream>
#include <cmath>
#include <string>

//This is a CPP that will be compiled under c++ standard 11
//compilable with g++ -o main main.cpp -std=c++11

using namespace std;

typedef struct Process {
    int PID;
    int priority;
    string state;
    int core;
    int turnTime;
    int waitTime;
    int cpuTime;
    int remainTime;

} Process;

int main() {
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