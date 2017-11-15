#include <iostream>
#include <vector>
#include "Process.cpp"
#include "Core.cpp"

using namespace std;

class ShortestJobFirst {

    private:
        Core core;

    public:
        ShortestJobFirst(Core *core) {
            this.core = core;
        }

        void execute() {

        }

        /**
         * Loop through and compare
         */
        void addNewProcess(Process *process) {
            vector<Process> *processQueue = this->core.getProcessQueue();

            for (int i=0; i<processQueue.size(); i++) {
                if ( (processQueue->at(i).getCpuTime()) > (process->getCpuTime()) ) {
                    processQueue->insert(i, process);
                    break;
                }
            }
        }

        void removeTerminatedProcess() {
            vector<Process> *processQueue = this->core.getProcessQueue();
            processQueue->pop_back();
        }
};