#include "..\Headers\Siec.h"
#include"..\Headers\ProcessesManager.h"
extern PCB* ActiveProcess;
extern ProcessesManager *pm;
Siec::Siec()
{
	zmienna = ConditionVariable();
}


Siec::~Siec()
{
}
bool Siec::wyslij(std::string wiad, int ID)
{
	//przeszukiwanie listy procesów w poszukiwaniu grupy procesu aktywnego (procesy mog¹ siê komunikowaæ tylko w obrêbie tej samej grupy)
	for (auto it = pm->getAllProcesseslist().begin(); it != pm->getAllProcesseslist().end(); it++)
	{
		if (ActiveProcess->GetGID() == (*it->begin())->GetGID())
		{
			//jeœli GID siê zgadza to szukamy procesu o wskazanym ID
			for (auto et = it->begin(); et != it->end(); et++)
			{
				if (ID == (*et)->GetPID())
				{
					(*et)->addToMessages(SMS(wiad));
					zmienna.wait(ActiveProcess);
					return true;
				}
			}
		}
	}
	//jeœli odpowiedni proces-odbiorca nie zosta³ zanleziony to zostanie zwrócona wartoœæ false
	return false;
}
std::unique_ptr<SMS> Siec::odbierz()
{
	if (ActiveProcess->messagessize() == 0) return nullptr;
	std::unique_ptr<SMS> pom = std::make_unique<SMS>(ActiveProcess->getMessage());
	ActiveProcess->deleteMessage();
	zmienna.signal();
	return pom;
}
void Siec::wyswietlwiad()
{
	if (ActiveProcess->messagessize() == 0) std::cout << "Brak wiadomosci w kontenerze aktywnego procesu!" << std::endl;
	else
	{
	std::cout << "Ostatnia wiadomosc aktywnego procesu:" << std::endl << "ID procesu wysylajacego: " << ActiveProcess->getMessage().getID() << std::endl << "ID procesu-odbiorcy: " << ActiveProcess->GetPID() << std::endl <<"ID grupy tych procesow: "<<ActiveProcess->GetGID()<<std::endl<< "Tresc wiadomosci: " << ActiveProcess->getMessage().getwiad() << std::endl;
	}
}