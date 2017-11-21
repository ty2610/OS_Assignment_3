#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <unistd.h>
#include <mutex>
#include <random>

//This is a CPP that will be compiled under c++ standard 11
//compilable with g++ -o main main.cpp -std=c++11 -lpthread

using namespace std;

struct Process {
    int PID;
    int priority;
    string state;
    int core;
    double turnTime;
    double cpuTime;
    double ioTime;
    double remainTime;
    int ioBursts;
    vector<double> ioBurstTimes;
    vector<double> cpuburstTimes;
    int ioBurstSpot;
    int cpuBurstSpot;
    double startTime;
    double waitTime;
    int cpuBursts;
    double restartTime;
    double cpuTimeLeft;
    bool kickedOff;
    double cpuBurstWaitTime;
    int robinLocation;
    double terminatedTime;
    double terminatedTotal;
};

struct CommandInput {
    //1-4
    int cores;
    //1-24
    int processors;
    //0-3
    int algorithm;
    //100-1000
    double contextSwitch;
    //200-2000
    double timeSlice;
}commandInput;

//Only need one
struct MainThread {
    vector<Process> processCollection;
    bool done;
    double applicationStart;
    int location;
}mainThreadObject;

struct Core {
    int id;
    bool idle;
    vector<Process> processCollection;
} ;

mutex mtx;

bool isNumber(const string& s);
void takeCommand(int argc, char *argv[]);
vector<Process> createProcesses();
void* executeRoundRobin(void* obj);
void* executeFirstComeFirstServe(void* obj);
void* executeShortestJobFirst(void* obj);
void* executePreemptivePriority(void* obj);
void insertPreemptivePriority(int index);
void* mainThreadProcess(void* obj);
void* processActivator(void* obj);
bool readyProcess();
void executeProcess(int timeToWait, double place);
void* displayOutput(void* obj);
int findHighestRobinLocation();

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
    pthread_create(&mainThread, NULL, &mainThreadProcess, NULL);

    pthread_join(mainThread, NULL);

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
    srand( time( NULL ) );
    for(int i=0; i<commandInput.processors; i++){
        Process tempProcess;
        tempProcess.PID = 1024 + i;
        if(i<=firstProcesses){
            tempProcess.state = "Ready";
            tempProcess.startTime = 0;
        } else {
            tempProcess.state = "Not Created";
            tempProcess.startTime = ((rand() % 7501) + 500);
        }
        if(commandInput.algorithm==3){
            tempProcess.priority = rand() % 5;
        } else {
            tempProcess.priority = 0;
        }
        tempProcess.ioBursts = (rand() % 9) + 2;
        tempProcess.cpuBursts = tempProcess.ioBursts + 1;
        double temp;
        tempProcess.cpuTime = 0;
        for(int j=0; j<tempProcess.cpuBursts; j++) {
            //https://stackoverflow.com/questions/25649495/how-to-insert-element-at-beginning-of-vector
            //used this to learn how to push to the back of a vector (just switched start() with end())
            //temp = (rand() % 5001) + 1000;
            temp = 1000;
            tempProcess.cpuburstTimes.insert(tempProcess.cpuburstTimes.end(),temp);
            tempProcess.cpuTime += temp;
        }
        tempProcess.cpuTimeLeft = tempProcess.cpuTime;
        tempProcess.ioTime = 0;
        for(int j =0; j<tempProcess.ioBursts; j++) {
            temp = (rand() % 5001) + 1000;
            tempProcess.ioBurstTimes.insert(tempProcess.ioBurstTimes.end(),temp);
            tempProcess.ioTime+=temp;
        }
        tempProcess.cpuBurstSpot = 0;
        tempProcess.ioBurstSpot = 0;
        tempProcess.kickedOff = false;
        tempProcess.cpuBurstWaitTime = 0;
        tempProcess.robinLocation = i;
        tempProcess.turnTime = 0;
        tempProcess.core = 4;
        processCollection.insert(processCollection.end(),tempProcess);
    }
    return processCollection;
}

