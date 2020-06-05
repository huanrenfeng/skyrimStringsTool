#define _CRT_SECURE_NO_WARNINGS
#include "Header.h"




const char* filename = "BeingADoggy_huan_french.ILSTRINGS";

int stringType = 1;

char **argvs;

//-swap,read transed.txt and output to filename specified by -f. no swap: output a totrans.txt file. 
//-f filename
int main(int argc, char *argv[]) {
	argvs = argv;

	bool shouldSwap = false;
	bool shouldFixBlockErr = false;

	if (argc > 1) {
		
		for (int i = 1; i < argc; i++) {
			//printf("%s\n", argv[i]);
			if (strcmp(argv[i], "-swap") == 0) {
				shouldSwap = true;
			}else if (strcmp(argv[i], "-f") == 0) {
				filename = argv[++i];
			}
			else if (strcmp(argv[i], "-fix") == 0) {
				shouldFixBlockErr = true;
			}
		}
	}

	if (shouldFixBlockErr) {
		fix();
		return 0;
	}
		
	getStringType();
	//printf("strType,%d\n", stringType);
	//getchar();


	if (shouldSwap)
		printf("shoudlswap\n");

	if (shouldSwap)
		step2();
	else
		step1();

	printf("press Enter to exit");
	getchar();
	return 0;
}

void getStringType() {
	int tail = strlen(filename)-1;

	char last;

	for (int i = tail; i >= 0; i--) {
		char c = filename[i];
		
		if (c == '.') {
			break;
		}
		last = c;
	}

	if (last == 's' || last == 'S')
		stringType = 0;
	else
		stringType = 1;
}

void step1(){
	

	auto sf = readFile(filename);
	interpFile(sf);
	outputIndexFile(sf);
}

void error() {
	printf("Something went wrong.\n-swap,read transed.txt and output to filename specified by -f. no swap: output a totrans.txt file.\n-f filename\n-fix: fix blocks for wrong ilstrings and dlstrings in the current directory.");
	getchar();
	exit(1);
}

//remove unused strings from data.
void trimFile(stringsFile* sf) {
	if (stringType == 0) {
		trimFileShort(sf);
	}
	else if (stringType == 1) {
		trimFileLong(sf);
	}
}

void trimFileLong(stringsFile* sf){

	vector<longString*> stringVector;
	vector<int> offsetVector;
	unordered_map<int, int> offsetIndexMap;		//offset to index

	int sl, index = 0, curoffset = 0;

	while (curoffset < sf->datasize) {

		longString* curstr = new longString;// (longString*)(sf->data + curoffset);
		curstr->length = *(uint32_t*)(sf->data + curoffset);
		curstr->str = sf->data + curoffset +4;	

		sl = strlen(curstr->str);

		if (sl+1 != curstr->length) {
			printf("trimtErr %d %d %d %d %s\n", index, sl, curstr->length, curoffset, curstr->str);
			error();
		}

		//curstr->length = strlen(curstr->str)+1;

		stringVector.push_back(curstr);

		offsetVector.push_back(curoffset);

		offsetIndexMap[curoffset] = index++;

		curoffset += 4 + sl + 1;
	}

	unordered_map<int, int> idIndexMap;

	for (int i = 0; i < sf->count; i++) {
		Entry e = sf->directory[i];
		idIndexMap[e.id] = offsetIndexMap[e.offset];

	}

	unordered_map<int, bool> validIndexMap;

	for (int i = 0; i < sf->count; i++) {
		validIndexMap[offsetIndexMap[sf->directory[i].offset]] = true;
	}

	if (validIndexMap.size() == offsetVector.size()) {
		return;
	}

	int newsize = 0;
	char* newdata;

	for (int j = 0; j < index; j++) {
		if (validIndexMap[j] == true) {
			newsize += 4 + stringVector[j]->length;
		}
	}

	newdata = (char*)malloc(newsize);
	curoffset = 0;
	for (int j = 0; j < index; j++) {


		if (validIndexMap[j] == true) {
			int lenth = 4 + stringVector[j]->length;
			offsetVector[j] = curoffset;
			memcpy(newdata + curoffset, &stringVector[j]->length, 4);
			memcpy(newdata + curoffset + 4, stringVector[j]->str, lenth - 4);
				
			curoffset += lenth;
		}


	}

	free(sf->data);
	sf->data = newdata;
	sf->datasize = newsize;

	for (int i = 0; i < sf->count; i++) {
		sf->directory[i].offset = offsetVector[idIndexMap[sf->directory[i].id]];
	}

	
}

