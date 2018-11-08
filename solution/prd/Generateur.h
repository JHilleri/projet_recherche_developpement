#pragma once
#include <random>
#include <chrono>
#include <string>


using namespace std;

class Generateur
{
public:
	Generateur();
	Generateur(string file_param);
	~Generateur();

	static int num_instance;

	int nb_J = 100;
	int nb_M = 5;

	int C_V = 200000;

	double size = 10; //taille du carré dans lequel seront placé les lieu

	int duree_tournee;

	double moy_cout_M = 5;
	double ecart_moy_cout_M = 2;

	double average_pij;

	string file_inst = "C:/Users/21709633t/source/repos/Manu_dominates_1/Sonja-Rohmer-heuristique/Instance/";

	vector<pair<int, int>> list_coord;

	default_random_engine gen;


	//loi uniforme
	uniform_int_distribution<int> distri_coord;

	//loi normale
	//maintenant, correle avec la distance entre les job
	//normal_distribution<double> distri_process_time;

	normal_distribution<double> distri_hWIP;
	normal_distribution<double> distri_hFIN;

	normal_distribution<double> distri_tardiness_penalty_cost;


	uniform_int_distribution<int> distri_due_date;

	//la quantité on oublie, fixé à 100
	uniform_int_distribution<int> distri_quant = uniform_int_distribution<int>(100, 100);

	void init_gene_instance() {
		//loi uniforme
		distri_coord = uniform_int_distribution<int>(-size / 2, size / 2);

		//loi normale


		//distri_process_time = normal_distribution<double>(size / 2, size/4);

		distri_hWIP = normal_distribution<double>(moy_cout_M, ecart_moy_cout_M);
		distri_hFIN = normal_distribution<double>(moy_cout_M, ecart_moy_cout_M);

		distri_tardiness_penalty_cost = normal_distribution<double>(moy_cout_M, ecart_moy_cout_M);

		distri_due_date = uniform_int_distribution<int>(size*nb_J * 0, size*nb_J*1.35);
	}


	int calc_manhatan_distance(int x_1, int y_1, int x_2, int y_2);

	int generate_pij();
	double average_pij_and_distance();

	void generate_job(ofstream* fichier, int num_j, int nb_M, int due_date);

	void generate_distancier(ofstream* fichier, int nb_J);

	void generate_instance(string path_and_suf, int nb_inst, int nb_Job = 0, int nb_Machine = 0, int taille_max_batch = 0);

	void generate_memo_instance(string prefixe, int nb_inst);

	void batch_decomposition(ofstream* fichier, int nb_J, int max_job_batch);

private:
	void generate_instance(ofstream* fichier, int taille_max_batch = 0);

};

