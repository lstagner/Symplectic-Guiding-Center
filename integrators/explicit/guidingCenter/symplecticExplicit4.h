// Symplectic integrator for guiding center, explicit 4 (paragraph 6.4.7)

#ifndef SYMPLECTICEXPLICIT4_H
#define SYMPLECTICEXPLICIT4_H

#include "../../variationalIntegrator.h"

using namespace Particles;

namespace Integrators{
	template <int DIM> class SymplecticExplicit4 : public VariationalIntegrator<DIM>
	{
		public:
			SymplecticExplicit4(Config::Config* config);
			~SymplecticExplicit4(){};

			PositionMomentumPoint<DIM> LegendreRight(PositionPoints<DIM> q, double h);
			PositionPoints<DIM> LegendreLeftInverse(PositionMomentumPoint<DIM> z, double h);

			//override generic system with a guiding center system.
			//We need to use guiding center EM fields
			GuidingCenter<DIM>* system;

			double mu;
	};

	template <int DIM> SymplecticExplicit4<DIM>::SymplecticExplicit4(Config::Config* config) : VariationalIntegrator<DIM>(config){
		if (DIM!=8) throw invalid_argument("Invalid dimension for symplectic explicit 4: please use 8.");
		system = guidingcenterFactory<DIM>(config->system,config);		
		mu = config->mu;
	}

	template <int DIM> PositionMomentumPoint<DIM> SymplecticExplicit4<DIM>::LegendreRight(PositionPoints<DIM> q, double h){

		PositionMomentumPoint<DIM> z;

		GuidingField field = system->fieldconfig->compute(q.q1);

		Matrix<double,DIM/2,DIM/2> M;
		Matrix<double,DIM/2,1> W,Q;

		//~ //BUILD M
		M(0,1) = field.Bdag(2);
		M(0,2) = -field.Bdag(1);
		M(1,0) = -field.Bdag(2);
		M(1,2) = field.Bdag(0);
		M(2,0) = field.Bdag(1);  
		M(2,1) = -field.Bdag(0);   
		M(0,3) = -field.b(0);
		M(1,3) = -field.b(1);
		M(2,3) = -field.b(2);
		M(3,0)=field.b(0);
		M(3,1)=field.b(1);
		M(3,2)=field.b(2);
		M(0,0) = M(1,1) = M(2,2) = M(3,3) = 0;

		Matrix<double,DIM/2,DIM/2> grad2h;
		grad2h.block(0,0,2,2) = mu*field.B_hessian;
		grad2h(3,3) = 1.;
		M += h/2.*grad2h;

		M/=2.;

		Matrix<double,DIM/2,1> dq = q.q1 - q.q0;
		Q = M*dq;

		z.q = q.q1;
		z.p.head(3) = Q.head(3) + field.Adag -h/2.*mu*field.B_grad;
		z.p(3) = Q(3) -h/2.*q.q1(3);

		return z;
	}
	template <int DIM> PositionPoints<DIM> SymplecticExplicit4<DIM>::LegendreLeftInverse(PositionMomentumPoint<DIM> z, double h){

		PositionPoints<DIM> q;

		GuidingField field = system->fieldconfig->compute(z.q); 
		  
		Matrix4d M;
		Vector4d W,Q;

		//~ //BUILD M
		M(0,1) = field.Bdag(2);
		M(0,2) = -field.Bdag(1);
		M(1,0) = -field.Bdag(2);
		M(1,2) = field.Bdag(0);
		M(2,0) = field.Bdag(1);  
		M(2,1) = -field.Bdag(0);   
		M(0,3) = -field.b(0);
		M(1,3) = -field.b(1);
		M(2,3) = -field.b(2);
		M(3,0)=field.b(0);
		M(3,1)=field.b(1);
		M(3,2)=field.b(2);
		M(0,0) = M(1,1) = M(2,2) = M(3,3) = 0;

		Matrix4d grad2h = Matrix4d::Zero();
		grad2h.block(0,0,2,2) = mu*field.B_hessian;
		grad2h(3,3) = 1.;
		M -= h/2.*grad2h;

		M/=2.;

		// //BUILD W
		W.head(3) = h/2.*(mu*field.B_grad) +field.Adag - z.p.head(3);
		W(3) = (h/2.*z.q(3) -z.p(3));

		Q = M.inverse()*W;

		q.q0 = z.q;
		q.q1.head(3) = (q.q0.head(3)+Q.head(3));
		q.q1(3) = (q.q0(3)+Q(3));

		return q;
	}

}
#endif