#ifndef GUIDINGCENTER_H
#define GUIDINGCENTER_H
#include "../utils/particleUtils.h"
#include "../emfields/guidingfield.h"
#include "../emfields/guidingfieldFactory.h"
#include "system.h"
#include <stdexcept>
#include <iostream>

using namespace Particles;
using namespace GuidingFields;
using namespace std;

namespace Systems{

	template <int DIM> class GuidingCenter : public System<DIM>{ 
		public:
			GuidingCenter(Config::Config* config);
			~GuidingCenter();

			double Hamiltonian(PositionMomentumPoint<DIM> z);
			Matrix<double,DIM,1> momentum(PositionPoints<DIM> q);
			Matrix<double,2*DIM,1> f_eq_motion(Matrix<double,2*DIM,1> z);

			GuidingFieldConfiguration *fieldconfig;

			static const double mu = 2.25E-6;
			static const double hx = 1.E-5;  //step for numerical derivative
	};

	template<int DIM> GuidingCenter<DIM>::GuidingCenter(Config::Config* config) : System<DIM>(config){
		if (DIM!=4) throw invalid_argument("          Invalid Guiding Center dimension: please use 4.");

		fieldconfig = guidingfieldFactory(config->magneticField,config);
	}

	template<int DIM> Matrix<double,DIM,1> GuidingCenter<DIM>::momentum(PositionPoints<DIM> q){

		GuidingField field = fieldconfig->compute(q.q0);
		Vector4d p;

		p(3) = 0.;
		p.head(3) = field.A+q.q0(3)*field.b;

		return p;
	};
	template<int DIM> double GuidingCenter<DIM>::Hamiltonian(PositionMomentumPoint<DIM> z){
		GuidingField field = this->fieldconfig->compute(z.q);
		return (0.5*z.q(3)*z.q(3)+mu*field.Bnorm);
	}

	template <int DIM> Matrix<double,2*DIM,1> GuidingCenter<DIM>::f_eq_motion(Matrix<double,2*DIM,1> z){
		Matrix<double,2*DIM,1> f;

		GuidingField field = fieldconfig->compute(z.head(4));

		f.setZero();
		
		Matrix<double,3,1> E_dag;
		double B_dag_par;
		B_dag_par = field.Bdag.dot(field.b);
		E_dag = -mu*field.B_grad;
		
		f.head(3) = (z(3)* field.Bdag - field.b.cross(E_dag))/B_dag_par;
		f(3) = field.Bdag.dot(E_dag)/B_dag_par;

		return f;	
	}
}

#endif