#ifndef FILESYSTEMEXCEPTION_H
#define FILESYSTEMEXECEPTION__H
#include <exception>

class outOfMemory : public std::exception
{
public:
	outOfMemory();
};

class objectNotFound : public std::exception
{
public:
	objectNotFound();
};

class objectExists : public std::exception
{
public:
	objectExists();
};

class fileToLong : public std::exception
{
public:
	fileToLong();
};
#endif FILESYSTEMEXCEPTION__H