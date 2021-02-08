#include<iostream>

#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;

float Processor::Utilization() { 
  float utilization{0};
  long idle_jiffies = LinuxParser::IdleJiffies();
  long active_jiffies = LinuxParser::ActiveJiffies();

  long diff_active_jiffies{active_jiffies - prev_active_jiffies_};
  long diff_idle_jiffes{idle_jiffies - prev_idle_jiffies_};
  long total_jiffies{diff_active_jiffies + diff_idle_jiffes};

  utilization = static_cast<float>(diff_active_jiffies)/total_jiffies;
  prev_active_jiffies_ = active_jiffies;
  prev_idle_jiffies_ = idle_jiffies;
  
  return utilization;
}