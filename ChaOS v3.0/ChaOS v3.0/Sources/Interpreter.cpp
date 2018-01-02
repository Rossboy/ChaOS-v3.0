#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "../Headers/Interpreter.h"
#include "../Headers/ProcessesManager.h"
#include "../Headers/ConditionVariable.h"
#include "../Headers/ChaOS_filesystem.h"
#include "../Headers/Siec.h"
#include "../Headers/ProcessScheduler.h"
#include "../Headers/MemoryManager.h"

extern PCB* ActiveProcess;
extern std::vector <std::string> ErrorsTab;
extern ProcessesManager *pm;
extern ProcessScheduler *ps;
extern MemoryManager *mm;
extern Siec *s;
extern ChaOS_filesystem *fs;
extern ConditionVariable *cv;

//funkcje do obs�ugi komend
namespace cmd {
	/*ARYTMETYKA*/
	void add(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje si� operacja dodawania..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] += ActiveProcess->registers[arg2];
		if(ActiveProcess->registers[arg1]==0)
		{
			ActiveProcess->zero = true;
		}
		else
		{
			ActiveProcess->zero = false;
		}
	}//done
	//ok
	void substract(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje si� operacja odejmowania..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] -= ActiveProcess->registers[arg2];
		if (ActiveProcess->registers[arg1] == 0)
		{
			ActiveProcess->zero = true;
		}
		else
		{
			ActiveProcess->zero = false;
		}
	}//done
	//ok
	void multiply(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje si� operacja mno�enia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] *= ActiveProcess->registers[arg2];
	}//done
	//ok
	void divide(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2; 
		std::clog << "Wykonuje si� operacja dzielenia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] /= ActiveProcess->registers[arg2];
	}//done
	//ok
	void increment(const std::vector<std::string>& Arguments)
	{
		int arg1;
		std::clog << "Wykonuje si� operacja inkrementacji..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		ActiveProcess->registers[arg1]++;
	}//done
	//ok
	void decrement(const std::vector<std::string>& Arguments)
	{
		
		int arg1;
		std::clog << "Wykonuje si� operacja dekrementacji..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		ActiveProcess->registers[arg1]--;
		if (ActiveProcess->registers[arg1] == 0)
		{
			ActiveProcess->zero = true;
		}
		else
		{
			ActiveProcess->zero = false;
		}
	}	//done
	//ok
	void move(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje si� operacja przypisania warto�ci..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] = arg2;
	}//done
	//ok
	///////////////////////////////////////////////////////////
	/*PLIKI*/
	void openFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje si� operacja otwarcia pliku..." << std::endl;
		fs->openFile(Arguments[0].c_str());
	}
	//ok
	void closeFile()
	{
		std::clog << "Wykonuje si� operacja zamykania pliku..." << std::endl;
		fs->closeFile();
	}
	//ok
	void makeFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje si� operacja tworzenia ";
		if (Arguments[1] == "plik") {
			std::clog << "pliku..." << std::endl;
			fs->create(Arguments[0].c_str(), ChaOS_filesystem::type::file);
		}
		else if (Arguments[1] == "folder") {
			std::clog << "folderu..." << std::endl;
			fs->create(Arguments[0].c_str(), ChaOS_filesystem::type::dir);
		}
		else {
			ActiveProcess->errorCode = 9;
		}
	}
	//ok
	void deleteFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje si� operacja usuwania pliku..." << std::endl;
		fs->remove(Arguments[0].c_str());
	}
	//ok
	void readFile()
	{
		std::clog << "Wykonuje si� operacja czytania pliku..." << std::endl;
		std::cout << fs->readFile()<<std::endl;
	}
	//ok
	void listFiles()
	{
		std::clog << "Wykonuje si� operacja listowania katalogu..." << std::endl;
		std::cout << fs->listDirectory() << std::endl;
	}
	//ok
	void changeFileName(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje si� operacja zmiany nazwy..." << std::endl;
		fs->rename(Arguments[0].c_str(), Arguments[1].c_str());
	}
	//ok
	void appendFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje si� operacja dopisywania do pliku..." << std::endl;
		fs->appendFile(Arguments[0]);
	}
	//ok
	void writeFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje si� operacja pisania do pliku..." << std::endl;
		fs->writeFile(Arguments[0]);
	}
	//ok

	///////////////////////////////////////////////////////////
	/*PROCESY*/

	void makeProcess(const std::vector<std::string>& Arguments)
	{

	}
	//tworzenie procesu
	//void executeProcess(const std::vector<std::string>& Arguments)
	//{

	//}
	//nie wiem czy to w g�le b�dzie

	//void listProcess(const std::vector<std::string>& Arguments)
	//{

	//}

	void killProcess(const std::vector<std::string>& Arguments)
	{
		pm->killProcess(atoi(Arguments[0].c_str()));
	}
	//ok

	///////////////////////////////////////////////////////////
	/* KOMUNIKACJA */
	void sendMessage(const std::vector<std::string>& Arguments)
	{

	}

	void readMessage(const std::vector<std::string>& Arguments)
	{

	}

	///////////////////////////////////////////////////////////
	/*OBS�UGA PROCESU*/
	void makePoint(const std::vector<std::string>& Arguments)
	{
		ActiveProcess->points.push_back(ActiveProcess->GetInstructionCounter());
	}
	//ok
	void jump(const std::vector<std::string>& Arguments)
	{
		ActiveProcess->SetInstructionCounter(atoi(Arguments[1].c_str()));
	}//ok
	//ok
	void jumpPoint(const std::vector<std::string>& Arguments)
	{
		ActiveProcess->SetInstructionCounter(ActiveProcess->points[atoi(Arguments[1].c_str())]);
	}//ok
	//ok
	void jumpZero(const std::vector<std::string>& Arguments)
	{
		if(ActiveProcess->zero)
		{
			jumpPoint(Arguments);
		}
	}//ok
	//ok

	void end()
	{
		PCB* temp = ActiveProcess;
		ActiveProcess = nullptr;
		pm->killProcess(temp->GetPID());
	}
	//ok

	void Return(const std::vector<std::string>& Arguments)
	{
		std::cout << ActiveProcess->registers[atoi(Arguments[0].c_str())] << std::endl;
		end();
	}
	//ok


}


