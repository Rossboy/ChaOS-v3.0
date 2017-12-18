#ifndef SIEC__H
#define SIEC__H
#include"..\Headers\SMS.h"
#include "..\Headers\ProcessesManager.h"
#include"..\Headers\ConditionVariable.h"
#include<memory>
class Siec
{
	ConditionVariable zmienna;
public:
	bool wyslij(std::string, int ID);
	std::unique_ptr<SMS> odbierz();
	void wyswietlwiad();
	Siec();
	~Siec();
};

#endif SIEC__H