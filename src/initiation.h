#ifndef INITIATION_H
#define INITIATION_H
/// \file initiation.h 
/// \brief Initiates the simulation


///\brief Initiates the simulation
class Initiation {
	
	friend class Material;
	friend class Hydrodynamics;

	//reference length, speed, and density for non-dimensionalization
	double _length;///<reference length for non dimensional value 
	double _v;///<reference speed for non dimensional value
	double _rho;///<reference density for non dimensional value
	double _T;///<reference temperature for non dimensional value

public:
	
	///the project name
	char Project_name[25];

	///\brief number of materials
	///
	///- the no. zero material is always the wall
	///- therefore at least 2 materials should be included
	int number_of_materials;
	///the global inputfile name: a *.cfg file
	char inputfile[25];

	/// \brief initial condition marker:
	///
	///- 0 initialize from the .cfg file; 
	///- 1 read from the .rst file particle by particle with non-dimensional data
	int initial_condition;
	///diagnose information maker: 1 output diagnose information
	int diagnose;
	///artificial viscosity
	double art_vis;

	///smoothinglength
	double smoothinglength;
	///the compuational domain size
	Vec2d box_size;
	///cell size
	double cell_size;
	///the inital particle distance
	double delta;
	///the ration between smoothing length and inital particle distance
	int hdelta;
	///cells matrix for real particles
	int x_cells, y_cells;
	///g force on particles
	Vec2d g_force;

	//timing control
	double Start_time;///<Simulation start time
	double End_time;///<Simulation end time
        double D_time;///<time interval for output (every D_time: output)

	Vec2d U0;///<inital flow speed (if initial condition is defined here)w
	double rho0;///<initial particle density(if initial condition is defined here)
	double p0;///<initial pressure(if initial condition is defined here)
	double T0;///<initial temperature(if initial condition is defined here)

	///Moving Least Squarr
	int MLS_MAX;

	///constructor
	///\param *project_name: a pointer to the project name array (the project name is defined as a main-function argument and therefore given as an additional command line element when running the progam) 
	explicit Initiation(const char *project_name);
	///non-dimensionalize initial condition and parameters
	void non_dimensionalize();
	///show information on screen
	void show_information();
	///predict the particle volume and mass
	void VolumeMass(Hydrodynamics &hydro, ParticleManager &particles, QuinticSpline &weight_function);

	//non-dimesionlize
	double non_dms_p(double p);///<a non dimensionalize method
	double non_dms_T(double T);///<a non dimensionalize method
	double non_dms_rho(double rho);///<a non dimensionalize method
	double non_dms_mass(double mass);///<a non dimensionalize method
	double non_dms_time(double time);///<a non dimensionalize method
	double non_dms_length(double length);///<a non dimensionalize method
	Vec2d non_dms_box_size(Vec2d box_size);///<a non dimensionalize method
	double non_dms_velocity(double velocity);///<a non dimensionalize method
	double non_dms_kinetic_viscosity(double nu);///<a non dimensionalize method
	Vec2d non_dms_velocity(Vec2d velocity);///<a non dimensionalize method
	double non_dms_acceleration(double acceleration);///<a non dimensionalize method
	Vec2d non_dms_acceleration(Vec2d acceleration);///<a non dimensionalize method
	double non_dms_viscosity(double mu);///<a non dimensionalize method
	double non_dms_heat_ratio(double cv);///<a non dimensionalize method
	double non_dms_heat_conduction(double kappa);///<a non dimensionalize method
	double non_dms_Boltzmann(double k_bltz);///<a non dimensionalize method
	double non_dms_surface(double sigma);///<a non dimensionalize method

	//re-dimesionlize
	double dms_p(double p_non);///<a re-dimensionalize method
	double dms_T(double T_non);///<a re-dimensionalize method
	double dms_rho(double rho_non);///<a re-dimensionalize method
	double dms_mass(double mass_non);///<a re-dimensionalize method
	double dms_time(double time_non);///<a re-dimensionalize method
	double dms_length(double length_non);///<a re-dimensionalize method
	Vec2d dms_box_size(Vec2d box_size_non);///<a re-dimensionalize method
	double dms_velocity(double velocity_non);///<a re-dimensionalize method
	Vec2d dms_velocity(Vec2d velocity_non);///<a re-dimensionalize method
	double dms_energy(double energy_non);///<a re-dimensionalize method
	double dms_acceleration(double acceleration_non);///<a re-dimensionalize method
	Vec2d dms_acceleration(Vec2d acceleration_non);///<a re-dimensionalize method
	double dms_viscosity(double mu_non);///<a re-dimensionalize method
	double dms_surface(double sigma_non);///<a re-dimensionalize method

};

#endif //INITIATION_H