void Interpreter::ExecuteCommand(const std::pair<int, int >&  CommandParameters, const std::vector<std::string>& Arguments)
{
	int arg1, arg2;
	switch (CommandParameters.first)
	{
	case 0://AD = Dodawanie
		cmd::add(Arguments);
		break;
	case 1://SB = Odejmowanie
		cmd::substract(Arguments);
		break;
	case 2://ML = MNO�ENIE
		cmd::multiply(Arguments);
		break;
	case 3://DV = DZIELENIE
		cmd::divide(Arguments);
		break;
	case 4://DR = DEKREMENTACJA
		cmd::decrement(Arguments);
		break;
	case 5://IR = INKREMENTACJA
		cmd::increment(Arguments);
		break;
	case 6://MV =PRZENOSZENIE WARTO�CI
		cmd::move(Arguments);
		break;

		//OPERACJE NA PLIKACH
	case 7://MF = Utw�rz plik
		cmd::makeFile(Arguments);
		break;
	case 8://SF = Zapis do pliku
	//	cmd::saveFile(Arguments);
		break;
	case 9://DF = Usuwanie pliku
		cmd::deleteFile(Arguments);
		break;
	case 10://RF = Odczyt pliku
		cmd::readFile(Arguments);
		break;
	case 11://LS = Listuj pliki
		cmd::listFiles(Arguments);
		break;
	case 12://CP = Zmie� nazw� pliku
		cmd::changeFileName(Arguments);
		break;

		//PROCESY
	case 13://MP = Utw�rz proces
		cmd::makeProcess(Arguments);
		break;
	case 14://EX = Wykonaj program?
		cmd::executeProcess(Arguments);
		break;
	case 15://PS = Wy�wietl procesy
		cmd::listProcess(Arguments);
		break;
	case 16://SM = Wy�lij komunikat
		cmd::sendMessage(Arguments);
		break;
	case 17://RM = Odczytaj komunikat
		cmd::readMessage(Arguments);
		break;
	case 18://ET = Utw�rz etykiet�
		cmd::makePoint(Arguments);
		break;
	case 19://JP = Skok bezwarunkowy
		cmd::jump(Arguments);
		break;
	case 20://JZ = Skok do etykiety je�li flaga zerowa jest ustawiona
		cmd::jumpZero(Arguments);
		break;
	case 21://JE = Skok do etykiety
		cmd::jumpPoint(Arguments);
		break;
	case 22://RT = Zwr��
		cmd::Return(Arguments);
		break;
	case 23://SP = Koniec programu
		cmd::end(Arguments);
		break;
	default:
		std::cout << "ERROR - NIE OBS�UGIWANE POLECENIE!" << std::endl;
		std::cin.ignore(1);

		break;
	}
	RegStatus();
}
//Wykonywanie rozkazu
void Interpreter::DoCommand()
{
	std::string command_code = getArgument();
	//Wczytywanie ID rozkazu, oraz ilo�ci argument�w
	std::pair<int, int > CommandParameters = GetParameters(command_code);
	std::vector<std::string>Arguments;

	//Wczytywanie Argument�w
	for (int i = 0; i < CommandParameters.second; i++) {
		Arguments.push_back(getArgument());//od osoby zarz�dzaj�cej pami�ci�
	}

	//Testowo - wy�wietlenie wczytanego rozkazu i jego argument�w;
	std::cout << "ID: " << CommandParameters.first << " | Command name: " << command_code;
	for (int i = 0; i < Arguments.size(); i++) {
		std::cout << " | Arg[" << i << "]: " << Arguments[i] << " ";
	}
	std::cin.ignore(2);

	//wykonanie rozkazu
	ExecuteCommand(CommandParameters, Arguments);
	if (ActiveProcess != nullptr) {

		RegStatus();

		//Wyb�r rozkazu

		//obs�uga b��d�w.
		if (ActiveProcess->errorCode != 0)
		{
			std::cout << ErrorsTab[ActiveProcess->errorCode] << std::endl;
			std::cout << "Na rzecz b��du, program zostaje zako�czony." << std::endl;
			pm->killProcess(ActiveProcess->GetPID());
		}
	}
}

