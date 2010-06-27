/// \file initiation.cpp
/// \author author Xiangyu Hu <Xiangyu.Hu@aer.mw.tum.de>
// \author changes by: Martin Bernreuther <Martin.Bernreuther@ipvs.uni-stuttgart.de>, 

//----------------------------------------------------------------------------------------
//      initialize the progam
//		initiation.cpp
//----------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>

#include <cstdio>
#include <cstdlib>

// ***** localincludes *****
#include "glbfunc.h"
#include "hydrodynamics.h"
#include "particlemanager.h"
#include "Kernel/kernel.h"
#include "initiation.h"

using namespace std;

//----------------------------------------------------------------------------------------
//							constructor
//----------------------------------------------------------------------------------------
Initiation::Initiation(const char *project_name) {
	
	char Key_word[25];

	//the project name
	strcpy(Project_name, project_name);

	//the input file name
	strcpy(inputfile, Project_name);
	strcat(inputfile, ".cfg");
	
	///<ul><li>check if inputfile exists (if not exit the program)
	ifstream fin(inputfile, ios::in);
	if (!fin) {
		cout<<"Initialtion: Cannot open "<< inputfile <<" \n";
		std::cout << __FILE__ << ':' << __LINE__ << std::endl;
		exit(1);
	}
	else cout<<"Initialtion: Read the global configuration data from "<< inputfile <<" \n"; 

	///<li>reading key words and configuration data from configuration file and assign them to the appropriate variable
	while(!fin.eof()) {
		
		//read a string block
		fin>>Key_word;
		
		//comparing the key words for simulation mode
		//1: liquids
		//2: gas dynamics
		if(!strcmp(Key_word, "SIMULATION_MODE")) fin>>simu_mode;

		//comparing the key words for initial condition input
		//0: Initialize the initial conditions from .cfg file
		//1: restart from a .rst file
		if(!strcmp(Key_word, "INITIAL_CONDITION")) fin>>initial_condition;

		//comparing the key words for domian size
		if(!strcmp(Key_word, "CELLS")) fin>>x_cells>>y_cells;

		//comparing the key words for cell size
		if(!strcmp(Key_word, "CELL_SIZE")) fin>>cell_size;

		//comparing the key words for supportlength
		if(!strcmp(Key_word, "SUPPORT_LENGTH")) fin>>supportlength;

		//comparing the key words for the ratio between cell size and initial particle width
		if(!strcmp(Key_word, "CELL_RATIO")) fin>>hdelta;

		//comparing the key words for the g force
		if(!strcmp(Key_word, "G_FORCE")) fin>>g_force[0]>>g_force[1];
 
		//comparing the key words for the artificial viscosity
		if(!strcmp(Key_word, "ARTIFICIAL_VISCOSITY")) fin>>art_vis;

		//comparing the key words for dimension
		if(!strcmp(Key_word, "DIMENSION")) fin>>_length>>_v>>_rho>>_T;
		
		//comparing the key words for number ofmaterials
		if(!strcmp(Key_word, "NUMBER_OF_MATERIALS")) fin>>number_of_materials;

		//comparing the key words for timing
		if(!strcmp(Key_word, "TIMING")) fin>>Start_time>>End_time>>D_time;

		//Initialize the initial conditions from .cfg file
		if (initial_condition==0) {
			//comparing the key words for the initial state
			if(!strcmp(Key_word, "INITIAL_STATES")) fin>>U0[0]>>U0[1]>>rho0>>p0>>T0;
		}

	}
	fin.close();

	///<li>create outdata directory
	const int sys_return = system("mkdir -p outdata");
	if (sys_return) {
	  std::cerr<<"system command faild"<< inputfile <<" \n";
	  std::cerr << __FILE__ << ':' << __LINE__ << std::endl;
	  exit(EXIT_FAILURE);
	}
	
	
	///<li>process the data <b>!!!Question!!!</b>
	box_size[0] = x_cells*cell_size; box_size[1] = y_cells*cell_size;
	delta = cell_size/hdelta;///(line 104) this is only true if h=cell_size (which is not necessarily given, as h, cell_size can be initiated independently in configuration file)
	
	///<li>output information to screen
	show_information();

	///<li>non-dimensionalize \n\n
	non_dimensionalize();

}
//----------------------------------------------------------------------------------------
//					show information to screen
//----------------------------------------------------------------------------------------
void Initiation::show_information() const 
{
	///- output general information on screen
  cout<<"The simulation mode is"<<simu_mode<<"! (1=liquids, 2=gas dynamics)\n";
	cout<<"The number of materials in the simulation is  "<<number_of_materials<<"\n";
	cout<<"The computational domain size is  "<<box_size[0]<<" micrometers x "<<box_size[1]<<" micrometers\n";
	cout<<"The cell size is "<<cell_size<<" micrometers \n";
	cout<<"The support length is "<<supportlength<<" micrometers \n";
	cout<<"The cell matrix size is "<<x_cells<<" x "<<y_cells<<"\n";
	cout<<"The ratio between cell size and initial particle width is "<<hdelta<<"\n";
	cout<<"The initial particle width is "<<delta<<" micrometers\n";
	cout<<"The g force is "<<g_force[0]<<" m/s^2 x "<<g_force[1]<<" m/s^2 \n";
	cout<<"The dimensionless reference length, speed, density and temperature are \n"
		<<_length<<" micrometer, "<<_v<<" m/s, "<<_rho<<" kg/m^3, "<<_T<<" K\n";

	///- output the timing information on screen
	cout<<"\nInitialtion: Time controlling:\nStarting time is "<<Start_time<<" \n";
	cout<<"Ending time is "<<End_time<<" \n";
	cout<<"Output time interval is "<<D_time<<" \n";

	///- output iniformation on initialization mode (.cfg file or .rst file)
	//Initialize the initial conditions from .cfg file
	if (initial_condition==0) {
		cout<<"\nInitialize the initial conditions from "<<inputfile<<" \n";
		cout<<"The initial flow speed is "<<U0[0]<<" m/s x "<<U0[1]<<" m/s\n";
		cout<<"The initial density is "<<rho0<<" kg/m^3\n";
		cout<<"The initial pressure is "<<p0<<" Pa\n";
		cout<<"The initial temperature is "<<T0<<" K\n";

	}
	
	//Initialize the initial conditions from .rst file
	if (initial_condition == 1)
		cout<<"Read the initial conditions from separated restat file "<<Project_name<<".rst \n";
}
//----------------------------------------------------------------------------------------
//					predict the particle volume and mass
//----------------------------------------------------------------------------------------
void Initiation::VolumeMass(Hydrodynamics &hydro, ParticleManager &particles, Kernel &weight_function)
{
  ///mass initiation is different of 1DSPH code: 
  ///here: mass is calculated by summing up the kernel function contributions for easch particle, which gives a kind of the inverse volume taken by each particle (not perfectly true at the discontinuity). together with rho (from initialization) a mass for each particle can be obtained.
  ///within the discontinuity zone, this mass varies because of the smoothing effect of the kernel summation.
  ///The mass for each particle  stays constant during the simuation.

  /// <ul><li>iterate particles on the particle list
  for (std::list<spParticle >::iterator p = hydro.particle_list.begin(); 
       p != hydro.particle_list.end(); 
       p++) {
					
    /// <ul><li> pick an origin particle
    spParticle prtl_org = *p;
    /// <li> build the nearest neighbor particle list for chosen origin particle
    assert(prtl_org != NULL);
    const std::list<spParticle> NNP_list = particles.BuildNNP(prtl_org->R);

    /// size of the list can be zero in some circumstances
    /// but in 1D (2D) shock it is not expected
    assert(NNP_list.size() > 0);

    double reciprocV = 0.0; 
    /// <li>iterate this Nearest Neighbor spParticle list
    for (std::list<spParticle >::const_iterator p1 = NNP_list.begin(); 
	 p1 != NNP_list.end(); 
	 p1++) {
			
      /// <ul><li> get a particle
      spParticle prtl_dest = *p1;

      /// <li> calculate distance (origin<-> neighbor)
      if ( prtl_dest->ID != prtl_dest->ID) {
	const double dstc = v_distance(prtl_org->R, prtl_dest->R);
	assert(dstc>0.0);
	/// <li> calculate weight function for given distance (w=0, if dist>supportlengtg) an summ it up </ul> 
	reciprocV += weight_function.w(dstc);
      } else {
	/// particle with itself
	reciprocV += weight_function.w(0.0);
      }
    }
    /// <li> calculate volume as reciprocal value of weight function
    reciprocV = 1.0/reciprocV;
    /// <li> save volume and mass in the respective particle list node (whih is each a spParticle object with all the particle properties) 
    prtl_org->V = reciprocV;
    prtl_org->m = prtl_org->rho*reciprocV;
  }
  cout<<"\n Volume and Mass successfully calculated\n ";
}
//----------------------------------------------------------------------------------------
//				Non-dimensionalize the initial condition and parameters
//----------------------------------------------------------------------------------------
void Initiation::non_dimensionalize()
{
        ///method calls the individual non_dms_methods vor each variable\n
        ///remark:to avoid confusion: the non dimensional variables have the same identifiers like the dimensional ones!!!\n\n
	box_size = non_dms_box_size(box_size);
	cell_size = non_dms_length(cell_size);
	supportlength = non_dms_length(supportlength);
	delta = non_dms_length(delta); 
	g_force = non_dms_acceleration(g_force);
	Start_time = non_dms_time(Start_time);
	End_time = non_dms_time(End_time);
	D_time = non_dms_time(D_time);

	//Bltzmann constant
	extern double k_bltz;
	k_bltz = non_dms_Boltzmann(k_bltz);
	
	//non-dimensionalize initial states
	if(initial_condition==0) {

		U0 = non_dms_velocity(U0);
		rho0 = non_dms_rho(rho0);
		p0 = non_dms_p(p0);
		T0 = non_dms_T(T0);
	}
}
//-------------------------------------------------------
//			Non_dimensionalize pressure
//-------------------------------------------------------
double Initiation::non_dms_p(const double p) const 
{
	return p/_v/_v/_rho;
}
//-------------------------------------------------------
//			Non_dimensionalize temperature
//-------------------------------------------------------
double Initiation::non_dms_T(const double T) const
{
	return T/_T;
}
//-------------------------------------------------------
//			Non_dimensionalize density
//-------------------------------------------------------
double Initiation::non_dms_rho(const double rho) const
{
	return rho/_rho;
}
//-------------------------------------------------------
//			Non_dimensionalize mass
//			in 2-dimension 
//-------------------------------------------------------
double Initiation::non_dms_mass(const double mass) const
{
	return mass/_rho/_length/_length/_length;
}
//-------------------------------------------------------
//			Non_dimensionalize time
//-------------------------------------------------------
double Initiation::non_dms_time(const double time) const
{
	return time*_v/_length;
}
//-------------------------------------------------------
//			Non_dimensionalize length
//-------------------------------------------------------
double Initiation::non_dms_length(const double length) const
{
	return length/_length;
}
//-------------------------------------------------------
//			Non_dimensionalize boxsize in 2-d
//-------------------------------------------------------
Vec2d Initiation::non_dms_box_size(const Vec2d box_size) const
{
	return box_size/_length;
}
//-------------------------------------------------------
//			Non_dimensionalize velocity
//-------------------------------------------------------
double Initiation::non_dms_velocity(const double velocity) const
{
	return velocity/_v;
}
//-------------------------------------------------------
//			Non_dimensionalize vector velocity
//-------------------------------------------------------
Vec2d Initiation::non_dms_velocity(const Vec2d velocity) const
{
	return velocity/_v;
}
//-------------------------------------------------------
//			Non_dimensionalize acceleration vector
//-------------------------------------------------------
Vec2d Initiation::non_dms_acceleration(const Vec2d acceleration) const
{
	 return acceleration*_length/_v/_v;
}
//-------------------------------------------------------
//			Non_dimensionalize viscosity
//-------------------------------------------------------
double Initiation::non_dms_viscosity(const double mu) const
{
	return mu/_v/_rho/_length;
}
//-------------------------------------------------------
//			Non_dimensionalize kinetic viscosity
//-------------------------------------------------------
double Initiation::non_dms_kinetic_viscosity(const double nu) const
{
	return nu/_v/_length;
}
//-------------------------------------------------------
//			Non_dimensionalize heat ratio
//-------------------------------------------------------
double Initiation::non_dms_heat_ratio(const double cv) const
{
	return cv*_T/_v/_v;

}
//-------------------------------------------------------
//			Non_dimensionalize Boltzmann constant
//-------------------------------------------------------
double Initiation::non_dms_Boltzmann(const double k_bltz) const
{
	return k_bltz*_T/_v/_v/_rho/_length/_length/_length;
}
//-------------------------------------------------------
//			Dimensionalize functions
//-------------------------------------------------------
double Initiation::dms_p(const double p_non) const
{
	return p_non*_v*_v*_rho;
}
//-------------------------------------------------------
double Initiation::dms_T(const double T_non) const
{
	return T_non*_T;
}
//-------------------------------------------------------
double Initiation::dms_rho(const double rho_non) const
{
	return rho_non*_rho;
}
//-------------------------------------------------------
double Initiation::dms_mass(const double mass_non) const
{
	return mass_non*_rho*_length*_length;
}
//-------------------------------------------------------
double Initiation::dms_time(const double time_non) const
{
	return time_non/_v*_length;
}
//-------------------------------------------------------
double Initiation::dms_length(const double length_non) const
{
	return length_non*_length;
}
//-------------------------------------------------------
Vec2d Initiation::dms_box_size(const Vec2d box_size_non) const
{
	return box_size_non*_length;
}
//-------------------------------------------------------
Vec2d Initiation::dms_velocity(const Vec2d velocity_non) const
{
	return velocity_non*_v;
}
//-------------------------------------------------------
double Initiation::dms_velocity(const double velocity_non) const
{
	return velocity_non*_v;
}
//-------------------------------------------------------
double Initiation::dms_energy(const double energy_non) const
{
	return energy_non*_v*_v;
}
//-------------------------------------------------------
Vec2d Initiation::dms_acceleration(const Vec2d acceleration_non) const
{
	 return acceleration_non*_v*_v/_length;
}
