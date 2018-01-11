#ifndef SIEC__H
#define SIEC__H
#include"..\Headers\SMS.h"
#include"..\Headers\ConditionVariable.h"
#include<memory>
#include<map>
class Siec
{
	std::map<int,ConditionVariable> cv;
public:
	void wyslij(std::string, int ID);
	void odbierz(int adres);
	void wyswietlwiadaktywnego();
	void wyswietlwiad();
	void sprawdz();
	Siec();
	~Siec();
};

#endif SIEC__H