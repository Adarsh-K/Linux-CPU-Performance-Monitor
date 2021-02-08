#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  

 private:
    long prev_active_jiffies_{0};
    long prev_idle_jiffies_{0};
};

#endif