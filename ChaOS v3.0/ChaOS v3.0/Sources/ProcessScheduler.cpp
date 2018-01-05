#include "../Headers/ProcessScheduler.h"
#include "../Headers/Interpreter.h"
#include <list>

extern PCB* ActiveProcess;
extern ProcessesManager *pm;
extern Interpreter* i;

void ProcessScheduler::RunProcess() {
	if (ActiveProcess == nullptr)
	{
		SRTSchedulingAlgorithm();
	}
	else {
		//Sprawdzenie, czy wyst¹pi³ jakiœ b³¹d lub proces zakoñczy³ siê wykonywaæ
		if (ActiveProcess->errorCode != 0 || ActiveProcess->GetState() == 4) {
			//std::cout << "Killing the process and removing it from lists it belonged to\n";
			pm->killProcess(ActiveProcess->GetPID());
			//Ustawienie ActiveProcess na nullptr, poniewa¿ proces nie zosta³ jeszcze wybrany przez planistê
			ActiveProcess = nullptr;
			//Zerowanie zmiennych pomocniczych do liczenia wykonanych instrukcji ActiveProcess
			startCounter = endCounter = differenceCounter = 0;
		}

		//Sprawdzenie, czy ActiveProcess jest ustawiony na jakiœ proces
		if (ActiveProcess != nullptr) {
			//Sprawdzenie rozmiaru listy readyProceesses -- size() > 1 oznacza, ¿e ActiveProcess nie jest procesem bezczynnoœci
			if (pm->readyProcesses.size() > 1) {
				//Wyliczenie, ile instrukcji wykona³o siê dla ActiveProcess
				endCounter = ActiveProcess->GetInstructionCounter();
				differenceCounter = endCounter - startCounter;
			}
			//Proces bezczynnoœci nie wykonuje instrukcji, wiêc nie ma powodu do ustawiania zmiennych pomocniczych
		}

		//Planista wywo³ywany za ka¿dym razem - obs³uguje wszystkie mo¿liwe przypadki
		SRTSchedulingAlgorithm();
	}
	//Uruchomienie metody DoCommand() interpretera, gdzie mam nadzieje zmieniany jest instructionCounter wykonywanego procesu
	i->DoCommand();
	///pytanie, czy wywo³ywaæ dla procesu bezczynnoœci

}

void ProcessScheduler::SRTSchedulingAlgorithm() {

	//Gdy rozmiar readyProcesses == 1 -- tym procesem jest proces bezczynnoœci
	//Gdy ActiveProcess == nullptr -- trzeba ustawiæ ActiveProcess jedynym elementem w readyProcesses
	if (pm->readyProcesses.size() == 1 && ActiveProcess == nullptr) {
		ActiveProcess = *pm->readyProcesses.begin();
		//Dla procesu bezczynnoœci nie trzeba ustawiaæ ¿adnych zmiennych pomocniczych, poniewa¿ nie wykonuje on ¿adnych instrukcji
		return;
	}

	//Gdy rozmiar readyProcesses == 1 -- procesem jest ponownie proces bezczynnoœci
	//Gdy Activeprocess != nullptr -- nie trzeba ustawiaæ ActiveProcess
	if (pm->readyProcesses.size() == 1 && ActiveProcess != nullptr) {
		//Nie trzeba te¿ niczego robiæ
		return;
	}

	//W pozosta³ych sytuacjach, kiedy proces bezczynnoœci nie jest jedynym dostêpnym procesem
	//Bêdzie on zawsze wyw³aszczony, poniewa¿ jego przewidywany czas burstTime jest specjalnie ustawiony na maksymalnie du¿y

	//Ustawienie iteratora pomocniczego na proces o najkrotszym burstTime zgodnie z algorytmem SRT
	iteratorToMinElement = std::min_element(pm->readyProcesses.begin(), pm->readyProcesses.end(), [](PCB* x, PCB* y) { return x->GetProcesBurstTime() < y->GetProcesBurstTime(); });

	//Gdy ActiveProcess == nullptr -- nie trzeba liczyc czasu dla poprzednio aktywnego procesu, poniewa¿ proces zostal:
	//1) wykonany		2)usuniet z powodu bledu		-- jeszcze jakieœ przypadki?
	if (ActiveProcess == nullptr) {
		//Ustawienie ActiveProcess najlepszym procesem zgodnie z SRT
		ActiveProcess = *iteratorToMinElement;

		//Ustawienie zmiennych pomocniczych do liczenia wykonanych instrukcji ActiveProcess
		startCounter = ActiveProcess->GetInstructionCounter();
		endCounter = differenceCounter = 0;
	}

	//ActiveProcess != nullptr -- ActiveProcess by³ ustawiony przez jakiœ proces
	else {
		//Sprawdzenie przy pomoy PID, czy wybrany proces jest na pewno innym procesem
		if ((*iteratorToMinElement)->GetPID() != ActiveProcess->GetPID()) {
			//Sprawdzenie, czy wybrany proces, obecnie ustawiony w iteratorze iteratorToMinElement, ma przewidywany czas krótszy od ActiveProcess
			if ((*iteratorToMinElement)->GetProcesBurstTime() < ActiveProcess->GetProcesBurstTime() - differenceCounter) { //nie wiem do konca, czy z tym differenceCounter, czy jescze nie
				//Wczeœniej trzeba obliczyæ nowy przewidywany czas dla procesu wyw³aszonego zgodnie ze wzorem
				ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * differenceCounter);
				//Ustawienie ActiveProcess
				ActiveProcess = *iteratorToMinElement;
				//Ustawienie zmiennych pomocniczych do liczenia wykonywanych instrukcji ActiveProcess
				startCounter = ActiveProcess->GetInstructionCounter();
				endCounter = differenceCounter = 0;
			}
		}
		//Sytuacja, w której zosta³ wybrany ten sam proces, co ActiveProcess
		//else {
			//raczej nic sie nie ma dziac
		//}
	}

}