void* mainThreadProcess(void* obj) {
    //looked up some timing techniques at
    //en.cppreference.com/w/cpp/chrome/c/clock
    mainThreadObject.applicationStart = 1000.0 * clock() / CLOCKS_PER_SEC;
    mainThreadObject.location = 0;
    vector<Process> processCollection = createProcesses();
    mainThreadObject.processCollection = processCollection;
    mainThreadObject.done = false;
    pthread_t outputThread;
    pthread_create(&outputThread, NULL, &displayOutput, NULL);
    //Create core threads
    pthread_t threads[commandInput.cores];
    Core *core = new Core[commandInput.cores];
    pthread_t processActivatorThread;
    pthread_create(&processActivatorThread, NULL, &processActivator, NULL);
    //looked up switch syntax
    //http://en.cppreference.com/w/cpp/language/switch
    //Will run the algorithm for the scheduler selected
    switch(commandInput.algorithm) {
        case 0:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = mainThreadObject.processCollection;
                pthread_create(&threads[i], NULL, &executeRoundRobin, (void*)&core[i]);
            }
            break;
        case 1:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = mainThreadObject.processCollection;
                pthread_create(&threads[i], NULL, &executeFirstComeFirstServe, (void*)&core[i]);
            }
            break;
        case 2:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = mainThreadObject.processCollection;
                pthread_create(&threads[i], NULL, &executeShortestJobFirst, (void*)&core[i]);
            }
            break;
        case 3:
            for (int i=0; i<commandInput.cores; i++) {
                core[i].id = i;
                core[i].idle = true;
                core[i].processCollection = mainThreadObject.processCollection;
                pthread_create(&threads[i], NULL, &executePreemptivePriority, (void*)&core[i]);
            }
            break;
    }

    //Join all threads.
    for (int i=0; i<commandInput.cores; i++) {
        pthread_join(threads[i], NULL);
    }

    mainThreadObject.done = true;

    pthread_join(outputThread,NULL);
    /*
     * Print CPU stats
     */
}

void* executeRoundRobin(void* obj) {
    Core *core = (Core *) obj;
    while (!mainThreadObject.done) {
        if (readyProcess()) {

            mtx.lock();
            int lowest;
            int place;
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                    lowest = mainThreadObject.processCollection.at(i).robinLocation;
                    place = i;
                    break;
                }
            }
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                     if (mainThreadObject.processCollection.at(i).robinLocation < lowest) {
                         lowest = mainThreadObject.processCollection.at(i).robinLocation;
                         place = i;
                    }

                }
            }

            //cout << "Executing Process " << place << endl;
            mainThreadObject.processCollection.at(place).state = "Executing";
            mainThreadObject.processCollection.at(place).core = core->id;
            mtx.unlock();
            bool full;
            if(mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(place).cpuBurstSpot] > commandInput.timeSlice) {
                mainThreadObject.processCollection.at(place).cpuBurstWaitTime = mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(place).cpuBurstSpot] - commandInput.timeSlice;
                mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(place).cpuBurstSpot] = mainThreadObject.processCollection.at(place).cpuBurstWaitTime;
                //sleep is ok to use with decimals, c++ 11 handles it
                sleep(commandInput.timeSlice/1000);
                full = false;
            } else {
                mainThreadObject.processCollection.at(place).cpuBurstWaitTime = 0;
                sleep(mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(
                        place).cpuBurstSpot] / 1000);
                mainThreadObject.processCollection.at(place).cpuBurstSpot++;
                full = true;
            }

            if (mainThreadObject.processCollection.at(place).cpuBurstSpot >= mainThreadObject.processCollection.at(place).cpuBursts) {
                mainThreadObject.processCollection.at(place).state = "Terminated";
                mainThreadObject.processCollection.at(place).terminatedTime = (1000.0 * clock() / CLOCKS_PER_SEC) - mainThreadObject.processCollection.at(place).startTime;
                mainThreadObject.processCollection.at(place).terminatedTotal = (1000.0 * clock() / CLOCKS_PER_SEC);
                //cout << "retiring process " << mainThreadObject.location << endl;
                continue;
            }
            mtx.lock();
            if(full){
                mainThreadObject.processCollection.at(place).state = "IO";
                mainThreadObject.processCollection.at(place).ioBurstSpot++;
                //cout << "putting process " << place << " in IO" << endl;
                //perform IO burst
                mainThreadObject.processCollection.at(place).restartTime = (1000.0 * clock() / CLOCKS_PER_SEC) + (mainThreadObject.processCollection.at(place).ioBurstTimes[mainThreadObject.processCollection.at(place).ioBurstSpot]);
            } else {
                mainThreadObject.processCollection.at(place).robinLocation = findHighestRobinLocation() + 1;
                mainThreadObject.processCollection.at(place).state = "Ready";
            }
            mtx.unlock();
            //Context switch
            sleep(commandInput.contextSwitch / 1000);
        }
    }
}

