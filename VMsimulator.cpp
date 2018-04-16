//Donald Tang and Wenkang Su
//Team Assignment 2
//I pledge my honor I have abided by the Stevens Honor System.

#include "page.h"
#include "process.h"
#include <string>
#include <typeinfo>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <sys/types.h>

#define MEMORY 512

using namespace std;

//contains contents plist, list each process and total memory size for each process
vector<vector<int> > processSize;
vector<vector<int> > trace;
deque<deque<Page*> > mainMemory; // physical memory
vector<Process*> processList;
vector<vector<Page*> > virtualMemory;
int pageSwap = 0;

//main function arguments
//1: plist - contains list of programs being loaded into main memory
//2: ptrace - deterministic series of memory access that emulates a real systems memory usage
//3: size of pages
//4: type of page replacement algorithms
//5: flag to turn on/off pre-paging

//make structures for process, page table that include page frames, each page frame should reference address range
//read in content from pfile and ptrace into an array to determine page table size 

//isInMemory is a helper function that given a processnum and page number, will determine if it is in main memory already
bool isInMemory(int processNum, int pref) {
	int bit = virtualMemory[processNum][pref]->validBit;
	if (bit == 0) {
		return false;
	} 
	return true;
}

//setReplaced0 is a helper function that given a page will set its valid bit back to 0
void setReplaced0(Page* p){
	p->validBit = 0;
}

//non prepaging FIFO algorithm
void prFIFO(int frameSize, int pageSize, int tnum) {
	int pid = 0; 
	int pref = 0;
	//loop through trace vector
	for (int i = 0; i < tnum; i++) {
		cout << "Loop i: " << i << endl;
		pid = trace[i][0];
		pref = trace[i][1];
		//get next page from front element of trace vector
		Page* nextPage = virtualMemory[pid][pref/pageSize];
		if (nextPage->validBit == 0) {
			cout << "Not in Memory" << endl;
			//if there is a free space in main memory, just push
			if (mainMemory[pid].size() < frameSize) {
				mainMemory[pid].push_back(nextPage);
				//nextPage->lastTimeAccessed = timeStamp;
				cout << "Free space in memory" << endl;
			} 
			//if main memory is full, pop the front and push new page to the back
			else {
				cout << "Attempting to replace..." << endl;
				//pop front and push nextPage to end
				setReplaced0(mainMemory[pid].front());
				mainMemory[pid].pop_front();
				mainMemory[pid].push_back(nextPage);
				pageSwap++;
				cout << "Replaced" << endl;
			}
			nextPage->validBit = 1;
		}
	}
}

//non prepaging LRU algorithm
void prLRU(int memSize, int pageSize){
	int pid = 0;
	int pref = 0;
	int index = 0;
	//loop through trace vector
	for(int i = 0; i < trace.size(); i++){
		cout << "loop i = " << i << endl;
		pid = trace[i][0];
		pref = trace[i][1];
		cout << "pid = " << pid << endl;
		//get next page based on next element of trace vector
		Page* nextPage = virtualMemory[pid][pref/pageSize];
		//if there is a page fault
		if(nextPage->validBit == 0){
			cout << "Not in memory." << endl;
			//if there is free space just push nextPage to back
			if (mainMemory[pid].size() < memSize) {
				mainMemory[pid].push_back(nextPage);
				nextPage->validBit = 1;
			} else{
				//if there is no free space swap the front
				mainMemory[pid].front()->validBit = 0;
				mainMemory[pid].pop_front();
				cout << "Pop front done." << endl;
				mainMemory[pid].push_back(nextPage);
				cout << "Push back done." << endl;
				nextPage->validBit = 1;
				cout << "Replace done." << endl;
				pageSwap++;
			}
		}
		//if nextPage is already in memory
		else{
			cout << "In memory." << endl;
			//get index of nextPage in memory, remove, and push to back
			for(int j = 0; j < mainMemory[pid].size(); j++){
				if(mainMemory[pid][j]->pageid == nextPage->pageid){
					index = j;
					break;
				}
			} 
			//push to back to update recently used page
			mainMemory[pid].erase(mainMemory[pid].begin() + index);
			mainMemory[pid].push_back(nextPage);
			cout << "Erase and push done." << endl;
		}
}}

