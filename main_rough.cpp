#include <iostream>
#include <vector>
#include <random>
#include <string>

/**
 * Processes Scheduling
 * @return
 */

using namespace std;

void processCommandInput(int argc, char *argv);
void initializeProcesses(vector<Process> *processQueue, int *nextPid, int *processesToCreate);
void* mainThreadProcess(void* obj);
void* coreProcess(void* obj);

//Only need one
struct MainThread {

}mainThread;

struct Core {
    int id;
    bool idle;
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

struct Process {
    //Start at 1024 and increment up
    int pid;
    //Random number between 2 and 10
    int ioBursts;
    //Random number between 1000 and 6000
    int ioTime;
    //Random number between 2 and 10
    int cpuBursts;
    //Random number between 1000 and 6000
    int cpuTime;
    //Random number between 0 and 4
    int priority;
};

int main(int argc, char *argv[]) {
    //pid starts at 1024 and increments by 1
    int nextPid = 1024;

    //Get all user input commands
    processCommandInput(argc, argv);

    //Initialize 1/3 of processes
    vector<Process> processQueue;
    int processesToCreate = commandInput.processesToRun / 3;
    initializeProcesses(&processQueue, &nextPid, &processesToCreate);

    //Create main thread
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, &mainThreadProcess, (void*)mainThread);

    //Create core threads
    pthread_t threads[commandInput.cpuCores];
    Core *core[commandInput.cpuCores];
    for (int i=0; i<commandInput.cpuCores; i++) {
        core[i]->id = i;
        core[i]->idle = true;
        pthread_create(&threads[i], NULL, &coreProcess, (void*)core[i]);
    }

