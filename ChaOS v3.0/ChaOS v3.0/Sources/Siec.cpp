#include "..\Headers\Siec.h"
#include "..\Headers\ProcessesManager.h"
extern PCB* ActiveProcess;

Siec::Siec()
{
}


Siec::~Siec()
{
}
bool Siec::wyslij(std::string wiad, int ID)
{
	for (auto it = allProcesses.begin(); it != allProcesses.end(); it++)
	{
		if(ActiveProcess->GetGID() == (*it->begin())->GetGID())
		for (auto et = it->begin(); et != it->end(); et++)
		{
			if (ID == (*et)->GetPID())
			{
				(*et)->addToMessages(SMS(wiad));
				return true;
			}
		}
	}
	return false;
}
SMS Siec::odbierz()
{
	SMS pom = ActiveProcess->getMessage();
	ActiveProcess->deleteMessage();
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