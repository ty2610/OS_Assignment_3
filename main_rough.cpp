#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "Core.cpp"
#include "Process.cpp"
#include "FirstComeFirstServe.cpp"
#include "PreemptivePriority.cpp"
#include "RoundRobin.cpp"
#include "ShortestJobFirst.cpp"

using namespace std;

void processCommandInput(int argc, char *argv);
void initializeProcesses(vector<Process> *processQueue, int *nextPid, int *processesToCreate);
void* mainThreadProcess(void* obj);
void* coreProcess(void* obj);

struct NewProcessTime {
    int currTime;
    int randomInterval;
    int startTime;
};

//Only need one
struct CommandInput {
    //1-4
    int cpuCores;
    //1-24
    int processesToRun;
    //0-3
    int schedAlgorithm;
    //100-1000
    int csOverhead;
    //200-2000
    int timeSlice;
}commandInput;

int main(int argc, char *argv[]) {
    //pid starts at 1024 and increments by 1
    int nextPid = 1024;

    //Get all user input commands
    processCommandInput(argc, argv);

    //Initialize 1/3 of processes
    vector<Process> processQueue;
    int processesToCreate = commandInput.processesToRun / 3;
    initializeProcesses(&processQueue, &nextPid, &processesToCreate);

    pthread_t threads[commandInput.cpuCores];
    Core *core[commandInput.cpuCores];

    //Create core threads
    for (int i=0; i<commandInput.cpuCores; i++) {
        core[i].setId(i);
        core[i].setIdle(true);
        pthread_create(&threads[i], NULL, &coreProcess, (void*)core[i]);
    }

    //Create main thread
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, &mainThreadProcess, (void*)processQueue, (void*)core);

    //Join all threads.
    for (int i=0; i<commandInput.cpuCores; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

/**
 * Update properties
 * Update process, queue when it gets a process
 * @param obj
 * @return
 */
void* coreProcess(void* obj) {
    Core *core = (Core*)obj;
    Process currProcess = core->process;
    //Wait for instructions from scheduler
    while (!currProcess.ready) {
        wait(1000);
    }
    //Update process stuff
    //If they're equal it means last time was a cpuburst
    if (currProcess.cpuBursts == currProcess.ioBursts) {
        //Take off for ioburst
        currProcess.doingIo = true;
        currProcess.ioBursts--;
        currProcess.remainingTime -= (currProcess.cpuTime / currProcess.cpuBursts);
    } else if ( (currProcess.startWaitTime + currProcess.waitTime) >= time(0)){
        //Check if wait time is over. time(0) gives current time
    } else {
        //Put on for cpu / Done by scheduler?
    }
}

/**
 * Maybe use to print the cpu stats
 * @param obj
 * @return
 */
void* mainThreadProcess(void* processes, void* core, void  addNewProcess) {
    vector<Process> processQueue = (vector<Process>) processes;
    Core *cores = (Core) core;
    int addProcessTime = (int)addProcessTime;
    //Add new process at random interval 500-8000 ms

    pthread_t processActivatorThread;
    pthread_create(&processActivatorThread, NULL, &processActivator, (void*)&processCollection);

    switch (commandInput.schedAlgorithm) {
        case 0 :
            RoundRobin roundRobin = new RoundRobin();
            roundRobin.execute();
        case 1 :
            FirstComeFirstServe firstComeFirstServe = new FirstComeFirstServe();
            firstComeFirstServe.execute();
        case 2 :
            ShortestJobFirst shortestJobFirst = new ShortestJobFirst();
            shortestJobFirst.execute();
        case 3 :
            PreemptivePriority preemptivePriority = new PreemptivePriority();
            preemptivePriority.execute();
    }
    /*
     * Print CPU stats
     */
    cout << "| PID | Priority | State | Core | Turn Time | Wait Time | CPU Time | Remain Time |" << endl;
    cout << "+-----+----------+-------+------+-----------+-----------+----------+-------------+" << endl;
    string state = "READY";
    for (int i=0; i<processQueue.size(); i++) {
        cout << "| " + to_string(processQueue[i].getPid()) + "   | " + to_string(processQueue[i].getPriority()) + "        | " +
                state + " | " + to_string(processQueue[i].getCore()) + "    | " + to_string(process.turnTime) +
                "         | " +
                to_string(processQueue[i].getWaitTime()) + "         | " + to_string(processQueue[i].getCpuTime()) +
                "        | " +
                to_string(processQueue[i].getRemainingTime()) + "           |" << endl;
    }
}

/**
 * -c: Number of CPU cores (1-4)
 * -p: Number of processes to run (1-24)
 * -s: Scheduling algorithm (0: round-robin, 1: first come first serve, 2: shortest job first, 3: preemptive priority)
 * -o: Context switching overhead in milliseconds (100-1000)
 * -t: Time slice in milliseconds - only used for round-robin algorithm (200 - 2000)
 */
void processCommandInput(int argc, char *argv[]) {
    string argumentIndicators[] = {"-c", "-p", "-s", "-o", "-t"};
    int amountOfArgumentIndicators = argumentIndicators.length();
    bool atValidArg = false;

    if (argc > 1) {
        for (int i=0; i<argc; i++) {
            if(find(begin(argumentIndicators), end(argumentIndicators), string(argv[i])) != end(argumentIndicators)){
                if(atValidArg) {
                    cout << "No argument provided with " << argv[i-1];
                    exit(0);
                } else {
                    atValidArg = true;
                }
            } else if (atValidArg) {
                switch (string(argv[i-1])) {
                    case "-c" :
                        if(string(argv[i])=="1" || string(argv[i])=="2" || string(argv[i])=="3" || string(argv[i])=="4") {
                            commandInput.cpuCores = stoi(argv[i]);
                        } else if (commandInput.cpuCores != 0) {
                            cout << "You can not specify an argument twice." << endl;
                            exit(1);
                        } else {
                            cout << "The given argument for -c must be an integer and 1-4: " << argv[i] << endl;
                            exit(2);
                        }
                        atValidArg = false;
                        break;
                    case "-p" :
                        if (isNumber(string(argv[i]))) {
                            int processorHolder = stoi(string(argv[i]));
                            if (processorHolder<1 || processorHolder>24) {
                                cout << "The given number for argument -p must be greater than 0 but smaller than 25: " << argv[i] << endl;
                                exit(3);
                            } else if (commandInput.processors != 0) {
                                cout << "You can not specify an argument twice." << endl;
                                exit(4);
                            } else {
                                commandInput.processesToRun = processorHolder;
                            }
                        } else {
                            cout << "The given command for argument -p is not an integer: " << argv[i] << endl;
                            exit(5);
                        }
                        atValidArg = false;
                        break;
                    case "s" :
                        if(isNumber(string(argv[i]))) {
                            int algorithmHolder = stoi(string(argv[i]));
                            if(algorithmHolder<0 || algorithmHolder>3){
                                cout << "The given number for argument -s must be smaller than 4: " << argv[i] << endl;
                                exit(6);
                            } else if (commandInput.algorithm != -1) {
                                cout << "You can not specify an argument twice." << endl;
                                exit(7);
                            } else {
                                commandInput.schedAlgorithm = algorithmHolder;
                            }
                        } else {
                            cout << "The given command for argument -s is not an integer: " << argv[i] << endl;
                            exit(8);
                        }
                        atValidArg = false;
                        break;
                    case "o" :
                        if(isNumber(string(argv[i]))) {
                            int contextSwitchHolder = stoi(string(argv[i]));
                            if(contextSwitchHolder<100 || contextSwitchHolder>1000){
                                cout << "The given number for argument -o must be smaller than 1001 and greater than 99: " << argv[i] << endl;
                                exit(9);
                            }  else if (commandInput.contextSwitch != 0) {
                                cout << "You can not specify an argument twice." << endl;
                                exit(10);
                            } else {
                                commandInput.csOverhead = contextSwitchHolder;
                            }
                        } else {
                            cout << "The given command for argument -o is not an integer: " << argv[i] << endl;
                           exit(11);
                        }
                        atValidArg = false;
                        break;
                    case "t" :
                        //may need if check to see if the given algorithm is round robin
                        //if it is, then don't do any error checking on the given value
                        //because we wont be using it anyways
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
                            cout << "The given command for argument -t is not an integer: " << argv[i] << endl;
                            exit(14);
                        }
                        atValidArg = false;
                        break;
                    default :
                        cout << "Incorrect command line argument: " << i << endl;
                       exit(15);
                }
            }  else {
                cout << "You must have arguments" << endl;
                exit(16);
            }
            if(commandInput.cores==0 || commandInput.algorithm==-1 || commandInput.processors==0 || commandInput.contextSwitch==0 || (commandInput.timeSlice==0 && commandInput.algorithm==0)) {
                cout << "The correct amount of arguments was not provided" << endl;
                exit(17);
            }
        }
    }
}

