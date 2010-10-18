///\file interaction.cpp
///\author Xiangyu Hu <Xiangyu.Hu@aer.mw.tum.de>
///\author changes by: Martin Bernreuther <Martin.Bernreuther@ipvs.uni-stuttgart.de>, 

//----------------------------------------------------------------------------------------
//		defines interaction between particles
//		interaction.cpp
//----------------------------------------------------------------------------------------
#include <glog/logging.h>
#include <assert.h>

// ***** localincludes *****
#include "src/Kernel/kernel.h"
#include "src/Interaction/interactionin.h"
#include "src/ParticleContext/particlecontext.h"
#include "src/particle.h"
#include "src/initiation.h"

//----------------------------------------------------------------------------------------
//					update pair forces
//----------------------------------------------------------------------------------------
void InteractionIn::UpdateForces() {
  ///  \todo{This is very slow}
  if (ini.context->Interacting(Org, Dest)) {
      const double rhoi = Org->rho; 
      const double rhoj = Dest->rho;
      const double Vi = mi/rhoi; 
      const double Vj = mj/rhoj;
      assert(Vi>0.0);
      assert(Vj>0.0);
      const double Vi2 = Vi*Vi; 
      const double Vj2 = Vj*Vj;
      const double shear_rij = 2.0*etai*etaj/(etai + etaj);
      const Vec2d Uij = Org->U - Dest->U;
      const double pi = Org->p; 
      const double pj = Dest->p;
      /// viscous forces
      const Vec2d dPdti_visc = -shear_rij*Fij*(Vi2 + Vj2) * Uij;
      /// pressure forces
      const Vec2d dPdti_pre = eij*Fij*rij*(pi*Vi2 + pj*Vj2);
      const Vec2d dPdti = dPdti_visc  + dPdti_pre;
      Org->dUdt += dPdti*rmi;
      Dest->dUdt -= dPdti*rmj;
  }
}

InteractionIn::InteractionIn(const spParticle prtl_org, const spParticle prtl_dest, 
	      spKernel weight_function, const double dstc,
	      const Initiation& ini): 
  Interaction(prtl_org, prtl_dest, weight_function, dstc, ini) {
  assert(ini.simu_mode == 1);
}

InteractionIn::~InteractionIn() {
}