#include "../Headers/ProcessScheduler.h"
#include "../Shell_.cpp"
#include <list>

void ProcessScheduler::RunProcess() {
	//Sprawdzenie, czy wyst¹pi³ b³¹d lub jakiœ proces siê po prostu wykona³ - w obu przypadkach wywo³ywana zostaje funkcja usuwania procesu killProcess()
	if (ActiveProcess->ErrorCode == 1 || ActiveProcess->GetState() == 4) {
		std::cout << "Killing the process and removing it from all the lists it belonged to\n";
		pm->killProcess(ActiveProcess->GetPID());
		ActiveProcess = nullptr;
	}

	else {
		// wyliczenie ile instrukcji siê wykona³o dla aktywnego procesu
		endCounter = ActiveProcess->GetInstructionCounter();
		differenceCounter = endCounter - startCounter;
	}

	//Uruchomienie planisty
	ProcessScheduler();

	//Wykonywanie instrukcji procesu + zmiana instructionCounter, co jest potrzebne przy liczeniu burstTime procesu
	i->DoCommand();
}

//Trzeba sprawdziæ lambdê, czy dobrze dzia³a dla iteratora, ew. zmieniæ wszystko na PCB*
void ProcessScheduler::SRTSchedulingAlgorithm() {
	//std::cout << "SRT Scheduling algorithm has been called, so I'm doing my job!\n";
	iteratorToMinElement = std::min_element(pm->readyProcesses.begin(), pm->readyProcesses.end(), [](PCB* x, PCB* y) { return x->GetProcesBurstTime() < y->GetProcesBurstTime(); });
	//Sprawdzenie, czy przypadkowo nie trafiliœmy na ten sam proces
	if ((*iteratorToMinElement)->GetPID() != ActiveProcess->GetPID() && (*iteratorToMinElement)->GetProcesBurstTime() < ActiveProcess->GetProcesBurstTime() - differenceCounter) {
		//Wyliczenie nowego przewidywanego czasu dostêpu do procesora przez proces, który by³ aktywny
		//std::cout << "Currently running process with PID: " << ActiveProcess->GetPID() << " lost processor, it has run: " << differenceCounter << " instructions and it's predicted burst time was: " << ActiveProcess->GetProcesBurstTime();
		ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * differenceCounter);
		//std::cout << " however the burst time has changed and is equal to: " << ActiveProcess->GetProcesBurstTime() << std::emdl;

		//W³aœciwa zmiana aktywnego procesu
		ActiveProcess = iteratorToMinElement; //albo *iteratorToMinElement
		//std::cout << "New process has been chosen, its PID: " << ActiveProcess->GetPID() << " and estimated burst time: " << ActiveProcess->GetProcesBurstTime() << std::emdl;
		
		//Ustawienie zmiennych pomocniczych
		startCounter = ActiveProcess->GetInstructionCounter();
		endCounter = differenceCounter = 0;
	}
}
