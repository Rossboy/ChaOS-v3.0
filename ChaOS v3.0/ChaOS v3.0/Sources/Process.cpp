#include "../Headers/Process.h"
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>

int PCB::processesCounter = 0;

PCB::PCB(std::string programName, int GID):points()
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
	this->errorCode = 0;
	this->zero = 0;
}

void PCB::SetState(State newState) 
{
	this->state = newState;
}
void PCB::SetProcesBurstTime(int newBurstTime)
{
	this->burstTime = newBurstTime;
}
int PCB::GetProcesBurstTime()
{
	return burstTime;
}
int PCB::GetPID()
{
	return PID;
}

int PCB::GetGID()
{
	return GID;
}
std::string PCB::GetFileName()
{
	return programName;
}
State PCB::GetState()
{
	return state;
}
void PCB::setPages(std::pair<int, bool>* newPages)
{
	this->pages = newPages;
}
std::pair<int, bool>* PCB::getPages()
{
	return this->pages;
}
void PCB::setPagesSize(int num)
{
	this->pagesSize = num;
}
int PCB::getPagesSize()
{
	return this->pagesSize;
}
void PCB::displayProcess()
{
	std::cout << "PID: " << GetPID() << ", GID: " << GetGID() << ", File name: " << GetFileName() << ", Burst time: " << GetProcesBurstTime() << ", State: " << GetState() << std::endl;
}
void PCB::SetInstructionCounter(int counter)
{
	this->instructionCounter = counter;
}
int PCB::GetInstructionCounter()
{
	return this->instructionCounter;
}
void PCB::addToMessages(SMS message) 
{
	this->messages.push_back(message);
}
SMS PCB::getMessage()
{
	// return this->messages.pop_front();
	auto popped = this->messages.front();
	return popped;
}
void PCB::deleteMessage()
{
	this->messages.pop_front();
}
int PCB::messagessize()
{
	return messages.size();
}