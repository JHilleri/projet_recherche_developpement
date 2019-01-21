#pragma once

#include <vector>
#include <iostream>

#include"Job.h"

using namespace std;

class Distancier
{
public:
	Distancier();
	Distancier(int nbj);
	Distancier(std::istream& input, int nb_job);
	~Distancier();

	void reecrire_distancier(ofstream* fichier, int nJob);

	int max_edge_on_sub_matrix(const vector<int>& batch_old);

	vector<vector<int>> tab;
	//vector<vector<int>*> tab;
	int nbjob;
	int nblieu;

	pair<int, int> coord_depot;

	int index_manu() const { return nbjob; }
	int index_depot() const { return nbjob + 1; }

	int dist_to_depot(int index);
	int dist_to_manu(int index);

	int dist(int ind1, int ind2) const;

	//calcul la distance moyenne entre 2 point tirées au
	//hasard dans un carré d'une certaine taille
	static int distance_moyenne(int nb_point, int taille_carre);


};

ostream& operator<<(ostream& os, Distancier dist);