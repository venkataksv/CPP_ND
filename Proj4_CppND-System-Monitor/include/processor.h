#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <string>
#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
    std::vector<std::string> cpuUtil;
    float previdle_ ;
    float prevtotal_ ;
    float prevnonidle_ ;
};

#endif