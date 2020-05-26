#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stol;
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
  filestream.close();
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
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
// Returns a vector of ints containing the number of each process
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

// DONE: Read and return the system memory utilization
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
  filestream.close();
  return (mem_total - mem_free) / mem_total;
}

// DONE: Read and return the system uptime
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
  filestream.close();
  // In case of failure, return 0
  return 0;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line{};
  string value{};
  int count = 0;
  long utime;
  long stime;
  long cutime;
  long cstime;
  // Create an input filestream object
  std::ifstream file(LinuxParser::kProcDirectory + to_string(pid) +
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
          default:
            break;
        }
      }
    }
  }
  file.close();
  return utime + stime + cutime + cstime;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu = LinuxParser::CpuUtilization();

  return stol(cpu[0]) + stol(cpu[1]) + stol(cpu[2]) + stol(cpu[5]) +
        stol(cpu[6]) + stol(cpu[7]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu = LinuxParser::CpuUtilization();

  return stol(cpu[3]) + stol(cpu[4]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
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
          return cpu_data;
        }
      }
    }
  }
  filestream.close();
  return cpu_data;
}

// DONE: Read and return the total number of processes
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
  fstream.close();
  return 0;
}

// DONE: Read and return the number of running processes
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
  fstream.close();
  return 0;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string command{};

  std::ifstream file_stream(kProcDirectory + std::to_string(pid) +
                            kCmdlineFilename);
  if (file_stream.is_open()) {
    getline(file_stream, command);
  } else {
    command = "Couldn't retrieve command";
  }

  file_stream.close();

  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string memory;
  std::string line;
  std::string key;
  std::string value;

  // Create an input file stream object
  std::ifstream file_stream(kProcDirectory + std::to_string(pid) +
                            kStatusFilename);
  // Check the file to be opened
  if (file_stream.is_open()) {
    // Parse the file line by line
    while (getline(file_stream, line)) {
      // Create an input stringstream object from the line we just read
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream isstream(line);

      isstream >> key;
      // Check if we reached the line that describes ram utilization
      if (key == "VmSize") {
        // Read that value
        isstream >> value;
        return std::to_string(std::stol(value) / 1000) + " Mb";
      }
      // If current line is not about ram utilization, skip the other info
      // and continue looping
      else {
        continue;
      }
    }
  }
  file_stream.close();
  return "0 Mb";
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string user;
  std::string line;
  std::string key;
  std::string value;

  // Create an input file stream object
  std::ifstream file_stream(kProcDirectory + std::to_string(pid) +
                            kStatusFilename);
  // Check the file to be opened
  if (file_stream.is_open()) {
    // Parse the file line by line
    while (getline(file_stream, line)) {
      // Create an input stringstream object from the line we just read
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream isstream(line);

      isstream >> key;
      // Check if we reached the line that lists the UIDs
      if (key == "Uid") {
        // Read that value
        isstream >> value;
        // Return the UID
        return value;
      }
      // If current line is not about users, skip the other info and continue
      // looping
      else {
        continue;
      }
    }
  }
  file_stream.close();
  return string();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  int uid = std::stoi(Uid(pid));
  std::string line;
  std::string key;
  std::string value;
  std::string x;

  std::ifstream file_stream(kPasswordPath);
  if (file_stream.is_open()) {
    while (getline(file_stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream input_sstream(line);

      input_sstream >> value >> x >> key;
      if (std::stoi(key) == uid) {
        return value;
      }
    }
  }
  file_stream.close();
  return string();
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line;
  std::string value;
  long count = 0;

  // Initialize a filestream and open the file
  std::ifstream file_stream(kProcDirectory + std::to_string(pid) +
                            kStatFilename);
  // Check that the file was opened without problems
  if (file_stream.is_open()) {
    // Read a line from the file
    while (getline(file_stream, line)) {
      // Create a stringstream from the line
      std::istringstream string_stream(line);
      while (string_stream >> value) {
        ++count;
        if (count == 22) {
          return std::stol(value) / sysconf(_SC_CLK_TCK);
        }
      }
    }
  }
  file_stream.close();
  return 0;
}