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
PCB shell("Shell", 999);
//funkcje do obs³ugi komend
namespace cmd {

	///////////////////////////////////////////////////////////
	/*OBS£UGA PROCESU*/
	void copyRegisters(const std::vector<std::string>& Arguments)
	{
		ActiveProcess->registers[atoi(Arguments[0].c_str())] = ActiveProcess->registers[atoi(Arguments[1].c_str())];
	}
	void makePoint(const std::vector<std::string>& Arguments)
	{
		ActiveProcess->points.push_back(ActiveProcess->GetInstructionCounter());
	}
	//ok
	void jump(const std::vector<std::string>& Arguments)
	{
		ActiveProcess->SetInstructionCounter(atoi(Arguments[0].c_str()));
	}//ok
	 //ok
	void jumpPoint(const std::vector<std::string>& Arguments)
	{
		ActiveProcess->SetInstructionCounter(ActiveProcess->points[atoi(Arguments[0].c_str())]);
	}//ok
	 //ok
	void jumpZero(const std::vector<std::string>& Arguments)
	{
		if (ActiveProcess->zero)
		{
			jump(Arguments);
		}
	}//ok
	 //oks
	void end()
	{
		ActiveProcess->SetState(Terminated);
	}
	//ok
	void Return(const std::vector<std::string>& Arguments)
	{
		std::cout<<"\n" << ActiveProcess->registers[atoi(Arguments[0].c_str())] << std::endl;
		end();
	}
	//ok