/**
 * Sort by
 */
void* executeFirstComeFirstServe(void* obj){
    //mainThreadObject.done = true;
    //cout << "also HERE" << endl;
    Core *core = (Core*)obj;
    Process currProcess;
    int lowest = 0;
    int place = 0;

    while (!mainThreadObject.done) {
        if (readyProcess()) {
            //lock must start HERE
            mtx.lock();
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                    lowest = mainThreadObject.processCollection.at(i).restartTime;
                    place = i;
                    break;
                }
            }
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                    if (mainThreadObject.processCollection.at(i).restartTime == 0) {
                        lowest = 0;
                        place = i;
                        break;
                    } else if (mainThreadObject.processCollection.at(i).restartTime < lowest) {
                        lowest = mainThreadObject.processCollection.at(i).restartTime;
                        place = i;
                    }

                }
            }

            //currProcess = mainThreadObject.processCollection.at(place);
            //cout << "Executing Process " << place << endl;
            mainThreadObject.processCollection.at(place).state = "Executing";
            mainThreadObject.processCollection.at(place).core = core->id;
            mtx.unlock();
            //LET GO OF KEY
            sleep(mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(place).cpuBurstSpot] / 1000);
            mainThreadObject.processCollection.at(place).cpuBurstSpot++;
            if (mainThreadObject.processCollection.at(place).cpuBurstSpot >= mainThreadObject.processCollection.at(place).cpuBursts) {
                mainThreadObject.processCollection.at(place).terminatedTime = (1000.0 * clock() / CLOCKS_PER_SEC) - mainThreadObject.processCollection.at(place).startTime;
                mainThreadObject.processCollection.at(place).terminatedTotal = (1000.0 * clock() / CLOCKS_PER_SEC);
                mainThreadObject.processCollection.at(place).state = "Terminated";
                //cout << "retiring process " << place << endl;
                continue;
            }
            //mainThreadObject.applicationStart = 1000.0 * clock() / CLOCKS_PER_SEC;
            //mainThreadObject.processCollection.at(place).startTime = now() + mainThreadObject.processCollection.at(place).ioBurstTimes[mainThreadObject.processCollection.at(place).ioBurstSpot];
            mtx.lock();

            mainThreadObject.processCollection.at(place).state = "IO";
            mainThreadObject.processCollection.at(place).ioBurstSpot++;
            //cout << "putting process " << place << " in IO" << endl;

            //perform IO burst
            mainThreadObject.processCollection.at(place).restartTime = (1000.0 * clock() / CLOCKS_PER_SEC) + (mainThreadObject.processCollection.at(place).ioBurstTimes[mainThreadObject.processCollection.at(place).ioBurstSpot]);
            mtx.unlock();
            //Context switch
            sleep(commandInput.contextSwitch / 1000);
        }
    }
}

