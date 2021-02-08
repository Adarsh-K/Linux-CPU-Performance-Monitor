#include <string>
#include<iostream>
#include "format.h"

using std::string;

// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    long minutes = seconds / 60;
    long hours = minutes / 60;

    return std::to_string(hours) + ":" +
           std::to_string(minutes % 60) + ":" + std::to_string(seconds % 60);
}