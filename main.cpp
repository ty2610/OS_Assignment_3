#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <sys/time.h>

//This is a CPP that will be compiled under c++ standard 11
//compilable with g++ -o main main.cpp -std=c++11

using namespace std;

struct Process {
    int PID;
    int priority;
    string state;
    int core;
    int turnTime;
    int waitTime;
    int cpuTime;
    int remainTime;
    int ioBursts;
    vector<int> ioBurstTimes;
    vector<int> cpuburstTimes;
    int startTime;
    int cpuBursts;
};

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

//Only need one
struct MainThread {

}mainThread;

struct Core {
    int id;
    bool idle;
    vector<Process> *processCollection;
} ;

bool isNumber(const string& s);
void takeCommand(int argc, char *argv[]);
vector<Process> createProcesses();
void* executeRoundRobin(void* obj);
void* executeFirstComeFirstServe(void* obj);
void* executeShortestJobFirst(void* obj);
void* executePreemptivePriority(void* obj);
void* mainThreadProcess(void* obj);
void* processActivator(void* obj);

int main(int argc, char *argv[]) {
    commandInput.cores = 0;
    commandInput.processors = 0;
    commandInput.algorithm = -1;
    commandInput.contextSwitch = 0;
    commandInput.timeSlice = 0;

    takeCommand(argc,argv);
    //checks to see if there are arguments, and error checks
    //Look over, I think this works, but it may be flawed

    //Create main thread
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, &mainThreadProcess, (void*)mainThread);

    pthread_join(mainThread, NULL);
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
/**
 * -c: Number of CPU cores (1-4)
 * -p: Number of processes to run (1-24)
 * -s: Scheduling algorithm (0: round-robin, 1: first come first serve, 2: shortest job first, 3: preemptive priority)
 * -o: Context switching overhead in milliseconds (100-1000)
 * -t: Time slice in milliseconds - only used for round-robin algorithm (200 - 2000)
 */
void takeCommand(int argc, char *argv[]) {
    string argumentIndicators[] = {"-c","-p","-s","-o","-t"};
    bool atArgument = false;
    if(argc > 1) {
        for(int i=1; i<argc; i++) {
            if(find(begin(argumentIndicators), end(argumentIndicators), string(argv[i])) != end(argumentIndicators)){
                if(atArgument == true) {
                    cout << "No argument provided with " << argv[i-1];
                    exit(0);
                } else {
                    atArgument = true;
                }
            } else if (atArgument == true) {
                if(string(argv[i-1]) == "-c"){
                    if(string(argv[i])=="1" || string(argv[i])=="2" || string(argv[i])=="3" || string(argv[i])=="4") {
                        commandInput.cores = stoi(argv[i]);
                    } else if (commandInput.cores != 0) {
                        cout << "You can not specify an argument twice." << endl;
                        exit(1);
                    } else {
                        cout << "The given argument for -c must be an integer and 1-4. " << argv[i] << endl;
                        exit(2);
                    }
                } else if(string(argv[i-1]) == "-p"){
                    if(isNumber(string(argv[i]))) {
                        int processorHolder = stoi(string(argv[i]));
                        if(processorHolder<1 || processorHolder>24){
                            cout << "The given number for argument -p must be greater than 0 but smaller than 25: " << argv[i] << endl;
                            exit(3);
                        } else if (commandInput.processors != 0) {
                            cout << "You can not specify an argument twice." << endl;
                            exit(4);
                        } else {
                            commandInput.processors = processorHolder;
                        }
                    } else {
                        cout << "The given command for argument -p is not an integer " << argv[i] << endl;
                        exit(5);
                    }
                } else if(string(argv[i-1]) == "-s"){
                    if(isNumber(string(argv[i]))) {
                        int algorithmHolder = stoi(string(argv[i]));
                        if(algorithmHolder<0 || algorithmHolder>3){
                            cout << "The given number for argument -s must be smaller than 4: " << argv[i] << endl;
                            exit(6);
                        } else if (commandInput.algorithm != -1) {
                            cout << "You can not specify an argument twice." << endl;
                            exit(7);
                        } else {
                            commandInput.algorithm = algorithmHolder;
                        }
                    } else {
                        cout << "The given command for argument -s is not an integer " << argv[i] << endl;
                        exit(8);
                    }
                } else if(string(argv[i-1]) == "-o"){
                    if(isNumber(string(argv[i]))) {
                        int contextSwitchHolder = stoi(string(argv[i]));
                        if(contextSwitchHolder<100 || contextSwitchHolder>1000){
                            cout << "The given number for argument -o must be smaller than 1001 and greater than 99: " << argv[i] << endl;
                            exit(9);
                        } else if (commandInput.contextSwitch != 0) {
                            cout << "You can not specify an argument twice." << endl;
                            exit(10);
                        } else {
                            commandInput.contextSwitch = contextSwitchHolder;
                        }
                    } else {
                        cout << "The given command for argument -o is not an integer " << argv[i] << endl;
                        exit(11);
                    }
                } else if(string(argv[i-1]) == "-t"){
                    if(isNumber(string(argv[i]))) {
                        int timeSliceHolder = stoi(string(argv[i]));
                        if(timeSliceHolder<200 || timeSliceHolder>2000){
                            cout << "The given number for argument -t must be smaller than 2001 and greater than 199: " << argv[i] << endl;
                            exit(12);
                        } else {
                            commandInput.timeSlice = timeSliceHolder;
                        }
                    } else if (commandInput.timeSlice != 0) {
                        cout << "You can not specify an argument twice." << endl;
                        exit(13);
                    } else {
                        cout << "The given command for argument -t is not an integer " << argv[i] << endl;
                        exit(14);
                    }
                }
                atArgument = false;
            } else {
                cout << "Incorrect command line argument " << i << endl;
                exit(15);
            }
        }
    } else {
        cout << "You must have arguments" << endl;
        exit(16);
    }
    if(commandInput.cores==0 || commandInput.algorithm==-1 || commandInput.processors==0 || commandInput.contextSwitch==0 || (commandInput.timeSlice==0 && commandInput.algorithm==0)) {
        cout << "The correct amount of arguments was not provided" << endl;
        exit(17);
    }
}

