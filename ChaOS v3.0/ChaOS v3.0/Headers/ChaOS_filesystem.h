﻿#pragma once

#include "../Headers/disk_drive.h"
#include "../Headers/file.h"
#include "../Headers/ChaOS_filesystem_exception.h"
#include "stack"

class ChaOS_filesystem final
{
public:
	enum class type { file = 'F', dir = 'D' };

	ChaOS_filesystem();
	~ChaOS_filesystem();

	// Tworzy obiekt w aktualnym folderze
		void create(const char* name, type t);

	// Usuwa obiekt z aktualnego folderu
		void remove(const char* name); 
	
	// Zwraca string opisujący zawartość aktualnego folderu
		std::string listDirectory();

	// Zmiania aktualny folder na wskazany folder
		void changeDirectory(const char* name); 

	// zmiania aktualny folder na wskazany folder
		void backDirectory(); 

	// Zmiania aktualny folder na wskazany folder
		void rootDirectory(); 

	// Zmienia nazwę obiektu w aktualnym folderze
		void rename(const char* name, const char* newname);
	
	// Szuka pliku/katalogu w aktualnym katalogu. Zwraca adres pierwszego sektora szukanego pliku/katalogu lub 0, gdy nie istnieje
		uShort search(const char* name, type t); 

	// Otwiera plik o wskazanej nazwie
		void openFile(const char* filename); 

	// Zapisuje otwarty plik
		void saveFile(); 

	// Zwalnia uchwyt currentFile
		void closeFile(); 

	// Zwraca string opisujący zadany sektor
		std::string printSector(const unsigned short number);

	// Zwraca string opisujący łańcuch sektorów zaczynając od 'first'
		std::string printSectorsChain(const unsigned short first);

	// Zwraca string opisujący stan dysku
		std::string printDiskStats();

	file* currentFile;
private:
	c_uShort allocateSector();
	void freeSector(uShort number);

	// Przechowują "wskaźniki" na plik/katalog
	uShort currentFileFirst;
	uShort currentFileSector;
	uShort currentDirFirst;
	uShort currentDirSector;
	uShort rootDirSector;

	disk_drive disk;

	std::stack<uShort> returnPath;

	void addRow(char* sector, char* row);
	void getRow(char* sector, char* row);
	bool equalName(const char* n1,const char* n2);
	uShort charArrSize(const char* arr);
	void toChar5(const char* arr,char* result5);
	std::string asBitVector(const int vector);
};

