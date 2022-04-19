#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  void UpdateVals(float idle, float total);

  // TODO: Declare any necessary private members
 private:
    float idle_;
    float active_;
    float total_;
};

#endif