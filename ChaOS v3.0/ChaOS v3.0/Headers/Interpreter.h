#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Process.h"


class Interpreter
{
	



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

		//Operacje na plikach
		std::make_pair("MF",std::make_pair(7,1)), //Utwórz plik
		std::make_pair("SF",std::make_pair(8,2)), //Zapis do pliku
		std::make_pair("DF",std::make_pair(9,1)), //Usuwanie pliku
		std::make_pair("RF",std::make_pair(10,1)), //Odczyt pliku
		std::make_pair("LS",std::make_pair(11,0)), //Listuj pliki
		std::make_pair("CP",std::make_pair(12,2)), //Zmieñ nazwê pliku

		//Procesy
		std::make_pair("MP",std::make_pair(13,0)), //Utwórz proces
		std::make_pair("EX",std::make_pair(14,1)), //Wykonaj program?
		std::make_pair("PS",std::make_pair(15,0)), //Wyœwietl procesy
		std::make_pair("SM",std::make_pair(16,2)), //Wyœlij komunikat
		std::make_pair("RM",std::make_pair(17,1)), //Odczytaj komunikat

		//Programowe
		std::make_pair("ET",std::make_pair(18,0)), //Utwórz etykietê
		std::make_pair("JP",std::make_pair(19,1)), //Skok bezwarunkowy
		std::make_pair("JZ",std::make_pair(20,1)), //Skok jeœli flaga zerowa jest ustawiona
		std::make_pair("CM",std::make_pair(21,2)), //Skok przyrównanie
		std::make_pair("RT",std::make_pair(22,1)), //Zwróæ
		std::make_pair("SP",std::make_pair(23,1)), //Koniec programu



		std::make_pair("ER",std::make_pair(99,0)) //Error - nie obs³ugiwane polecenie

	};


	std::pair<int, int > GetParameters(std::string& cmd);

	//Tymczasowe wpisywanie argumentówl
	std::string getArgument();

	//Stan rejestrów
	void RegStatus();

public:
	Interpreter() {};

	//Wykonaj rozkaz (kod rozkazu)
	void DoCommand();
};
