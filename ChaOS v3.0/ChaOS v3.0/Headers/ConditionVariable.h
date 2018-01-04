#ifndef CONDITIONVARIABLES__H
#define CONDITIONVARIABLES__H

#include <list>
#include "../Headers/Process.h"
#include "../Headers/ProcessesManager.h"

extern ProcessesManager *pm;

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
#endif CONDITIONVARIABLESS__H