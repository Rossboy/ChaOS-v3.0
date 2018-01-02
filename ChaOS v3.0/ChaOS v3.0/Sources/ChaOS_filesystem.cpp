#include "../Headers/ChaOS_filesystem.h"

#include <iostream>
#include <iomanip>
#include <sstream>

extern PCB* ActiveProcess;

union char_short
{
	unsigned short USHORT;
	char CHAR[2];
};

union char_int
{
	unsigned int INT;
	char CHAR[4];
};

bool getBit(unsigned int INT, unsigned int i)
{
	return INT & (1 << i);
}

template <typename T>
void setBit(T& INT, unsigned int i, bool value)
{
	if (value)
		INT |= (1 << i);
	else
		INT &= ~(1 << i);
}


ChaOS_filesystem::ChaOS_filesystem()
{
	// short na char(diskSize)
	char_short temp; temp.USHORT = disk.diskSize;

	// int na char(bitVector)
	char_int temp2; temp2.INT = (~0);

	// Oznaczamy zajęty sektor 0
	setBit(temp2.CHAR[0], 0, 0);

	//Tworzenie i zapis VCB
		// diskSize(short), sectorSize(char), numberOfSectors(char), _,  _, _, _, _,
		// freeSectorCount(char), _, _, _,  _, _, _, _,
		// freeSectorBitVector(int),  _, _, _, _,
		// _, _, _, _, _, _, _, _
	char VCB[32] = { temp.CHAR[1], temp.CHAR[0], static_cast<char>(disk.sectorSize), static_cast<char>(disk.numberOfSectors),		0,0,0,0,
		static_cast<char>(disk.numberOfSectors - 1),		0,0,0,0,0,0,0,
		temp2.CHAR[3], temp2.CHAR[2], temp2.CHAR[1], temp2.CHAR[0],			0,0,0,0,
		0,0,0,0, 0,0,0,0 };
	disk.writeSector(0, VCB);

	//Tworzenie katalogu ROOT
	rootDirSector = allocateSector();

	char sector[32] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 'R', 'O', 'O', 'T', '\0', rootDirSector, 1, 0 };
	disk.writeSector(rootDirSector, sector);

	currentDirFirst = rootDirSector;
	currentDirSector = rootDirSector;
}

ChaOS_filesystem::~ChaOS_filesystem()
{
	closeFile();
}


c_uShort ChaOS_filesystem::allocateSector()
{
	char VCB[32];
	char freeSectorCount;

	disk.readSector(0, VCB);
	freeSectorCount = VCB[8];
	char_int bitVector; bitVector.CHAR[3] = VCB[16]; bitVector.CHAR[2] = VCB[17]; bitVector.CHAR[1] = VCB[18]; bitVector.CHAR[0] = VCB[19];//char[4] to int

	if (freeSectorCount == 0)
	{
		//throw outOfMemory();
		ActiveProcess->errorCode = 2; // Brak wolnych sektorów na dysku.
		return 0;
	}

	for (unsigned int i = 0; i < disk.numberOfSectors; i++) // wyszukiwanie pierwszego wolnego sektora
	{
		if (getBit(bitVector.INT, i))
		{
			setBit(bitVector.INT, i, 0);
			freeSectorCount--;

			VCB[8] = freeSectorCount;
			VCB[16] = bitVector.CHAR[3]; VCB[17] = bitVector.CHAR[2]; VCB[18] = bitVector.CHAR[1]; VCB[19] = bitVector.CHAR[0];

			disk.writeSector(0, VCB);
			return i;
		}
	}
}

void ChaOS_filesystem::freeSector(uShort number)
{
	char VCB[32];
	disk.readSector(0, VCB);
	char& freeSectorCount = VCB[8];

	char_int bitVector;
	bitVector.CHAR[3] = VCB[16]; bitVector.CHAR[2] = VCB[17]; bitVector.CHAR[1] = VCB[18]; bitVector.CHAR[0] = VCB[19];//char[4] to int

	setBit(bitVector.INT, number, 1);
	freeSectorCount++;

	VCB[16] = bitVector.CHAR[3]; VCB[17] = bitVector.CHAR[2]; VCB[18] = bitVector.CHAR[1]; VCB[19] = bitVector.CHAR[0];
	disk.writeSector(0, VCB);
}


