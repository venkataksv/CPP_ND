#include "processor.h"
#include "linux_parser.h"
// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    cpuUtil = LinuxParser::CpuUtilization();

    float idle = std::stof(cpuUtil[3]) + std::stof(cpuUtil[4]) ;
    float nonIdle =  std::stof(cpuUtil[0]) +  std::stof(cpuUtil[1]) +  std::stof(cpuUtil[2]) +  std::stof(cpuUtil[5]) +  std::stof(cpuUtil[6]) +  std::stof(cpuUtil[7]) ;
	
  	float total = idle + nonIdle;
    //float totald = total - prevtotal_;
    //float idled = idle - previdle_;

    //previdle_ = idle;
    //prevnonidle_ = nonIdle;
    //prevtotal_ = total;
  
    return ( nonIdle / total );

    
}