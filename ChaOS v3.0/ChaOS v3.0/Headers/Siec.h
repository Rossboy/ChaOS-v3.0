#ifndef SIEC__H
#define SIEC__H
//#include "..\Headers\ConditionVariable.h"
#include"..\Headers\SMS.h"
#include<memory>
class Siec
{
	//ConditionVariable zmienna;
public:
	bool wyslij(std::string, int ID);
	std::unique_ptr<SMS> odbierz();
	void wyswietlwiad();
	Siec();
	~Siec();
};

#endif SIEC__H