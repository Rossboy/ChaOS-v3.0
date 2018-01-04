#ifndef INTERPRETER__H
#define INTERPRETER__H
#include <iostream>
#include <string>
#include <vector>
#include "Process.h"


class Interpreter
{
	


	//liczba rozkazów = 32 + error (99)
	//tablica rozkazów <KOD ROZKAZU <ID rozkazu/iloœæ argumentów>>
	std::vector<std::pair<std::string, std::pair<int, int >>> CommandTab{

		//Arytmetyka
		std::make_pair("AD",std::make_pair(0,2)),//DODAWANIE 
		std::make_pair("SB",std::make_pair(1,2)),//ODEJMOWANIE
		std::make_pair("ML",std::make_pair(2,2)), //MNO¯ENIE
		std::make_pair("DV",std::make_pair(3,2)), //DZIELENIE
		std::make_pair("DR",std::make_pair(4,1)), //DEKREMENTACJA
		std::make_pair("IR",std::make_pair(5,1)), //INKREMENTACJA
		std::make_pair("MV",std::make_pair(6,2)), //PRZENOSZENIE WARTOŒCI

		//Pamiêæ
		std::make_pair("MR",std::make_pair(24,2)), //Czytanie z pamiêci
		std::make_pair("MW",std::make_pair(25,2)), //Wpisywanie do pamiêci


		//Operacje na plikach
		std::make_pair("CF",std::make_pair(7,2)), //Utwórz plik/folder
		std::make_pair("SF",std::make_pair(8,1)), //Zapis do pliku //////do podmiany
		std::make_pair("WRITE",std::make_pair(8,1)), //Zapis do pliku //////do podmiany dla shella
		std::make_pair("RF",std::make_pair(9,1)), //Usuwanie pliku
		std::make_pair("FR",std::make_pair(10,0)), //Odczyt pliku
		std::make_pair("READ",std::make_pair(10,0)), //Odczyt pliku dla shella
		std::make_pair("LD",std::make_pair(11,0)), //Listuj pliki
		std::make_pair("REN",std::make_pair(12,2)), //Zmieñ nazwê pliku
		std::make_pair("AP",std::make_pair(17,1)), //Dopisz do pliku
		std::make_pair("CD",std::make_pair(27,1)), //PrzejdŸ do folderu
		std::make_pair("RD",std::make_pair(28,0)), //PrzejdŸ do folderu ROOT
		std::make_pair("BD",std::make_pair(29,0)), //PrzejdŸ do folderu poprzedniego
		std::make_pair("OF",std::make_pair(30,1)), //Otwórz plik
		std::make_pair("CLF",std::make_pair(31,0)), //Zamknij plik

		//Procesy
		std::make_pair("CP",std::make_pair(13,2)), //Utwórz proces
		std::make_pair("EX",std::make_pair(14,1)), //Wykonaj program?
		std::make_pair("PS",std::make_pair(15,0)), //Wyœwietl procesy
		std::make_pair("SM",std::make_pair(16,2)), //Wyœlij komunikat
		std::make_pair("RM",std::make_pair(17,1)), //Odczytaj komunikat
		std::make_pair("KP",std::make_pair(26,1)), //Zabij proces

		//Programowe
		std::make_pair("ET",std::make_pair(18,0)), //Utwórz etykietê
		std::make_pair("JP",std::make_pair(19,1)), //Skok bezwarunkowy
		std::make_pair("JZ",std::make_pair(20,1)), //Skok do etykiety jeœli flaga zerowa jest ustawiona
		std::make_pair("JE",std::make_pair(21,1)), //Skok do etykiety
		std::make_pair("RT",std::make_pair(22,1)), //Zwróæ
		std::make_pair("SP",std::make_pair(23,1)), //Koniec programu



		std::make_pair("ER",std::make_pair(99,0)) //Error - nie obs³ugiwane polecenie

	};


	std::pair<int, int > GetParameters(std::string& cmd);

	//Tymczasowe wpisywanie argumentówl
	std::string getArgument();

	//Stan rejestrów
	void RegStatus();
	//Wykonanie rozkazu
	void ExecuteCommand(const std::pair<int, int>& CommandParameters, const std::vector<std::string>& Arguments);


	
public:
	Interpreter() {};

	//Wykonaj rozkaz (kod rozkazu)
	void DoCommand();
	//Wykonanie rozkazu od Shella (podmianka procesu i brak obs³ugi b³êdów w sposób zabijaj¹cy proces)
	void DoShellCommand(std::vector<std::string> cmd);
};
#endif INTERPRETER__H