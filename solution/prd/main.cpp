#include <cstdlib>
#include <ctime>

#include <iostream>
#include <fstream>
#include <chrono>

#include "Instance.h"
#include "Generateur.h"
#include "Fct_lin.h"
#include "Solveur_min_IC.h"
#include "Branch_and_bound.h"




using namespace std;

int main(int argc, char **argv)
{
	unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
	srand(seed1);

	vector<int> tab_method = vector<int>({
		Solveur_min_IC::CPLEX,
		Solveur_min_IC::B_and_B,
		Solveur_min_IC::heuristic_near,
		Solveur_min_IC::near,
		Solveur_min_IC::EDD_near,
		Solveur_min_IC::nearx4,
		Solveur_min_IC::EDD_nearx4//*/
		});


	//chemin d'instance à modifier
	string chemin_instance = "C:/Users/21709633t/source/repos/Projet_PRD/Sonja-Rohmer-heuristique/Instance/";
	string name_instance = "Instance_sans_batch.txt";
	Instance* instance = new Instance(chemin_instance + name_instance);

	//Les différents jobs sont indexé de 0 à nb_job - 1
	//batch prédéfini représente l'ordonnacement  et la mise par lot
	//Attention, cette structure sous entends que les batchs sont continus dans l'ordonnacement, 
	//ce n'est pas forcément le cas
	vector<vector<int>> batch_predefini = Fct_lin::build_regular_batch_repartition(100, 10);

	cout << "Ordonnancement et mise en batch proposée" << endl;
	for each (auto var in batch_predefini)
	{
		cout << "{";
		for each (auto i in var)
		{
			cout << i << ", ";
		}cout << "}; ";
	}cout << endl;

	//tu peux changer la méthode en paramètre
	Solveur_min_IC solveur;
	solveur.solve(instance, batch_predefini, Solveur_min_IC::B_and_B, Solveur_min_IC::init_as_optima);

	cout << endl << "FINI";
	return 0;
}