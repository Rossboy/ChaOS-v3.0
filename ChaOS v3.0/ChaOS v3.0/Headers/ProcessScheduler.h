#ifndef PROCESSSCHEDULER__H
#define PROCESSSCHEDULER__H
#include "ProcessesManager.h"
#include "Process.h"
#include <iostream>
#include <list>

class ProcessScheduler {
private:
	std::list<PCB*>::iterator iteratorToMinElement;
	//zmienne pomocnicze potrzebne do wyliczenia nowego burstTime, kiedy zajdzie potrzeba:
	short differenceCounter , startCounter, endCounter{ 0 };
	//procedura odpowiadaj¹ca za planowanie przydzia³u procesora
	void SRTSchedulingAlgorithm();
public:
	//procedura sluzaca do uruchomienia procesu i wywo³ania planisty
	void RunProcess();
};
#endif PROCESSSCHEDULER__H