void ChaOS_filesystem::create(const char * f, type t)
{
	char name[5]; toChar5(f, name);

	if (search(name, type::dir) || search(name, type::file))
	{
		//throw objectExists();
		ActiveProcess->errorCode = 3;// Obiekt o podanej nazwie już istnieje.
		return;
	}

	// Nowy wpis
		//NAZWA[5], PIERWSZY, ROZMIAR, TYP
	char row[8] = { name[0], name[1], name[2], name[3], name[4], 0, 1, char(t) };

	char dirSector[32];
	bool fileCreated = 0;
	currentDirSector = currentDirFirst;


	while (!fileCreated)
	{
		disk.readSector(currentDirSector, dirSector);

		for (int i = 0, j = 0; j < 3 && !fileCreated; i += 8, j++)
		{
			if (dirSector[i] == 0) // znaleziono wolne miejsce na wpis
			{
				row[5] = allocateSector();
				char sector[32] = {};

				if (t == type::dir)
				{
					toChar5(f, &sector[24]);
					sector[29] = row[5]; //pierwszy
					sector[30] = 1; //rozmiar
					sector[31] = 0;      //następny

					row[6] = 0; //w liście zawartości nie przechowuje rozmiaru katalogu
				}

				addRow(&dirSector[i], row);
				disk.writeSector(row[5], sector); //wpisz tablicę do przydzielonego sektora
				disk.writeSector(currentDirSector, dirSector);
				fileCreated = true;
			}
		}

		//jeśli w danym sektorze nie ma miejsca na wpisy
		if (!fileCreated)
		{
			// jeśli następny sektor nie istnieje
			if (dirSector[31] == 0)
			{
				dirSector[31] = allocateSector();
				disk.writeSector(currentDirSector, dirSector);

				//aktuazlizacja rozmiaru katalogu (informacja w pierwszym sektorze)
				char firstSector[32];
				disk.readSector(currentDirFirst, firstSector);
				firstSector[30]++;
				disk.writeSector(currentDirFirst, firstSector);
				//

				//wyzerowanie następnego sektora aktualnego katalogu
				char nextSector[32] = { 0 };
				currentDirSector = dirSector[31];
				disk.writeSector(currentDirSector, nextSector);
			}
			else
			{
				currentDirSector = dirSector[31];
			}
		}

	}

	currentDirSector = currentDirFirst;
}

void ChaOS_filesystem::remove(const char * f)
{
	char name[5]; toChar5(f, name);
	uShort file = search(name, type::file), dir = search(name, type::dir);

	if (!(file || dir))
	{
		//throw objectNotFound();
		ActiveProcess->errorCode = 2;
		return;
	}
	if (file)
	{
		if (!ActiveProcess->currentFile)
		{
			ActiveProcess->errorCode = 5;
			return;
		}

		if (!equalName(ActiveProcess->currentFile->filename, name))
		{
			ActiveProcess->errorCode = 6;
			return;
		}
	}

	// Wyszukiwanie pierwszego sektora do zwolnienia
	int currentSectorToFree = search(name, type::file) != 0 ? search(name, type::file) : search(name, type::dir);

	// Zwalnianie kolejnych sektorów
	char dirSector[32] = {};
	do {
		disk.readSector(currentSectorToFree, dirSector);
		freeSector(currentSectorToFree);
		currentSectorToFree = dirSector[31];
	} while (dirSector[31]);
	//

	char row[8] = {};
	currentDirSector = currentDirFirst;

	// Przeszukiwanie aktualnego katalogu, by usunąć wpis
	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);

		for (int i = 0, j = 0; i < 3; i++, j += 8)
		{
			getRow(&dirSector[j], row);
			if (equalName(row, name)) //znaleziono wpis
			{
				for (int x = 0; x < 8; x++)
				{
					row[x] = 0;
				}
				addRow(&dirSector[j], row);
			}
		}
		disk.writeSector(currentDirSector, dirSector);

		currentDirSector = dirSector[31];
	}

	currentDirSector = currentDirFirst;
	if (file)
	{
		closeFile();
	}
}


