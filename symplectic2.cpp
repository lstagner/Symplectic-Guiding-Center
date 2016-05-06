#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdexcept>

#include "utils/particle.h"
#include "config.h"

using namespace std;
using namespace Particles;

void setPrintPrecision(int print_precision, ofstream& out);
void printToFile(int t,Config::Config* config, Particle<Config::DIM>* particle, ofstream& out);

int main(int argc, char* argv[]){

    //configuration and create new particle
    ofstream out("out/out.txt");
    Config::Config* config = new Config::Config();
    Particle<Config::DIM>* particle = new Particle<Config::DIM>(config);

    // ofstream out(config->outFile.c_str());  //output file
    if (!out.is_open()) throw invalid_argument("Can't open output file for write");

    setPrintPrecision(config->print_precision,out);

    cout << "time step: " << config->h << endl;
    cout << "Initialization: " << endl;
    cout << "q_init:\t" << particle->get_q0().transpose() << endl;
    cout << "p_init:\t" << particle->get_p0().transpose() << endl;
    cout << "q0:\t" << particle->get_q1().transpose() << endl;
    cout << "p0:\t" << particle->get_p1().transpose() << endl;
    printToFile(0,config,particle, out);
  
    // ******
    //MAIN LOOP
    // ******
    for (int t=config->time_offset + 1;t<=config->max_t+config->time_offset;t++){

        if ((config->print_timestep_mult>0) && (t%config->print_timestep_mult==0)) cout << "Timestep " << t << endl;

        particle->StepForward();

        if (t==1) {
          cout << "q1:\t" << particle->get_q1().transpose() << endl;
          cout << "p1:\t" << particle->get_p1().transpose() << endl;
        }

        //EXIT IF THE ERROR IS TOO HIGH
        if ((config->exit_on_error) && (abs(particle->Eerr0)>config->error_threshold)){
          cout << "Timestep: " << t << "\tError too high! Exiting." << endl;
          break;
        }

        //PRINT TO FILE
        if (((t+config->print_timestep_offset)%config->file_timestep_mult)==0) printToFile(t,config,particle, out);
    }
  
    return 0;
}

void setPrintPrecision(int print_precision, ofstream& out){
    cout.setf(std::ios::scientific);
    cout.precision(print_precision);
    out.setf(std::ios::fixed);
    out.precision(print_precision);
    out.setf(ios::showpoint);
}

void printToFile(int t,Config::Config* config, Particle<Config::DIM>* particle, ofstream& out){
    out 
        << (t) << " " 
        << (t)/config->orbit_normalize << " " 
        << particle->Eerr1 << " " 
        << particle->get_q1().transpose() << " " 
        << particle->get_p1().transpose() << " " 
        << (particle->get_p1()-particle->mom1).transpose();
    out << endl;

}