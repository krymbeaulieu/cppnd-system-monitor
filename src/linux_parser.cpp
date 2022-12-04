#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, val; 
  int mTotal, mFree;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while(linestream>>key>>val){
        if (key == "MemTotal:") mTotal = stoi(val);
        else if (key == "MemAvailable:") mFree = stoi(val);
        }
      }
    }
  
  return (float)(mTotal - mFree) / mTotal; 
  }

long LinuxParser::UpTime() { 
  string line;
  string val = "0.0"; //uptime is in s, no need to change
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
      while (std::getline(stream, line)) {
          std::istringstream linestream(line);
          linestream >> val;
          return stol(val);
      }
  }
  return stol(val);}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return ActiveJiffies() + IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) { 
  long jiffies{0};
  string line, val;
  int count{0};

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
      std::getline(stream,line);
      std::istringstream linestream(line);
      while (linestream >> val) {
          // actives are 14, 15, 16, 17 in manual (count starts at 0 so -1 to the vals)
          if (count == 13 || count == 14 || count == 15 || count == 16){
            jiffies += stol(val);
          }
          count++;
      }
  }
  
  return jiffies / sysconf(_SC_CLK_TCK); }

long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies{CpuUtilization()};
  long val{0};
  // actives are not kidle, kiowait
  //apparently size is unsigned so it warns against signed i vs unsigned size. force int to make that warning go away...
  for (int i = 0; i<(int) jiffies.size(); i++){
    if ((i==CPUStates::kIdle_) || (i==CPUStates::kIOwait_)){
      continue; //don't add idles
    }
    val += stol(jiffies[i]);
  }
  return val;}


long LinuxParser::IdleJiffies() { 
  vector<string> jiffies{CpuUtilization()};
  // idle = kidle, kiowait
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]); }

vector<string> LinuxParser::CpuUtilization() { 
  string line, key, val;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);

    linestream  >> key; //cpu
    //read them all
    while(linestream >> val){
      jiffies.emplace_back(val);
    }

  }
  
  return jiffies; 
}

int LinuxParser::TotalProcesses() { 
  string line, key, val;
  int procs{0};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes"){
        linestream >> procs;
        return procs;
        // break;
      }
    }
  }

  return procs; 
  }

int LinuxParser::RunningProcesses() { 
  string line, key;
  string val = "0";
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running"){
        linestream >> val;
        return stoi(val);
      }
    }
  }
  return stoi(val); 
  }

string LinuxParser::Command(int pid) { 

  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line = "";
  
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line; 
}

string LinuxParser::Ram(int pid) { 
  string line, key;
  string val = "";
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:"){
        linestream >> val;
        //found ram in kb, / 100 for mb
        return to_string(stol(val) / 1000);
      }
    }
  }
  return val; 
  }

string LinuxParser::Uid(int pid) { 
  string line, key;
  string val = "";
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:"){
        linestream >> val;
        //found user id
        return val;
      }
    }
  }
  return val; 
  }

string LinuxParser::User(int pid) { 
  string uID_val = Uid(pid);
  string line, key, x, id; // david:x:1000:1000:David Silver (example)
  string val = "";

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()){

    while(std::getline(stream,line)){
      std::replace(line.begin(),line.end(),':',' ');
      std::istringstream linestream(line);
      linestream >> key >> x >> id;
      if (id == uID_val){
        linestream >> val;
        //found user, return the username in key not the uid val
        return key;
      }
    }
  }
  return val;
}

long LinuxParser::UpTime(int pid) {
  string line, val; 
  vector<string> vals;
  long starttime{0};

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    while(linestream >>val){
      vals.emplace_back(val);
    }   
    starttime = stol(vals[21]); // man says 22 for start time, vals idx starts with 0
    
  }
  return starttime / sysconf(_SC_CLK_TCK);
}
