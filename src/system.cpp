#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector<int> pids{LinuxParser::Pids()};
  set<int> pid_set;

  for (Process const& process : processes_) {
    pid_set.insert(process.Pid());
  }

  for (int pid : pids) {
    if (pid_set.find(pid) == pid_set.end()) { // if pid is not in set
      Process p(pid);
      processes_.emplace_back(p);             // inserting only new pids
    }
  }

  for (Process& process : processes_) {
    process.CalculateCpuUtilization();
  }
  
  std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
  return processes_; 
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }