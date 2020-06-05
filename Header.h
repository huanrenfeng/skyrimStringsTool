#pragma once
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unordered_map>
using namespace std;

struct Entry {
	uint32_t id;
	uint32_t offset;
};

struct stringsFile {
	uint32_t count;
	uint32_t datasize;
	Entry* directory;
	char* data;
};

struct longString {
	uint32_t length;
	char* str;
};

void fix();
void fixF(const char* fn);
void error();
void step1();
void step2();
void step2short();
void getStringType();

void interpFile(stringsFile* sf);
stringsFile* readFile(const char* fn);
void outputIndexFile(stringsFile* sf);
void trimFileShort(stringsFile* sf);
void trimFileLong(stringsFile* sf);

#include <windows.h> 

void findFile(char* str, vector<char*>* names) {
	HANDLE hFind;
	WIN32_FIND_DATA FindData;


	hFind = FindFirstFile(str, &FindData);


	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	do
	{
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		char* name = (char*)malloc(strlen(FindData.cFileName) + 1);
		strcpy(name, FindData.cFileName);
		names->push_back(name);
	} while (FindNextFile(hFind, &FindData));

	FindClose(hFind);
}