void* executeShortestJobFirst(void* obj) {
    Core *core = (Core *) obj;
    int shortest = 0;
    int place = 0;

    while (!mainThreadObject.done) {
        if (readyProcess()) {
            //lock must start HERE
            mtx.lock();
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                    shortest = mainThreadObject.processCollection.at(i).cpuTimeLeft;
                    place = i;
                    break;
                }
            }
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                    if (mainThreadObject.processCollection.at(i).cpuTimeLeft < shortest) {
                        shortest = mainThreadObject.processCollection.at(i).cpuTimeLeft;
                        place = i;
                    }

                }
            }
            //currProcess = mainThreadObject.processCollection.at(place);
            //cout << "Executing Process " << place << endl;
            mainThreadObject.processCollection.at(place).state = "Executing";
            mainThreadObject.processCollection.at(place).core = core->id;
            mtx.unlock();
            //LET GO OF KEY
            sleep(mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(
                    place).cpuBurstSpot] / 1000);
            mainThreadObject.processCollection.at(place).cpuTimeLeft -= mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(
                    place).cpuBurstSpot];
            if(mainThreadObject.processCollection.at(place).cpuTimeLeft < 0){
                mainThreadObject.processCollection.at(place).cpuTimeLeft = 0;
            }
            mainThreadObject.processCollection.at(place).cpuBurstSpot++;
            if (mainThreadObject.processCollection.at(place).cpuBurstSpot >=
                mainThreadObject.processCollection.at(place).cpuBursts) {
                mainThreadObject.processCollection.at(place).terminatedTime = (1000.0 * clock() / CLOCKS_PER_SEC) - mainThreadObject.processCollection.at(place).startTime;
                mainThreadObject.processCollection.at(place).terminatedTotal = (1000.0 * clock() / CLOCKS_PER_SEC);
                mainThreadObject.processCollection.at(place).state = "Terminated";
                //cout << "retiring process " << place << endl;
                continue;
            }
            //mainThreadObject.applicationStart = 1000.0 * clock() / CLOCKS_PER_SEC;
            //mainThreadObject.processCollection.at(place).startTime = now() + mainThreadObject.processCollection.at(place).ioBurstTimes[mainThreadObject.processCollection.at(place).ioBurstSpot];
            mtx.lock();

            mainThreadObject.processCollection.at(place).restartTime = (1000.0 * clock() / CLOCKS_PER_SEC) +
                                                                       (mainThreadObject.processCollection.at(
                                                                               place).ioBurstTimes[mainThreadObject.processCollection.at(
                                                                               place).ioBurstSpot]);
            mainThreadObject.processCollection.at(place).state = "IO";
            mainThreadObject.processCollection.at(place).ioBurstSpot++;

            //cout << "putting process " << place << " in IO" << endl;
            mtx.unlock();
            sleep(commandInput.contextSwitch/1000);
        }
    }
}

void* executePreemptivePriority(void* obj) {
    Core *core = (Core*)obj;
    //Process currProcess;
    int lowestPriority = 0;
    int place = 0;

    while (!mainThreadObject.done) {
        if (readyProcess()) {
            //lock must start HERE
            mtx.lock();
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                    lowestPriority = mainThreadObject.processCollection.at(i).priority;
                    place = i;
                    break;
                }
            }
            for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
                if (mainThreadObject.processCollection.at(i).state == "Ready") {
                    if (mainThreadObject.processCollection.at(i).priority == 0) {
                        lowestPriority = 0;
                        place = i;
                        break;
                    } else if (mainThreadObject.processCollection.at(i).priority < lowestPriority) {
                        lowestPriority = mainThreadObject.processCollection.at(i).priority;
                        place = i;
                    }

                }
            }
            //currProcess = mainThreadObject.processCollection.at(place);
            //cout << "Executing Process " << place << endl;
            mainThreadObject.processCollection.at(place).state = "Executing";
            mainThreadObject.processCollection.at(place).core = core->id;
            mtx.unlock();
            //LET GO OF KEY
            executeProcess(
                    mainThreadObject.processCollection.at(place).cpuburstTimes[mainThreadObject.processCollection.at(
                            place).cpuBurstSpot], place);
            mtx.lock();
            if(mainThreadObject.processCollection.at(place).kickedOff == false) {
                mainThreadObject.processCollection.at(place).cpuBurstSpot++;
                if (mainThreadObject.processCollection.at(place).cpuBurstSpot >=
                    mainThreadObject.processCollection.at(place).cpuBursts) {
                    mainThreadObject.processCollection.at(place).terminatedTime = (1000.0 * clock() / CLOCKS_PER_SEC) - mainThreadObject.processCollection.at(place).startTime;
                    mainThreadObject.processCollection.at(place).terminatedTotal = (1000.0 * clock() / CLOCKS_PER_SEC);
                    mainThreadObject.processCollection.at(place).state = "Terminated";
                    //cout << "retiring process " << place << endl;
                    mtx.unlock();
                    continue;
                }

                mainThreadObject.processCollection.at(place).restartTime = (1000.0 * clock() / CLOCKS_PER_SEC) +
                                                                           (mainThreadObject.processCollection.at(
                                                                                   place).ioBurstTimes[mainThreadObject.processCollection.at(
                                                                                   place).ioBurstSpot]);
                mainThreadObject.processCollection.at(place).state = "IO";
                mainThreadObject.processCollection.at(place).ioBurstSpot++;
                //cout << "putting process " << place << " in IO" << endl;
            } else {
                mainThreadObject.processCollection.at(place).kickedOff = false;
            }
            mtx.unlock();
            sleep(commandInput.contextSwitch / 1000);
        }
    }
}