std::string ChaOS_filesystem::listDirectory()
{
	currentDirFirst = ActiveProcess->currentDir;

	std::ostringstream result;
	char dirSector[32];
	disk.readSector(ActiveProcess->currentDir, dirSector);
	result << " Current directory is ";
	for (int i = 24; i < 29; i++)
	{
		result << dirSector[i];
	}
	result << " (size: " << std::setw(2) << unsigned short(dirSector[30]) << ")";
	result << "\nName\tFirst\tSize\tType\n";

	currentDirSector = currentDirFirst;

	// Wypisywanie kolejnych wpisów
	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);
		for (int i = 0, j = 0; i < 3; i++, j += 8)
		{
			if (dirSector[j] != 0)
			{
				result << " ";
				for (int i = 0; i < 5; i++)
				{
					result << dirSector[i + j];
				}
				result << "\t " << (unsigned int)dirSector[5 + j] << "\t " << ((dirSector[7 + j] == char(type::dir)) ? ' ' : char(dirSector[6 + j] + 48)) << "\t " << dirSector[7 + j] << std::endl;
			}
		}
		currentDirSector = dirSector[31];
	}
	currentDirSector = currentDirFirst;
	return result.str();
}

void ChaOS_filesystem::changeDirectory(const char * name)
{
	uShort temp = search(name, type::dir);
	if (temp)
	{
		ActiveProcess->returnPath.push(currentDirFirst);
		ActiveProcess->currentDir = temp;
	}
	else
	{
		//throw objectNotFound();
		ActiveProcess->errorCode = 4; // Obiekt o podanej nazwie nie został znaleziony.
		return;
	}
}

void ChaOS_filesystem::backDirectory()
{
	if (ActiveProcess->returnPath.empty())
	{
		rootDirectory();
	}
	else
	{
		ActiveProcess->currentDir = ActiveProcess->returnPath.top();
		ActiveProcess->returnPath.pop();
	}
}

void ChaOS_filesystem::rootDirectory()
{
	while (!ActiveProcess->returnPath.empty())
	{
		ActiveProcess->returnPath.pop();
	}
	ActiveProcess->currentDir = rootDirSector;
}


void ChaOS_filesystem::rename(const char * f, const char* newname)
{
	char name[5]; toChar5(f, name);

	if (!(search(name, type::file) || search(name, type::dir)))
	{
		//throw objectNotFound();
		ActiveProcess->errorCode = 4;
		return;
	}

	currentDirSector = currentDirFirst;

	char row[8] = {};
	char dirSector[32] = {};

	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);

		for (int i = 0, j = 0; i < 3; i++, j += 8)
		{
			getRow(&dirSector[j], row);
			if (equalName(row, name))
			{
				char n[5];
				toChar5(newname, n);
				for (int i = 0; i < 5; i++) row[i] = n[i];
				addRow(&dirSector[j], row);
			}
		}
		disk.writeSector(currentDirSector, dirSector);
		currentDirSector = dirSector[31];
	}

	currentDirSector = currentDirFirst;
}

uShort ChaOS_filesystem::search(const char * filename, type t)
{
	char name[5]; toChar5(filename, name);
	char row[8] = {};
	char dirSector[32] = {};

	currentDirSector = currentDirFirst;

	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);

		getRow(dirSector, row);
		if (equalName(row, name) && row[7] == char(t)) return row[5];

		getRow(&dirSector[8], row);
		if (equalName(row, name) && row[7] == char(t)) return row[5];

		getRow(&dirSector[16], row);
		if (equalName(row, name) && row[7] == char(t)) return row[5];

		currentDirSector = dirSector[31];
	}

	currentDirSector = currentDirFirst;
	return 0;
}

