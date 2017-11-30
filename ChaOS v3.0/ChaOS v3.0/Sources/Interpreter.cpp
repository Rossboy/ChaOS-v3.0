#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "../Headers/Interpreter.h"


//Wykonywanie rozkazu
void Interpreter::DoCommand(PCB ActivePCB)
{
	std::string command_code = getArgument(ActivePCB);
	//Wczytywanie ID rozkazu, oraz iloœci argumentów
	std::pair<int, int > CommandParameters = GetParameters(command_code);
	std::vector<std::string>Arguments;

	//Wczytywanie Argumentów
	for (int i = 0; i < CommandParameters.second; i++) {
		Arguments.push_back(getArgument(ActivePCB));//od osoby zarz¹dzaj¹cej pamiêci¹
	}

	//Testowo - wyœwietlenie wczytanego rozkazu i jego argumentów;
	std::cout << "ID: " << CommandParameters.first << " | Command name: " << command_code;
	for (int i = 0; i < Arguments.size(); i++) {
		std::cout << " | Arg[" << i << "]: " << Arguments[i] << " ";
	}
	std::cin.ignore(2);

	RegStatus();

	//Wybór rozkazu
	int arg1, arg2;
	switch (CommandParameters.first)
	{
	case 0://AD = Dodawanie
		std::clog << "Wykonuje siê operacja dodawania..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		REG[arg1] += REG[arg2];
		RegStatus();
		break;
	case 1://SB = Odejmowanie
		std::clog << "Wykonuje siê operacja odejmowania..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		REG[arg1] -= REG[arg2];
		RegStatus();
		break;
	case 2://ML = MNO¯ENIE
		std::clog << "Wykonuje siê operacja mno¿enia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		REG[arg1] *= REG[arg2];
		RegStatus();
		break;
	case 3://DV = DZIELENIE
		std::clog << "Wykonuje siê operacja dzielenia..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		REG[arg1] /= REG[arg2];
		RegStatus();
		break;
	case 4://DR = DEKREMENTACJA
		std::clog << "Wykonuje siê operacja dekrementacji..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		REG[arg1]--;
		RegStatus();
		break;
	case 5://IR = INKREMENTACJA
		std::clog << "Wykonuje siê operacja inkrementacji..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		REG[arg1]++;
		RegStatus();
		break;
	case 6://MV =PRZENOSZENIE WARTOŒCI
		std::clog << "Wykonuje siê operacja przypisania wartoœci..." << std::endl;
		arg1 = atoi(Arguments[0].c_str());
		arg2 = atoi(Arguments[1].c_str());
		REG[arg1] = arg2;
		RegStatus();
		break;

		//OPERACJE NA PLIKACH
	case 7://MF = Utwórz plik
		break;
	case 8://SF = Zapis do pliku
		break;
	case 9://DF = Usuwanie pliku
		break;
	case 10://RF = Odczyt pliku
		break;
	case 11://LS = Listuj pliki
		break;
	case 12://CP = Zmieñ nazwê pliku
		break;

		//PROCESY
	case 13://MP = Utwórz proces
		break;
	case 14://EX = Wykonaj program?
		break;
	case 15://PS = Wyœwietl procesy
		break;
	case 16://SM = Wyœlij komunikat
		break;
	case 17://RM = Odczytaj komunikat
		break;
	case 18://ET = Utwórz etykietê
		break;
	case 19://JP = Skok bezwarunkowy
		break;
	case 20://JZ = Skok jeœli flaga zerowa jest ustawiona
		break;
	case 21://CM = Skok przyrównanie
		break;
	case 22://RT = Zwróæ
		break;
	case 23://SP = Koniec programu
		break;
	default:
		std::cout << "ERROR - NIE OBS£UGIWANE POLECENIE!" << std::endl;
		std::cin.ignore(1);

		break;
	}


}


//Pobiera ID oraz iloœæ parametrów z tablicy rozkazów
std::pair<int, int > Interpreter::GetParameters(const std::string& cmd)
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
std::string Interpreter::getArgument(PCB ActivePCB)
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
	std::cout << "R0: " << REG[0] << " | R1: " << REG[1] << " | R2 " << REG[2] << std::endl;
	std::cin.ignore(1);
}
