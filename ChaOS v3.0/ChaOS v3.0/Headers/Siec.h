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
	bool wyslij(std::string, int ID);
	std::unique_ptr<SMS> odbierz();
	void wyswietlwiadaktywnego();
	void wyswietlwiad();
	void sprawdz();
	Siec();
	~Siec();
};

#endif SIEC__H