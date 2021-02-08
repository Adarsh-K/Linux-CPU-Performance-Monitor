#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  float mem_total, mem_free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          mem_total = value;
        }
        if (key == "MemFree:") {
          mem_free = value;
          return (mem_total - mem_free) / mem_total;
        }
      }
    }
  }
  return 0.0;
}

long LinuxParser::UpTime() {
  string line;
  long uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

long LinuxParser::ActiveJiffies(int pid) {
  string line, token;
  long total_jiffies{0};
  long utime{0}, stime{0}, cutime{0}, cstime{0};
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    for (int i = 0; filestream >> token; i++) {
      if (i == 13) utime = stol(token);
      if (i == 14) stime = stol(token);
      if (i == 15) cutime = stol(token);
      if (i == 16) cstime = stol(token);
    }
    total_jiffies = utime + stime + cutime + cstime;
  }
  return total_jiffies;
}

long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_time = CpuUtilization();
  long active_jiffies = stol(cpu_time[CPUStates::kUser_]) + stol(cpu_time[CPUStates::kNice_]) + 
                        stol(cpu_time[CPUStates::kSystem_]) + stol(cpu_time[CPUStates::kIdle_]) +
                        stol(cpu_time[CPUStates::kIOwait_]) + stol(cpu_time[CPUStates::kIRQ_]) +
                        stol(cpu_time[CPUStates::kSoftIRQ_]) + stol(cpu_time[CPUStates::kSteal_]);
  return active_jiffies; 
}

long LinuxParser::IdleJiffies() { 
  vector<string> cpu_time = CpuUtilization();
  return stol(cpu_time[CPUStates::kIdle_]) + stol(cpu_time[CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  string line, cpu, cpu_time;
  vector<string> cpu_utilizations;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream  >> cpu;

    while (linestream >> cpu_time) {
      cpu_utilizations.emplace_back(cpu_time);
    }
  }
  return cpu_utilizations;
}

int LinuxParser::TotalProcesses() {
  string key, value;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") return stoi(value);
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string key, value;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") return stoi(value);
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line, command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return std::to_string(value/1000);
        }
      }
    }
  }
  return string("0");
}

string LinuxParser::Uid(int pid) {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return std::to_string(value);
        }
      }
    }
  }
  return string("0");
}

string LinuxParser::User(int pid) {
  string line;
  string user, x, uid_for_user;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid_for_user) {
        if (uid_for_user == Uid(pid)) {
          return user;
        }
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  long int uptime{0};
  string token;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatFilename);

  if (filestream.is_open()) {
    int i = 0;
    while (filestream >> token) {
      if (i == 13) {
        uptime = stol(token) / sysconf(_SC_CLK_TCK);
        return uptime;
      }
      i++;
    }
  }
  return uptime;
}