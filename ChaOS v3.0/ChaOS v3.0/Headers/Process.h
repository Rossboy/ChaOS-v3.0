#pragma once
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>

enum State { New, Ready, Waiting, Running, Terminated };


class Process
{
private:
	int PID;
	int GID;
	State state;
	int instructionCounter;
	int registers[4];
	int burstTime;
	std::string programName;
	std::list<std::string>messages;
	std::pair<int, bool>* pages;
	int pagesSize;


public:
	static int processesCounter;
	Process(std::string fileName, int GID);
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
	void addToMessages(std::string message);
	std::string getMessage();
};