#include <iostream>
#include <string>
#include <list>
#include "../Headers/ProcessesManager.h"

//Tworzenie w konstruktorze pierwszej listy dla wszystkich procesów ,listy 
ProcessesManager::ProcessesManager()
{
	std::list<PCB*>list;
	allProcesses.push_back(list); // GroupID == 0
}

//Tworzenie procesu. Jeszcze brak grupowania procesów
void ProcessesManager::createProcess(std::string programName, int GID)
{
	PCB* newProcess = new PCB(programName, GID);


	
	std::list<std::list<PCB*>>::iterator it = allProcesses.begin();
	(*it).push_back(newProcess);

	waitingProcesses.push_front(newProcess);

}
void ProcessesManager::killProcess(int PID)
{
	if (waitingProcesses.empty() == false)
	{

		for (std::list<PCB*>::iterator it = waitingProcesses.begin(); it != waitingProcesses.end(); it++)
		{
			if ((*it)->GetPID() == PID)
			{
				it = waitingProcesses.erase(it);
			}
		}
	}

	if (readyProcesses.empty() == false)
	{
		for (std::list<PCB*>::iterator it = readyProcesses.begin(); it != readyProcesses.end(); it++)
		{
			if ((*it)->GetPID() == PID)
			{
				it = readyProcesses.erase(it);
			}
		}
	}

	if (allProcesses.empty() == false)
	{

		for (std::list<std::list<PCB*>>::iterator it = allProcesses.begin(); it != allProcesses.end(); it++)
		{
			for (std::list<PCB*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if ((*it2)->GetPID() == PID && it->empty() == false)
				{
					delete *it2;
					//deallocateMemory(*it2);
					it2 = it->erase(it2);
				}
			}
		}
	}
}

void ProcessesManager::displayAllProcesses()
{
	std::cout << "List of all processes:" << std::endl;
	if (allProcesses.empty() == false)
	{

		for (std::list<std::list<PCB*>>::iterator it = allProcesses.begin(); it != allProcesses.end(); it++)
		{
			for (std::list<PCB*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				(*it2)->displayProcess();
			}
		}
	}
	else std::cout << "List is empty" << std::endl;
}
void ProcessesManager::displayWaitingProcesses()
{
	std::cout << "List of waiting processes:" << std::endl;
	if (waitingProcesses.empty() == false)
	{
		for (std::list<PCB*>::iterator it = waitingProcesses.begin(); it != waitingProcesses.end(); it++)
		{
			(*it)->displayProcess();
		}

	}
	else std::cout << "List is empty" << std::endl;
}
void ProcessesManager::displayReadyProcesses()
{
	std::cout << "List of ready processes:" << std::endl;
	if (readyProcesses.empty() == false)
	{
		for (std::list<PCB*>::iterator it = readyProcesses.begin(); it != readyProcesses.end(); it++)
		{
			(*it)->displayProcess();
		}

	}
	else std::cout << "List is empty" << std::endl;
}

// Albert -- napisalem se funkcje do zwracania std::list<Process*>readyProcesses i std::list<PCB*>waitingProcesses
std::list<PCB*> ProcessesManager::GiveReadyProcessesList() {
	return readyProcesses;
}
std::list<PCB*> ProcessesManager::GiveWaitingProcessesList() {
	return waitingProcesses;
}
std::list<std::list<PCB*>> ProcessesManager::getAllProcesseslist()
{
	return allProcesses;
}