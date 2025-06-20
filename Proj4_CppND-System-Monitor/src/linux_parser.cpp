#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
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
  if(directory == nullptr)
  {
        std::cerr << "Error: Cannot open directory " << kProcDirectory << std::endl;
    	return {};
  }
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
  string value;
  long MemT = 0;
  long MemA = 0;
  long bufferM = 0;
  long cacheM = 0;
  std::ifstream istream(kProcDirectory + kMeminfoFilename);
  if(istream.is_open())
  {
	while(std::getline(istream,line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream>>key>>value)
      {
        if(key == "MemTotal")
        {
          MemT = std::stol(value);
        }
        else if(key == "MemFree")
        {
          MemA = std::stol(value); 
        }
        else if(key == "Buffers")
        {
          bufferM = std::stol(value); 
        }
        else if(key == "Cached")
        {
          cacheM = std::stol(value); 
        }
      }
    }
    
  }
  float usedM = MemA + bufferM + cacheM;
  if(MemT == 0) {return 0.0;}
  return (MemT - usedM)*1.0/MemT; 

}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
	string line;
    std::ifstream istream(kProcDirectory + kUptimeFilename );
  	string stime;
  	long uTime;
  	if(!istream.is_open())
    {
		return -1;
    }
    std::getline(istream,line);      
    std::stringstream strstream(line);       
    strstream>>stime;
  	if (stime.empty()) {
    std::cerr << "Error: Uptime string is empty" << std::endl;
    return -1;
	}
    try
    {
    	uTime = std::stol(stime);  
    }
  	catch (const std::exception& e)
    {
      std::cerr << "Error: Conversion failed - " << e.what() << std::endl;
      return -1;
    }
  
	return uTime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
	
    vector<string> cpuUtil = LinuxParser::CpuUtilization();
    long totalUtil = 0;

    vector<CPUStates> s = {kUser_,kNice_, kSystem_, kIdle_, kIOwait_,kIRQ_,kSoftIRQ_,kSteal_,kGuest_,kGuestNice_};
    
    for(CPUStates c : s)
    {
      totalUtil+=stol(cpuUtil[c]);
    }
    
    return totalUtil;


}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
	string line;
    std::ifstream istream(kProcDirectory + to_string(pid) + kStatFilename);
    string value;
    long AJiffies = 0;

    if (istream.is_open()) {
        std::getline(istream, line);
        std::istringstream linestream(line);

        for (int i = 1; i < 13; i++) { linestream >> value; } // ✅ Skip first 13 values
        linestream >> value; AJiffies += std::stol(value); // ✅ Read utime (14th)
        linestream >> value; AJiffies += std::stol(value); // ✅ Read stime (15th)
    }
    return AJiffies;

}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
	vector<string> jiffies = LinuxParser::CpuUtilization();

	long val = stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +        
             stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
             stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);

  return val;

}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 

  vector<string> jiffies = LinuxParser::CpuUtilization();

	long val = stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]) ;      
             

  return val;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  std::ifstream istream(kProcDirectory + kStatFilename);
  vector<string> cpuUtil{};
  std::getline(istream,line);
  std::stringstream str(line);
  
  string s;
  str>>s;

  while(str>>s)
  {
    cpuUtil.push_back(s);
  }

  return cpuUtil;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(int pid) { 
  std::ifstream istream(kProcDirectory + to_string(pid) + kStatFilename);
  vector<string> cpuUtil{};
  std::string line;
  std::string sr;
  if(istream.is_open())
  {
  		while(std::getline(istream,line))
        {
  			std::stringstream ss(line);
          	for(int i=0;i<13;i++)
              ss>>sr;
          	for(int i=13;i<17;i++)
            {
              ss>>sr;
              cpuUtil.push_back(sr);
            }
            for(int i=17;i<20;i++)
            {
              ss>>sr;
            }
          	ss>>sr;
          	cpuUtil.push_back(sr);
        }
   }
  return cpuUtil;
}


// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
	  string line;
  	std::ifstream istream(kProcDirectory + kStatFilename);
  	string key;
  	string value;
  	int nProcesses = 0;
  	while(std::getline(istream,line))
    {
      std::stringstream str(line);
      while(str>>key>>value)
      {
        if(key == "processes")
        {
          nProcesses = std::stoi(value);
          return nProcesses;
        }
      }
    }
  return nProcesses;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
	string line;
  	std::ifstream istream(kProcDirectory + kStatFilename);
  	string key;
  	string value;
  	int nProc = 0;
  	while(std::getline(istream,line))
    {
      std::stringstream str(line);
      while(str>>key>>value)
      {
        if(key == "procs_running")
        {
          nProc = std::stoi(value);
          return nProc;
        }
      }
    }
    return nProc;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream istream( kProcDirectory+'/'+to_string(pid)+kCmdlineFilename );
  std::stringstream st;

  if(istream.is_open())
  {
      while(std::getline(istream,line))
      {
        std::replace(line.begin(), line.end(), '\0', ' ');
        st<<line;
      }
  }
  return st.str();
  
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
   
   std::ifstream istr(kProcDirectory+to_string(pid)+kStatusFilename);
   string line;
   std::string key, value;
   if(istr.is_open())
   {
        while(std::getline(istr, line))
        {
          std::stringstream str(line);
          while(str>>key>>value)
          {
            if(key=="VmSize:")
              return to_string(std::stol(value)/1024);
          }
        }
   }
   return to_string(std::stol(value)/1024); 
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 

  string line;
  string key,value;
  std::ifstream stream(kProcDirectory+'/'+to_string(pid)+kStatusFilename);
  if(stream.is_open())
  {
    while(std::getline(stream,line))
    {
      std::replace(line.begin(), line.end(), ':',' ');
      std::stringstream str(line);
	  
      str>>key>>value;
      if(key == "Uid"){ return value; }
    }
  }
  return value;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::ifstream istream(kPasswordPath);
  string Uid = LinuxParser::Uid(pid);
  
  string line;
  
  if(!istream.is_open())
  {
    std::cerr<<"Failed to open the stream"<<"\n";
    return to_string(-1);
  }
  auto uid = LinuxParser::Uid(pid);
  std::string value1, value2, value3;
  
  while(std::getline(istream, line))
  {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::stringstream istr(line);
    istr>>value1>>value2>>value3;
    if(uid == value3)
      return value1;
  }
  
  return value1;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 

  	string line;
    std::ifstream istream(kProcDirectory + '/' + to_string(pid) + kStatFilename );
  	string stime;
  	long uTime;
  	if(!istream.is_open())
    {
		return -1;
    }

    while(std::getline(istream,line))
    {    
    std::stringstream strstream(line);
    for(int i=0;i<21;i++)         
      strstream>>stime;
    
    strstream>>stime;
  	if (stime.empty()) {
      std::cerr << "Error: Uptime string is empty" << std::endl;
      return -1;
  	}
    
    try
    {
    	uTime = std::stol(stime);  
    }
  	catch (const std::exception& e)
    {
      std::cerr << "Error: Conversion failed - " << e.what() << std::endl;
      return -1;
    }
    }
	return LinuxParser::UpTime() - uTime/sysconf(_SC_CLK_TCK);
  

}
