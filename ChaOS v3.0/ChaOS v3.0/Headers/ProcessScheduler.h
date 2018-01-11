#ifndef PROCESSSCHEDULER__H
#define PROCESSSCHEDULER__H

#include "ProcessesManager.h"
#include "Process.h"
#include <iostream>
#include <list>

class ProcessScheduler {
private:
	//zmienna pomocnicza, potrzebna podczas wybierania procesu o najkrótszym czasie
	std::list<PCB*>::iterator iteratorToMinElement;
	//zmienna pomocnicza do zliczania instrukcji wykonanych przez ActiveProcess
	short instructions{ 0 };
	//Procedura wywo³ywana w RunProcess(), odpowiedzialna za ustawianie ActiveProcess metod¹ SRT
	void SRTSchedulingAlgorithm();
public:
	//Procedura wywo³ywana przed ka¿dym wykonaniem instrukcji ActiveProcess - z jej poziomu wywo³ywany jest planista oraz metoda DoCommand() interpretera
	void RunProcess();
	void RunProcess(int);
};
#endif PROCESSSCHEDULER__H