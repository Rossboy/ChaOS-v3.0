#include "../Headers/ConditionVariable.h"
#include "../Headers/Process.h"
#include <iostream>


ConditionVariable::ConditionVariable()
{
	this->resourceOccupied = false;
}


// Zmienia stan procesu na waiting, dodaje do listy procesów oczekuj¹cych 
void ConditionVariable::wait(PCB* process)
{
	if (resourceOccupied)
	{
		process->SetState(State::Waiting);
		waitingProcesses.push_back(process);
		this->resourceOccupied = false;
	}
	if (this->waitingProcesses.empty() || !resourceOccupied)
	{
		process->SetState(State::Ready);
		this->resourceOccupied = false;
		pm->AddProcessToReady(process);
	}
}


// Zmienia stan procesu na gotowy, usuwa go z listy procesów oczekuj¹cych i w³¹cza planistê.
// Jeœli nic nie czeka pod zmienn¹ warunkow¹ to wywo³anie metody jest ignorowane.
void ConditionVariable::signal()
{
	if (!resourceOccupied && !waitingProcesses.empty())
	{
		PCB* temp = waitingProcesses.front();
		temp->SetState(State::Ready);
		pm->AddProcessToReady(temp);
		waitingProcesses.pop_front();
		this->resourceOccupied = true;
		delete temp;
	}
}


// Wypisuje jakieœ pierdo³y o PCB do sprawdzenia co tam siedzi w liœcie.
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