void trimFileShort(stringsFile* sf) {
	vector<char*> stringVector;
	vector<int> offsetVector;
	unordered_map<int, int> offsetIndexMap;		//offset to index

	int sl, index = 0, curoffset = 0;

	while (curoffset < sf->datasize) {

		char* curstr = sf->data + curoffset;

		sl = strlen(curstr);
		stringVector.push_back(curstr);
		

		offsetVector.push_back(curoffset);

		offsetIndexMap[curoffset] = index++;

		curoffset += sl + 1;
	}

	unordered_map<int, int> idIndexMap;

	for (int i = 0; i < sf->count; i++) {
		Entry *e = &sf->directory[i];
		idIndexMap[e->id] = offsetIndexMap[e->offset];
			
	}

	unordered_map<int, bool> validIndexMap;

	for (int i = 0; i < sf->count; i++) {
		validIndexMap[offsetIndexMap[sf->directory[i].offset]] = true;
	}
	if (validIndexMap.size() == offsetVector.size()) {
		return;
	}

	int newsize = 0;
	char* newdata;

	for (int j = 0; j < index; j++) {
		if (validIndexMap[j] == true) {
			newsize += strlen(stringVector[j]) + 1;
		}
	}

	newdata = (char*)malloc(newsize);
	curoffset = 0;
	for (int j = 0; j < index; j++) {
		

		if (validIndexMap[j] == true) {
			int lenth = strlen(stringVector[j]) + 1;
			offsetVector[j] = curoffset;
			memcpy(newdata + curoffset, stringVector[j], lenth);
			curoffset += lenth;
		}

		
	}
	free(sf->data);
	sf->data = newdata;
	sf->datasize = newsize;

	for (int i = 0; i < sf->count; i++) {
		sf->directory[i].offset = offsetVector[idIndexMap[sf->directory[i].id]];
	}


}

void outputIndexFile(stringsFile* sf) {
	FILE* f;
	f = fopen("totrans.txt", "wb");
	if (f == 0) {
		error();
	}

	for (int i = 0; i < sf->count; i++) {
		int id = sf->directory[i].id;
		int offset = sf->directory[i].offset;

		if (stringType == 1) {
			offset += 4;
		}

		fprintf(f, "%d\t%s\n", id, sf->data + offset);

	}
	
	fclose(f);
}

void interpFile(stringsFile* sf) {
	cout << "count " << sf->count << '\n';
	cout << "datasize " << sf->datasize << '\n';

	for (int i = 0; i < sf->count; i++) {
		int offset = sf->directory[i].offset;

		char* str = sf->data + offset;

		if (stringType == 1) {
			str += 4;
		}

		printf("item %d \t offset %d \t %s\n", sf->directory[i].id, offset, str);
	}

	//block data check.

	int curoffset = 0,sl=0;

	if(stringType==1)
		while (curoffset < sf->datasize) {

			longString* curstr = new longString;
			curstr->length = *(uint32_t*)(sf->data + curoffset);
			curstr->str = sf->data + curoffset + 4;

			sl = strlen(curstr->str);

			if (sl + 1 != curstr->length) {
				printf("BlockCheckErr %d %d %d %s\n", sl, curstr->length, curoffset, curstr->str);
				error();
			}

			curoffset += 4 + sl + 1;
		}
}

