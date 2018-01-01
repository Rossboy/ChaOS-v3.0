#ifndef SIEC__H
#define SIEC__H
#include"..\Headers\SMS.h"
#include<memory>
class Siec
{
public:
	bool wyslij(std::string, int ID);
	std::unique_ptr<SMS> odbierz();
	void wyswietlwiad();
	Siec();
	~Siec();
};

#endif SIEC__H