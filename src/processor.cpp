#include "processor.h"

#include <sstream>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::vector;

// Return the aggregate CPU utilization
float Processor::Utilization() {
  float idle_r, non_idle_r;

  idle_r = LinuxParser::IdleJiffies();
  non_idle_r = LinuxParser::ActiveJiffies();

  return non_idle_r / (non_idle_r + idle_r);
}