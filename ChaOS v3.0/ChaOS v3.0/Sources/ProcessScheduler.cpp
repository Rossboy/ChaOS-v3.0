#include "../Headers/ProcessScheduler.h"
#include "../Headers/Interpreter.h"
#include <list>

extern PCB* ActiveProcess;
extern ProcessesManager *pm;
extern Interpreter* i;

void ProcessScheduler::RunProcess(int count)
{
	for (int i = 0; i < count; i++)
		RunProcess();
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
				//Ustawienie ActiveProcess na nullptr, poniewa¿ proces nie zosta³ jeszcze wybrany przez planistê
				//ActiveProcess = nullptr;
				//Zerowanie zmiennych pomocniczych do liczenia wykonanych instrukcji ActiveProcess
				startCounter = endCounter = differenceCounter = 0;

				//dodane zeby ActiveProcess nie byl ustawiany na nullptr, zgodnie z ¿yczeniem
				/*for (std::list<PCB*>::iterator p = pm->readyProcesses.begin(); p != pm->readyProcesses.end(); ++p)
				{
					if ((*p)->GetPID() == 1)
					{
						ActiveProcess = *p;
					}
				}
				std::cout << "[Albert]:\tPowinien byc bezczynnosci, a jest:   " << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << std::endl;*/

				SRTSchedulingAlgorithm();
			}
		}

		else
		{
			//Sprawdzenie, czy ActiveProcess jest ustawiony na jakiœ proces
			if (ActiveProcess != nullptr) // != nullptr  
			{
				//Sprawdzenie rozmiaru listy readyProceesses -- size() > 1 oznacza, ¿e ActiveProcess nie jest procesem bezczynnoœci
				if (pm->readyProcesses.size() > 1) {
					//Wyliczenie, ile instrukcji wykona³o siê dla ActiveProcess
					endCounter = ActiveProcess->GetInstructionCounter();
					differenceCounter = endCounter - startCounter;
					//ActiveProcess->SetProcesBurstTime(ActiveProcess->GetProcesBurstTime() - 1);
				}
				//Proces bezczynnoœci nie wykonuje instrukcji, wiêc nie ma powodu do ustawiania zmiennych pomocniczych
			}

			//Planista wywo³ywany za ka¿dym razem - obs³uguje wszystkie mo¿liwe przypadki
			SRTSchedulingAlgorithm();
		}
	}
	//Uruchomienie metody DoCommand() interpretera, gdzie mam nadzieje zmieniany jest instructionCounter wykonywanego procesu
	i->DoCommand();
	///pytanie, czy wywo³ywaæ dla procesu bezczynnoœci

}