void Interpreter::DoShellCommand(std::vector<std::string> cmd)
{
	std::string command_code = cmd[0];
	//Wczytywanie ID rozkazu, oraz ilo�ci argument�w
	std::pair<int, int > CommandParameters = GetParameters(command_code);
	std::vector<std::string>Arguments;
	PCB* Temp = ActiveProcess;
	//ActiveProcess = pm->allProcesses.fi
	//Wczytywanie Argument�w
	for (int i = 1; i < CommandParameters.second; i++) {
		Arguments.push_back(cmd[i]);
	}

	//Testowo - wy�wietlenie wczytanego rozkazu i jego argument�w;
	std::cout << "ID: " << CommandParameters.first << " | Command name: " << command_code;
	for (int i = 0; i < Arguments.size(); i++) {
		std::cout << " | Arg[" << i << "]: " << Arguments[i] << " ";
	}
	std::cin.ignore(2);

	//wykonanie rozkazu
	ExecuteCommand(CommandParameters, Arguments);


	if(ActiveProcess->errorCode!=0)
	{
		std::cout << ErrorsTab[ActiveProcess->errorCode] << std::endl;
		ActiveProcess->errorCode = 0;
	}
	ActiveProcess = Temp;
	
}


//Pobiera ID oraz ilo�� parametr�w z tablicy rozkaz�w
std::pair<int, int > Interpreter::GetParameters(std::string& cmd)
{
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
	for (int i = 0; i < CommandTab.size(); i++) {
		if (CommandTab[i].first == cmd) {
			return CommandTab[i].second;
		}
	}
	cmd = "ER";
	return CommandTab[CommandTab.size() - 1].second;
}



//Symulacja podawania argumentu
std::string Interpreter::getArgument()
{
	std::string arg;
	std::clog << "Wprowad� argument:";
	std::cin >> arg;
	std::clog << std::endl;
	return arg;
}

//Wy�wietla aktualny stan rejestr�w
void Interpreter::RegStatus()
{
	std::cout << "Aktualny stan rejestr�w" << std::endl;
	std::cout << "R0: " << ActiveProcess->registers[0] << " | R1: " << ActiveProcess->registers[1] << " | R2 " << ActiveProcess->registers[2] << std::endl;
	std::cin.ignore(1);
}