void ChaOS_filesystem::openFile(const char * filename)
{
	currentDirFirst = currentDirSector = ActiveProcess->currentDir;

	char name[5]; toChar5(filename, name);

	if (!search(name, type::file))
	{
		//throw objectNotFound();
		ActiveProcess->errorCode = 4;
		return;
	}

	char row[8] = {};
	char dirSector[32] = {};
	char sizeInSectors;

	// Wyszukiwanie wpisu dot. pliku.
	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);

		for (int i = 0, j = 0; i < 3; i++, j += 8)
		{
			getRow(&dirSector[j], row);
			if (equalName(row, name))
			{
				currentFileSector = currentFileFirst = row[5];
				sizeInSectors = row[6];
			}
		}
		currentDirSector = dirSector[31];
	}

	if (ActiveProcess->currentFile)
	{
		ActiveProcess->errorCode = 7;
		return;
	}

	ActiveProcess->currentFile = new file;
	//filename
	for (int i = 0; i < 5; i++) { ActiveProcess->currentFile->filename[i] = name[i]; }

	//filesizeInsectors
	ActiveProcess->currentFile->fileSizeInSectors = sizeInSectors;

	//firstector
	ActiveProcess->currentFile->firstSector = currentFileFirst;

	//read firstsector
	char fileSector[32];
	disk.readSector(currentFileFirst, fileSector);
	ActiveProcess->currentFile->fileSize = fileSector[0];

	ActiveProcess->currentFile->fileDir = currentDirFirst;

	ActiveProcess->currentDir = this->currentDirFirst;

	fileSynchronization[currentFileFirst].wait(ActiveProcess);
}

void ChaOS_filesystem::writeFile(const std::string& text)
{
	if (ActiveProcess->currentFile == nullptr)
	{
		ActiveProcess->errorCode = 5;
		return;
	}
	currentFileFirst = ActiveProcess->currentFile->firstSector;

	ActiveProcess->currentFile->fileContent = text;
	saveFile();
}

std::string ChaOS_filesystem::readFile()
{
	if (ActiveProcess->currentFile == nullptr)
	{
		ActiveProcess->errorCode = 5;
		return "";
	}

	currentFileFirst = ActiveProcess->currentFile->firstSector;
	char fileSector[32];

	disk.readSector(currentFileFirst, fileSector);

	uint8_t charsToRead = fileSector[0];

	// Odczyt pierwszego sektora
	for (int j = 1; j < 31 && charsToRead; j++)
	{
		ActiveProcess->currentFile->fileContent += fileSector[j];
		charsToRead--;
	}
	currentFileSector = fileSector[31];

	// Odczyt kolejnych sektorów
	while (currentFileSector && charsToRead)
	{
		disk.readSector(currentFileSector, fileSector);
		for (int j = 0; j < 31 && charsToRead; j++)
		{
			ActiveProcess->currentFile->fileContent += fileSector[j];
			charsToRead--;
		}
		currentFileSector = fileSector[31];
	}
	//
	currentFileSector = currentFileFirst;
	return ActiveProcess->currentFile->fileContent;
}

void ChaOS_filesystem::appendFile(const std::string& text)
{
	if (ActiveProcess->currentFile == nullptr)
	{
		ActiveProcess->errorCode = 5;
		return;
	}
	currentFileFirst = ActiveProcess->currentFile->firstSector;

	char fileSector[32], VCB[32];
	disk.readSector(0, VCB);
	unsigned int charsToWrite = text.size();

	if ((float(charsToWrite) / 31) + float(ActiveProcess->currentFile->fileSize) > float(VCB[8]) + float(ActiveProcess->currentFile->fileSizeInSectors))
	{
		//throw outOfMemory();
		ActiveProcess->errorCode = 2;
		return;
	}
	if (int(charsToWrite) + int(ActiveProcess->currentFile->fileSize) > 255)
	{
		charsToWrite = 255 - int(ActiveProcess->currentFile->fileSize);
	}

	currentFileSector = currentFileFirst;

	// Przepisywanie zawartość pliku do sektora

	while (currentFileSector && charsToWrite)
	{
		disk.readSector(ActiveProcess->currentFile->firstSector, fileSector);
		fileSector[0] += charsToWrite;
		disk.writeSector(ActiveProcess->currentFile->firstSector, fileSector);

		for (int i = 1; i < ActiveProcess->currentFile->fileSizeInSectors; i++)
		{
			disk.readSector(currentFileSector, fileSector);
			currentFileSector = fileSector[31];
		}

		disk.readSector(currentFileSector, fileSector);
		int pozycja = (ActiveProcess->currentFile->fileSize + 1) % 31;

		disk.readSector(currentFileSector, fileSector);
		for (int j = pozycja; j < 31 && charsToWrite; j++)
		{
			fileSector[j] = text[ActiveProcess->currentFile->fileContent.size() - charsToWrite];
			charsToWrite--;
		}

		if (charsToWrite > 0 && fileSector[31] == 0) //przydział nowego sektora
		{
			ActiveProcess->currentFile->fileSizeInSectors++;
			fileSector[31] = allocateSector();
		}
		disk.writeSector(currentFileSector, fileSector);

		currentFileSector = fileSector[31];
		pozycja = 0;
	}

	ActiveProcess->currentFile->fileContent.erase(0, 1);

	currentFileSector = currentFileFirst;
	//////////////////////////////////////////////////////////

	// Aktualizacja wpisu w aktualnym katalogu (długość pliku)
	currentDirFirst = ActiveProcess->currentFile->fileDir;
	currentDirSector = currentDirFirst;

	char row[8] = {};
	char dirSector[32] = {};

	disk.readSector(currentDirSector, dirSector);

	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);
		for (int i = 0, j = 0; i < 3; i++, j += 8)
		{

			getRow(&dirSector[j], row);
			if (equalName(ActiveProcess->currentFile->filename, row))
			{
				row[6] = ActiveProcess->currentFile->fileSizeInSectors;
				addRow(&dirSector[j], row);
			}
		}
		disk.writeSector(currentDirSector, dirSector);
		currentDirSector = dirSector[31];
	}
	//
	currentDirSector = currentDirFirst;
	ActiveProcess->currentDir = currentDirFirst;
}

