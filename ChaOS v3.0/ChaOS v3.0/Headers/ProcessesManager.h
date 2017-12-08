#include <iostream>
#include <string>
#include <list>
#include "Process.h"
//#include "disk_drive.h"
#include <map>

class ProcessesManager {
protected:
	std::list<std::list<PCB*>>allProcesses;
	std::list<PCB*>waitingProcesses;
	std::list<PCB*>readyProcesses;
public:
	ProcessesManager(); //Poprawi³em bo by³o {}; i przez to by³ problem ¿e konstruktor jest ju¿ zdefiniowany - Bartek
	void createProcess(std::string fileName, int GID);
	void killProcess(int PID);
	void displayAllProcesses();
	void displayWaitingProcesses();
	void displayReadyProcesses();
	// Albert -- napisalem se funkcje do zwracania std::list<Process*>readyProcesses;
	std::list<PCB*> GiveReadyProcessesList();
	std::list<PCB*> GiveWaitingProcessesList();
};