    //Join all threads.
    for (int i=0; i<commandInput.cpuCores; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

/**
 * Do whatever a core needs to do.
 * If idle, get new process
 * Update process, queue when it gets a process
 * @param obj
 * @return
 */
void* coreProcess(void* obj) {
    Core *core = (Core*)obj;
}

/**
 * Maybe use to print the cpu stats
 * @param obj
 * @return
 */
void* mainThreadProcess(void* obj) {
    MainThread *mainThread = (MainThread*)obj;

    /*
     * Print CPU stats
     */
}

/**
 *
 ********* SUPER INEFFICIENT IMPLEMENTATION *******
 ********* NO ERROR CHECKING YET ********
 *
 * -c: Number of CPU cores (1-4)
 * -p: Number of processes to run (1-24)
 * -s: Scheduling algorithm (0: round-robin, 1: first come first serve, 2: shortest job first, 3: preemptive priority)
 * -o: Context switching overhead in milliseconds (100-1000)
 * -t: Time slice in milliseconds - only used for round-robin algorithm (200 - 2000)
 */
void processCommandInput(int argc, char *argv) {
    string argumentIndicators[] = {"-c", "-p", "-s", "-o", "-t"};
    int amountOfArgumentIndicators = argumentIndicators.length();
    bool atValidArg = false;

    if (argc > 1) {
        for (int i=0; i<argc; i++) {
            if(find(begin(argumentIndicators), end(argumentIndicators), string(argv[i])) != end(argumentIndicators)){
                if(atValidArg) {
                    cout << "No argument provided with " << argv[i-1];
                    return 0;
                } else {
                    atValidArg = true;
                }
            } else if (atValidArg) {
                switch (string(argv[i-1])) {
                    case "-c" :
                        if(string(argv[i])=="1" || string(argv[i])=="2" || string(argv[i])=="3" || string(argv[i])=="4") {
                            commandInput.cpuCores = stoi(argv[i]);
                        } else {
                            cout << "The given argument for -c must be an integer and 1-4: " << argv[i] << endl;
                            return 0;
                        }
                        atValidArg = false;
                        break;
                    case "-p" :
                        if (isNumber(string(argv[i]))) {
                            int processorHolder = stoi(string(argv[i]));
                            if (processorHolder<1 || processorHolder>24) {
                                cout << "The given number for argument -p must be greater than 0 but smaller than 25: " << argv[i] << endl;
                                return 0;
                            } else {
                                commandInput.processesToRun = processorHolder;
                            }
                        } else {
                            cout << "The given command for argument -p is not an integer: " << argv[i] << endl;
                            return 0;
                        }
                        atValidArg = false;
                        break;
                    case "s" :
                        if(isNumber(string(argv[i]))) {
                            int algorithmHolder = stoi(string(argv[i]));
                            if(algorithmHolder<0 || algorithmHolder>3){
                                cout << "The given number for argument -s must be smaller than 4: " << argv[i] << endl;
                                return 0;
                            } else {
                                commandInput.schedAlgorithm = algorithmHolder;
                            }
                        } else {
                            cout << "The given command for argument -s is not an integer: " << argv[i] << endl;
                            return 0;
                        }
                        atValidArg = false;
                        break;
                    case "o" :
                        if(isNumber(string(argv[i]))) {
                            int contextSwitchHolder = stoi(string(argv[i]));
                            if(contextSwitchHolder<100 || contextSwitchHolder>1000){
                                cout << "The given number for argument -o must be smaller than 1001 and greater than 99: " << argv[i] << endl;
                                return 0;
                            } else {
                                commandInput.csOverhead = contextSwitchHolder;
                            }
                        } else {
                            cout << "The given command for argument -o is not an integer: " << argv[i] << endl;
                            return 0;
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
                                return 0;
                            } else {
                                commandInput.timeSlice = timeSliceHolder;
                            }
                        } else {
                            cout << "The given command for argument -t is not an integer: " << argv[i] << endl;
                            return 0;
                        }
                        atValidArg = false;
                        break;
                    default :
                        cout << "Incorrect command line argument: " << i << endl;
                        return 0;
                }
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

        newProcess->pid = *nextPid;
        newProcess->ioBursts = ioBurstRand(mt);
        newProcess->ioTime = ioTimeRand(mt);
        newProcess->cpuBursts = cpuBurstRand(mt);
        newProcess->cpuTime = cpuTimeRand(mt);

        //Put the process in a vector (queue)
        processQueue->push_back(*newProcess);
        nextPid++;
        //Update how many processes are yet to run
        commandInput.processesToRun--;
    }
}

/**
 * NOT DONE *
 * Add newly created "processes" to the end of the ready queue
 * When a core becomes available, schedule the "process" at the front of the ready queue on that core
 * If a "process's" time slice expires before the CPU burst is done, remove "process" from core and place at the end of the ready queue
 * When a "process" finishes an I/O burst, place it at the end of the ready queue
 */
void roundRobin(Process *process, CommandInput *commandInput) {
    //Check that io is done
    process->cpuBursts--;
    process->ioBursts--;
    //Compare timeslice and length of cpuburst
    /*
     * If the timeSlice is less than cpuTime / cpuBurst, recalculate
     */
    int cpuBurstTime = process->cpuTime / process->cpuBursts;
    if (commandInput->timeSlice > cpuBurstTime) {
        process->cpuBursts = process->cpuTime / commandInput->timeSlice;
    }
}

/**
 * Add newly created "processes" to the end of the ready queue
 * When a core becomes available, schedule the "process" at the front of the ready queue on that core
 * When a "process" finishes an I/O burst, place it at the end of the ready queue
 */
void firstComeFirstServe() {

}

/**
 * Add newly created "processes" to the ready queue in a position that is based on their remaining CPU time
 * When a core becomes available, schedule the "process" at the front of the ready queue (i.e. has the least amount of remaining CPU time) on that core
 * When a "process" finishes an I/O burst, place it in the ready queue at a position that is based on its remaining CPU time
 */
void shortestJobFirst() {

}

/**
 * Add newly created "processes" to the ready queue in a position that is based on priority number (0: highest prioity, 4: lowest priority)
 * If processes have the same priorty, then revert to first come first serve
 * When a core becomes available, schedule the "process" at the front of the ready queue (i.e. has the highest priority) on that core
 * When a "process" finishes an I/O burst, place it in the ready queue at a position that is based on its priority
 * IMPORTANT NOTE: If a "process" is newly created or finishes an I/O burst and has a higher priority than a "process" currently running on a core, then the lowest priority "process" that is running should be preempted removed from the CPU and placed in the ready queue
 */
void preemptivePriority() {

}