void ProcessScheduler::SRTSchedulingAlgorithm()
{
	//to zobaczymy, czy trzeba
	//Gdy rozmiar readyProcesses == 1 -- tym procesem jest proces bezczynnoœci
	//Gdy ActiveProcess == nullptr -- trzeba ustawiæ ActiveProcess jedynym elementem w readyProcesses
	if (pm->readyProcesses.size() == 1 && ActiveProcess == nullptr) //==nullptr proces bzeczynnosci jeszcze nie jest ustawiony
	{
		ActiveProcess = *pm->readyProcesses.begin();
		//Dla procesu bezczynnoœci nie trzeba ustawiaæ ¿adnych zmiennych pomocniczych, poniewa¿ nie wykonuje on ¿adnych instrukcji
		return;
	}

	//Gdy rozmiar readyProcesses == 1 -- procesem jest ponownie proces bezczynnoœci
	//Gdy Activeprocess != nullptr -- nie trzeba ustawiaæ ActiveProcess
	if (pm->readyProcesses.size() == 1 && ActiveProcess->GetPID() == 1) //czyli ustawiony proces bezczynnosci
	{
		//Nie trzeba te¿ niczego robiæ
		return;
	}

	//W pozosta³ych sytuacjach, kiedy proces bezczynnoœci nie jest jedynym dostêpnym procesem
	//Bêdzie on zawsze wyw³aszczony, poniewa¿ jego przewidywany czas burstTime jest specjalnie ustawiony na maksymalnie du¿y

	//Tylko, czy to kurka dobrze, ¿e siê ci¹gle wylicza nowy....? czy nie bedzie sie zmieniac o 1 ciagle?
	//sprawdzic
	//Zgodnie z tym, co mowi dr Bartoszek, trzeba liczyc czas przed porownaniem
	if (ActiveProcess->GetPID() != 1)
	{
		std::cout << std::endl;
		std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << differenceCounter << "\tinstrukcji" << std::endl;
		std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\t burst time przed!" << std::endl;
		ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * differenceCounter);
		std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\tburst time po!" << std::endl;
		std::cout << std::endl;
	}

	//Ustawienie iteratora pomocniczego na proces o najkrotszym burstTime zgodnie z algorytmem SRT
	iteratorToMinElement = std::min_element(pm->readyProcesses.begin(), pm->readyProcesses.end(), [](PCB* x, PCB* y) { return x->GetProcesBurstTime() < y->GetProcesBurstTime(); });

	//nie wiem czy to sie kiedykolwiek wywola jak nie ma nullptra ustawianego
	//Gdy ActiveProcess == nullptr -- nie trzeba liczyc czasu dla poprzednio aktywnego procesu, poniewa¿ proces zostal:
	//1) wykonany		2)usuniet z powodu bledu		-- jeszcze jakieœ przypadki?
	if (ActiveProcess == nullptr) //czy to sie wywola jesli usunalem ustawiania ActiveProcess na nullptr
	{
		//Ustawienie ActiveProcess najlepszym procesem zgodnie z SRT
		ActiveProcess = *iteratorToMinElement;

		//helpBurstTime = ActiveProcess->GetProcesBurstTime();
		//Ustawienie zmiennych pomocniczych do liczenia wykonanych instrukcji ActiveProcess
		startCounter = ActiveProcess->GetInstructionCounter();
		endCounter = differenceCounter = 0;
	}

	//ActiveProcess != nullptr -- ActiveProcess by³ ustawiony przez jakiœ proces
	else
	{
		//Zgodnie z tym, co mowi dr Bartoszek, trzeba liczyc czas przed porownaniem
		//ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * differenceCounter);

		//Sprawdzenie przy pomoy PID, czy wybrany proces jest na pewno innym procesem
		if ((*iteratorToMinElement)->GetPID() != ActiveProcess->GetPID())
		{
			//Sprawdzenie, czy wybrany proces, obecnie ustawiony w iteratorze iteratorToMinElement, ma przewidywany czas krótszy od ActiveProcess
			if ((*iteratorToMinElement)->GetProcesBurstTime() < ActiveProcess->GetProcesBurstTime()) //nie wiem do konca, czy z tym differenceCounter, czy jescze nie /// - differenceCounter
			{
				//if and else should be together
				//if (ActiveProcess->GetPID() != 1)
				//{
					//Wczeœniej trzeba obliczyæ nowy przewidywany czas dla procesu wyw³aszonego zgodnie ze wzorem
					//ActiveProcess->SetProcesBurstTime(.5 * helpBurstTime + .5 * differenceCounter);
					//std::cout << "[Albert]:\t" << "Liczenie nowego burstTime dla:" << ActiveProcess->GetPID() << "\t bursttime: " << helpBurstTime << "\twykonanych rozkazow: " << differenceCounter << "\twyliczony bursttime: " << ActiveProcess->GetProcesBurstTime() << "\n";
					//Ustawienie ActiveProcess
					ActiveProcess = *iteratorToMinElement;
					//std::cout << "[Albert]:\tNowy ActiveProcess: " << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\n";
					//Ustawienie zmiennych pomocniczych do liczenia wykonywanych instrukcji ActiveProcess
					//helpBurstTime = ActiveProcess->GetProcesBurstTime();
					startCounter = ActiveProcess->GetInstructionCounter();
					endCounter = differenceCounter = 0;
				//}
				//else {
					//Ustawienie ActiveProcess
							//ActiveProcess = *iteratorToMinElement;
					//std::cout << "[Albert]:\tNowy ActiveProcess:" << ActiveProcess->GetPID() << "\t " << ActiveProcess->GetProcesBurstTime() << "\n";
					//Ustawienie zmiennych pomocniczych do liczenia wykonywanych instrukcji ActiveProcess
					//helpBurstTime = ActiveProcess->GetProcesBurstTime();
							//startCounter = ActiveProcess->GetInstructionCounter();
							//endCounter = differenceCounter = 0;
				//}
			}
			else 
			{
				return;
			}
		}
		//Sytuacja, w której zosta³ wybrany ten sam proces, co ActiveProcess
		else 
		{
			return;
		}
	}

}