void initializeProcesses(vector<Process> *processQueue, int *nextPid, int *processesToCreate) {
    //Create new process and put on queue
    for (int i=0; i<*processesToCreate; i++) {
        Process *newProcess = new Process();

        //Give all properties a random value within the range outlined in assignment
        random_device randomDevice;
        mt19937 mt(randomDevice());
        uniform_real_distribution<int> ioBurstRand(2, 10);
        uniform_real_distribution<int> ioTimeRand(2, 10);
        uniform_real_distribution<int> cpuBurstRand(1000, 6000);
        uniform_real_distribution<int> cpuTimeRand(1000, 6000);
        uniform_real_distribution<int> priorityRand(0, 4);

        newProcess->setPriority(*nextPid);
        newProcess->setIoBursts(ioBurstRand(mt));
        newProcess->setIoTime(ioTimeRand(mt));
        newProcess->setCpuBursts(cpuBurstRand(mt));
        newProcess->setCpuTime(cpuTimeRand(mt));

        //Put the process in a vector (queue)
        processQueue->push_back(newProcess);
        nextPid++;
        //Update how many processes are yet to run
        commandInput.processesToRun--;
    }

    void* processActivator(void* obj) {
        vector<Process> *processCollection = (vector<Process>*)obj;
        /*
         * This is where we will have an infinite loop that keeps
         * checking the time of the simulation to switch processes
         * to ready when the randomized time has elapsed
         *
         */
    }
}