void insertPreemptivePriority(int index) {
    //Loop executing items and check if they should be replaced
    int lowestPriority =-1;
    int place =-1;
    for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
        if (mainThreadObject.processCollection.at(i).state == "Executing") {
            lowestPriority = mainThreadObject.processCollection.at(i).priority;
            place = i;
            break;
        }
    }
    if(place == -1 || lowestPriority == -1) {
        return;
    }
    for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
        if (mainThreadObject.processCollection.at(i).state == "Executing") {
            if (mainThreadObject.processCollection.at(i).priority == 0) {
                lowestPriority = 0;
                place = i;
                break;
            } else if (mainThreadObject.processCollection.at(i).priority < lowestPriority) {
                lowestPriority = mainThreadObject.processCollection.at(i).priority;
                place = i;
            }

        }
    }
    //Replace if higher priority than current executing
    if (mainThreadObject.processCollection.at(place).priority > mainThreadObject.processCollection.at(index).priority) {
        mainThreadObject.processCollection.at(place).state = "Ready";
        mainThreadObject.processCollection.at(place).kickedOff = true;
    }
}

/**
 * Separate thread that creates processes at random intervals
 * @param obj
 * @return
 */
void* processActivator(void* obj){
    int count = 0;
    while (!mainThreadObject.done) {
        sleep(.25);
        count = 0;
        //cout << "HERE" << endl;
        for (int i=0; i<mainThreadObject.processCollection.size(); i++) {
            //cout << processCollection->at(i).state << endl;
            mtx.lock();
            if ((mainThreadObject.processCollection.at(i).state == "IO" && (1000.0 * clock() / CLOCKS_PER_SEC) >= mainThreadObject.processCollection.at(i).restartTime) || (mainThreadObject.processCollection.at(i).state == "Not Created" && (1000.0 * clock() / CLOCKS_PER_SEC) >= mainThreadObject.processCollection.at(i).startTime)) {
                //MUST ADD LOCK HERE TO
                if (mainThreadObject.processCollection.at(i).state == "IO") {
                    //cout << i << " process is done with IO" << endl;
                } else {
                    //cout << i << " process is being created" << endl;
                }
                if(commandInput.algorithm == 3) {
                    insertPreemptivePriority(i);
                } else if(commandInput.algorithm == 0) {
                    mainThreadObject.processCollection.at(i).robinLocation = findHighestRobinLocation() + 1;
                }

                mainThreadObject.processCollection.at(i).state = "Ready";

            }
            mtx.unlock();
            if (mainThreadObject.processCollection.at(i).state == "Terminated") {
                count++;
            }
        }
        //cout << count << endl;
        if(count==mainThreadObject.processCollection.size()) {
            sleep(.75);
            mainThreadObject.done = true;
        }
    }
    /*
     * This is where we will have an infinite loop that keeps
     * checking the time of the simulation to switch processes
     * to ready when the randomized time has elapsed
     *
     */
}

