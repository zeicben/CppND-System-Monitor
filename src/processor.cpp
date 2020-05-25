#include "processor.h"

#include <sstream>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<string> cpu_data;
  cpu_data = LinuxParser::CpuUtilization();

  float user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  float idle_r, non_idle_r;

  user = std::stof(cpu_data[0]);
  nice = std::stof(cpu_data[1]);
  system = std::stof(cpu_data[2]);
  idle = std::stof(cpu_data[3]);
  iowait = std::stof(cpu_data[4]);
  irq = std::stof(cpu_data[5]);
  softirq = std::stof(cpu_data[6]);
  steal = std::stof(cpu_data[7]);
  guest = std::stof(cpu_data[8]);
  guest_nice = std::stof(cpu_data[9]);

  idle_r = idle + iowait;
  non_idle_r = user + nice + system + irq + softirq + steal;

  return idle_r / (non_idle_r + idle_r);
    // return 0.3;
}