#ifndef PROCESS__H
#define PROCESS__H
#include"..\Headers\SMS.h"
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <stack>
#include "..\Headers\file.h"
enum State { New, Ready, Waiting, Running, Terminated };


class PCB
{
private:
	int PID;
	int GID;
	State state;
	int instructionCounter;

	int burstTime;
	std::string programName;
	std::list<SMS>messages;
	std::pair<int, bool>* pages;
	int pagesSize;


public:
	int registers[4]; 
	static int processesCounter;
	PCB(std::string fileName, int GID);
	void SetState(State newState);
	void SetProcesBurstTime(int newBurstTime);
	int GetProcesBurstTime();
	int GetPID();
	int GetGID();
	std::string GetFileName();
	State GetState();
	void displayProcess();
	void SetInstructionCounter(int counter);
	int GetInstructionCounter();
	void setPages(std::pair<int, bool>* newPages);
	std::pair<int, bool>* getPages();
	void setPagesSize(int num);
	int getPagesSize();
	void addToMessages(SMS message);
	SMS getMessage();
	//Konrad: doda³em usuwanie bo potrzebujê te¿ geta do wyœwietlania wiadomoœci
	void deleteMessage();
	//Konrad: d³ugoœci te¿ bêdê potrzebowa³
	int messagessize();

	//Stanislaw: Plik otwarty przez proces
	file* currentFile;
	unsigned short currentDir;
	std::stack<uShort> returnPath;
	//Stanislaw: nie rzucam wyjątków, tylko ustawiam flagę, tak jak ustaliliśmy
	bool error_flag;
};
#endif PROCESS__H