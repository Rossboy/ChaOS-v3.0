#include <iostream>
#include <string>
#include <list>
#include <climits>
#include <algorithm>
#include <fstream>
#include "../Headers/ProcessesManager.h"
#include "../Headers/MemoryManager.h"

extern MemoryManager *mm;
extern PCB* ActiveProcess;
extern PCB shell;
//Tworzenie w konstruktorze pierwszej listy dla wszystkich procesów ,listy 
ProcessesManager::ProcessesManager()
{
	createProcess("pb", 0); //Tworzenie procesu bezczynnosci GID = 0
}

//Tworzenie procesu. Juz z grupowaniem
void ProcessesManager::createProcess(std::string fileName, int GID)
{

	bool GroupExist = true;
	PCB* newProcess = new PCB(fileName, GID);

	std::string program;
	std::ifstream file;
	fileName = fileName + ".txt";
	std::string path = fileName;
	file.open(path);
	if (file.good())
	{
		std::string napis;
		while (!file.eof())
		{
			std::getline(file, napis);
			program = program + napis + " ";
		}
		file.close();

		//tmczasowe bo tutaj wpisujemy kod programu
		mm->allocateMemory(newProcess, program, program.size());
		newProcess->SetProcesBurstTime(program.size() % 13);
		/*Przypadek kiedy dodawany jest proces bezczynnosci*/
		if (GID == 0) {
			std::list<PCB*>list;
			allProcesses.push_back(list); // GroupID == 0
			std::list<std::list<PCB*>>::iterator it = allProcesses.begin();
			std::list<PCB*>::iterator it2 = (*it).begin();
			newProcess->SetProcesBurstTime(INT_MAX);
			(*it).push_back(newProcess);
			ActiveProcess = newProcess;
		}
		else {
			/*Sprawdzamy czy dana grupa juz istnieje*/
			for (std::list<std::list<PCB*>>::iterator it = allProcesses.begin(); it != allProcesses.end(); it++) {
				std::list<PCB*>::iterator it2 = (*it).begin();
				if ((*it2)->GetGID() == GID) {
					(*it).push_back(newProcess);
					GroupExist = true;
					break;
				}
				else {
					GroupExist = false;
				}
			}
			/*Jezeli nie to tworzymy nowa*/
			if (GroupExist == false) {
				std::list<PCB*>list;
				allProcesses.push_back(list);
				std::list<std::list<PCB*>>::iterator it = allProcesses.begin();
				for (int i = 0; i < allProcesses.size() - 1; i++) {
					it++;
				}
				(*it).push_back(newProcess);
				std::list<PCB*>::iterator it2 = (*it).begin();
			}
		}

		readyProcesses.push_back(newProcess);
	}
	else
	{
		rlutil::setColor(rlutil::LIGHTRED);
		std::cout << "Error! Nie udalo otworzyc sie pliku z programem asemblerowym z dysku Windows!" << std::endl;
		rlutil::setColor(rlutil::LIGHTGREEN);
	}
}

