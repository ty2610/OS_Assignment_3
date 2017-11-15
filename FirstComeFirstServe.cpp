#include <iostream>
#include <vector>
#include "Process.cpp"
#include "Core.cpp"

using namespace std;

class FirstComeFirstServe {

    private:
        Core core;

    public:
        FirstComeFirstServe(Core *core) {
            this.core = core;
        }

        void execute() {

        }

        void addNewProcess(Process *process) {
            vector<Process> *processQueue = this->core.getProcessQueue();
            processQueue->push_back(process);
        }

        void removeTerminatedProcess() {
            vector<Process> *processQueue = this->core.getProcessQueue();
            processQueue->pop_back();
        }
};