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
using std::stof;


// DONE: Return this process's ID
Process::Process(int pid) : pid_(pid){}

int Process::Pid() const { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() { 
  
  vector<string> cpuUtil = LinuxParser::CpuUtilization(Pid());
    
  long systemUpTime = LinuxParser::UpTime();
  long processUpTime = std::stol(cpuUtil[4]);
  
  // based on https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  float seconds = systemUpTime - (processUpTime*1.0/sysconf(_SC_CLK_TCK));
  float totalTime = 0.0;
  
  for(int i=0;i<4;i++)
  {
    totalTime += std::stof(cpuUtil[i]);
  }
   
   if(seconds!=0)
   {
	   cpu_usage_ = (totalTime/sysconf(_SC_CLK_TCK))/seconds; 
   }
  
 
  return cpu_usage_;

}

// TODO: Return the command that generated this process
string Process::Command() { 
    int pid = Pid();   
    return LinuxParser::Command(pid); 
}

// DONE: Return this process's memory utilization
string Process::Ram() const { 
    return LinuxParser::Ram(Pid());
}

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    
    return a.cpu_usage_ < cpu_usage_ ;
}