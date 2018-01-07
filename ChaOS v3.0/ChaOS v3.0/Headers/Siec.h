#ifndef SIEC__H
#define SIEC__H
#include"..\Headers\SMS.h"
#include"..\Headers\ConditionVariable.h"
#include<memory>
class Siec
{
	std::vector<ConditionVariable> cv;
public:
	bool wyslij(std::string, int ID);
	std::unique_ptr<SMS> odbierz();
	void wyswietlwiadaktywnego();
	void wyswietlwiad();
	Siec();
	~Siec();
};

#endif SIEC__H