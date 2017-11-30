#include <iostream>
#include <string>
#include <list>
#include "Process.h"
//#include "disk_drive.h"
#include <map>

class ProcessesManager {
private:
	std::list<std::list<Process*>>allProcesses;
	std::list<Process*>waitingProcesses;
	std::list<Process*>readyProcesses;
public:
	ProcessesManager() {};
	void createProcess(std::string fileName, int GID);
	void killProcess(int PID);
	void displayAllProcesses();
	void displayWaitingProcesses();
	void displayReadyProcesses();
	// Albert -- napisalem se funkcje do zwracania std::list<Process*>readyProcesses;
	std::list<Process*> GiveReadyProcessesList();
};