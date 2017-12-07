#include "../Headers/ConditionVariable.h"
//#include "ProcessScheduling.h"		Czekam na Alberta XD
#include "../Headers/Process.h"
#include <iostream>

ConditionVariable::ConditionVariable()
{
	this->resourceOccupied = false;
}

// Zmienia stan procesu na waiting, dodaje do listy procesów oczekuj¹cych 
void ConditionVariable::wait(PCB* process)
{
	// Tych ifów nie jestem pewna - zobaczymy podczas testów jak to bêdzie dzia³aæ
	//if (resourceOccupied)
	//{
		process->SetState(State::Waiting);
		waitingProcesses.push_back(process);
		// SRTSchedulingAlgorithm();
	//}
	//else
	//{
	//	process->SetState(State::Ready);
		// Dodanie do listy procesów oczekuj¹cych na liœcie planisty
		// SRTSchedulingAlgorithm();
	//}
}

// Zmienia stan procesu na gotowy, usuwa go z listy procesów oczekuj¹cych i w³¹cza planistê.
// Jeœli nic nie czeka pod zmienn¹ warunkow¹ to wywo³anie metody jest ignorowane.
void ConditionVariable::signal()
{
	if (!resourceOccupied && !waitingProcesses.empty())
	{
		PCB* temp = waitingProcesses.front();
		temp->SetState(State::Ready);
		// Dodanie do listy procesów oczekuj¹cych na liœcie planisty
		waitingProcesses.pop_front();
		//this->resourceOccupied = true;
		//SRTSchedulingAlgorithm();
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