//memsize is the amount of frames allocated to each process in main memory
void prClock (int memSize, int pageSize) {
	int clockHand = 0;
	int pid = 0;
	int pref = 0;
	for (int i = 0; i < trace.size(); i++) {
		cout << "Loop: " << i << endl;
		//determine the next process from trace vector
		pid = trace[i][0];
		//determine the next page from trace vector
		pref = trace[i][1];
		//retrieve page needed 
		Page* nextPage = virtualMemory[pid][pref/pageSize];
		//tmp gets the page of the current clock
		Page* tmp = mainMemory[pid][clockHand];
		//if there is a page fault
		//if page needed is not in memory, we need to push it to memory
		cout << "We have a page fault" << endl;
		if (nextPage->validBit == 0) {
			//if memory has free space, push it to the back
			cout << "Attempting to push to main memory..." << endl;
			if (mainMemory[pid].size() < memSize) {
				mainMemory[pid].push_back(nextPage);
				cout << "Pushed to main memory" << endl;
			}
			//if main memory is full, we need to iterate pointer and replace first frame with R bit = 0
			else {
				cout << "Finding page to replace..." << endl;
				bool foundReplace = false;
				
				//while r bit of tmp is not 0
				while (foundReplace == false) {
					//if we reach one full loop around clock with no bit = 0, we will replace first one
					//if the RBit of the current page inspected is 0, replace it with nextPage
					if(tmp->rBit == 0) {
						cout << "Found page to replace. Replacing now..." << endl;
						setReplaced0(tmp);
						mainMemory[pid][clockHand] = nextPage;
						//mainMemory[pid].assign(clockHand, nextPage);
						nextPage->validBit = 1;
						//rbit does not need to updated, already set to 0
						pageSwap++;
						foundReplace = true;
						cout << "Finished replacing page" << endl;
					}
					//if tmp's bit is 1, reset tmp's bit to 0
					if (tmp->rBit == 1) {
						tmp->rBit = 0;
					}
					//update clockHand and page it is pointing to
					clockHand++;
					if (clockHand == mainMemory[pid].size()) {
						//update clock
						clockHand = 0;
					} 
					tmp = mainMemory[pid][clockHand];
				}
			}
		}
		//if nextPage is already in memory, iterate to the index where page is located and update clockHand to stay at index of nextPage
		else {
			bool foundInMemory = false;
			virtualMemory[pid][pref/pageSize]->rBit = 1;
			//loop through mainMemory to find nextPage
			/*cout << "Finding nextPage in memory..." << endl;
			while (foundInMemory == false) {
				cout << "Test 1" << endl;
				//cout << "Test 2" << endl;
				if (nextPage->pageid == tmp->pageid) {
					cout << "Nextpage: " << nextPage->pageid << " Current Page: " << tmp->pageid << endl;
					foundInMemory = true;
				}
				clockHand++;
				if (clockHand == mainMemory[pid].size()) {
					clockHand = 0;
				}
				//cout << "Clockhand: " << clockHand << " Size: " << mainMemory[pid].size() << endl;
				tmp = mainMemory[pid][clockHand];
			}
			//at this point clock should be pointing to nextPage in memory*/
		}
	}
}

//helper function for prepaging to determine index of next page not in memory
int getSeqPage(int pid, int pref, int pageSize) {
	//cout << "In seq function" << endl;
	bool found = false;
	int index = pref/pageSize + 1;
	int processSize = processList[pid]->size;
	//continously look for page not in memory until one is found
	while (found == false) {
		if (index > processSize/pageSize){
			//cout << index << endl;
			return -1;
		}
		//check bounds
		if (virtualMemory[pid][index]->validBit == 0) {
			found = true;
			return index;
		}
		index++;
	}
}
/*int getSeqPage(int i, int pageSize) {
	cout << "In seq function" << endl;
	bool found = false;
	int index = i + 1;
	//continously look for page not in memory until one is found
	while (found == false) {
		cout << "Test 1" << endl;
		//check bounds
		if (index >= trace.size()){
			//cout << index << endl;
			return -1;
		}
		cout << "Test 2" << endl;
		int pid = trace[index][0];
		int pref = trace[index][1];
		cout << "Test 3" << endl;
		cout << pid << " " << pref << endl;
		if (virtualMemory[pid][pref/pageSize]->validBit == 0) {
			found = true;
			return index;
		}
		index++;
		cout << "Finished!" << endl;
	}
}*/

