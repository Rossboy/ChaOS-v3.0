#include "ProcessScheduler.h"
#include <list>
void ProcessScheduler::GetReadyProcessesList() {
	//readyProcesses = pm.GiveReadyProcessesList();
}

void ProcessScheduler::GetWaitingProcessesList() {
	//waitingProcesses = pm.GiveWaitingProcessesList();
}

//to czy to beda wskazniki, czy co - zalezy od implementacji zmiennych w innych modulach
void ProcessScheduler::RunProcess() {
	//if (error == true && shell::currentProcess->getState() == 4) { // 4 stands for terminated
	//	ProcessesManager::killProcess(currentProcess);
	//	currentProcess = nullptr; //albo defaultowy
	//}

	///sprawdzenie czy powinien byc wybierany nowy proces wgle v.1

	//currentProcess = ProcessScheduler();
	// Interpreter::DoCommand(); -- jako ze jest globalny, to bedzie do niego dostêp
	// currentProcessTime++; //inkrementacja licznika do porownania z bursttimem danego czasu
}

void ProcessScheduler::SRTSchedulingAlgorithm() {
	std::list<Process*>::iterator iteratorToMinElement = std::min_element(readyProcesses.begin(), readyProcesses.end(), [](Process* x, Process* y) { return x->GetProcesBurstTime() < y->GetProcesBurstTime(); });

}

//void ProcessScheduler::SRTSchedulingAlgorithm() {
//	std::list<Process*>::iterator iteratorToMinElement;
//	int currentProcessActualBurstTime = 0;
//	int minBurstTime = 100;
//	while (readyProcesses.size() > 1) { // > 1 poniewa¿ zawsze bêdzie wewn¹trz proces bezczynnoœci
//		for (std::list<Process*>::iterator it = readyProcesses.begin(); it != readyProcesses.end(); it++) {
//			if ((*it)->GetProcesBurstTime() <= minBurstTime) {
//				minBurstTime = (*it)->GetProcesBurstTime();
//				iteratorToMinElement = it;
//			}
//		}
//
//		//Wywo³anie funkcji GoProcess(iteratorToMinElement) z wywo³aniami wszystkich wa¿nych funkcji
//		//Zmienianie wartoœci currentProcessActualBurstTime potrzebnej do obliczenia burst time po wykonaniu instrukcji
//
//
///sprawdzenie czy powinien byc wybierany nowy proces wgle v.2

//
//		// this part should run when some instruction on process has been done in order to count new burstTime for process
//		(*iteratorToMinElement)->SetProcesBurstTime(.5 * currentProcessActualBurstTime + .5 * (*iteratorToMinElement)->GetProcesBurstTime); //ustawianie nowego czasu burstTime procesu
//
//																																			//sprawdzanie czy proces jest skonczony; jeœli tak - zmieniæ mu stan lub po prostu go usun¹æ.
//	}
//}


///	-----------------------------------------------------------------------------------------------
//	GoMethod()
//		sprawdzenie, czy jest error - KillProcess
//		uruchomienie ProcessScheduler()
//		gdy ten sam proces - zwiekszenie licznika
//		gdy inny - obliczenie nowego czasu oraz reset licznika
//		sprawdzenie, czy proces jest calkowicie wykonany (potrzebne bedzie odpowiednie pole)
//			gdy wykonany - usunac go z kolejek (KillProcess())

///	-----------------------------------------------------------------------------------------------
//	ProcessScheduler()
//		sprawdzenie, czy waitingList zawiera proces juz gotowy do uruchomienia - przeniesienie go do readyList
//		sprawdzanie, czy jakis proces w readyList ma czas krotszy od aktualnie wykonywanego
//		ustawieinie nowego procesu do wykonywania
//		gdy proces bedzie mial zmieniony stan (zmiana listy) nalezy obliczyc nowy burst time

///	-----------------------------------------------------------------------------------------------
//	gdzies trzeba sprawdzic, czy proces moze byc wykonywany, poniewaz, gdy nie moze trzeba go przeniesc do waitingList

///	-----------------------------------------------------------------------------------------------
