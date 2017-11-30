#include "../Headers/Process.h"
#pragma once
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>

int Process::processesCounter = 0;

Process::Process(std::string programName, int GID)
{
	this->PID = processesCounter++;
	this->GID = GID;
	this->state = State::New;
	this->instructionCounter = 0;
	this->registers[0] = 0;
	this->registers[1] = 0;
	this->registers[2] = 0;
	this->registers[3] = 0;
	this->burstTime = 10;
	this->programName = programName;
	this->state = State::Waiting;
}

void Process::SetState(State newState) 
{
	this->state = newState;
}
void Process::SetProcesBurstTime(int newBurstTime)
{
	this->burstTime = newBurstTime;
}
int Process::GetProcesBurstTime()
{
	return burstTime;
}
int Process::GetPID()
{
	return PID;
}

int Process::GetGID()
{
	return GID;
}
std::string Process::GetFileName()
{
	return programName;
}
State Process::GetState()
{
	return state;
}
void Process::setPages(std::pair<int, bool>* newPages)
{
	this->pages = newPages;
}
std::pair<int, bool>* Process::getPages()
{
	return this->pages;
}
void Process::setPagesSize(int num)
{
	this->pagesSize = num;
}
int Process::getPagesSize()
{
	return this->pagesSize;
}
void Process::displayProcess()
{
	std::cout << "PID: " << GetPID() << ", GID: " << GetGID() << ", File name: " << GetFileName() << ", Burst time: " << GetProcesBurstTime() << ", State: " << GetState() << std::endl;
}
void Process::SetInstructionCounter(int counter)
{
	this->instructionCounter = counter;
}
int Process::GetInstructionCounter()
{
	return this->instructionCounter;
}
void Process::addToMessages(std::string message) 
{
	this->messages.push_back(message);
}
std::string Process::getMessage()
{
	return this->messages.pop_front();
}