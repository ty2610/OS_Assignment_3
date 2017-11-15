class Core {
    private:
        int id;
        bool idle;
        Process process;
        vector<Process> processQueue;
        vector<Process> ioQueue;

    public:
        /* Constructor */
        Core(int *id, Process *process) {
            this->id = id;
            this->process = process;
            this.idle = true;
        }

        /* Functionality */

        /* Mutators */

        int getId() {
            return this->id;
        }
        void setId(int id) {
            this->id = id;
        }

        bool getIdle() {
            return this->idle;
        }
        void setIdle(bool idle) {
            this->idle = idle;
        }

        Process getProcess() {
            return this->process;
        }
        void setProcess(Process *process) {
            this->process = process;
        }

        vector<Process>* getProcessQueue() {
            return *this->processQueue;
        }
        void setProcessQueue(vector<process> *processQueue) {
            this->processQueue = processQueue;
        }

        vector<Process>* getIoQueue() {
            return *this->ioQueue;
        }
        void setIoQueue(vector<process> *ioQueue) {
            this->ioQueue = ioQueue;
        }
};
