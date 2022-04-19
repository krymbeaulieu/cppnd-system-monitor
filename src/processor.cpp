#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float total_d, idle_d; // deltas
    float total_old = Processor::total_;
    float total_new = LinuxParser::Jiffies(); 
    float idle_old = Processor::idle_;
    float idle_new = LinuxParser::IdleJiffies();
    
    UpdateVals(idle_new,total_new); //update for next time

    //calulating
    total_d = float(total_new) - float(total_old); 
    idle_d = float(idle_new) - float(idle_old);
    
    return (total_d - idle_d)/total_d;
    // (total_d - idle_d) / total_d
    }

void Processor::UpdateVals(float idle, float total){
    idle_ = idle;
    total_ = total;
    active_ = total-idle;
    
}