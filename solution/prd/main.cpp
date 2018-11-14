#include <cstdlib>
#include <ctime>

#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>

#include "Instance.h"
#include "Generateur.h"
#include "Fct_lin.h"
#include "Solveur_min_IC.h"
#include "Branch_and_bound.h"
#include "batch_list.h"

using namespace std;

int main(int argc, char **argv)
{
	if (argc < 2) // checking parameters
	{
		std::cerr << "invalid parameters, usage \"prd.exe [path to the instance]\"" << std::endl;
		return 0;
	}
	std::filesystem::path instance_path = std::filesystem::absolute({ argv[1] });
	if (!std::filesystem::is_regular_file(instance_path))
	{
		std::cerr << "invalid instance path : \"" << instance_path << "\"" << std::endl;
		return 0;
	}

	unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
	srand(seed1);


	std::ifstream instance_file{ instance_path };
	Instance instance{ instance_file };

	//Les différents jobs sont indexé de 0 à nb_job - 1
	//batch prédéfini représente l'ordonnacement  et la mise par lot
	//Attention, cette structure sous entends que les batchs sont continus dans l'ordonnacement, 
	//ce n'est pas forcément le cas
	batch_list batch_predefini = Fct_lin::build_regular_batch_repartition(100, 10);

	std::cout << "Ordonnancement et mise en batch proposée" << std::endl;
	std::cout << batch_predefini << std::endl;

	//tu peux changer la méthode en paramètre
	Solveur_min_IC solveur;
	solveur.solve(&instance, batch_predefini, algorithme_de_resolution::B_and_B, Solveur_min_IC::init_as_optima);

	cout << endl << "FINI";
	return 0;
}