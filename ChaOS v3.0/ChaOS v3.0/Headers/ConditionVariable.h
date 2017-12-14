#pragma once
#include "../Headers/Process.h"
#include <list>


class ConditionVariable
{
private:
	bool resourceOccupied;
	std::list<PCB*> waitingProcesses;

public:
	ConditionVariable();
	void wait(PCB* process);
	void signal();
	void displayWaitingProcesses();
	bool getResourceOccupied();
};