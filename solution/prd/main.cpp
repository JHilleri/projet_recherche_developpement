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

	//Les diff�rents jobs sont index� de 0 � nb_job - 1
	//batch pr�d�fini repr�sente l'ordonnacement  et la mise par lot
	//Attention, cette structure sous entends que les batchs sont continus dans l'ordonnacement, 
	//ce n'est pas forc�ment le cas
	batch_list batch_predefini = Fct_lin::build_regular_batch_repartition(100, 10);

	std::cout << "Ordonnancement et mise en batch propos�e" << std::endl;
	std::cout << batch_predefini << std::endl;

	//tu peux changer la m�thode en param�tre
	Solveur_min_IC solveur;
	solveur.solve(&instance, batch_predefini, algorithme_de_resolution::B_and_B, Solveur_min_IC::init_as_optima);

	cout << endl << "FINI";
	return 0;
}