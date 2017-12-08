#pragma once
#include"SMS.h"
class Siec : private ProcessesManager
{
public:
	bool wyslij(std::string, int ID);
	SMS odbierz();
	void wyswietlwiad();
	Siec();
	~Siec();
};

