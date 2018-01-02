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

//funkcje do obs³ugi komend
namespace cmd {
	void add(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje siê operacja dodawania..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] += ActiveProcess->registers[arg2];
	}//done

	void substract(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje siê operacja odejmowania..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] -= ActiveProcess->registers[arg2];
	}//done

	void multiply(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje siê operacja mno¿enia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] *= ActiveProcess->registers[arg2];
	}//done

	void divide(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2; 
		std::clog << "Wykonuje siê operacja dzielenia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] /= ActiveProcess->registers[arg2];
	}//done

	void increment(const std::vector<std::string>& Arguments)
	{
		int arg1;
		std::clog << "Wykonuje siê operacja inkrementacji..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		ActiveProcess->registers[arg1]++;
	}//done

	void decrement(const std::vector<std::string>& Arguments)
	{
		
		int arg1;
		std::clog << "Wykonuje siê operacja dekrementacji..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		ActiveProcess->registers[arg1]--;
	}	//done


	void move(const std::vector<std::string>& Arguments)
	{
		int arg1, arg2;
		std::clog << "Wykonuje siê operacja przypisania wartoœci..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		ActiveProcess->registers[arg1] = arg2;
	}//done

	void openFile(const std::vector<std::string>& Arguments)
	{
		
	}

	void closeFile(const std::vector<std::string>& Arguments)
	{
		
	}

	void makeFile(const std::vector<std::string>& Arguments)
	{
		
	}

	void saveFile(const std::vector<std::string>& Arguments)
	{

	}


	void deleteFile(const std::vector<std::string>& Arguments)
	{

	}

	void readFile(const std::vector<std::string>& Arguments)
	{

	}

	void listFiles(const std::vector<std::string>& Arguments)
	{

	}

	void changeFileName(const std::vector<std::string>& Arguments)
	{

	}

	void makeProcess(const std::vector<std::string>& Arguments)
	{

	}

	void executeProcess(const std::vector<std::string>& Arguments)
	{

	}

	void listProcess(const std::vector<std::string>& Arguments)
	{

	}

	void sendMessage(const std::vector<std::string>& Arguments)
	{

	}

	void readMessage(const std::vector<std::string>& Arguments)
	{

	}

	void makePoint(const std::vector<std::string>& Arguments)
	{

	}

	void jump(const std::vector<std::string>& Arguments)
	{

	}

	void jumpZero(const std::vector<std::string>& Arguments)
	{

	}

	void jumpPoint(const std::vector<std::string>& Arguments)
	{
		
	}

	void Return(const std::vector<std::string>& Arguments)
	{

	}
	
	void end(const std::vector<std::string>& Arguments)
	{

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
		cmd::saveFile(Arguments);
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
	case 12://CP = Zmieñ nazwê pliku
		cmd::changeFileName(Arguments);
		break;

		//PROCESY
	case 13://MP = Utwórz proces
		cmd::makeProcess(Arguments);
		break;
	case 14://EX = Wykonaj program?
		cmd::executeProcess(Arguments);
		break;
	case 15://PS = Wyœwietl procesy
		cmd::listProcess(Arguments);
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
		cmd::end(Arguments);
		break;
	default:
		std::cout << "ERROR - NIE OBS£UGIWANE POLECENIE!" << std::endl;
		std::cin.ignore(1);

		break;
	}
	RegStatus();
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
		Arguments.push_back(getArgument());//od osoby zarz¹dzaj¹cej pamiêci¹
	}

	//Testowo - wyœwietlenie wczytanego rozkazu i jego argumentów;
	std::cout << "ID: " << CommandParameters.first << " | Command name: " << command_code;
	for (int i = 0; i < Arguments.size(); i++) {
		std::cout << " | Arg[" << i << "]: " << Arguments[i] << " ";
	}
	std::cin.ignore(2);

	//wykonanie rozkazu
	ExecuteCommand(CommandParameters, Arguments);


	RegStatus();

	//Wybór rozkazu
	
	//obs³uga b³êdów.
	if(ActiveProcess->ErrorCode!=0)
	{
		std::cout << ErrorsTab[ActiveProcess->ErrorCode] << std::endl;
		std::cout << "Na rzecz b³êdu, program zostaje zakoñczony." << std::endl;
		pm->killProcess(ActiveProcess->GetPID());
	}

}

void Interpreter::DoShellCommand(std::vector<std::string> cmd)
{
	std::string command_code = cmd[0];
	//Wczytywanie ID rozkazu, oraz iloœci argumentów
	std::pair<int, int > CommandParameters = GetParameters(command_code);
	std::vector<std::string>Arguments;

	//Wczytywanie Argumentów
	for (int i = 0; i < CommandParameters.second; i++) {
		Arguments.push_back(cmd[1+i]);
	}

	//Testowo - wyœwietlenie wczytanego rozkazu i jego argumentów;
	std::cout << "ID: " << CommandParameters.first << " | Command name: " << command_code;
	for (int i = 0; i < Arguments.size(); i++) {
		std::cout << " | Arg[" << i << "]: " << Arguments[i] << " ";
	}
	std::cin.ignore(2);

	//wykonanie rozkazu
	ExecuteCommand(CommandParameters, Arguments);


	RegStatus();

	
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
	std::string arg;
	std::clog << "WprowadŸ argument:";
	std::cin >> arg;
	std::clog << std::endl;
	return arg;
}

//Wyœwietla aktualny stan rejestrów
void Interpreter::RegStatus()
{
	std::cout << "Aktualny stan rejestrów" << std::endl;
	std::cout << "R0: " << ActiveProcess->registers[0] << " | R1: " << ActiveProcess->registers[1] << " | R2 " << ActiveProcess->registers[2] << std::endl;
	std::cin.ignore(1);
}

