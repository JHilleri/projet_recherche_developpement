#pragma once


#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>

#include "Instance.h"
#include "Fct_lin.h"


//
//Attention cancer!
//Ce qu'il faut savoir:
//
//Fct_lin generate_fct_with_branch_and_bound(Instance * inst, const vector<int>& batch, int min_a, int max_b);
//
//Genere une fonction F_k de manière exacte *point 
//

//****
//Structure pour branch and bound en exact
//(definition des bornes etc...)
//****




class Covering_tree {

public:

	Instance& inst;

	struct Edge {
		int n_1;
		int n_2;
		int cost;
	};

	Edge create_edge(int n1, int n2, int c) {
		Edge e;
		e.n_1 = n1;
		e.n_2 = n2;
		e.cost = c;
		return e;
	}

	map<int, vector<int>> link_with;

	void connect_ss_ens(vector<vector<int>>& sous_ens);

	void rec_reconstruct(vector<int>& sous_ens, int node, set<int>& already_assigned);
	vector<vector<int>> reconstruct_ss_ens_from_tab_link_with();

public:

	vector<Edge> edges;

	Covering_tree(Instance& instance) :inst{ instance } {};
	Covering_tree(Instance& inst, const vector<int>& batch_old);

	void delete_node(int indexnode);
	int sum_edge();
};

class Branch_and_bound {
public:
	Branch_and_bound(Instance& instance);
	static Tournee test;
	static int ind_test;
	static vector<bool> vec_bool;

	Fct_lin generate_fct_with_branch_and_bound(const vector<int>& batch_old, int min_a, int max_b);

	static void evaluation_of_instance_file(string path, string prefixe);

private:

	Instance& inst;

	int nb_resting_job;
	list<int> job_resting_for_assignement;

	Tournee tournee;

	int last_visited_site_index;

	Fct_lin upper_bound;
	//Covering_tree covering_tree;

	Fct_lin init_upper_bound(const vector<int>& batch_old, int min_a, int max_b);

	//void go_in(int new_node);
	//void go_out(int new_node, int old_node);

	class Vec_date_cost {
	public:
		int min_a; int max_b;
		vector<int> tab_date_cout;

		Vec_date_cost(int a, int b, int val = 0) {
			min_a = a; max_b = b;
			tab_date_cout = vector<int>(max_b - min_a + 1, val);
		}
		int get_cost(int date/*in [min_a, max_b]*/) { return tab_date_cout[date - min_a]; }
		void add_cost(int date, int cost) { tab_date_cout[date - min_a] += cost; }
		void put_date_to_minus_one(int date) { tab_date_cout[date - min_a] = -1; }
		bool date_is_valide(int date) {
			return tab_date_cout[date - min_a] != -1;
		}
	};

	void rec_branch_and_bound(Covering_tree covering_tree/*passage par copie*/,
		int branch_node, Vec_date_cost tab_date_cost, int time_travel_for_last_job);

	//
	Vec_date_cost min_assigment(Covering_tree& covering_tree, Vec_date_cost& tab_date_cost, int travel_time_to_last_job, int branch_node, const vector<vector<int>>& test);
	Vec_date_cost min_assigment(Covering_tree& covering_tree, Vec_date_cost& tab_date_cost, int travel_time_to_last_job, int branch_node);
	//fonction dedié a l'algo hongrois
	//selection zero couvrant et non couvrant

	static void aux_0_couvrant_non_couvrant(const vector<vector<int>>& matrix, vector<int>& zero_couvrant, vector<vector<int>>& zero_barre);
	static void aux_ligne_marquee_colonne_barree(vector<int>& zero_couvrant, vector < vector<int>>& zero_barre, vector<bool>& ligne_marquee, vector<bool>& colonne_marquee);
	static void aux_retrait_addition(vector<vector<int>>& matrix, vector<bool>& ligne_marque, vector<bool>& colonne_marque);

	static void aux_approfondisement_allocation_de_ligne(vector<vector<int>>& zero_barre, vector<int>& zero_couvrant, vector<int>& zero_ouvrant_col);
	static int rec_approfondissement_de_ligne(int index_ligne, vector<vector<int>>& zero_barre, vector<int>& zero_couvrant, vector<int>& zero_ouvrant_col, vector<int>& colonne_i_acces_via_lin);

};