#ifndef GUIDINGCENTER_H
#define GUIDINGCENTER_H
#include "../utils/particleUtils.h"
#include "../emfields/guidingfield.h"
#include "../emfields/guidingfieldFactory.h"
#include "system.h"
#include <stdexcept>
#include <iostream>

using namespace ParticleUtils;
using namespace GuidingFields;
using namespace std;

namespace Systems{

	template <int DIM> class GuidingCenter : public System<DIM>{ 
		public:
			GuidingCenter(Config::Config* config);
			~GuidingCenter();

			double Hamiltonian(PositionMomentumPoint<DIM> z);
			Matrix<double,DIM,1> momentum(PositionPoints<DIM> q);

			GuidingFieldConfiguration *fieldconfig;

			static const double mu = 2.25E-6;
			static const double hx = 1.E-5;  //step for numerical derivative
	};

	template<int DIM> GuidingCenter<DIM>::GuidingCenter(Config::Config* config) : System<DIM>(config){
		if (DIM!=4) throw invalid_argument("          Invalid Guiding Center dimension: please use 4.");
		if (config->magneticField=="") throw invalid_argument("Invalid magnetic field");

		fieldconfig = guidingfieldFactory(config->magneticField,config);
	}

	template<int DIM> Matrix<double,DIM,1> GuidingCenter<DIM>::momentum(PositionPoints<DIM> q){

		GuidingField field = fieldconfig->compute(q.q0);
		Vector4d p;
		Vector3d x = q.q0.head(3);

		p(3) = 0.;
		p.head(3) = field.A+q.q0(3)*field.b;

		return p;
	};
	template<int DIM> double GuidingCenter<DIM>::Hamiltonian(PositionMomentumPoint<DIM> z){
		GuidingField field = this->fieldconfig->compute(z.q);
		return (0.5*z.q(3)*z.q(3)+mu*field.Bnorm);
	}

}

#endif