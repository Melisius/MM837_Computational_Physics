/****************************************************************************
 * Program.cc
 * Main program for execution of Potts model
 * **************************************************************************/

#include<iostream>
#include<fstream>
#include<vector>
#include"iocontrol.h"
#include"Lattice.h"
#include"Properties.h"
#include"json.hpp"

using namespace std;
using json = nlohmann::json;

int main(){
	int L, q, thermalization_sweeps, simulation_sweeps, sample_frequency,
		sweep_method, hybrid_typewrite_freqency, random_seed, check_critial_T;
	double beta, acceptance_ratio;
	
	// Initialize iocontrol
	print_results results(1);
	results.write_input();
	
	// Load parameters from file
	ifstream ifs("parameters.json");
	json j = json::parse(ifs);
	L = j["L"];
	q = j["q"];
	thermalization_sweeps = j["thermalization_sweeps"];
	simulation_sweeps = j["simulation_sweeps"];
	sample_frequency = j["sample_frequency"];
	sweep_method = j["sweep_method"];
	hybrid_typewrite_freqency = j["hybrid_typewrite_freqency"];
	random_seed = j["random_seed"];
	beta = j["beta"];
	check_critial_T = j["simulate_crital_temperaure"];
	
	if (check_critial_T == 1){beta = log(1+sqrt((double)(q)));}
	
	// Make container for energies
	vector<double> energy_container(simulation_sweeps/sample_frequency, 0);
	
	// Initialize lattice and sweeper method
	Lattice lattice_func(random_seed, L, q, beta, sweep_method, hybrid_typewrite_freqency);

	for (int i=0; i<thermalization_sweeps; i++){
		lattice_func.do_sweep();
	}

	for (int i=1; i<simulation_sweeps+1; i++){
		lattice_func.do_sweep();
		if (i%sample_frequency == 0){
			//energy_container[i/sample_frequency] = lattice_func.return_energy();
			energy_container[i/sample_frequency] = lattice_func.return_magnetization();
		}
	}
	
	results.write_acceptance_ratio(lattice_func.return_acceptance_ratio());
	// Calculate properties
	/*
	properties property_func(energy_container);
	property_func.calc_autocovariance();
	property_func.calc_autocorrelation();
	property_func.calc_integrated_autocorr();
	*/
	results.write_energy(energy_container);
	//results.write_integrated_autocorr(property_func.return_integrated_autocorr());
	//results.write_autocorr(property_func.return_autocorr());
	
	// Finalize IO
	results.close_files();
}