	/*ARYTMETYKA*/
	void add(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje siê operacja dodawania..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] += ActiveProcess->registers[arg2];
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
	void substract(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje siê operacja odejmowania..." << std::endl;
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
		std::clog << "Wykonuje siê operacja mno¿enia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] *= ActiveProcess->registers[arg2];
	}//done
	//ok
	void divide(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje siê operacja dzielenia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] /= ActiveProcess->registers[arg2];
	}//done
	//ok
	void increment(const std::vector<std::string>& Arguments)
	{
		int arg1;
		std::clog << "Wykonuje siê operacja inkrementacji..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		ActiveProcess->registers[arg1]++;
	}//done
	//ok
	void decrement(const std::vector<std::string>& Arguments)
	{

		int arg1;
		std::clog << "Wykonuje siê operacja dekrementacji..." << std::endl;
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
		std::clog << "Wykonuje siê operacja przypisania wartoœci..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] = arg2;
	}//done
	//ok

	void equal(const std::vector<std::string>& Arguments)
	{

	}


	void lessThan(const std::vector<std::string>& Arguments)
	{

	}
	
	
	void equalOrLessThan(const std::vector<std::string>& Arguments) 
	{
	if(ActiveProcess->registers[atoi(Arguments[0].c_str())] - atoi(Arguments[0].c_str())<=0)
	{
		ActiveProcess->zero = true;
	}else
	{
		ActiveProcess->zero = false;
	}
	}
	
	///////////////////////////////////////////////////////////
	/*PLIKI*/
	void openFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje siê operacja otwarcia pliku..." << std::endl;
		fs->openFile(Arguments[0].c_str());
	}
	//ok
	void closeFile()
	{
		std::clog << "Wykonuje siê operacja zamykania pliku..." << std::endl;
		fs->closeFile();
	}
	//ok
	void makeFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje siê operacja tworzenia ";
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
		std::clog << "Wykonuje siê operacja usuwania pliku..." << std::endl;
		fs->remove(Arguments[0].c_str());
	}
	//ok
	void readFile()
	{
		std::clog << "Wykonuje siê operacja czytania pliku..." << std::endl;
		std::cout << fs->readFile() << std::endl;
	}
	//ok
	void listFiles()
	{
		std::clog << "Wykonuje siê operacja listowania katalogu..." << std::endl;
		std::cout << fs->listDirectory() << std::endl;
	}
	//ok
	void changeFileName(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje siê operacja zmiany nazwy..." << std::endl;
		fs->rename(Arguments[0].c_str(), Arguments[1].c_str());
	}
	//ok
	void appendFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje siê operacja dopisywania do pliku..." << std::endl;
		fs->appendFile(Arguments[0]);
	}
	//ok
	void writeFile(const std::vector<std::string>& Arguments)
	{
		std::clog << "Wykonuje siê operacja pisania do pliku..." << std::endl;
		fs->writeFile(Arguments[0]);
	}
	//ok
	void changeDir(const std::vector<std::string>& Arguments)
	{
		fs->changeDirectory(Arguments[0].c_str());
	}
	//ok
	void rootDir()
	{
		fs->rootDirectory();
	}
	//ok
	void backDir()
	{
		fs->backDirectory();
	}
	//ok

	///////////////////////////////////////////////////////////
	/*PROCESY*/

	void makeProcess(const std::vector<std::string>& Arguments)
	{
		pm->createProcess(Arguments[0], atoi(Arguments[1].c_str()));
	}
	//tworzenie procesu, do uzupe³nienia

	//tworzenie procesu
	//void executeProcess(const std::vector<std::string>& Arguments)
	//{

	//}
	//nie wiem czy to w góle bêdzie

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
		s->wyslij(Arguments[0],atoi(Arguments[1].c_str()));
	}

	void readMessage(const std::vector<std::string>& Arguments)
	{
		s->odbierz();
	}

	
	///////////////////////////////////////////////////////////
	/*PAMIÊÆ*/
	void readMemory(const std::vector<std::string>& Arguments)
	{
		int registerIndex = stoi(Arguments[0]);
		string memContetn = mm->readString(ActiveProcess, stoi(Arguments[1]));
		ActiveProcess->registers[registerIndex] = stoi(memContetn);
	}
	void writeMemory(const std::vector<std::string>& Arguments)
	{
		int registerIndex = stoi(Arguments[0]);
		string memContetn = to_string(ActiveProcess->registers[registerIndex]);
		mm->writeString(ActiveProcess, stoi(Arguments[1]),memContetn);
	}
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
		case 2://ML = MNO¯ENIE
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
		case 6://MV =PRZENOSZENIE WARTOŒCI
			cmd::move(Arguments);
			break;

			//OPERACJE NA PLIKACH
		case 7://MF = Utwórz plik
			cmd::makeFile(Arguments);
			break;
		case 8://SF = Zapis do pliku
			cmd::writeFile(Arguments);
			break;
		case 9://DF = Usuwanie pliku
			cmd::deleteFile(Arguments);
			break;
		case 10://RF = Odczyt pliku
			cmd::readFile();
			break;
		case 11://LS = Listuj pliki
			cmd::listFiles();
			break;
		case 12://CP = Zmieñ nazwê pliku
			cmd::changeFileName(Arguments);
			break;

			//PROCESY
		case 13://MP = Utwórz proces
			cmd::makeProcess(Arguments);
			break;
		case 14://EX = Wykonaj program?
			//cmd::executeProcess(Arguments);
			break;
		case 15://PS = Wyœwietl procesy
			//cmd::listProcess(Arguments);
			break;
		case 16://SM = Wyœlij komunikat
			cmd::sendMessage(Arguments);
			break;
		case 17://RM = Odczytaj komunikat
			cmd::readMessage(Arguments);
			break;
		case 18://ET = Utwórz etykietê
			cmd::makePoint(Arguments);
			break;
		case 19://JP = Skok bezwarunkowy
			cmd::jump(Arguments);
			break;
		case 20://JZ = Skok do etykiety jeœli flaga zerowa jest ustawiona
			cmd::jumpZero(Arguments);
			break;
		case 21://JE = Skok do etykiety
			cmd::jumpPoint(Arguments);
			break;
		case 22://RT = Zwróæ
			cmd::Return(Arguments);
			break;
		case 23://SP = Koniec programu
			cmd::end();
			break;
		case 24://MR - czytaj pamiêæ
			cmd::readMemory(Arguments);
			break;
		case 25://MW - wpisuj do pamiêci
			cmd::writeMemory(Arguments);
			break;
		case 26://KP - zabij proces
			cmd::killProcess(Arguments);
			break;
		case 27://CD - przejdŸ do kadalogu
			cmd::changeDir(Arguments);
			break;
		case 28://RD - przejdŸ do katalogu g³ównego
			cmd::rootDir();
			break;
		case 29://BD - cofnij siê do katalogu "wy¿ej"
			cmd::backDir();
			break;
		case 30://OF - otwórz plik
			cmd::openFile(Arguments);
			break;
		case 31://CLF - zamknij plik
			cmd::closeFile();
			break;
		case 32://EL - mniejsze równe
			cmd::equalOrLessThan(Arguments);
			break;
		case 33://MC - kopiuj rejestr 
			cmd::copyRegisters(Arguments);
			break;
		case 34://AP - dopisz do pliku
			cmd::appendFile(Arguments);
			break;
		default:
			std::cout << "ERROR - NIE OBS£UGIWANE POLECENIE!" << std::endl;

			break;
		}

		//RegStatus();
}
//Wykonywanie rozkazu
void Interpreter::DoCommand()
{
	std::string command_code = getArgument();
	//Wczytywanie ID rozkazu, oraz iloœci argumentów

	std::pair<int, int > CommandParameters = GetParameters(command_code);
	std::vector<std::string>Arguments;

	//Wczytywanie Argumentów
	for (int i = 0; i < CommandParameters.second; i++) {
		std::string finalArgument = getArgument();
		bool run = true;
		std::string tmp;
		if(finalArgument=="\"")
		{
			finalArgument = "";
			while(run)
			{
				tmp = getArgument();
				if(tmp=="\"")
				{
					run = false;
				}else
				{
					finalArgument += tmp + " ";
				}

			}
		}
		Arguments.push_back(finalArgument);
		
	}

	//Testowo - wyœwietlenie wczytanego rozkazu i jego argumentów;
	std::cout << "ID: " << CommandParameters.first << " | Command name: " << command_code;
	for (int i = 0; i < Arguments.size(); i++) {
		std::cout << " | Arg[" << i << "]: " << Arguments[i] << " ";
	}


	//wykonanie rozkazu
	ExecuteCommand(CommandParameters, Arguments);
	if (ActiveProcess != nullptr) {

		RegStatus();

		//Wybór rozkazu

		//obs³uga b³êdów.
		if (ActiveProcess->errorCode != 0)
		{
			std::cout << ErrorsTab[ActiveProcess->errorCode] << std::endl;
			std::cout << "Na rzecz b³êdu, program zostaje zakoñczony." << std::endl;
			pm->killProcess(ActiveProcess->GetPID());
		}
	}
}