int findHighestRobinLocation() {
    int highest = 0;
    for(int i=0; i<mainThreadObject.processCollection.size(); i++) {
        if(mainThreadObject.processCollection.at(i).robinLocation>highest){
            highest = mainThreadObject.processCollection.at(i).robinLocation;
        }
    }
    return highest;
}

bool readyProcess() {
    mtx.lock();
    for(int i=0; i<mainThreadObject.processCollection.size(); i++) {
        if(mainThreadObject.processCollection.at(i).state=="Ready"){
            mtx.unlock();
            return true;
        }
    }
    mtx.unlock();
    return false;
}

/**
 * Execute the CPU time
 * Remove if priority tells to do so
 */
void executeProcess(int timeToWait, double place) {
    double currTime = 1000.0 * clock() / CLOCKS_PER_SEC;
    double endTime = currTime + timeToWait;
    while (mainThreadObject.processCollection.at(place).state == "Executing") {
        if (endTime <= (1000.0 * clock() / CLOCKS_PER_SEC)) {
            return;
        }
    }
    mainThreadObject.processCollection.at(place).kickedOff = true;
}

void* displayOutput(void* obj){

    printf("| %6s | %8s | %11s | %4s | %9s | %9s | %8s | %11s |\n", "PID", "Priority", "State", "Core", "Turn Time", "Wait Time", "CPU Time", "Remain Time");
    printf("+--------+----------+------------+------+-----------+-----------+----------+-------------+\n");
    bool hasRun = false;
    //writes the line to the terminal
    while (!mainThreadObject.done) {
        double timeOneCpu;
        double cpuRemainTime;
        double elapsedTime;
        string onCore;
        if(hasRun) {
            for (int i=0; i<mainThreadObject.processCollection.size(); i++) {
                fputs("\033[A\033[2K", stdout);
                rewind(stdout);
            }
        }
        mtx.lock();
        for (int i = 0; i < mainThreadObject.processCollection.size(); i++) {
            Process process = mainThreadObject.processCollection.at(i);
            timeOneCpu = 0;
            cpuRemainTime = 0;
            elapsedTime = 0;
            onCore = "0";
            for(int j=0; j<process.cpuBurstSpot; j++) {
                timeOneCpu += process.cpuburstTimes[j];
            }
            cpuRemainTime = process.cpuTime - timeOneCpu;
            if(cpuRemainTime<0){
                cpuRemainTime = 0;
            }
            if(process.state=="Terminated"){
                elapsedTime = process.terminatedTime;
            } else if(process.state=="Not Created") {
                elapsedTime = 0;
            } else {
                elapsedTime = (1000.0 * clock() / CLOCKS_PER_SEC) - process.startTime;
            }

            if(process.state != "Executing") {
                onCore = "--";
            } else {
                onCore = to_string(process.core);
            }
            printf("| %6d | %8d | %11s | %4s | %9.3f | %9.3f | %8.3f | %11.3f |\n", process.PID, process.priority,
                   process.state.c_str(), onCore.c_str(), elapsedTime, process.waitTime, timeOneCpu,
                   cpuRemainTime);
            hasRun = true;
        }
        mtx.unlock();
        sleep(1);
    }


    double turnAround = 0;
    for(int i=0; i<mainThreadObject.processCollection.size(); i++) {
        turnAround += mainThreadObject.processCollection.at(i).terminatedTotal;
    }
    turnAround = turnAround / mainThreadObject.processCollection.size();
    cout << "Average turnaround time: " << turnAround << endl;
    double totalCPU = 0;
    for(int i=0; i<mainThreadObject.processCollection.size(); i++) {
        for(int j=0; j<mainThreadObject.processCollection.at(i).cpuBursts; j++){
            totalCPU += mainThreadObject.processCollection.at(i).cpuburstTimes[j];
        }
    }
    cout << "Average CPU utilization: " << (totalCPU / commandInput.cores) * (1000.0 * clock() / CLOCKS_PER_SEC) << endl;
}