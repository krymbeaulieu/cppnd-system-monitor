#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    pid_ = pid;
}

int Process::Pid() { return Process::pid_; }  

float Process::CpuUtilization() const{ return (float) LinuxParser::ActiveJiffies(pid_) / LinuxParser::UpTime(); }

string Process::Command() { return LinuxParser::Command(Process::pid_); }

string Process::Ram() { return LinuxParser::Ram(Process::pid_); }

string Process::User() { return LinuxParser::User(Process::pid_); } 

long int Process::UpTime() { return LinuxParser::UpTime() - LinuxParser::UpTime(Process::pid_); }

bool Process::operator<(Process const& a) const { 
    return CpuUtilization() < a.CpuUtilization();
    }