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
	if (ActiveProcess == nullptr)
	{
		SRTSchedulingAlgorithm();
	}

	else
	{
		//Sprawdzenie, czy wyst¹pi³ jakiœ b³¹d lub proces zakoñczy³ siê wykonywaæ
		if (ActiveProcess->errorCode != 0 || ActiveProcess->GetState() == 4)
		{
			while (ActiveProcess->errorCode != 0 || ActiveProcess->GetState() == 4)
			{
				pm->killProcess(ActiveProcess->GetPID());
				ActiveProcess = nullptr;
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
			SRTSchedulingAlgorithm();
		}
	}

	//Uruchomienie metody DoCommand() interpretera i wykonanie instrukcji ActiveProcess
	i->DoCommand();

	if (ActiveProcess->GetPID() > 1) {
		//Dla ActiveProcess innego ni¿ proces bezczynnoœci liczone s¹ wykonane instrukcje
		instructions++;
	}
}

void ProcessScheduler::SRTSchedulingAlgorithm()
{
	//Przypadek, w którym jest dostêpny tylko jeden proces w liœcie readyProcesses (proces bezczynnoœci)
	if (pm->readyProcesses.size() == 1)
	{
		ActiveProcess = *pm->readyProcesses.begin();
		instructions = 0;
		return;
	}
	//Sprawdzenie, czy ActiveProcess jest ustawiony na jakiœ proces i czy jest to proces inny od procesu bezczynnoœci
	if (ActiveProcess->GetPID() > 1 && ActiveProcess != nullptr) {
		//W tej sytuacji nastêpuje wyliczenie nowego czasu dla ActiveProcess
		ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * instructions);
	}

	//Ustawienie iteratora pomocniczego na proces z listy readyProcesses na ten o najkrótszym czassie, zgodnie z ide¹ SRT
	iteratorToMinElement = std::min_element(pm->readyProcesses.begin(), pm->readyProcesses.end(), [](PCB* x, PCB* y) { return x->GetProcesBurstTime() < y->GetProcesBurstTime(); });

	if (ActiveProcess == nullptr)
	{
		ActiveProcess = *iteratorToMinElement;
		instructions = 0;
		return;
	}
	else
	{
		//Sprawdzenie przy pomoy PID, czy wybrany proces jest na pewno innym procesem, dodatkowe zabezpieczenie
		if ((*iteratorToMinElement)->GetPID() != ActiveProcess->GetPID())
		{
			//Dodatkowe sprawdzenie, czy czas wybranego procesu jest krótszy od ActiveProcess
			if ((*iteratorToMinElement)->GetProcesBurstTime() < ActiveProcess->GetProcesBurstTime())
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

		//Sytuacja, w której zosta³ wybrany ten sam proces, co ActiveProcess, co znaczy, ¿e ActiveProcess wci¹¿ ma najkrótszy czas
		else
		{
			return;
		}
	}


}