bool exceedLimit(int pid, int pref, int pageSize){
	return ((pref/pageSize + 1) > processList[pid]->size/pageSize);
}

//prepaging FIFO
void ppFIFO(int frameSize, int pageSize, int tnum) {
	int pid = 0; 
	int pref = 0;
	int numPushed = 0;
	cout << "Reach FIFO" << endl;
	//loop through trace vector
	for (int i = 0; i < tnum; i++) {
		cout << "Loop i: " << i << endl;
		pid = trace[i][0];
		pref = trace[i][1];
		//retrieve next page
		Page* nextPage = virtualMemory[pid][pref/pageSize];
		//if there is a page fault
		if (nextPage->validBit == 0) {
			cout << "There is a page fault!" << endl;
			//if there is a free space in main memory, just push
			int numFreeSpace = frameSize - mainMemory[pid].size();
			int check = getSeqPage(pid, pref, pageSize);
			//int check = getSeqPage(pid, pageSize);
			//if we are not at end, we can prepage 2 pages
			cout << "Checking if we can get sequential page..." << endl;
			if (check != -1) {
				cout << "We are able to get a sequential page!" << endl;
				//int seq = getSeqPage(pid, pref, pageSize);
				//retrieve sequential page that is not in memory
				Page* seqNextPage = virtualMemory[pid][check];
				cout << "Retrieved sequential page!" << endl;
				//if there is only 1 free space, push nextPage to end, replace seqpage by popping front
				if (numFreeSpace == 1) {
					mainMemory[pid].push_back(nextPage);
					setReplaced0(mainMemory[pid].front());
					mainMemory[pid].pop_front();
					mainMemory[pid].push_back(seqNextPage);
					pageSwap++;
				}
				//if there are two free spaces, push both to back
				if (numFreeSpace >= 2) {
					mainMemory[pid].push_back(nextPage);
					mainMemory[pid].push_back(seqNextPage);
				}
				//if there are no free space, we need to pop the first 2 pages and then push next and seq page to back
				else {
					cout << "Attempting to replace..." << endl;
					setReplaced0(mainMemory[pid].front());
					mainMemory[pid].pop_front();
					setReplaced0(mainMemory[pid].front());
					mainMemory[pid].pop_front();
					mainMemory[pid].push_back(nextPage);
					mainMemory[pid].push_back(seqNextPage);
					cout << "Replaced" << endl;
					pageSwap++;
				}
				//update valid bits
				nextPage->validBit = 1;
				seqNextPage->validBit = 1;
			}
			//if we are at boundary, we cannot prepage 2 pages, only 1 page
			else {
				//if there is one free space, push nextpage to back
				if (numFreeSpace >=1) {
					mainMemory[pid].push_back(nextPage);
				} else {
					//we still need to push seq page, so we pop front and push seq page to back
					setReplaced0(mainMemory[pid].front());
					mainMemory[pid].pop_front();
					mainMemory[pid].push_back(nextPage);
					pageSwap++;
				}
				//update valid bit after pushing 
				nextPage->validBit = 1;
			}
		}
	}
}

