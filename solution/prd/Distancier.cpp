#include "Distancier.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

Distancier::Distancier()
{
}
Distancier::~Distancier()
{
	/*for each (vector<int>* var in tab)
	{
	delete var;
	}*/
}

Distancier::Distancier(int nbj)
{
	this->nbjob = nbj;
	this->nblieu = nbj + 2;

	this->tab = vector<vector<int>>(nblieu, vector<int>(nblieu, 0));
	//this->tab = vector<vector<int>*>(nblieu, new vector<int>(nblieu, 0));

}

Distancier::Distancier(ifstream* fichier, int nb_job)
{

	nbjob = nb_job;
	nblieu = nbjob + 2;

	string tmp;
	getline(*fichier, tmp);
	getline(*fichier, tmp);

	*fichier >> coord_depot.first >> coord_depot.second;

	tab = vector<vector<int>>(nblieu);
	//tab = vector<vector<int>*>(nblieu);
	for (int i = 0; i < nblieu; i++)
	{
		tab[i] = vector<int>(i + 1);
		//tab[i] = new vector<int>(i + 1);
		for (int j = 0; j < i + 1; j++)
		{
			//*fichier >> (*tab[i])[j];
			*fichier >> (tab[i])[j];
		}
	}
}

void Distancier::reecrire_distancier(ofstream* fichier, int nJob)
{
	*fichier << coord_depot.first << " " << coord_depot.second << endl;

	for (int i = 0; i < nblieu - 1; i++)
	{
		for (int j = 0; j < i + 1; j++)
		{
			*fichier << tab[i][j] << "\t";
		} *fichier << endl;
	}
}

int Distancier::max_edge_on_sub_matrix(const vector<int>& batch)
{
	int max_dist = 0;
	int distance;
	for (int i = 0; i < batch.size(); i++)
	{
		distance = dist(index_manu(), batch[i]);
		if (distance > max_dist) {
			max_dist = distance;
		}

		for (int j = i + 1; j < batch.size(); j++)
		{
			distance = dist(batch[i], batch[j]);
			if (distance > max_dist) {
				max_dist = distance;
			}
		}
	}
	return max_dist;
}

int Distancier::dist_to_depot(int index)
{
	//return (*tab[index])[nbjob + 1];
	return (tab[index])[nbjob + 1];
}

int Distancier::dist_to_manu(int index)
{
	//return (*tab[index])[nbjob];
	return tab[index][nbjob];
}

int Distancier::dist(int ind1, int ind2) {
	if (ind2 > ind1) {
		//return (*tab[ind2])[ind1];
		return (tab[ind2][ind1]);
	}
	//return (*tab[ind1])[ind2];
	return (tab[ind1][ind2]);
}

ostream& operator<<(ostream& os, Distancier dist);
ostream& operator<<(ostream& os, Distancier dist)
{
	//for each (vector<int>* var in dist.tab)
	for (vector<int> var : dist.tab)
	{
		//for each (int nb in *var)
		for (int nb : var)
		{
			os << nb << " ";
		}
		os << endl;
	}
	return os;
}




int Distancier::distance_moyenne(int nb_point, int taille_carre)
{
	vector < pair<int, int>> coord(nb_point);
	for (int i = 0; i < nb_point; i++)
	{
		coord[i].first = rand() % (taille_carre + 1);
		coord[i].second = rand() % (taille_carre + 1);
	}

	double sum_distance = 0;
	for (int i = 0; i < nb_point; i++)
	{
		auto& p1 = coord[i];
		for (int j = 0; j < i; j++)
		{
			auto& p2 = coord[j];

			sum_distance += sqrt(
				pow(p1.first - p2.first, 2) +
				pow(p1.second - p2.second, 2)
			);
		}
	}

	double moyenne = sum_distance / (double)(((nb_point + 1)*nb_point) / 2);
	//cout << " Taille du carre : " << taille_carre << "  ==> " << moyenne << endl;
	cout << taille_carre << "\t" << moyenne << endl;

	return moyenne;
}