#include "stdafx.h"
#include "Generateur.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;



Generateur::Generateur()
{
	unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
	gen = default_random_engine(seed1);
}

Generateur::Generateur(string file_param)
{
	unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
	gen = default_random_engine(seed1);
}


Generateur::~Generateur()
{
}

void Generateur::generate_instance(ofstream* fichier, int taille_max_batch)
{
	list_coord = vector<pair<int, int>>(nb_J + 2);

	average_pij = 0.0;

	*fichier << nb_J << "\t" << nb_M << endl;
	*fichier << C_V << endl;


	if (taille_max_batch != 0) batch_decomposition(fichier, nb_J, taille_max_batch);


	vector<int> vect_due_date = vector<int>(nb_J);

	for (int i = 0; i < vect_due_date.size(); i++)
	{
		vect_due_date[i] = (int)distri_due_date(gen);
	}
	sort(vect_due_date.begin(), vect_due_date.end());

	int i = 0;
	for each (int var in vect_due_date)
	{
		generate_job(fichier, i, nb_M, var);
		i++;
	}

	average_pij /= nb_M * nb_J;

	*fichier << "Distancier : " << endl;

	//*** Coordonné du dépot
	list_coord[nb_J].first = (int)distri_coord(gen);
	list_coord[nb_J].second = (int)distri_coord(gen);

	*fichier << list_coord[nb_J].first << "\t" << list_coord[nb_J].second << endl;

	generate_distancier(fichier, nb_J);
}

int write_positive_1(ofstream* fichier, int num) {
	if (num <= 1) {
		*fichier << 1 << "\t";
		return 1;
	}
	else {
		*fichier << num << "\t";
		return num;
	}
}

int Generateur::calc_manhatan_distance(int x_1, int y_1, int x_2, int y_2)
{
	return abs(x_1 - x_2) + abs(y_1 - y_2);
}

int Generateur::generate_pij()
{
	int x_1 = distri_coord(gen);
	int y_1 = distri_coord(gen);
	int x_2 = distri_coord(gen);
	int y_2 = distri_coord(gen);

	return calc_manhatan_distance(x_1, y_1, x_2, y_2);

}

double Generateur::average_pij_and_distance()
{
	int NB = 1500;

	double sum = 0.0;

	for (int i = 0; i < NB; i++)
	{
		sum += generate_pij();
	}

	return sum / NB;
}

void Generateur::generate_job(ofstream* fichier, int num_j, int nb_M, int due_date = -1)
{
	*fichier << "Job " << num_j << " : " << endl;

	for (int i = 0; i < nb_M; i++)
	{
		average_pij += write_positive_1(fichier, generate_pij());
	}
	*fichier << endl;

	//*** Coordonée
	int x = (int)distri_coord(gen);
	int y = (int)distri_coord(gen);
	list_coord[num_j].first = x;
	list_coord[num_j].second = y;
	*fichier << x << "\t" << y << endl;
	//*** FIN Coordonée

	//*** Coût d'inventaire variable
	for (int i = 0; i < nb_M - 1; i++) {
		write_positive_1(fichier, distri_hWIP(gen));
	}
	write_positive_1(fichier, distri_hFIN(gen));
	*fichier << endl;

	//*** FIN Coût d'inventaire variable

	//*** Due date
	if (due_date == -1) {
		*fichier
			<< (int)distri_due_date(gen) << "\t";
	}
	else {
		*fichier << due_date << "\t";
	}

	//*** Coût d'inventaire fixe
	int quant = distri_quant(gen);
	*fichier
		<< quant << "\t"
		/*<< (int)(prix*ratio_hWIP) << "\t"
		<< (int)(prix*ratio_hFIN) << "\t"
		<< (int)(prix*eps[1]) << "\t"
		<< (int)(prix*eps[1] * 1.3) << endl;*/
		<< (int)(1) << "\t"
		<< (int)(2) << "\t";
	//<< (int)(1) << "\t";
	write_positive_1(fichier, distri_tardiness_penalty_cost(gen));
	*fichier << (int)(9) << endl;
}