//prepaging LRU
void ppLRU(int memSize, int pageSize){
	int pid = 0;
	int pref = 0;
	int index = 0;
	//loop through trace vector
	for(int i = 0; i < trace.size(); i++){
		cout << "loop i = " << i << endl;
		pid = trace[i][0];
		pref = trace[i][1];
		//retrieve next page from trace vector
		Page* nextPage = virtualMemory[pid][pref/pageSize];
		//check if we can get a sequential page that is not in memory
		int check = getSeqPage(pid,pref,pageSize);
		//int check = getSeqPage(i, pageSize);
		//if there is a page fault
		if(nextPage->validBit == 0){
			cout << "Not in memory." << endl;
			//if there is free space in memory
			if (mainMemory[pid].size() < memSize) {
				mainMemory[pid].push_back(nextPage);
				nextPage->validBit = 1;
			}
			//if there is no more space, pop front and push to back
			if (mainMemory[pid].size() >= memSize){
				mainMemory[pid].front()->validBit = 0;
				mainMemory[pid].pop_front();
				cout << "Pop front done." << endl;
				mainMemory[pid].push_back(nextPage);
				cout << "Push back done." << endl;
				nextPage->validBit = 1;
				cout << "Set Bit Done." << endl;
				pageSwap++;
			}
			//if we can get a sequential page, we need to add it to mainMemory
			if(check != -1){
				//retrieve sequential page
				Page* seqPage = virtualMemory[pid][check];
				//if there is still free space remaining, just push to back
				if (memSize - mainMemory[pid].size() >= 1){
					mainMemory[pid].push_back(seqPage);
					seqPage->validBit = 1;
				}else{ 	
					//if there is no more free space, we need to pop front and push seq page to back
					mainMemory[pid].front()->validBit = 0;
					mainMemory[pid].pop_front();			
					mainMemory[pid].push_back(seqPage);
					cout << "Push back done." << endl;
					seqPage->validBit = 1;
					cout << "Replace done." << endl;
				}
			}
		}
		//if nextPage is already in memory
		else{
			cout << "In memory." << endl;
			//find index of nextPage in memory
			for(int j = 0; j < mainMemory[pid].size(); j++){
				if(mainMemory[pid][j]->pageid == nextPage->pageid){
					index = j;
					break;
				}
			}
			//take nextPage out of memory and push it to back
			mainMemory[pid].erase(mainMemory[pid].begin() + index);
			mainMemory[pid].push_back(nextPage);
			cout << "Erase and push done." << endl;
		}
	}
}


//memsize is the amount of frames allocated to each process in main memory
void ppClock (int memSize, int pageSize) {
	int clockHand = 0;
	int pid = 0;
	int pref = 0;
	for (int i = 0; i < trace.size(); i++) {
		cout << "Loop: " << i << endl;
		//determine the next process from trace vector
		pid = trace[i][0];
		//determine the next page from trace vector
		pref = trace[i][1];
		//retrieve page needed 
		Page* nextPage = virtualMemory[pid][pref/pageSize];
		//tmp gets the page of the current clock
		Page* tmp = mainMemory[pid][clockHand];
		//if there is a page fault
		int numFreeSpace = memSize - mainMemory[pid].size();
		cout << numFreeSpace << endl;
		//if page needed is not in memory, we need to push it to memory
		if (nextPage->validBit == 0) {
			int check = getSeqPage(pid, pref, pageSize);
			//if we can grab sequential page after next page
			if (check != -1) {
				cout << "Getting second page" << endl;
				Page* seqNextPage = virtualMemory[pid][check];
				//if memory has free space, push it to the back
				//check amount of free space avaiable
				//int numFreeSpace = memSize - mainMemory[pid].size();
				//if there are 2 free spaces, we push both to back
				if (numFreeSpace >= 2) {
					mainMemory[pid].push_back(nextPage);
					mainMemory[pid].push_back(seqNextPage);
				}
				if (numFreeSpace == 1) {
					mainMemory[pid].push_back(nextPage);
					bool foundReplaceLast = false;
					while (foundReplaceLast == false) {
						//if the RBit of the current page inspected is 0, replace it with nextPage
						if(tmp->rBit == 0) {
							cout << "Found page to replace. Replacing now..." << endl;
							tmp->validBit = 0;
							mainMemory[pid][clockHand] = seqNextPage;
							//rbit does not need to updated, already set to 0
							foundReplaceLast = true;
							pageSwap++;
							cout << "Finished replacing page" << endl;
						}
						//if tmp's bit is 1, reset tmp's bit to 0
						if (tmp->rBit == 1) {
							tmp->rBit = 0;
						}
						clockHand++;
						//if we reach one full loop around clock, reset clockHand to 0
						if (clockHand == mainMemory[pid].size()) {
							//reset clockHand to 0
							clockHand = 0;
						} 
						tmp = mainMemory[pid][clockHand];
					}
				}
				//if main memory is full, we need to iterate pointer and replace two frames with R bit = 0
				else {
					cout << "Finding page to replace..." << endl;
					int note = 0;
					vector<int> slot;
					while(note != 2){
						while(tmp->rBit == 1){
							cout << "level 1" << endl;
							tmp->rBit = 0;
							clockHand++;
							if (clockHand >= mainMemory[pid].size()) {
							//update clock
								clockHand = 0;
							}
							tmp = mainMemory[pid][clockHand];
						}
						//when we reach an rBit = 0
						slot.push_back(clockHand);
						note++;
						cout << "level 2" << endl;
						clockHand++;
						if (clockHand >= mainMemory[pid].size()) {
							//update clock
							clockHand = 0;
						}
						tmp = mainMemory[pid][clockHand];
						//slot.push_back(clockHand);
						//note++;
					}
					pageSwap++;
					mainMemory[pid][slot[0]] = nextPage;
					mainMemory[pid][slot[1]] = seqNextPage;
				}
			}
			//else case occurs when prepaging will exceed the memory cap of the process
			else {
				cout << "Finding page to replace..." << endl;
				bool foundReplace = false;
				//while page with r bit of 0 has not been found yet
				while (foundReplace == false) {
					//if the RBit of the current page inspected is 0, replace it with nextPage
					if(tmp->rBit == 0) {
						cout << "Found page to replace. Replacing now..." << endl;
						tmp->validBit = 0;
						mainMemory[pid][clockHand] = nextPage;
						nextPage->validBit = 1;
						//rbit does not need to updated, already set to 0
						pageSwap++;
						foundReplace = true;
						cout << "Finished replacing page" << endl;
					}
					//if tmp's bit is 1, reset tmp's bit to 0
					if (tmp->rBit == 1) {
						tmp->rBit = 0;
					}
					clockHand++;
					//if we reach one full loop around clock with no bit = 0, we will replace one based on FIFO algorithm
					if (clockHand == mainMemory[pid].size()) {
						//update clock
						clockHand = 0;
					} 
					tmp = mainMemory[pid][clockHand];
				}
			}
		}
		//if nextPage is already in memory, iterate to the index where page is located and update clockHand to stay at index of nextPage
		else {
			cout << "Already in memory!" << endl;
			virtualMemory[pid][pref/pageSize]->rBit = 1;
			/*while (tmp->pageid != nextPage->pageid) {
				if (clockHand > mainMemory[pid].size()) {
					clockHand = 0;
				}
				clockHand++;
				tmp = mainMemory[pid][clockHand];
			}*/
		}
	}
}


