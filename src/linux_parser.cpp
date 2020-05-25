#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  // Create an input fstream using the file that stores OS information
  std::ifstream filestream(kOSPath);
  // If the file opened without problems
  if (filestream.is_open()) {
    // Repeat the process bellow while we have lines in the file
    while (std::getline(filestream, line)) {
      // Replaces some of the characters of the line (which is a string) for
      // processing the stream better
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      // Create a stringstream from the current line
      std::istringstream linestream(line);
      // Read (key, value) pairs until the line ends. In this example, there is
      // only 1 (key, value) pair on each line
      while (linestream >> key >> value) {
        // Check if the key is the one which has the OS's name as value
        if (key == "PRETTY_NAME") {
          // Replace '_' with ' ' in order to reverse the replacement made
          // earlier
          std::replace(value.begin(), value.end(), '_', ' ');
          // Return the obtained string
          return value;
        }
      }
    }
  }
  // Return empty string if finding the OS name failed
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  std::string value;
  string kb;

  float mem_total = 0;
  float mem_free = 0;

  // Create an input fstream using the file stored at `kMeminfoFilename`
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  // Check if the stream has opened
  if (filestream.is_open()) {
    // Iterate through each line of the stream
    while (getline(filestream, line)) {
      // Replace some characters by others (if needed)
      std::replace(line.begin(), line.end(), ':', ' ');
      // Build an `istringstream` object based on the line
      std::istringstream linestream(line);
      // Check if the keys are the ones we search for
      while (linestream >> key >> value >> kb) {
        // return 0.4;
        if (key == "MemTotal") {
          // Convert from string to long long
          mem_total = std::stof(value);
        }
        if (key == "MemFree") {
          mem_free = std::stof(value);
        }
      }
    }
  }
  return (mem_total - mem_free) / mem_total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string up_time;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    string line;

    // Get a line of the file (file only has one line, anyways)
    getline(filestream, line);
    // Create a stringstream object based on that line
    std::istringstream linesstream(line);
    // Get the up time as a string
    linesstream >> up_time;
    // `up_time` is float, so before casting it to long, we turn it into float
    return long(stof(up_time));
  }
  // In case of failure, return 0
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  // string user, nice, system, idle, iowait, irq, softirq, steal, guest,
  //     guest_nice;
  // long idle_r, non_idle_r;
  string line;
  string str;
  vector<string> cpu_data{};

  // Create the ifstream for the file
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      // Create the istringstream object from the line
      std::istringstream linestream(line);
      while (linestream >> str) {
        // If we reached the line with info about the cpu
        if (str == "cpu") {
          while (linestream >> str) {
            cpu_data.push_back(str);
          }
          // linestream >> user >> nice >> system >> idle >> iowait >> softirq >>
          //     steal >> guest >> guest_nice;

          // idle_r = std::stol(idle) + std::stol(iowait);
          // non_idle_r = std::stol(user) + std::stol(nice) + std::stol(system) +
          //            std::stol(irq) + std::stol(softirq) + std::stol(steal);

          // return std::to_string(idle_r / (idle_r + non_idle_r));
          return cpu_data;
        }
      }
    }
  }

  return cpu_data;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  int procs_running;

  // Open file as input file stream
  std::ifstream fstream(kProcDirectory + kStatFilename);
  if (fstream.is_open()) {
    while (getline(fstream, line)) {
      std::istringstream sstream(line);
      sstream >> key >> value;
      // If the key is not `processes` skip this line; the key is supposed
      // to be the first element of the line
      if (key != "processes")
        continue;
      else {
        procs_running = stoi(value);
        return procs_running;
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  int procs_running;

  // Open file as input file stream
  std::ifstream fstream(kProcDirectory + kStatFilename);
  if (fstream.is_open()) {
    while (getline(fstream, line)) {
      std::istringstream sstream(line);
      sstream >> key >> value;
      // If the key is not `procs_running` skip this line; the key is supposed
      // to be the first element of the line
      if (key != "procs_running")
        continue;
      else {
        procs_running = stoi(value);
        return procs_running;
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }