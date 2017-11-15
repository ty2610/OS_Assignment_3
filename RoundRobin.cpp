#include <iostream>
#include <vector>
#include "Process.cpp"
#include "Core.cpp"

using namespace std;

class RoundRobin {
    private:
        Core core;

    public:
        RoundRobin(Core *core) {
            this->core = core;
        }

        /**
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
            //If the timeSlice is less than cpuTime / cpuBurst, recalculate
            int cpuBurstTime = process->cpuTime / process->cpuBursts;
            if (commandInput->timeSlice > cpuBurstTime) {
                process->cpuBursts = process->cpuTime / commandInput->timeSlice;
            }
        }

        void execute () {
            //Check for idle cores
            for (int i = 0; i < core.size(); i++) {
                if (core.getIdle()) {
                    //Assign a process
                    //Need checks for different algorithms
                    core.setProcess(processQueue.pop_back());
                    core.getProcess().setReady(true);
                } else if (cores[i].getProcess().getDone()) {
                    core.setProcess(NULL);
                    core.setIdle(true);
                } else if (cores[i].getProcess().getDoingIo()) {
                    core.setProcess(NULL);
                    core.setIdle(true);
                }
            }
        }

        /**
         * Add new process to end of "queue"
         * @param process
         */
        void roundRobinAddNewProcess(Process *process) {
            vector<Process> *processQueue = this->core.getProcessQueue();
            processQueue->push_back(process);
        }

        /**
         * Pop process in front of "queue"
         */
        void roundRobinRemoveTerminatedProcess() {
            vector<Process> *processQueue = this->core.getProcessQueue();
            processQueue->pop_back();
        }

        /**
         * Move from ready queue to io queue
         */
        void roundRobinPutInIoQueue() {
            vector<Process> *processQueue = this->core.getProcessQueue();
            Process process = processQueue.at(0);
            vector<Process> *ioQueue = this->core.getIoQueue();
            ioQueue->push_back(process);
            processQueue->pop_back();
        }

        /**
         * Take process off front and put on back
         */
        void roundRobinContextSwitch() {
            vector<Process> *processQueue = this->core.getProcessQueue();
            Process process = processQueue.at(0);
            processQueue->push_back(process);
            processQueue->pop_back();
        }
};
