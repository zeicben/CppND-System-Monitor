#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return _id; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  string line{};
  string value{};
  int count = 0;
  int uptime;
  float utime;
  float stime;
  float cutime;
  float cstime;
  float starttime;
  float total_time;
  float seconds;

  std::ifstream file(LinuxParser::kProcDirectory + to_string(_id) +
                     LinuxParser::kStatFilename);
  if (file.is_open()) {
    while (getline(file, line)) {
      std::istringstream sstream(line);
      while (sstream >> value) {
        ++count;
        switch (count) {
          case 14:
            utime = stof(value);
            break;
          case 15:
            stime = stof(value);
            break;
          case 16:
            cutime = stof(value);
            break;
          case 17:
            cstime = stof(value);
            break;
          case 22:
            starttime = stof(value);
            break;
          default:
            break;
        }
      }
    }
  }

  file = std::ifstream(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
  if (file.is_open()) {
    if (getline(file, line)){
      std::istringstream sstream(line);
      sstream >> value;
      uptime = std::stof(value);
    }
  }

  total_time = (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
  seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));

  return total_time/seconds;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(_id); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(_id); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(_id); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(_id); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (CpuUtilization() < a.CpuUtilization());
}