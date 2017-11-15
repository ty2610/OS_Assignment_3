#include <iostream>
#include <vector>
#include "Process.cpp"
#include "Core.cpp"

using namespace std;

class PreemptivePriority {

    private:
        Core core;

    public:
        PreemptivePriority(Core *core) {
            this.core = core;
        }

        void execute() {

        }

        /**
         * Used for adding a new process. And for putting back after IO burst.
         * Loop and compare priority (0 is highest)
         */
        void addNewProcess(Process *process) {
            if (this->core.getProcess().getPriority() > process->getPriority()) {
                //Take off current process
                Process removeProcess = core.getProcess();
                core.getProcessQueue()->push_back(removeProcess);
                //Put on new process
                core.setProcess(process);
            } else {
                for (int i=0; i<processQueue.size(); i++) {
                    if ( (this->core.getProcessQueue()->at(i).getPriority()) > (process->getPriority()) ) {
                        processQueue->insert(i, process);
                        break;
                    }
                }
            }
        }

        void removeTerminatedProcess() {
            vector<Process> *processQueue = this->core.getProcessQueue();
            processQueue->pop_back();
        }
};