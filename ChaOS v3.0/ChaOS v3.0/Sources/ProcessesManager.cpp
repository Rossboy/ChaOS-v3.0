#include <iostream>
#include <string>
#include <list>
#include <climits>
#include "../Headers/ProcessesManager.h"

//Tworzenie w konstruktorze pierwszej listy dla wszystkich procesów ,listy 
ProcessesManager::ProcessesManager()
{
	createProcess("programbezczynnosci.txt", 0); //Tworzenie procesu bezczynnosci GID = 0
}

//Tworzenie procesu. Juz z grupowaniem
void ProcessesManager::createProcess(std::string programName, int GID)
{

	bool GroupExist = true;
	PCB* newProcess = new PCB(programName, GID);

	/*Przypadek kiedy dodawany jest proces bezczynnosci*/
	if (GID == 0)
	{
		std::list<PCB*>list;
		allProcesses.push_back(list); // GroupID == 0
		std::list<std::list<PCB*>>::iterator it = allProcesses.begin();
		std::list<PCB*>::iterator it2 = (*it).begin();
		newProcess->SetProcesBurstTime(INT_MAX);
		(*it).push_back(newProcess);
	}
	else {
		/*Sprawdzamy czy dana grupa juz istnieje*/
		for (std::list<std::list<PCB*>>::iterator it = allProcesses.begin(); it != allProcesses.end(); it++)
		{
			std::list<PCB*>::iterator it2 = (*it).begin();
			if ((*it2)->GetGID() == GID)
			{
				(*it).push_back(newProcess);
				GroupExist = true;
				break;
			}
			else
			{
				GroupExist = false;
			}
		}
		/*Jezeli nie to tworzymy nowa*/
		if (GroupExist == false)
		{
			std::list<PCB*>list;
			allProcesses.push_back(list);
			std::list<std::list<PCB*>>::iterator it = allProcesses.begin();
			for (int i = 0; i < allProcesses.size() - 1; i++)
			{
				it++;
			}
			(*it).push_back(newProcess);
			std::list<PCB*>::iterator it2 = (*it).begin();
		}
	}

	readyProcesses.push_back(newProcess);

}
/*Zabijanie procesu*/
void ProcessesManager::killProcess(int PID)
{
	if (PID == 0)
	{
		std::cout << " Nie mozna wykonac operacji na procesie bezczynnosci. " << std::endl;
	}
	else
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
					if (it->size() != 1)
					{
						it2 = it->erase(it2);
					}
					else it = allProcesses.erase(it);
					break;
				}
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