void Generateur::generate_distancier(ofstream* fichier, int nb_J) {

	int nb_lieu = nb_J + 1;

	int dist; double moy = 0; int cpt = 0;

	for (int i = 0; i < nb_lieu; i++)
	{
		for (int j = 0; j < i + 1; j++)
		{
			//Distance manhattan
			dist = calc_manhatan_distance(list_coord[i].first, list_coord[i].second, list_coord[j].first, list_coord[j].second);

			//Distance euclidienne arrondi :(
			/* dist = (int)(sqrt(
			pow(list_coord[i].first - list_coord[j].first, 2) +
			pow(list_coord[i].second - list_coord[j].second, 2)));*/

			*fichier << dist << "\t";

			moy += dist;
			cpt++;

		}
		*fichier << endl;
	}

	moy /= cpt;

	//cout << moy << endl;
}

void Generateur::generate_instance(string path_and_suf, int nb_inst, int nb_Job, int nb_Machine, int taille_max_batch)
{

	if (nb_Job != 0) {
		nb_J = nb_Job;
	}

	if (nb_Machine != 0) {
		nb_M = nb_Machine;
	}

	init_gene_instance();

	for (int i = 0; i < nb_inst; i++)
	{
		ofstream fichier(path_and_suf + to_string(i) + ".txt", ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert

		if (fichier) {

			generate_instance(&fichier, taille_max_batch);

		}
		else {
			cerr << "Impossible d'ouvrir le fichier !" << endl;
		}
	}

}

/*void Generateur::generate_instance_un_batch(string path_and_suf, int nb_inst, int nb_Job)
{
nb_J = nb_Job;

init_gene_1batch_instance();

ofstream fichier;
for (int i = 0; i < nb_inst; i++)
{
fichier = ofstream(path_and_suf + to_string(i) + ".txt", ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert

if (fichier) {

generate_instance(&fichier);

fichier.close();
}
else {
cerr << "Impossible d'ouvrir le fichier !" << endl;
}
}
}*/

/*void Generateur::generate_instance_test_algo_routing(string prefix, int nb_job, int nb_inst, int taille_carre, int moy_cout_retard, int ecart_cout_retard, bool calcul_de_l_opti)
{
nb_J = nb_job;
size = taille_carre;
moy_cout_M = moy_cout_retard;
ecart_moy_cout_M = ecart_cout_retard;

init_gene_1batch_instance();

distri_tardiness_penalty_cost = normal_distribution<double>(moy_cout_retard, ecart_cout_retard);

ofstream fichier; string file;
for (int i = 0; i < nb_inst; i++)
{
file = file_inst + "Instance_" + prefix + "/Inst_" + prefix +
"_" + to_string(i) + ".txt";

fichier = ofstream(file, ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert

if (fichier) {

generate_instance(&fichier);

fichier.close();

}
else {
cerr << "Impossible d'ouvrir le fichier !" << endl;
}
}
generate_memo_instance(prefix, nb_inst);
}*/

void Generateur::generate_memo_instance(string prefix, int nb_inst)
{

	ofstream fichier = ofstream(file_inst + "Instance_" + prefix + "/1Memo_inst_" + prefix +
		".txt", ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert

	if (fichier) {

		fichier << nb_inst << " " << duree_tournee << "\n\n";

		fichier << "Nb instance: \t" << nb_inst << "\n\n";

		fichier << "Nb job: \t\t" << nb_J << "\n"
			<< "Taille carre: \t" << size << "\n"
			<< "(Tps tournee): \t" << duree_tournee << "\n\n"
			<< "Cout moy retard: \t" << moy_cout_M << "\n"
			<< "Ecart cout moy retard: \t" << ecart_moy_cout_M << "\n";

		fichier.close();
	}
	else {
		cerr << "Impossible d'ouvrir le fichier memo !" << endl;
	}
}

void Generateur::batch_decomposition(ofstream * fichier, int nb_J, int max_job_batch)
{
	int min = 5;
	if (max_job_batch == 5) { min = 1; }

	int nb_j_ranges = 0;

	int random;

	while (nb_J - nb_j_ranges > max_job_batch) {

		random = (rand() % (max_job_batch - min + 1)) + min;

		nb_j_ranges += random;

		*fichier << random << " ";
	}

	*fichier << nb_J - nb_j_ranges << endl;
}


