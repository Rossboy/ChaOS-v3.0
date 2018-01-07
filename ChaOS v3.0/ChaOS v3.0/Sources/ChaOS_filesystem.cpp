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
	disk.loadFromFile();
}

ChaOS_filesystem::~ChaOS_filesystem()
{
	closeFile();
}


uShort ChaOS_filesystem::getRootDir()
{
	return rootDirSector;
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
	currentDirSector = currentDirFirst = ActiveProcess->currentDir;


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
				else
				{
					sector[1] = 1;
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

	if (dir)
	{
		if (!isDirEmpty(dir))
		{
			ActiveProcess->errorCode = 11;
			return;
		}
	}

	// Wyszukiwanie pierwszego sektora do zwolnienia
	//int currentSectorToFree = search(name, type::file) != 0 ? search(name, type::file) : search(name, type::dir);
	int currentSectorToFree = file != 0 ? file : dir;

	// Zwalnianie kolejnych sektorów
	char dirSector[32] = {};
	do {
		disk.readSector(currentSectorToFree, dirSector);
		freeSector(currentSectorToFree);
		currentSectorToFree = dirSector[31];
	} while (dirSector[31]);
	//

	char row[8] = {};
	currentDirSector = currentDirFirst = ActiveProcess->currentDir;

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
	currentDirSector = currentDirFirst = ActiveProcess->currentDir;
	//std::cout << "list dir: " << currentDirFirst << std::endl;

	std::ostringstream result;
	char dirSector[32];
	disk.readSector(ActiveProcess->currentDir, dirSector);
	result << "\n+----------------------------+-----------+---------+\n" << "|";

	result << " Current directory is ";
	for (int i = 24; i < 29; i++)
	{
		result << dirSector[i];
	}
	result << " | First: " << std::left << std::setw(2) << unsigned short(dirSector[29]) << " | Size: " << std::left << std::setw(2) << unsigned short(dirSector[30]) << "|";
	result << "\n+=========+=========++=======+=+=========+=========+\n" << "| NAME    | TYPE    || FIRST   | CHARS   | SECTORS |\n" << "+---------+---------++---------+---------+---------+\n";

	char row[8];

	// Wypisywanie kolejnych wpisów
	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);
		for (int i = 0, j = 0; i < 3; i++, j += 8)
		{
			if (dirSector[j] != 0)
			{
				getRow(&dirSector[j], row);
				result << "| ";
				for (int i = 0; i < 5; i++)
				{
					result << row[i];
				}
				char fileSector[32];
				disk.readSector(row[5], fileSector);
				result << "   | " << row[7] << "       || " << std::left << std::setw(2) << int(row[5]) << "      | ";
				if (row[7] == char(type::file))
				{
					result << std::left << std::setw(3) << (((unsigned int)fileSector[0]) & 255) << "     | " << std::left << std::setw(2) << (((unsigned int)fileSector[1]) & 255) << "      |";
				}
				else
				{
					char sector[32];
					disk.readSector(row[5], sector);
					result << "        | " << (((unsigned int)fileSector[30]) & 255) << "       |";
				}
				//result << "" << (unsigned int)dirSector[5 + j] << "  |" << ((dirSector[7 + j] == char(type::dir)) ? ' ' : char(dirSector[6 + j] + 48)) << "\t " << dirSector[7 + j] << std::endl;
				result << "\n+---------+---------++---------+---------+---------+\n";
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
	int file = search(name, type::file);
	// czy obiekt istnieje
	if (!(file || search(name, type::dir)))
	{
		//throw objectNotFound();
		ActiveProcess->errorCode = 4;
		return;
	}

	// czy nowa nazwa nie jest już zajęta
	char newn[5]; toChar5(newname, newn);
	if (search(newn, type::file) || search(newn, type::dir))
	{
		ActiveProcess->errorCode = 3;
		return;
	}

	if (file)
	{
		// proces nie ma otwartego pliku
		if (!ActiveProcess->currentFile)
		{
			ActiveProcess->errorCode = 5;
			return;
		}

		//czy otwarty plik to ten do zmiany nazwy
		//if (!equalName(ActiveProcess->currentFile->filename, name))
		if (ActiveProcess->currentFile->firstSector != file)
		{
			ActiveProcess->errorCode = 6;
			return;
		}
	}

	currentDirSector = currentDirFirst = ActiveProcess->currentDir;

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

	currentDirSector = currentDirFirst = ActiveProcess->currentDir;

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

	////filesizeInsectors
	//ActiveProcess->currentFile->fileSizeInSectors = sizeInSectors;

	//firstector
	ActiveProcess->currentFile->firstSector = currentFileFirst;

	//read firstsector
	char fileSector[32];
	disk.readSector(currentFileFirst, fileSector);
	ActiveProcess->currentFile->fileSize = fileSector[0];
	//filesizeInsectors
	ActiveProcess->currentFile->fileSizeInSectors = fileSector[1];

	ActiveProcess->currentFile->fileDir = currentDirFirst;

	ActiveProcess->currentDir = this->currentDirFirst;

	fileSynchronization[currentFileFirst].wait(ActiveProcess);
	//std::cout << "FS LOG: Open: file: " << currentFileFirst << std::endl;
}

void ChaOS_filesystem::writeFile(const std::string& text)
{
	if (ActiveProcess->currentFile == nullptr)
	{
		ActiveProcess->errorCode = 5;
		return;
	}
	currentFileFirst = ActiveProcess->currentFile->firstSector;

	//ActiveProcess->currentFile->fileContent = text;
	saveFile(text);
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
	ActiveProcess->currentFile->fileSizeInSectors = fileSector[1];
	std::string result;
	// Odczyt pierwszego sektora
	for (int j = 2; j < 31 && charsToRead; j++)
	{
		result += fileSector[j];
		charsToRead--;
	}
	currentFileSector = fileSector[31];

	// Odczyt kolejnych sektorów
	while (currentFileSector && charsToRead)
	{
		disk.readSector(currentFileSector, fileSector);
		for (int j = 0; j < 31 && charsToRead; j++)
		{
			result += fileSector[j];
			charsToRead--;
		}
		currentFileSector = fileSector[31];
	}
	//
	currentFileSector = currentFileFirst;
	return result;
}


void ChaOS_filesystem::appendFile(const std::string& text)
{
	if (ActiveProcess->currentFile == nullptr)
	{
		ActiveProcess->errorCode = 5;
		return;
	}
	currentFileFirst = ActiveProcess->currentFile->firstSector;

	char fileSector[32], VCB[32], fileSector3[32];
	disk.readSector(0, VCB);
	unsigned int charsToWrite = text.size();

	disk.readSector(currentFileFirst, fileSector3);
	char sectors = ActiveProcess->currentFile->fileSizeInSectors = fileSector3[1];
	char chars = ActiveProcess->currentFile->fileSize = fileSector3[0];
	auto stringToWrite = text;

	//czy tekst się zmieści
	if ((unsigned int(charsToWrite) & 255) + (unsigned int(chars) & 255) > 255)
	{
		ActiveProcess->errorCode = 12;
		return;
	}

	currentFileSector = currentFileFirst;

	//wyczytanie odpowieniego sektora
	if (sectors > 1)
	{
		for (int i = 1; i < sectors; i++)
		{
			currentFileSector = fileSector3[31];
			disk.readSector(currentFileSector, fileSector3);
		}
	}

	//oblicznie przesunięcia
	unsigned int pozycja = (unsigned int(chars) + 2u + unsigned int(sectors) - 1u);

	//przepisanie do początkowego sektora sektora
	pozycja %= 32;
	for (unsigned int i = pozycja; i < 31 && charsToWrite; i++)
	{
		fileSector3[i] = stringToWrite.at(0);
		stringToWrite.erase(0, 1);
		charsToWrite--;
	}
	//przydział nowego sektora
	if (charsToWrite && fileSector3[31] == 0)
	{
		fileSector3[31] = allocateSector();
		if (fileSector3[31] == 0)
		{
			ActiveProcess->errorCode = 2;
			return;
		}
	}
	disk.writeSector(currentFileSector, fileSector3);


	currentFileSector = fileSector3[31];

	///////////////^^^^^^^^^^^^^^^^^^^^^

	char fileSector2[32];
	// Przepisywanie zawartość pliku do następnego sektora
	while (currentFileSector && charsToWrite)
	{

		sectors++;
		disk.readSector(currentFileSector, fileSector2);
		for (int j = 0; j < 31 && charsToWrite; j++)
		{
			fileSector2[j] = stringToWrite.at(0);
			stringToWrite.erase(0, 1);
			charsToWrite--;
		}

		if (charsToWrite > 0 && fileSector2[31] == 0) //przydział nowego sektora
		{
			fileSector2[31] = allocateSector();
			if (fileSector2[31] == 0)
			{
				ActiveProcess->errorCode = 2;
				return;
			}
		}

		disk.writeSector(currentFileSector, fileSector2);

		currentFileSector = fileSector2[31];
	}
	disk.readSector(currentFileFirst, fileSector2);
	fileSector2[1] = ActiveProcess->currentFile->fileSizeInSectors = sectors;
	fileSector2[0] = ActiveProcess->currentFile->fileSize = chars + text.size();
	disk.writeSector(currentFileFirst, fileSector2);

	currentFileSector = currentFileFirst;

	//////////////////////////////////////////////////////////

	// Aktualizacja wpisu w aktualnym katalogu (długość pliku)
	currentDirFirst = ActiveProcess->currentFile->fileDir;
	currentDirSector = currentDirFirst = ActiveProcess->currentDir;

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

void ChaOS_filesystem::saveFile(const std::string& text)
{
	//fileInfoSynch();
	if (ActiveProcess->currentFile == nullptr)
	{
		ActiveProcess->errorCode = 5;
		return;
	}
	currentFileFirst = ActiveProcess->currentFile->firstSector;

	char fileSector[32], VCB[32];
	disk.readSector(0, VCB);
	unsigned int charsToWrite = text.size() + 2;
	ActiveProcess->currentFile->fileSize = charsToWrite - 2;

	disk.readSector(currentFileFirst, fileSector);
	auto sectors = ActiveProcess->currentFile->fileSizeInSectors = fileSector[1];

	if ((float(charsToWrite) / 31) > float(VCB[8]) + float(sectors))
	{
		//throw outOfMemory();
		ActiveProcess->errorCode = 2;
		return;
	}

	currentFileSector = currentFileFirst;
	auto stringToWrite = text;
	stringToWrite.insert(stringToWrite.begin(), ActiveProcess->currentFile->fileSizeInSectors);
	stringToWrite.insert(stringToWrite.begin(), stringToWrite.size());
	ActiveProcess->currentFile->fileSizeInSectors = 0;

	// Przepisywanie zawartość pliku do sektora
	while (currentFileSector && charsToWrite)
	{
		ActiveProcess->currentFile->fileSizeInSectors++;
		disk.readSector(currentFileSector, fileSector);
		for (int j = 0; j < 31 && charsToWrite; j++)
		{
			fileSector[j] = stringToWrite[stringToWrite.size() - charsToWrite];
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
	disk.readSector(currentFileFirst, fileSector);
	fileSector[1] = ActiveProcess->currentFile->fileSizeInSectors;
	fileSector[0] = ActiveProcess->currentFile->fileSize;
	disk.writeSector(currentFileFirst, fileSector);

	currentFileSector = currentFileFirst;
	//////////////////////////////////////////////////////////

	// Aktualizacja wpisu w aktualnym katalogu (długość pliku)
	currentDirFirst = ActiveProcess->currentFile->fileDir;
	currentDirSector = currentDirFirst = ActiveProcess->currentDir;

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
	if (ActiveProcess)
	{
		if (ActiveProcess->currentFile)
		{
			//std::cout << "FS LOG: CLOSE: file: " << ActiveProcess->currentFile->firstSector << std::endl;
			fileSynchronization[ActiveProcess->currentFile->firstSector].signal();
			delete ActiveProcess->currentFile;
		}
		ActiveProcess->currentFile = nullptr;
	}
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
	result << "  _ _ SEKTOR: " << std::setw(2) << number << " _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ " << std::endl;
	for (int i = 0; i < 4; i++)
	{
		result << "  ";
		for (int j = i * 8; j < i * 8 + 8; j++)
		{
			result << " " << check(sector[j]);
		}
		result << "   ";
		for (int j = i * 8; j < i * 8 + 8; j++)
		{
			result << std::setfill(' ') << std::setw(3) << ((uShort(~0) >> 8) & (sector[j])) << " ";
		}
		result << "\n";
	}
	result << "\n";
	return result.str();
}

// Zwraca string opisujący łańcuch sektorów zaczynając od 'first'
std::string ChaOS_filesystem::printSectorsChain(const unsigned short first)
{
	std::string result;
	char sector[32];
	disk.readSector(first, sector);
	char current = first;

	if (current == 0)
	{
		result = printSector(0);
		return result;
	}

	if (current == 32)
	{
		for (int i = 0; i < 32; i++)
		{
			result += printSector(i);
		}
		return result;
	}

	if (current == 100)
	{
		clearDisk();
		return ("[Format OK -> Disk is empty]\n" + printDiskStats());

	}

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
	result << "     Sektory: " << std::endl;
	result << "     " << asBitVector(freeSectorBitVector) << std::endl << std::endl;;
	result << "     Liczba wolnych sektorów: " << unsigned int(freeSectorCount) << "/" << disk.numberOfSectors << std::endl;
	result << "     Wolne: " << setBitNumber(freeSectorBitVector) << std::endl<<std::endl;;
	result << "     Liczba zajętych sektorów: " << unsigned int(disk.numberOfSectors - freeSectorCount) << "/" << disk.numberOfSectors << std::endl;
	result << "     Zajęte: " << setBitNumber(~freeSectorBitVector) << std::endl<< std::endl;
	result << "     Zmienne warunkowe - otwarte pliki (ID): ";
	for (int i = 0; i < 32; i++)
	{
		if (fileSynchronization[i].getResourceOccupied())
			result << i << " ";
	}
	result << std::endl;
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

//zastępuje znaki formatujące przez '.'
char ChaOS_filesystem::check(const char& c)
{
	if (c == 7 || c == 8 || c == 9 || c == 10 || c == 13) return '.';
	return c;
}

bool ChaOS_filesystem::isDirEmpty(uShort dir)
{
	char row[8] = {};
	char dirSector[32] = {};

	currentDirSector = dir;

	while (currentDirSector)
	{
		disk.readSector(currentDirSector, dirSector);

		for (int i = 0; i < 24; i += 8)
		{
			getRow(dirSector, row);
			if (row[0] != 0) return 0;
		}
		currentDirSector = dirSector[31];
	}

	currentDirSector = currentDirFirst;
	return 1;
}

void ChaOS_filesystem::clearDisk()
{
	disk.clear();
	char_short temp; temp.USHORT = disk.diskSize;
	char_int temp2; temp2.INT = (~0);
	setBit(temp2.CHAR[0], 0, 0);
	char VCB[32] = { temp.CHAR[1], temp.CHAR[0], static_cast<char>(disk.sectorSize), static_cast<char>(disk.numberOfSectors),		0,0,0,0,
		static_cast<char>(disk.numberOfSectors - 1),		0,0,0,0,0,0,0,
		temp2.CHAR[3], temp2.CHAR[2], temp2.CHAR[1], temp2.CHAR[0],			0,0,0,0,
		0,0,0,0, 0,0,0,0 };
	disk.writeSector(0, VCB);
	rootDirSector = allocateSector();
	char sector[32] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 'R', 'O', 'O', 'T', '\0', rootDirSector, 1, 0 };
	disk.writeSector(rootDirSector, sector);
	currentDirFirst = rootDirSector;
	currentDirSector = rootDirSector;
}

std::string ChaOS_filesystem::setBitNumber(const int vector)
{
	std::ostringstream res;
	for (int i = 0; i < disk.numberOfSectors; i++)
	{
		if ((vector >> i) & 1) res << i << " ";
	}
	return res.str();
}