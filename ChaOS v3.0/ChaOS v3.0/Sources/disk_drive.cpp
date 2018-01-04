#include "../Headers/disk_drive.h"
#include "iostream"
#include "fstream"


disk_drive::disk_drive()
	: diskSize(1024), sectorSize(32), numberOfSectors(diskSize / sectorSize)
{
	diskSpace = new char[diskSize];
	clear();
}

disk_drive::~disk_drive()
{
	std::fstream file;
	file.open("disk.ChaOS_FS", std::ios::out | std::ios::trunc | std::ios::binary);
	if (file.good())
	{
		file.write(diskSpace, 1024);
		std::cout << "[disk content saved]" << std::endl;
	}
	file.close();

	delete[diskSize] diskSpace;
}

void disk_drive::readSector(c_uShort sectorNumber, char sector[])
{
	for (int i = sectorNumber*sectorSize, j = 0; i < (sectorSize*sectorNumber + sectorSize); i++, j++)
	{
		sector[j] = diskSpace[i];
	}
}

void disk_drive::writeSector(c_uShort sectorNumber, char sector[])
{
	for (int i = sectorNumber*sectorSize, j = 0; i < (sectorSize*sectorNumber + sectorSize); i++, j++)
	{
		diskSpace[i] = sector[j];
	}
}

void disk_drive::clear()
{
	for (int i = 0; i < diskSize; i++)
	{
		diskSpace[i] = 0x0;
	}
}

void disk_drive::loadFromFile()
{
	std::fstream file;
	file.open("disk.ChaOS_FS", std::ios::in | std::ios::binary);
	if (file.good())
	{
		file.read(diskSpace, 1024);
		file.close();
	}
	else
	{
		std::clog << "[cannot open disk file -> ChaOS_FS disk is empty]" << std::endl;
	}
}
