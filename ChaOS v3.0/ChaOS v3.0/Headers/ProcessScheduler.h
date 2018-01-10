#ifndef PROCESSSCHEDULER__H
#define PROCESSSCHEDULER__H

#include "ProcessesManager.h"
#include "Process.h"
#include <iostream>
#include <list>

class ProcessScheduler {
private:
	//zmienna pomocnicza, potrzebna podczas wybierania procesu o najkrotszym czasie, lecz przed ustawieniem ActiveProcess
	std::list<PCB*>::iterator iteratorToMinElement;
	//differenceCounter	-- licznik okreslajacy, ile instrukcji wykonalo sie dla ActiveProcess
	//startCounter		-- licznik okreslajacy, ktora instrukcja byla wykonana jako pierwsza dla nowo-przypisanego ActiveProcess
	//endCounter		-- licznik okreslajacy, ktora instrukcja byla wykonana dla ActiveProcess ostatnio (albo bedzie wykonana..?)
	short differenceCounter, startCounter, endCounter, instructions{ 0 };
	//int helpBurstTime;
	//Procedura odpowiedzialna za ustawianie ActiveProcess metoda SRT - Shortest Remaining Time - wywlaszczajaca wersja SJF
	void SRTSchedulingAlgorithm();
public:
	//Procedura wywolywana przed kazdym wykonaniem instrukcji - z jej poziomu wywolywany jest planista (SRTSchedulingAlgorithm) oraz metoda DoCommand z interpretera
	void RunProcess();
	void RunProcess(int);
};
#endif PROCESSSCHEDULER__H