int main(int argc, char * const argv[]) {

	int sizeOfPages;
	bool prepage;
	string flag;
	string prAlgo;
	string Algo;
	istringstream iss;
	
	if (argc != 6) {
		cerr << "Error: Please enter the correct amount of arguments" << endl;
		return 1;
	}
	//error check for plist
	ifstream in_1(argv[1]);
	if (!(in_1)) {
		cerr << "Error: Please input a valid plist file" << endl;
		return 1;
	}
	//put vector of current line at pline into vector processSize
	int proNum, proMem;
	while(in_1 >> proNum >> proMem){
		vector<int> pro;
		pro.push_back(proNum);
		pro.push_back(proMem);
		processSize.push_back(pro);
		//cout << pro[0] << endl; 
	}
	cout << "end" << endl;
	//error check for ptrace
	ifstream in_2(argv[2]);
	if (!(in_2)) {
		cerr << "Error: Please input a valid ptrace file" << endl;
		return 1;
	}
	//put vector of current line at ptrace into queue 
	//example: push first line of ptrace <0, 300> into queue trace
	while(in_2 >> proNum >> proMem){
		vector<int> tpro;
		tpro.push_back(proNum);
		tpro.push_back(proMem);
		trace.push_back(tpro);
		//cout << tpro[0] << endl; 
	}/*
	for(int k = 0; k < tnum; k++){
		cout << trace[k][0];
	}*/
	//error checking for aguments
	//check if size of pages argument is an integer
	sizeOfPages = atoi(argv[3]);
	cout << "Debug: size of Pages = " << sizeOfPages << endl;
	/*if ((typeid(argv[3]) != typeid(1))) {
		cerr << "Error: Please enter a valid integer for size of pages" << endl;
		return 1;
	}*/
	//check if size of pages has valid non negative integer
	if (sizeOfPages <= 0) {
		cerr << "Error: Please enter a positive integer for size of pages" << endl;
		return 1;
	}
	cout << "Debug: Reach arg 4." << endl;
	//check if page replacement algorithm argument is a string
	iss.str(argv[4]);
	if(!(iss >> prAlgo)) {
		cerr << "Error: Please enter a valid argument for page replacement algorithm" << endl;
		return 1;
	}
	iss.clear();
	//check validity of strings against available page replacement algorithms
	for (int i = 0; i < prAlgo.length(); i++) {
		Algo += tolower(prAlgo[i]);
	}
	if ((Algo != "fifo") && (Algo != "lru") && (Algo != "clock")) {
		cerr << "Error: Please enter a valid page replacement algorithm to use" << endl;
		return 1;
	}
	cout << "Debug: Reach arg 5." << endl;
	iss.str(argv[5]);
	if (!(iss >> flag)) {
		cerr << "Error: please enter a valid argument for flag" << endl;
	}
	//check prepaging input
	cout << "Check flag" << endl;
	if (((flag.compare("+") != 0) && (flag.compare("-")) != 0)) {
		cerr << "Error: Please enter a valid flag for prepaging" << endl;
		return 1;
	}
	//set prepaging values
	if (flag.compare("+") == 0) {
		prepage = true;
	} else {
		prepage = false;
	}
	int pnum = processSize.size();
	int tnum = trace.size();
	cout << "Debug: Reach 1st for-loop." << endl;
	cout << "Debug: Creating page tables" << endl;
	//char alphaName [10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
	//create page table for each process based on total memory and size of pages
	for (int i = 0; i < pnum; i++) {	
		proMem = processSize[i][1];
		//push a new process (array of pages) to vector Processes
		processList.push_back(new Process(i, proMem, sizeOfPages));
		//cout << "loop: " << i << processList[i]->size << endl;
	}
	
	cout << "Debug: Reach 2nd for-loop: Initialize Virtual Memory" << endl;
	for(int i = 0; i < pnum; i++){
		for(int j = 0; j < MEMORY/sizeOfPages/pnum; j++){
			processList[i]->pageTable[j]->validBit = 1;
		}
		virtualMemory.push_back(processList[i]->pageTable);
	}

	cout << "Test: Iterate though virtual memory to print all the pageName" << endl;
	/*for(int i = 0; i < pnum; i++){
		for(int j = 0;j < processList[i]->size/sizeOfPages; j++){
			cout << "pid: " << virtualMemory[i][j]->pid
			<< "      pageid: " << virtualMemory[i][j]->pageid << endl;
		}
	}*/

	cout << "Debug: Reach 3rd for-loop: Initialize Main Memory" << endl;
	//numFramesis the number of frames of each process that is going to be initialized in main memory
	int memSize = MEMORY/sizeOfPages;
	int numProcesses = pnum;
	//numFrames is the amount of frames each process is allocated in main memory
	int numFrames = memSize/numProcesses;
	//loop over # of processes 
	for (int i = 0; i < numProcesses; i++) {
		deque<Page*> processFrames;
		//insert numFrames amount of pages for each iteration of a process
		for(int j = 0; j < numFrames; j++){
			processFrames.push_back(virtualMemory[i][j]);
		}
		//push vector of equal amount of pages for each process into mainMemory
		mainMemory.push_back(processFrames);
		//reset processFrames for next iteration of page
		processFrames.clear();
	}
	
	cout << "Debug: Reach Algo Check." << endl;
	/*prFIFO(numFrames, sizeOfPages, tnum);
	prClock(numFrames, sizeOfPages);
	ppFIFO(numFrames, sizeOfPages, tnum);
	ppClock(numFrames, sizeOfPages);
	cout << "Finished testing!" << endl;*/
	if (Algo == "fifo" && flag == "-") {
		prFIFO (numFrames, sizeOfPages, tnum);
	} else if (Algo == "fifo" && flag == "+") {
		ppFIFO(numFrames, sizeOfPages, tnum);
	} else if (Algo == "lru" && flag == "-") {
		prLRU(numFrames, sizeOfPages);
	} else if (Algo == "lru" && flag == "+") {
		ppLRU(numFrames, sizeOfPages);
	} else if (Algo == "clock" && flag == "-") {
		prClock(numFrames, sizeOfPages);
	} else {
		ppClock(numFrames, sizeOfPages);
	}
	cout << "Number of page swaps for algorithm: " << Algo << " of page size " << sizeOfPages << " with prepaging " << flag << ": " << pageSwap << endl;

}
