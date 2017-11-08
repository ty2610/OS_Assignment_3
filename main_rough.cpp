#include <iostream>
#include <vector>
#include <random>
#include <string>

/**
 * Processes Scheduling
 * @return
 */

using namespace std;

void processCommandInput(string *userInputRaw);
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

int main() {
    //pid starts at 1024 and increments by 1
    int nextPid = 1024;

    string userInputRaw;
    getline(cin, userInputRaw);

    //Get all user input commands
    processCommandInput(&userInputRaw);

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
void processCommandInput(string *userInputRaw) {
    string userInput = *userInputRaw;
    for (int i=0; i<userInput.length(); i++) {
        if (userInput[i] == '-') {
            char command = userInput[i + 1];
            //We know next char will be a space so start at char after the space
            int commandIndex = i+3;
            char currChar = userInput[commandIndex];
            //Store the data after the -x in commandData
            string commandData;
            while (currChar != ' ') {
                commandData += userInput[commandIndex];
                commandIndex++;
                currChar = userInput[commandIndex];
            }
            //Update i so that we don't recheck chars again
            i = commandIndex + 1;
            switch(command) {
                case 'c' :
                    commandInput.cpuCores = atoi(commandData);
                    break;
                case 'p' :
                    commandInput.processesToRun = atoi(commandData);
                    break;
                case 's' :
                    commandInput.schedAlgorithm = atoi(commandData);
                    break;
                case 'o' :
                    commandInput.csOverhead = atoi(commandData);
                    break;
                case 't' :
                    commandInput.timeSlice = atoi(commandData);
                    break;
                default :
                    break;
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