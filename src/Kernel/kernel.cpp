/// \file kernel.cpp
/// \author Xiangyu Hu <Xiangyu.Hu@aer.mw.tum.de>
/// \author changes by: Martin Bernreuther <Martin.Bernreuther@ipvs.uni-stuttgart.de>
/// \author changes by: Andreas Mattes
/// \brief the abstract kernel base class
///


//----------------------------------------------------------------------------------------
//      The abstract kernel base class for the SPH method
//		kernel.cpp
//----------------------------------------------------------------------------------------

#include <iostream>

#include <cstdlib>
#include <glog/logging.h>

// ***** local includes *****
#include "glbfunc.h"
#include "Kernel/kernel.h"

//constructor to inizialize the data members
Kernel::Kernel(const double supportlength)
    : supportlength(supportlength)
{
    if (supportlength <= 0.0)
    {
      LOG(INFO) << "\n FAILURE: The Kernel gets a non-positive smoothing length \"" 
		<< supportlength << "\"!\n";
      LOG(INFO) << __FILE__ << ':' << __LINE__ << std::endl;
      exit(1);
    }
}
//----------------------------------------------------------------------------------------
//					de-constructor
//----------------------------------------------------------------------------------------
///deconstructor
Kernel::~Kernel()
{
}
