#include "../Headers/disk_drive.h"
#include "iostream"


disk_drive::disk_drive()
	: diskSize(1024), sectorSize(32), numberOfSectors(diskSize / sectorSize)
{
	diskSpace = new char[diskSize];
	clear();
}

disk_drive::~disk_drive()
{
	delete[diskSize] diskSpace;
}

void disk_drive::readSector(c_uShort sectorNumber, char sector[])
{
	for (int i = sectorNumber*sectorSize, j=0; i < (sectorSize*sectorNumber + sectorSize); i++, j++)
	{
		sector[j] = diskSpace[i];
	}
}

void disk_drive::writeSector(c_uShort sectorNumber, char sector[])
{
	for (int i = sectorNumber*sectorSize, j=0; i < (sectorSize*sectorNumber+sectorSize); i++, j++)
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