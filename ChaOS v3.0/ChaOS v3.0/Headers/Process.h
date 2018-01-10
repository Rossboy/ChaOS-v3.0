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

struct Page
{
	int frameOccupied;
	bool inMemory;
};

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
	Page* pageTable;
	int pageTableSize;

public:
	std::vector<int> points;
	int registers[4];
	static int processesCounter;
	PCB(std::string fileName, int GID);
	void setStateAndMoveToRespectiveList(State newState);
	void SetProcesBurstTime(int newBurstTime);
	int GetProcesBurstTime();
	int GetPID();
	int GetGID();
	std::string GetFileName();
	State GetState();
	void displayProcess();
	void SetInstructionCounter(int counter);
	int GetInstructionCounter();
	void setPageTable(Page* newpageTable);
	Page* getPageTable();
	void setPageTableSize(int num);
	int getPageTableSize();
	void addToMessages(SMS message);
	SMS getMessage();
	//Konrad: doda³em usuwanie bo potrzebujê te¿ geta do wyœwietlania wiadomoœci
	void deleteMessage();
	//Konrad: d³ugoœci te¿ bêdê potrzebowa³
	std::list<SMS> getMessages();

	//Stanislaw: Plik otwarty przez proces
	file* currentFile;
	unsigned short currentDir;
	std::stack<uShort> returnPath;
	//Stanislaw: nie rzucam wyjatkow, tylko ustawiam flage, tak jak ustalilismy
	
	bool zero;
	int errorCode;
	bool wait;
};
#endif PROCESS__H