void ChaOS_filesystem::saveFile()
{
	if (ActiveProcess->currentFile == nullptr)
	{
		ActiveProcess->errorCode = 5;
		return;
	}
	currentFileFirst = ActiveProcess->currentFile->firstSector;

	char fileSector[32], VCB[32];
	disk.readSector(0, VCB);
	unsigned int charsToWrite = ActiveProcess->currentFile->fileSize + 1;

	if ((float(charsToWrite) / 31) > float(VCB[8]) + float(ActiveProcess->currentFile->fileSizeInSectors))
	{
		//throw outOfMemory();
		ActiveProcess->errorCode = 2;
		return;
	}

	currentFileSector = currentFileFirst;
	ActiveProcess->currentFile->fileContent.insert(ActiveProcess->currentFile->fileContent.begin(), ActiveProcess->currentFile->fileSize);
	ActiveProcess->currentFile->fileSizeInSectors = 0;

	// Przepisywanie zawartość pliku do sektora
	while (currentFileSector && charsToWrite)
	{
		ActiveProcess->currentFile->fileSizeInSectors++;
		disk.readSector(currentFileSector, fileSector);
		for (int j = 0; j < 31 && charsToWrite; j++)
		{
			fileSector[j] = ActiveProcess->currentFile->fileContent[ActiveProcess->currentFile->fileContent.size() - charsToWrite];
			charsToWrite--;
		}

		if (charsToWrite > 0 && fileSector[31] == 0) //przydział nowego sektora
		{
			fileSector[31] = allocateSector();
		}

		if (!charsToWrite && fileSector[31]) //zwalnianie niepotrzebnych sektorów
		{
			char sector[32];
			char currentSectorToFree = fileSector[31];
			char nextToFree;
			while (currentSectorToFree)
			{
				disk.readSector(currentSectorToFree, sector);
				nextToFree = sector[31];
				sector[31] = 0;
				disk.writeSector(currentSectorToFree, sector);
				freeSector(currentSectorToFree);
				currentSectorToFree = nextToFree;
			}
			fileSector[31] = 0;
		}

		disk.writeSector(currentFileSector, fileSector);

		currentFileSector = fileSector[31];
	}

	ActiveProcess->currentFile->fileContent.erase(0, 1);

	currentFileSector = currentFileFirst;
	//////////////////////////////////////////////////////////

	// Aktualizacja wpisu w aktualnym katalogu (długość pliku)
	currentDirFirst = ActiveProcess->currentFile->fileDir;
	currentDirSector = currentDirFirst;

	char row[8] = {};
	char dirSector[32] = {};

	disk.readSector(currentDirSector, dirSector);

	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);
		for (int i = 0, j = 0; i < 3; i++, j += 8)
		{

			getRow(&dirSector[j], row);
			if (equalName(ActiveProcess->currentFile->filename, row))
			{
				row[6] = ActiveProcess->currentFile->fileSizeInSectors;
				addRow(&dirSector[j], row);
			}
		}
		disk.writeSector(currentDirSector, dirSector);
		currentDirSector = dirSector[31];
	}
	//
	currentDirSector = currentDirFirst;
	ActiveProcess->currentDir = currentDirFirst;
}

