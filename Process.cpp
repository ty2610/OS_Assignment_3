class Process {

    private:
        bool doingIo;
        bool done;
        bool ready;
        int core;
        int cpuBursts; //Random number between 2 and 10
        int cpuTime; //Random number between 1000 and 6000
        int ioBursts; //Random number between 2 and 10
        int ioTime; //Random number between 1000 and 6000
        int pid; //Start at 1024 and increment up
        int priority; //Random number between 0 and 4
        int remainingTime;
        int startWaitTime;
        int waitTime;

    public:
        Process() {
            this->doingIo = false;
            this->done = false;
            this->ready = true;
        }

        int getPid() {
            return this->pid;
        }
        void setPid(int pid) {
            this.pid = pid;
        }

        int getIoBursts() {
            return this->ioBursts;
        }
        void setIoBursts(int ioBursts) {
            this->ioBursts = ioBursts;
        }

        int getIoTime() {
            return this->ioTime;
        }
        void setIoTime(int ioTime) {
            this->ioTime = ioTime;
        }

        int getCpuBursts() {
            return this->cpuBursts;
        }
        void setCpuBursts(int cpuBursts) {
            this->cpuBursts = cpuBursts;
        }

        int getCpuTime() {
            return this->cpuTime;
        }
        void setCpuTime(int cpuTime) {
            this->cpuTime = cpuTime;
        }

        int getPriority() {
            return this->priority;
        }
        void setPriority(int priority) {
            this->priority = priority;
        }

        int getStartWaitTime() {
            return this->startWaitTime;
        }
        void setStartWaitTime(int startWaitTime) {
            this->startWaitTime = startWaitTime;
        }

        int getRemainingTime() {
            return this->remainingTime;
        }
        void setRemainingTime(int remainingTime) {
            this->remainingTime = remainingTime;
        }

        int getWaitTime() {
            return this.waitTime();
        }
        void setWaitTime() {
            this.waitTime = waitTime;
        }

        bool getDone() {
            return this->done;
        }
        void setDone(bool done) {
            this->done = done;
        }

        bool getDoingIo() {
            return this->doingIo;
        }
        void setDoingIo(bool doingIo) {
            this->doingIo = doingIo;
        }

        int getCore() {
            return this->core;
        }
        void setCore(int core) {
            this->core = core;
        }

        bool getReady() {
            return this->ready;
        }
        void setReady(bool ready) {
            this->ready = ready;
        }
};
