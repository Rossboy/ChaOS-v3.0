#ifndef CONDITIONVARIABLES__H
#define CONDITIONVARIABLES__H
#include "../Headers/Process.h"
#include "../Headers/ProcessesManager.h"
#include <list>

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