void ChaOS_filesystem::closeFile()
{
	if (ActiveProcess->currentFile) delete ActiveProcess->currentFile;
	ActiveProcess->currentFile = nullptr;
	fileSynchronization[currentFileFirst].signal();
}


// do kontroli dysku 
// Zwraca string opisujący zadany sektor
std::string  ChaOS_filesystem::printSector(const unsigned short number)
{
	if (number >= disk.numberOfSectors)
	{
		//throw objectNotFound();
		ActiveProcess->errorCode = 4;
		return "";
	}

	std::ostringstream result;
	char sector[32];

	disk.readSector(number, sector);
	result << "_ _ SEKTOR: " << number << " _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ " << std::endl;

	for (int i = 0; i < 4; i++)
	{
		for (int j = i * 8; j < i * 8 + 8; j++)
		{
			result << " " << sector[j];
		}
		result << "   ";
		for (int j = i * 8; j < i * 8 + 8; j++)
		{
			result << std::setfill(' ') << std::setw(3) << ((uShort(~0) >> 8) & (sector[j])) << " ";
		}
		result << "\n";
	}
	return result.str();
}

// Zwraca string opisujący łańcuch sektorów zaczynając od 'first'
std::string ChaOS_filesystem::printSectorsChain(const unsigned short first)
{
	std::string result;
	char sector[32];
	disk.readSector(first, sector);
	char current = first;
	while (current)
	{
		disk.readSector(current, sector);
		result += printSector(current);
		current = sector[31];
	}
	return result;
}

// Zwraca string opisujący stan dysku
std::string ChaOS_filesystem::printDiskStats()
{
	std::ostringstream result;
	char VCB[32];
	char freeSectorCount;
	unsigned int freeSectorBitVector;

	disk.readSector(0, VCB);
	freeSectorCount = VCB[8];

	char_int temp; temp.CHAR[3] = VCB[16]; temp.CHAR[2] = VCB[17]; temp.CHAR[1] = VCB[18]; temp.CHAR[0] = VCB[19];//char[4] to int
	freeSectorBitVector = temp.INT;

	result << "FreeSectorCount: " << unsigned int(freeSectorCount) << "/" << disk.numberOfSectors << "   " << asBitVector(freeSectorBitVector) << std::endl;
	return result.str();
}

// Wpisuje wiersz w okreslone miejsce w sektorze
void ChaOS_filesystem::addRow(char * sector, char* row)
{
	for (int i = 0; i < 8; i++)
	{
		sector[i] = row[i];
	}
}

// Pobiera wiersz z określonego miejsca w sektorze
void ChaOS_filesystem::getRow(char * sector, char* row)
{
	for (int i = 0; i < 8; i++)
	{
		row[i] = sector[i];
	}
}

// Porównuje dwie nazwy zapisane w char[5]
bool ChaOS_filesystem::equalName(const char * n1, const char * n2)
{
	for (int i = 0; i < 5; i++)
	{
		if (n1[i] != n2[i]) return false;
	}
	return true;
}

// Określa długość tablicy char*
uShort ChaOS_filesystem::charArrSize(const char * arr)
{
	uShort result = 0;
	for (int i = 0; arr[i] != 0; i++)
	{
		result++;
	}
	return result;
}

// Przycina(/rozszerza) zawartość char* tak by zmieściła się w char[5]
void ChaOS_filesystem::toChar5(const char * arr, char* result5)
{
	for (int i = 0; i < 5; i++)
	{
		result5[i] = 0;
	}
	int size = charArrSize(arr) < 5 ? charArrSize(arr) : 5;
	for (int i = 0; i < size; i++)
	{
		result5[i] = arr[i];
	}
}

//Przedstawia int w postaci wektora bitów (zapisane w string)
std::string ChaOS_filesystem::asBitVector(const int vector)
{
	std::string result = "{0}";
	int copy = vector;
	for (int i = 0; i < sizeof(int) * 8; i++)
	{
		result += ((copy & 1) ? 176 : 178);
		copy >>= 1;
	}
	return result += "{31}";
}