stringsFile* readFile(const char* fn) {

	FILE* f;
	f = fopen(fn, "rb");

	if (f == 0) {
		error();
	}

	stringsFile* sf = new stringsFile;

	fread(&sf->count, 4, 1, f);
	fread(&sf->datasize, 4, 1, f);

	sf->directory = new Entry[sf->count];

	fread(sf->directory, sizeof(Entry), sf->count, f);

	sf->data = (char*)malloc(sf->datasize);

	fread(sf->data, 1, sf->datasize, f);

	fclose(f);
	return sf;
}

void writeFile(stringsFile* sf,const char* fn) {
	FILE* f;
	f = fopen(fn, "wb");
	if (f == 0) {
		error();
	}

	fwrite(&sf->count, 4, 1, f);
	fwrite(&sf->datasize, 4, 1, f);
	fwrite(sf->directory, sizeof(Entry), sf->count, f);
	fwrite(sf->data, 1, sf->datasize, f);
	fclose(f);
}

void addStringForID(stringsFile* sf,int id, char* str) {
	int newsize = sf->datasize + strlen(str) +1;

	if (stringType == 1) {
		newsize += 4;
	}

	char* newData = (char*)malloc(newsize);

	//update the offset to tail for the corresponding id.
	for (int i = 0; i < sf->count; i++) {
		if (sf->directory[i].id == id) {
			sf->directory[i].offset = sf->datasize;
			break;
		}
	}
	memcpy(newData, sf->data, sf->datasize);
	free(sf->data);

	if(stringType==0)
		memcpy(newData + sf->datasize, str, strlen(str) + 1);
	else if (stringType == 1) {
		uint32_t strlenth = strlen(str) +1 ;
		memcpy(newData + sf->datasize, &strlenth,  4);
		memcpy(newData + sf->datasize +4, str, strlenth);
	}

	sf->datasize = newsize;
	
	sf->data = newData;
}


void step2() {
	if (stringType == 0)
		step2short();
	else {
		stringsFile* sf = readFile(filename);
		

		std::unordered_map<int, int> offsetmap;
		for (int i = 0; i < sf->count; i++) {
			auto e = sf->directory + i;
			offsetmap[e->id] = e->offset;
		}

		vector<longString> stringVector;
		vector<int> offsetVector;

		int sl, curoffset = 0;

		while (curoffset < sf->datasize) {

			longString curstr = {};
			curstr.length= *(uint32_t*)(sf->data + curoffset);
			curstr.str= (char*)(sf->data + curoffset +4);

			sl = strlen(curstr.str);
			stringVector.push_back(curstr);
			
			offsetVector.push_back(curoffset);

			curoffset += 4+sl + 1;
		}

		

		FILE* f;
		f = fopen("transed.txt", "rb");
		if (f == 0) {
			error();
		}

		int id = 0;  char c;
		while (true) {

			char string[2000];
			int r = fscanf(f, "%d\t%2000[^\n]%c", &id, string, &c);		//https://stackoverflow.com/questions/8097620/how-to-read-from-input-until-newline-is-found-using-scanf
			if (r <= 0) {
				break;
			}
			//printf("%d\t%s\n", id, string);
			char* ostring = sf->data + offsetmap[id] +4;

			if (strcmp(ostring, string) != 0) {

				bool has = false;

				for (int i = 0; i < stringVector.size(); i++)
				{
					if (strcmp(stringVector[i].str, string) == 0) {
						sf->directory[id].offset = offsetVector[i];
						has = true;
						break;
					}
				}

				if (!has) {
					addStringForID(sf, id, string);
				}
			}
		}
		fclose(f);
		//interpFile(sf);
		trimFile(sf);
		interpFile(sf);
		writeFile(sf, filename);
	}
}
//use transed.txt to update the original file
void step2short() {
	

	stringsFile* sf = readFile(filename);
	

	std::unordered_map<int, int> offsetmap;
	for (int i = 0; i < sf->count; i++) {
		offsetmap[sf->directory[i].id] = sf->directory[i].offset;
	}

	vector<char*> stringVector;
	vector<int> offsetVector;

	int sl,curoffset=0;

	while(curoffset<sf->datasize) {

		char* curstr = sf->data + curoffset;

		sl = strlen(curstr);
		stringVector.push_back(curstr);
		
		offsetVector.push_back(curoffset);
		curoffset += sl + 1;
	}

	

	FILE* f;
	f = fopen("transed.txt", "rb");
	if (f == 0) {
		error();
	}

	int id = 0; char string[2000]; char c;

	//check for every translated string.
	while (true) {
		
		
		int r = fscanf(f, "%d\t%2000[^\n]%c",&id,string,&c);		//https://stackoverflow.com/questions/8097620/how-to-read-from-input-until-newline-is-found-using-scanf
		if (r <= 0) {
			break;
		}
		printf("%d\t%s\n", id, string);		
		char* ostring = sf->data + offsetmap[id];


		//see if the new string exists.
		if (strcmp(ostring, string) != 0) {

			bool has = false;

			for (int i=0;i<stringVector.size();i++)
			{
				if (strcmp(stringVector[i], string) == 0) {
					sf->directory[id].offset = offsetVector[i];
					has = true;
					break;
				}
			}

			if (!has) {
				addStringForID(sf, id, string);
			}
		}
	}
	fclose(f);
	//interpFile(sf);
	trimFile(sf);
	interpFile(sf);
	writeFile(sf, filename);
}


