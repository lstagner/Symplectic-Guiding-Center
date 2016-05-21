#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "../systems/system.h"
#include "../systems/systemFactory.h"
#include <stdexcept>

using namespace Systems;

namespace Integrators{
	template <int DIM> class Integrator
	{
		public:
			Integrator(Config::Config* config);
			~Integrator(){};
			
			virtual Matrix<double,DIM,1> StepForward(Matrix<double,DIM,1> z, double h) = 0;

			System<DIM>* system;

			virtual PhaseSpacePoints<DIM> initialize(PhaseSpacePoints<DIM> z, initializationType init, double h);
	};

	template <int DIM> PhaseSpacePoints<DIM> Integrator<DIM>::initialize(PhaseSpacePoints<DIM> z, initializationType init, double h){
		if (init==INIT_MANUAL){
			z.z1 = z.z0;
			return z;
		}
		else throw invalid_argument("invalid initialization type");
	}

	template<int DIM> Integrator<DIM>::Integrator(Config::Config* config){
		if (config->h==0) throw invalid_argument("Invalid timestep h");
		system = systemFactory<DIM>(config->system,config);	
	}
}

#endif