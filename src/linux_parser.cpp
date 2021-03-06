#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string val;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "PRETTY_NAME") {
          std::replace(val.begin(), val.end(), '_', ' ');
          return val;
        }
      }
    }
  }
  return val;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os,version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >>version>> kernel;
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
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string memory_total, freeMemory, line, key, val;
  int captured = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      while (linestream >> key >> val && captured != 2) {

        if (key == "MemTotal") {
          memory_total = val;
          captured += 1;
        } else if (key == "MemFree") {
          freeMemory = val;
          captured += 1;
        }
      }
    }
  }

  return ((float)stol(memory_total) - (float)stol(freeMemory)) / (float)stol(memory_total);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, totalUptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> totalUptime;
  }
  return stol(totalUptime);
}



// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }


// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }




// TODO: Read and return CPU utilization=
vector<string> LinuxParser::CpuUtilization() {
  string line, cpuString;
  vector<string> res(8, "temp");
  
  std::ifstream stream(LinuxParser::kProcDirectory +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);    
     linestream >> cpuString >> res[0] >> res[1] >> res[2] >> res[3] >> res[4] >>
        res[5] >> res[6] >> res[7];

  }
  return res;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcs, captured = 0;
  string line, key, val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> val && captured != 1) {
        if (std::all_of(val.begin(), val.end(), isdigit)&&key == "processes") {
          
          totalProcs = std::stoi(val);
          captured += 1;
        }
      }
    }
  }
  return totalProcs;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int runningProcs, captured = 0;
  string line, key, val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> val && captured != 1) {
        if (std::all_of(val.begin(), val.end(), isdigit)&&key == "procs_running") {
          runningProcs = std::stoi(val);
          captured += 1;
        }
      }
    }
  }
  return runningProcs;
}

float LinuxParser::CpuUtilization(int pid) {
  string temp, line;
  float utime, stime, cutime, cstime, starttime, total_time, seconds;
  int i = 0;
  std::ifstream filestream(kProcDirectory +"/"+ std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (i < 22) {
      linestream >> temp;
      if (i == 13)
        utime = stof(temp);
      else if (i == 14)
        stime = stof(temp);
      else if (i == 15)
        cutime = stof(temp);
      else if (i == 16)
        cstime = stof(temp);
      else if (i == 21)
        starttime = stof(temp);
      i++;
    }
  }

  total_time = utime + stime + cutime + cstime;
  seconds = UpTime() - starttime / (float)sysconf(_SC_CLK_TCK);
  return total_time / sysconf(_SC_CLK_TCK) / seconds;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory +"/"+ std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) std::getline(filestream, line);
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, val;

  std::ifstream filestream(kProcDirectory+"/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "VmSize") return val;
      }
    }
  }
  return val;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, val;
  std::ifstream filestream(kProcDirectory+"/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "Uid") return val;
      }
    }
  }
  return val;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, user, x, uidVal;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uidVal) {
        if (uidVal == uid) return user;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, temp;
  std::ifstream filestream(kProcDirectory+"/" + std::to_string(pid) +
                           kStatFilename);
  int i = 0;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (i++ < 22) {
      linestream >> temp;
    }
  }


  return std::stol(temp) / sysconf(_SC_CLK_TCK);
}