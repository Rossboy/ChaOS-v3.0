#pragma once
#include "Headers\ProcessesManager.h"
#include <iostream>
#include <list>

class ProcessScheduler {
private:
	// zmienna do liczenia ile czasu proces znajduje sie w procesorze; potrzebny do wyliczenia nowego burstTime, kiedy zajdzie potrzeba:
	short currentProcessTime{ 0 };
	// lista procesow, ktore nie sa gotowe do przydzielenia procesorowi; pobrane z PCB:
	std::list<Process*> waitingProcesses;
	// lista procesow gotowych do przydzielenia procesorowi; pobrane z PCB:
	std::list<Process*> readyProcesses;
	// procedura sluzaca do pobrania listy procesow, ktore nie sa gotowe do przydzielenia procesorowi:
	void GetWaitingProcessesList();
	// procedura sluzaca do pobrania listy procesow gotowych do przydzielenia procesorowi:
	void GetReadyProcessesList();
	
public:
	// procedura sluzaca wybraniu procesu i przydzieleniu mu procesora
	void SRTSchedulingAlgorithm();
	// procedura sluzaca uruchomieniu procesu oraz liczeniu jego czasu wykonywania
	void RunProcess(); // zamiast nazwy GoMethod(), ktora nie ma zadnego sensu
};