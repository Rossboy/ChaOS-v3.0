#ifndef SIEC__H
#define SIEC__H
#include"..\Headers\SMS.h"
#include "..\Headers\ProcessesManager.h"
class Siec
{
public:
	bool wyslij(std::string, int ID);
	SMS odbierz();
	void wyswietlwiad();
	Siec();
	~Siec();
};

#endif SIEC__H