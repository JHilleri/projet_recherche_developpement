#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>

#include "Instance.h"
#include "batch_list.h"

class Tournee
{
public:
	Tournee() { nb = 0; /*tab_c = new int[0];*/ };
	~Tournee() {};

	int* tab_c;//tab client
	int nb;//nb of client

		   //Operateur de voisinage
		   //SWAP
	void swap(int i, int j);
	void reverse_swap(int i, int j);

	//EBFSR
	void ebfsr(int i, int j);
	void reverse_ebfsr(int i, int j);



	bool equal(const Tournee& t) const;
	Tournee copy() const;
	void suppr();
	void print() const;

	int eval_routing_cost(const Instance& inst) const;

	static Tournee create_tournee(const vector<int>& vect);
	static Tournee create_tournee_vide(int nb_site);
	void add_job(int job);
	void suppr_job();
	static Tournee create_tournee_random(const vector<int>& vect);
	static Tournee create_tournee_EDD(const Instance& inst, const vector<int>& vect);
	static Tournee create_tournee_nearest_insertion(const Instance& inst, const vector<int>& vect);

	void inverse_tournee();
	void inverse_half_tournee();

	void suffle();
};

//represente une portion de la fonction linéaire par morceau
struct Link
{
	int t; //la date de départ
	int c; //cout de la tourné pour la date de départ t
	int alpha; //coefficient directeiur
	Tournee tournee; //ref vers la tournée donnant ce coeff

					 //indique si le segment est localement optimal
	bool is_locally_optimal;
};



class Fct_lin
{
public:
	Fct_lin() {};

	int min_a; //the minimal departure date possible
	int max_b; //the maximal departure date possible

	std::vector<Link> tab_link;

	int eval_max_b;
	int routing_cost;

	int aire_sous_la_courbe;


	//create a function lin for specific travel on specific intervalle
	static Fct_lin create_fct_lin(const Tournee& road, Instance& inst, int min_a, int min_b);
	static Fct_lin create_fct_max(int min_a, int min_b);

	//Create a fct lin which is the minimum of the two other
	static Fct_lin minimum_fct(const Fct_lin& f1, const Fct_lin& f2);

	static Link create_link(int t, int c, int alpha, const Tournee& tr);
	static Link copy_link(const Link& l);

	//eval a fct lin on a point t which is on is intervalle
	int eval_fct_lin(int t)const;

	Tournee found_tournee(int t)const;

	//eval a portion of a fct lin on a point t (this portion could be write as ax+b)
	static int eval_link(const Link & l, int t);
	int eval_alpha_lin(int t)const;
	//give the intersection abcisse of two portion of fonction
	double static intersection_t(const Link& l1, const Link& l2);


	int calcul_aire_sous_la_courbe(int new_min_a = numeric_limits<int>::min(), int new_min_b = numeric_limits<int>::min()) const;
	static double ratio_courbe_inf(const Fct_lin& fct_best, const Fct_lin& second);

	void print_fct_lin()const;

	static const int EDD = 0;
	static const int EDDx4 = 1;
	static const int near = 2;
	static const int nearx4 = 3;
	static const int EDD_near = 4;
	static const int EDD_nearx4 = 5;
	//
	static const int B_and_B = 7;

	static pair<int, int> a_b_inf(Instance& inst, const vector<int>& batch_old);

	static Fct_lin generate_fct(Instance& inst, const vector<int>& batch_old, int method,
		int min_a = numeric_limits<int>::min(), int max_b = numeric_limits<int>::min());

	static Fct_lin generate_pure_random_fct(Instance&  inst, const vector<int>& batch_old, int min_a, int max_b);

	static Fct_lin generate_fct_with_local_search(Instance&  inst, Tournee& init_tournee, int min_a, int max_b);
	static Fct_lin generate_fct_with_four_extrema(Instance&  inst, Tournee& init_tournee, int min_a, int max_b);

	static Fct_lin rec_generate_fct_with_total_enumeration(Instance&  inst, Fct_lin& best, Tournee& init_tournee, list<int>& batch_old, int size);
	static Fct_lin generate_fct_with_total_enumeration(Instance&  inst, vector<int>& batch_old, int min_a, int max_b);

	static int search_optima_with_big_neiborhood(Instance & inst, Tournee& tournee_c, int depart);

	static batch_list build_regular_batch_repartition(int nb_job, int nb_j_batch);

	static int compteur;
};














