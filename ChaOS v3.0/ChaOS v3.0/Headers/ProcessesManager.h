#ifndef PROCESSESMANAGER__H
#define PROCESSESMANAGER__H

#include <iostream>
#include <string>
#include <list>
#include "Process.h"
//#include "disk_drive.h"
#include <map>

class ProcessesManager {
public:
	std::list<std::list<PCB*>>allProcesses{};
	std::list<PCB*>waitingProcesses{};
	std::list<PCB*>list{};
	std::list<PCB*>readyProcesses{};
	ProcessesManager(); //Poprawi³em bo by³o {}; i przez to by³ problem ¿e konstruktor jest ju¿ zdefiniowany - Bartek
	void createProcess(std::string fileName, int GID);
	void killProcess(int PID);
	void displayAllProcesses();
	void displayWaitingProcesses();
	void displayReadyProcesses();
	PCB * findPCBbyPID(int PID);
	// Albert -- napisalem se funkcje do zwracania std::list<Process*>readyProcesses;
	std::list<PCB*> GiveReadyProcessesList();
	std::list<PCB*> GiveWaitingProcessesList();
	void ProcessesManager::RemoveProcessFromReady(PCB*);
	//Konrad: potrzebowa³em tego heh
	std::list<std::list<PCB*>> getAllProcesseslist();
	void AddProcessToReady(PCB*);
};
#endif PROCESSESMANAGER__H