void fix() {
	vector<char*> names;
	
	char str[1024];
	GetCurrentDirectory(1024, str);
	printf("%s\n", str);
	
	strcat(str, "\\*.ILSTRINGS");

	findFile(str, &names);
	str[strlen(str) - 9] = 'D';
	findFile(str, &names);


	for (int i = 0; i < names.size(); i++) {
		printf("fix %s\n", names[i]);
		fixF(names[i]);
	}

	getchar();

	return;
}

void fixF(const char* fn) {
	auto sf = readFile(fn);

	vector<char*> stringVector;
	vector<int> strlenVector;

	unordered_map<int, int> offsetIndexMap;
	
	int curoffset = 0, sl, index = 0;

	while (curoffset < sf->datasize) {						//read data, store string, generate offsetIndexMap
	
		char* str = sf->data + curoffset + 4;
		sl = strlen(str);
		stringVector.push_back(str);
		strlenVector.push_back(sl);

		//cout << curoffset << " " << index <<endl;

		offsetIndexMap[curoffset] = index++;
	
		//cout << offsetIndexMap[curoffset] << " " << index << endl;

		curoffset += 4 + sl + 1;
	}

	unordered_map<int, int> idIndexMap;

	index = 0;
	do {													//read directory, generate idIndexMap
		auto e = sf->directory + index;
		idIndexMap[e->id] = offsetIndexMap[e->offset];

	} while (++index < sf->count);
	

	int newsize = 0;										//generate newdata
	
	for (int i = 0; i < stringVector.size(); i++) {
		newsize += strlenVector[i] + 5;
	}
	
	char* newdata = (char*)malloc(newsize);
	
	newsize = 0;
	unique_ptr<int[]> newIndexOffsets(new int[stringVector.size()]);			//https://stackoverflow.com/questions/51696838/generic-vector-class-using-unique-ptr-follow-up
	
	for (int i = 0; i < stringVector.size(); i++) {
		char* str = stringVector[i];
	
		uint32_t strl = strlen(str) + 1;
		memcpy(newdata + newsize, &strl, 4);
		memcpy(newdata + newsize + 4, str, strl);
	
		
		newIndexOffsets[i] = newsize;						//store newIndexOffsets

		newsize += 4 + strl;
	}
	
	index = 0;
	do {
		auto e = sf->directory + index;
		e->offset = newIndexOffsets[idIndexMap[e->id]];		//update offset

	}while (++index < sf->count);
	
	free(sf->data);
	sf->data = newdata;
	
	
	writeFile(sf, fn);
}