void Interpreter::DoShellCommand(std::vector<std::string> cmd)
{
	std::string command_code = cmd[0];
	//Wczytywanie ID rozkazu, oraz iloœci argumentów
	std::pair<int, int > CommandParameters = GetParameters(command_code);
	std::vector<std::string>Arguments;
	PCB* Temp = ActiveProcess;
	ActiveProcess = &shell;
	//Wczytywanie Argumentów
	for (int i = 1; i <= CommandParameters.second; i++) {

	
		Arguments.push_back(cmd[i]);
	}

	//Testowo - wyœwietlenie wczytanego rozkazu i jego argumentów;
	std::cout << "ID: " << CommandParameters.first << " | Command name: " << command_code;
	for (int i = 0; i < Arguments.size(); i++) {
		std::cout << " | Arg[" << i << "]: " << Arguments[i] << " ";

	}
	//std::cin.ignore(1);

	//wykonanie rozkazu
	ExecuteCommand(CommandParameters, Arguments);

	if (ActiveProcess != nullptr) {
		if (ActiveProcess->errorCode != 0 && ActiveProcess != nullptr)
		{
			std::cout << ErrorsTab[ActiveProcess->errorCode] << std::endl;
			ActiveProcess->errorCode = 0;
		}
		if (ActiveProcess != nullptr)ActiveProcess = Temp;
	}
}


//Pobiera ID oraz iloœæ parametrów z tablicy rozkazów
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
	std::string arg = mm->readString(ActiveProcess, ActiveProcess->GetInstructionCounter());;
	ActiveProcess->SetInstructionCounter(ActiveProcess->GetInstructionCounter() + arg.size() + 1);

	/*std::clog << "WprowadŸ argument:";
	std::cin >> arg;
	std::clog << std::endl;*/
	return arg;
}

//Wyœwietla aktualny stan rejestrów
void Interpreter::RegStatus()
{
	std::cout << "Aktualny stan rejestrów" << std::endl;
	std::cout << "R0: " << ActiveProcess->registers[0] << " | R1: " << ActiveProcess->registers[1] << " | R2 " << ActiveProcess->registers[2] << " | R2 " << ActiveProcess->registers[3] <<std::endl;
	//std::cin.ignore(1);
}

