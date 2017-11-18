#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

void output();

int main(){
	output();
}

void output(){
	int PID, priority, core;
	string state;
	double turnTime, waitTime, cpuTime, remainTime;
	
	printf("| %6s | %8s | %10s | %4s | %9s | %9s | %8s | %11s |\n", "PID", "Priority", "State", "Core", "Turn Time", "Wait Time", "CPU Time", "Remain Time");
	printf("+--------+----------+------------+------+-----------+-----------+----------+-------------+\n");

	//writes the line to the terminal
	for(int i = 0; i < N; i++){
		printf("| %6d | %8d | %10s | %4d | %9.3f | %9.3f | %8.3f | %11.3f |\n", PID, priority, state, core, turnTime, waitTime, cpuTime, remainTime);
	}

	//erase the lines from the terminal
	for(int j = 0; j < N; j++){
		fputs("\033[A\033[2K", stdout);
	}
	rewind(stdout);
}

