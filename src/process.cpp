#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const { return cpu_; }

void Process::CalculateCpuUtilization() {
  long active_jiffies = LinuxParser::ActiveJiffies(pid_);
  long system_jiffies = LinuxParser::Jiffies();

  long diff_active_jiffies{active_jiffies - prev_active_jiffies_};
  long diff_system_jiffies{system_jiffies - prev_total_jiffies_};

  cpu_ = static_cast<float>(diff_active_jiffies) / diff_system_jiffies;
  prev_active_jiffies_ = active_jiffies;
  prev_total_jiffies_ = system_jiffies;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}

bool Process::operator>(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}