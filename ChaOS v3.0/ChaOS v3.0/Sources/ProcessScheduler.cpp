#include "../Headers/ProcessScheduler.h"
#include "../Headers/Interpreter.h"
#include <list>

extern PCB* ActiveProcess;
extern ProcessesManager *pm;
extern Interpreter* i;

void ProcessScheduler::RunProcess(int count)
{
	for (int i = 0; i < count; i++)
	{
		RunProcess();
	}
}

void ProcessScheduler::RunProcess()
{
	if (ActiveProcess == nullptr || ActiveProcess->GetState()!=State::Ready || ActiveProcess->wait)
	{
		ActiveProcess = pm->findPCBbyPID(1);
		SRTSchedulingAlgorithm();
	}

	else
	{
		//Sprawdzenie, czy wystapil jakis blad lub proces zakonczyl sie wykonywac
		if (ActiveProcess->errorCode != 0 || ActiveProcess->GetState() == 4)
		{
			while (ActiveProcess->errorCode != 0 || ActiveProcess->GetState() == 4)
			{
				pm->killProcess(ActiveProcess->GetPID());
				ActiveProcess = pm->findPCBbyPID(1);
				instructions = 0;

				//zabezpieczenie przed nullptr; zanim cos sie stanie - ustawiany jest proces bezczynnosci
				for (std::list<PCB*>::iterator p = pm->readyProcesses.begin(); p != pm->readyProcesses.end(); ++p)
				{
					if ((*p)->GetPID() == 1)
					{
						ActiveProcess = *p;
					}
				}

				SRTSchedulingAlgorithm();
			}
		}

		else
		{
			//Planista wywolywany za kazdym razem - obsluguje wszystkie mozliwe przypadki
			SRTSchedulingAlgorithm();
		}
	}

	//Uruchomienie metody DoCommand() interpretera, gdzie mam nadzieje zmieniany jest instructionCounter wykonywanego procesu
	i->DoCommand();

	if (ActiveProcess->GetPID() > 1) {
		instructions++;
		//std::cout << "Ten" << ActiveProcess->GetPID() << "\t" << instructions << std::endl;
	}
	if(ActiveProcess->wait)
	{
		ActiveProcess->wait = false;
		ActiveProcess = pm->findPCBbyPID(1);
		SRTSchedulingAlgorithm();

	}
}

void ProcessScheduler::SRTSchedulingAlgorithm()
{
	if (pm->readyProcesses.size() == 1) // == nullptr proces bzeczynnosci jeszcze nie jest ustawiony   && ActiveProcess == nullptr
	{
		ActiveProcess = *pm->readyProcesses.begin();
		instructions = 0;
		return;
	}
	if (ActiveProcess->GetPID() > 1 && ActiveProcess != nullptr) {
		std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << instructions << std::endl;
		//std::cout << std::endl;
		//std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << differenceCounter << "\tinstrukcji" << std::endl;
		std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\t burst time przed!" << std::endl;
		ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * instructions);
		std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\tburst time po!" << std::endl;
	}

	//Ustawienie iteratora pomocniczego na proces o najkrotszym burstTime zgodnie z algorytmem SRT
	iteratorToMinElement = std::min_element(pm->readyProcesses.begin(), pm->readyProcesses.end(), [](PCB* x, PCB* y) { return x->GetProcesBurstTime() < y->GetProcesBurstTime(); });

	if (ActiveProcess == nullptr)
	{
		//Ustawienie ActiveProcess najlepszym procesem zgodnie z SRT
		ActiveProcess = *iteratorToMinElement;
		instructions = 0;
		return;
	}

	//ActiveProcess != nullptr
	else
	{
		//Sprawdzenie przy pomoy PID, czy wybrany proces jest na pewno innym procesem
		if ((*iteratorToMinElement)->GetPID() != ActiveProcess->GetPID())
		{
			//Sprawdzenie, czy wybrany proces, obecnie ustawiony w iteratorze iteratorToMinElement, ma przewidywany czas krotszy od ActiveProcess
			if ((*iteratorToMinElement)->GetProcesBurstTime() < ActiveProcess->GetProcesBurstTime()) //nie wiem do konca, czy z tym differenceCounter, czy jescze nie /// - differenceCounter
			{
				ActiveProcess = *iteratorToMinElement;
				instructions = 0;
				return;
			}
			else
			{
				return;
			}
		}

		//Sytuacja, w ktorej zostal wybrany ten sam proces, co ActiveProcess
		else
		{
			return;
		}
	}


}