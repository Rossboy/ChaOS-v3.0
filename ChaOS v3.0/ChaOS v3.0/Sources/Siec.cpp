#include "..\Headers\Siec.h"
#include"..\Headers\ProcessesManager.h"
#include"..\Headers\MemoryManager.h"
extern PCB* ActiveProcess;
extern ProcessesManager *pm;
extern MemoryManager *mm;
Siec::Siec()
{
}


Siec::~Siec()
{
}
void Siec::wyslij(std::string wiad, int ID)
{
	if (wiad.size() > 16)
	{
		ActiveProcess->errorCode = 15;
		return;
	}
	else
	{
		std::list<std::list<PCB*>> lista = pm->getAllProcesseslist();
		//przeszukiwanie listy procesów w poszukiwaniu grupy procesu aktywnego (procesy mog¹ siê komunikowaæ tylko w obrêbie tej samej grupy)
		for (auto it = lista.begin(); it != lista.end(); it++)
		{
			if (ActiveProcess->GetGID() == (*it->begin())->GetGID())
			{
				//jeœli GID siê zgadza to szukamy procesu o wskazanym ID
				for (auto et = it->begin(); et != it->end(); et++)
				{
					if (ID == (*et)->GetPID())
					{
						//stworzenie zmiennej warunkowej odpowiadaj¹cej za synchroniczn¹ komunikacjê
						ConditionVariable x;
						x.lockmessagesender();
						cv.emplace(std::make_pair(ActiveProcess->GetPID(), x));
						//dodanie wiadomoœci do kolejki w PCB
						(*et)->addToMessages(SMS(wiad));
						cv[ActiveProcess->GetPID()].wait(ActiveProcess);
						if (cv.find(ID) != cv.end())
						{
							cv[ID].signal();
							cv.erase(ID);
						}
						return;
					}
				}
			}
		}
		ActiveProcess->errorCode = 14;
	}
}
void Siec::sprawdz()
{
	if (ActiveProcess->getMessages().size() == 0)
	{
		ConditionVariable x;
		x.lockmessagesender();
		cv.emplace(std::make_pair(ActiveProcess->GetPID(), x));
		cv[ActiveProcess->GetPID()].wait(ActiveProcess);
	}
}
void Siec::odbierz(int adres)
{
	if (ActiveProcess->getMessages().size() == 0)
	{
		ActiveProcess->errorCode = 16;
		return;
	}
	else
	{
		SMS pom = ActiveProcess->getMessage();
		ActiveProcess->deleteMessage();
		mm->writeString(ActiveProcess, adres, pom.getwiad());
		//skopiowanie listy wskaŸników do aktywnych procesów ¿eby iterowanie nie wywali³o programu w kosmos
		std::list<std::list<PCB*>> lista = pm->getAllProcesseslist();
		for (auto it = lista.begin(); it != lista.end(); it++)
		{
			//sprawdzenie czy procesu maj¹ to samo ID grupy
			if (ActiveProcess->GetGID() == (*it->begin())->GetGID())
			{
				for (auto et = it->begin(); et != it->end(); et++)
				{
					if (pom.getID() == (*et)->GetPID())
					{
						//zmiana procesu nadawcy na gotowy (czeka³ na odebranie wiadomoœci)
						cv[pom.getID()].signal();
						cv.erase(pom.getID());
					}
				}
			}
		}
	}
}
void Siec::wyswietlwiadaktywnego()
{
	if (ActiveProcess->getMessages().size() == 0) std::cout << "Brak wiadomosci w kontenerze aktywnego procesu!" << std::endl;
	else
	{
	int i = 1;
	std::cout << "ID aktywnego procesu: " << ActiveProcess->GetPID() << std::endl << "ID grupy tego procesu: " << ActiveProcess->GetGID() << std::endl;
	std::list<SMS> lista = ActiveProcess->getMessages();
	for(auto it=lista.begin();it!=lista.end();it++)
	std::cout << "Wiadomosc nr "<< i << ":" << std::endl << "ID procesu wysylajacego: " << it->getID() << std::endl  << std::endl << "Tresc wiadomosci: " << it->getwiad() << std::endl;
	i++;
	}
}
void Siec::wyswietlwiad()
{
	std::list<std::list<PCB*>> lista = pm->getAllProcesseslist();
	if (lista.empty()==true) std::cout << "Nie istnieje zaden proces!" << std::endl;
	else
	{
		//wyswietlanie wiadomosci wszystkich procesow grupami
		for (std::list<std::list<PCB*>>::iterator it = lista.begin(); it != lista.end(); it++)
		{
			std::cout << "Wyswietlanie wiadomosci procesow z grupy o ID " << (*it->begin())->GetGID() << std::endl;
			for (std::list<PCB*>::iterator et = it->begin(); et != it->end(); et++)
			{
				//sprawdzenie czy proces posiada jakies nieodczytane wiadomosci
				std::list<SMS> wiadomosci = (*et)->getMessages();
				if (wiadomosci.size() == 0) std::cout << "Brak wiadomosci w kontenerze procesu o ID " << (*et)->GetPID() << std::endl;
				else
				{
					std::cout << "Wiadomosci w kontenerze procesu o ID " << (*et)->GetPID() << std::endl;
					int i = 1;
					//iteracja po liscie wiadomosci procesu wyswietlajac wszystkie
					for (std::list<SMS>::iterator zt = wiadomosci.begin(); zt != wiadomosci.end(); zt++)
					{
						std::cout << "Wiadomosc nr " << i << ":" << std::endl << "ID procesu wysylajacego: " << zt->getID() << std::endl << "Tresc wiadomosci: " << zt->getwiad() << std::endl;
						i++;
					}
				}
			}
		}
	}
}