void ProcessesManager::createProcess(std::string fileName, int GID, int AddidtionalSpace)
{

	bool GroupExist = true;
	PCB* newProcess = new PCB(fileName, GID);

	std::string program;
	std::ifstream file;
	fileName = fileName + ".txt";
	std::string path = fileName;
	file.open(path);
	if (file.good())
	{
		std::string napis;
		while (!file.eof())
		{
			std::getline(file, napis);
			program = program + napis + " ";
		}
		file.close();

		//tmczasowe bo tutaj wpisujemy kod programu
		mm->allocateMemory(newProcess, program, program.size()+AddidtionalSpace);
		newProcess->SetProcesBurstTime(program.size() % 13);
		/*Przypadek kiedy dodawany jest proces bezczynnosci*/
		if (GID == 0) {
			std::list<PCB*>list;
			allProcesses.push_back(list); // GroupID == 0
			std::list<std::list<PCB*>>::iterator it = allProcesses.begin();
			std::list<PCB*>::iterator it2 = (*it).begin();
			newProcess->SetProcesBurstTime(INT_MAX);
			(*it).push_back(newProcess);
			ActiveProcess = newProcess;
		}
		else {
			/*Sprawdzamy czy dana grupa juz istnieje*/
			for (std::list<std::list<PCB*>>::iterator it = allProcesses.begin(); it != allProcesses.end(); it++) {
				std::list<PCB*>::iterator it2 = (*it).begin();
				if ((*it2)->GetGID() == GID) {
					(*it).push_back(newProcess);
					GroupExist = true;
					break;
				}
				else {
					GroupExist = false;
				}
			}
			/*Jezeli nie to tworzymy nowa*/
			if (GroupExist == false) {
				std::list<PCB*>list;
				allProcesses.push_back(list);
				std::list<std::list<PCB*>>::iterator it = allProcesses.begin();
				for (int i = 0; i < allProcesses.size() - 1; i++) {
					it++;
				}
				(*it).push_back(newProcess);
				std::list<PCB*>::iterator it2 = (*it).begin();
			}
		}

		readyProcesses.push_back(newProcess);
	}
	else
	{
		rlutil::setColor(rlutil::LIGHTRED);
		std::cout << "Error! Nie udalo otworzyc sie pliku z programem asemblerowym z dysku Windows!" << std::endl;
		rlutil::setColor(rlutil::LIGHTGREEN);
	}
}
/*Zabijanie procesu*/
void ProcessesManager::killProcess(int PID)
{
	if (PID == 1)
	{
		std::cout << " Nie mozna wykonac operacji na procesie bezczynnosci. \n";
	}
	else
	{
		PCB* toRemove = findPCBbyPID(PID);
		if (ActiveProcess == toRemove)
		{
			ActiveProcess = nullptr;
		}
		RemoveProcessFromWaiting(toRemove);
		RemoveProcessFromReady(toRemove);

		if (allProcesses.empty() == false)
		{

			// wskaznik na PCB do usuniecia -- Bartek
			toRemove = nullptr;
			// wskaznik do listy ktora bedzie usunieta z listy list, bo bedzie pusta -- Bartek
			std::list<PCB * > * listToRemove = nullptr;
			// wskaznik do listy wskaznikow PCB z ktorego usuwamy-- Bartek
			std::list<PCB * > * removeFrom = nullptr;

			// iterujemy po listcie list -- Bartek
			// &_list - dlatego, ¿e potrzebujemy adresu listy  ktorej pozniej bêdziemy modyfikowac -- Bartek
			for (auto &_list : allProcesses)
			{
				for (auto element : _list)
				{
					if (element->GetPID() == PID)
					{
						// element do usunieca i lista do usunieca -- Bartek
						toRemove = element;
						removeFrom = &_list;
						// jezeli lista przed usunieciem jest rowna 1, to po usunieciu bedzie pusta i trzeba ja usunac -- Bartek
						if (_list.size() == 1)
							listToRemove = &_list;
					}
				}
			}
			// jezeli znaleziono element do usuniecia -- Bartek
			if (toRemove != nullptr)
			{
				// usuwanie procesu z pamieci i systemu normalnego i windowsa -- Bartek
				removeFrom->remove(toRemove);
				mm->deallocateMemory(toRemove); // -- Bartek
				delete toRemove;
			}
			// jezeli lista zawierajacy element jest pusta to ja usuwamy -- Bartek
			if (listToRemove != nullptr)
			{
				allProcesses.remove(*listToRemove);
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

PCB * ProcessesManager::findPCBbyPID(int PID)
{
	for (auto ptr : readyProcesses)
	{
		if (ptr->GetPID() == PID)
			return ptr;
	}
	for (auto ptr : waitingProcesses)
	{
		if (ptr->GetPID() == PID)
			return ptr;
	}
	return nullptr;
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
//Metoda dodaj¹ca proces do listy gotowoœci
void ProcessesManager::AddProcessToReady(PCB* p) {
	if (p != &shell) {
		readyProcesses.push_back(p);
	}
}
void ProcessesManager::RemoveProcessFromReady(PCB* p)
{
	/*Szpachlowanko */
	//if (readyProcesses.empty() == false)
	//{
	//	// wskaznik na PCB do usuniecia -- Bartek
	//	std::vector<PCB *> toRemove;

	//	// poszukujemy w liscie procesow gotowych PCB o PID ktory chcemy usunac -- Bartek
	//	for (auto element : readyProcesses)
	//	{
	//		if (element->GetGID() == p->GetPID())
	//		{
	//			// zaleziono wskaznik -- Bartek
	//			toRemove.push_back(element);
	//			break;
	//		}
	//	}

	//	// jezeli znaleziono taki PCB to go usuwamy z listy -- Bartek
	//	if (toRemove.empty() == false)
	//	{
	//		for (auto x : toRemove)
	//			readyProcesses.remove(x);
	//	}

	//}
	readyProcesses.remove_if([p](PCB * toRemove) {return p == toRemove; });

}
void ProcessesManager::AddProcessToWaiting(PCB* p) {
	if (p != &shell) {
		waitingProcesses.push_back(p);
	}
}
void ProcessesManager::RemoveProcessFromWaiting(PCB* p)
{

	/*szpachlowanko*/
	//if (waitingProcesses.empty() == false)
	//{
	//	// wskaznik na PCB do usuniecia -- Bartek
	//	std::vector<PCB *> toRemove;

	//	// poszukujemy w liscie procesow czekajacych PCB o PID ktory chcemy usunac -- Bartek
	//	for (auto element : waitingProcesses)
	//	{
	//		if (element->GetGID() == p->GetPID())
	//		{
	//			// zaleziono wskaznik -- Bartek
	//			toRemove.push_back(element);
	//			break;
	//		}
	//	}

	//	// jezeli znaleziono taki PCB to go usuwamy z listy -- Bartek
	//	if (toRemove.empty() == false)
	//	{
	//		for (auto x : toRemove)
	//			waitingProcesses.remove(x);
	//	}
	//}
	waitingProcesses.remove_if([p](PCB * toRemove) {return p == toRemove; });
}

