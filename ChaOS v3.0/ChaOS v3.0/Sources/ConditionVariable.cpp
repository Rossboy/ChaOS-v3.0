#include "../Headers/ConditionVariable.h"
#include "../Headers/Process.h"
#include <iostream>

extern PCB shell;
ConditionVariable::ConditionVariable()
{
	this->resourceOccupied = false;
}


// Zmienia stan procesu na waiting, dodaje do listy procesow oczekujacych 
void ConditionVariable::wait(PCB* process)
{
	if (resourceOccupied)
	{
		process->setStateAndMoveToRespectiveList(State::Waiting);
		process->wait = true;
		waitingProcesses.push_back(process);
		this->resourceOccupied = true;
		//std::cout << "ZMIENNA WARUNKOWA \nProces: \n";
		//process->displayProcess();
		//std::cout << "poprzez wait(PCB* process) znalazl sie na liscie procesow oczekujacych." << std::endl;
	}
	else if (!resourceOccupied && this->waitingProcesses.empty())
	{
		process->setStateAndMoveToRespectiveList(State::Ready);
		this->resourceOccupied = true;
		//std::cout << "ZMIENNA WARUNKOWA \nProces: \n";
		//process->displayProcess();
		//std::cout << "poprzez wait(PCB* process) przeszedl od razu do stanu ready." << std::endl;
	}
	else
	{
		std::cout << "Blad z zmiennej warunkowej w metodzie wait(PCB*)." << std::endl;
	}
}


// Zmienia stan procesu na gotowy, usuwa go z listy procesow oczekujacych i wlacza planiste.
// Jesli nic nie czeka pod zmienna warunkowa to wywolanie metody jest ignorowane.
void ConditionVariable::signal()
{
	if (resourceOccupied && !waitingProcesses.empty())
	{
		auto temp = waitingProcesses.front();
		temp->setStateAndMoveToRespectiveList(State::Ready);
		waitingProcesses.pop_front();

		if (this->waitingProcesses.empty())
		{
			this->resourceOccupied = false;
		}
		else
		{
			this->resourceOccupied = true;
		}

		//std::cout << "ZMIENNA WARUNKOWA \nProces:\n";
		//temp->displayProcess();
		//std::cout << "poprzez signal() jest w stanie ready." << std::endl;

		//delete temp;
	}
	else
	{
		resourceOccupied = false;
	}
}


// Wypisuje jakies pierdoly o PCB do sprawdzenia co tam siedzi w liscie.
void ConditionVariable::displayWaitingProcesses()
{
	for (std::list<PCB*>::iterator it = waitingProcesses.begin(); it != waitingProcesses.end(); it++)
	{
		(*it)->displayProcess();
	}
}


bool ConditionVariable::getResourceOccupied()
{
	return resourceOccupied;
}


//Konrad - dopisalem zeby proces-nadawca nie przechodzil od razu do ready
void ConditionVariable::lockmessagesender()
{
	this->resourceOccupied = true;
}