vector<Process> createProcesses() {
    int firstProcesses = floor(commandInput.processors/3);
    vector<Process> processCollection;
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
        for(int j=0; j<tempProcess.cpuBursts; j++) {
            //https://stackoverflow.com/questions/25649495/how-to-insert-element-at-beginning-of-vector
            //used this to learn how to push to the back of a vector (just switched start() with end())
            tempProcess.cpuburstTimes.insert(tempProcess.cpuburstTimes.end(),(rand() % 5001) + 1000);
        }
        for(int j =0; j<tempProcess.ioBursts; j++) {
            tempProcess.ioBurstTimes.insert(tempProcess.ioBurstTimes.end(),(rand() % 5001) + 1000);
        }
        processCollection.insert(processCollection.end(),tempProcess);
    }
    return processCollection;
}

void* mainThreadProcess(void* obj) {
    MainThread *mainThread = (MainThread*)obj;
    vector<Process> processCollection = createProcesses();
    //Create core threads
    pthread_t threads[commandInput.cores];
    Core *core = new Core[commandInput.cores];
    pthread_t processActivatorThread;
    pthread_create(&processActivatorThread, NULL, &processActivator, (void*)&processCollection);
    //looked up switch syntax
    //http://en.cppreference.com/w/cpp/language/switch
    //Will run the algorithm for the scheduler selected
    switch(commandInput.algorithm) {
        case 0:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = &processCollection;
                pthread_create(&threads[i], NULL, &executeRoundRobin, (void*)&core[i]);
            }
            break;
        case 1:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = &processCollection;
                pthread_create(&threads[i], NULL, &executeFirstComeFirstServe, (void*)&core[i]);
            }
            break;
        case 2:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = &processCollection;
                pthread_create(&threads[i], NULL, &executeShortestJobFirst, (void*)&core[i]);
            }
            break;
        case 3:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = &processCollection;
                pthread_create(&threads[i], NULL, &executePreemptivePriority, (void*)&core[i]);
            }
            break;
    }

    //Join all threads.
    for (int i=0; i<commandInput.cores; i++) {
        pthread_join(threads[i], NULL);
    }
    /*
     * Print CPU stats
     */
}

void* executeRoundRobin(void* obj) {
    Core *core = (Core*)obj;

}
void* executeFirstComeFirstServe(void* obj){
    Core *core = (Core*)obj;

}
void* executeShortestJobFirst(void* obj){
    Core *core = (Core*)obj;

}
void* executePreemptivePriority(void* obj) {
    Core *core = (Core*)obj;

}
void* processActivator(void* obj){
    vector<Process> *processCollection = (vector<Process>*)obj;
    /*
     * This is where we will have an infinite loop that keeps
     * checking the time of the simulation to switch processes
     * to ready when the randomized time has elapsed
     *
     */
}

//Use to get approporiate time for each process
void getTime(){
  struct timeval now;
  gettimeofday(&now, NULL);
}

//https://stackoverflow.com/questions/2150291/how-do-i-measure-a-time-interval-in-c
//Use to get turn time, wait time, and CPU time
double getElapsedTime(double t1, double t2){
	double result;
	
	result = (t2.tv_sec - t1.tv_sec) * 1000.0;
	result += (t2.tv_usec - t1.tv_usec) / 1000.0;
	
	return result;
}
