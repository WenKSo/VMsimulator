// Name: Wenkang Su, Donald Tang
// I pledge my honor that I have abided by the Stevens Honor System. 
#ifndef PAGE_H_
#define PAGE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

class Page {
    
public:
    int pid;
    int pageid;
    int validBit;
    int rBit; //reference bit for LRU and clock 
    int lastTimeAccessed;
    
    Page (int pid, int pageid, int validBit, int rBit) {
        this->pid = pid;
        this->pageid = pageid;
        this->validBit = validBit;
        this->rBit = rBit;
    }
  
    bool isValid() {
        if (validBit == 0) {
            return false;
        }
        return true;
    }

    void setRBit(int num) {
        rBit = num;
    }
};

#endif /* PAGE_H_ */
