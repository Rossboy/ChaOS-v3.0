#pragma once
#include <string>
// Klasa file reprezentuje plik na którym pracuje OS.
// 
// 1. Wpisywanie do pliku (podany obiekt string)
// 2. Odczytywanie pliku (zwracany obiekt string)

using uShort = unsigned short;

class file final
{
	friend class ChaOS_filesystem;
public:
	file();
	~file();
	//void write(const std::string& text);
	//std::string read();
	//void append(const std::string& text);
	
private:
	std::string fileContent;
	char filename[5];
	unsigned int fileSize;
	uShort fileSizeInSectors;
	uShort firstSector;
	uShort fileDir;
};

