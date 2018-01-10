#ifndef PROCESSSCHEDULER__H
#define PROCESSSCHEDULER__H

#include "ProcessesManager.h"
#include "Process.h"
#include <iostream>
#include <list>

class ProcessScheduler {
private:
	//zmienna pomocnicza, potrzebna podczas wybierania procesu o najkrótszym czasie, lecz przed ustawieniem ActiveProcess
	std::list<PCB*>::iterator iteratorToMinElement;
	//differenceCounter	-- licznik okreœlaj¹cy, ile instrukcji wykona³o siê dla ActiveProcess
	//startCounter		-- licznik okreœlaj¹cy, która instrukcja by³a wykonana jako pierwsza dla nowo-przypisanego ActiveProcess
	//endCounter		-- licznik okreœlaj¹cy, która instrukcja by³a wykonana dla ActiveProcess ostatnio (albo bêdzie wykonana..?)
	short differenceCounter, startCounter, endCounter, instructions{ 0 };
	//int helpBurstTime;
	//Procedura odpowiedzialna za ustawianie ActiveProcess metod¹ SRT - Shortest Remaining Time - wyw³aszczaj¹c¹ wersj¹ SJF
	void SRTSchedulingAlgorithm();
public:
	//Procedura wywo³ywana przed ka¿dym wykonaniem instrukcji - z jej poziomu wywo³ywany jest planista (SRTSchedulingAlgorithm) oraz metoda DoCommand z interpretera
	void RunProcess();
	void ProcessScheduler::RunProcess(int);
};
#endif PROCESSSCHEDULER__H