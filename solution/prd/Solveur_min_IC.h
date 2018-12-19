#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <tuple>
#include <random>
#include <chrono>
#include <list>

#include <ilcplex/ilocplex.h>

#include "Fct_lin.h"
#include "Instance.h"
#include "Branch_and_bound.h"
#include "batch_list.h"

using namespace std;

// TODO : remove unused attribute in Struct_Retour
struct Struct_Retour
{
	int cpl_time_ms;
	int IC;
	int PC;
	int IC_PC;
	int nb_tournee; // unused
	double av_tournee; // unused
	int nb_link; // unused
	double av_link; // unused
	double ecart_a_opti;

};

/**
	Enumerate the available algorithms
	TODO : replace with fonctors
**/
enum class algorithme_de_resolution
{
	heuristic_near = -1,
	EDD,
	EDDx4,
	near,
	nearx4,
	EDD_near,
	EDD_nearx4,
	CPLEX,
	B_and_B
};

class Solveur_min_IC
{
public:
	Solveur_min_IC(Instance &instance, batch_list& tab_batch);
	~Solveur_min_IC();

	//retourne le temps de CPLEX en ms 
	//int init_instance_avec_calcul_a_l_optima(Instance* inst, /*vector<int>rm,*/ vector<vector<int>> tab_Batch);

	//******
	//Utiliser pour g�n�rer les instances (fixe les due dates pour les rendre "int�ressante")

	//Modifie les due date des jobs de l'instance au vue de leurs fenetre de temps au depart

	/*
		Set the batch departure window
		TODO : Adapt the methode to handle job sheduling variations
	*/
	void ajust_due_date_with_windows_a_b(); 
	void ajust_due_date_with_approx(int nb_j_batch);
	//******

	static const int init_as_optima = 0;
	static const int comp_with_optima = 1;
	static const int nothing_special = 2;

	//***
	//Cette fonction reprend l'algorithme global de r�solution pr�sent� dans l'article
	//****
	Struct_Retour solve(algorithme_de_resolution approx_method = algorithme_de_resolution::EDD_nearx4, int mode = nothing_special);


	//Creation d'une fonction F_k en utilisant CPLEX
	//evalue exactement le PPCM du batch pour toute date de d�part comprise entre min_a et max_ avec un intervalle d�finie
	Fct_lin eval_exact_with_CPLEX(const vector<int>& bat, int depart_min_a, int depart_max_b, int intervalle);
	vector<int> solve_routing_1_batch(const vector<int>& bat, int departure_date);


	//****
	//element stocker d'apres la resolution exact
	vector<vector<int>> tab_Batch_ref;
	vector<Fct_lin> best_fct_par_batch;
	vector<pair<int, int>> a_b_par_batch;
	int global_optima;
	//****

private:

	Instance& inst;
	batch_list tab_Batch;

	int big_M = 1000;
	int depart_au_plus_tot = 0;

	IloEnv env;//variable d'environnement
	IloModel model; //mod�le, on l'associe directement � l'environnement
	IloCplex cplex; //solveur cplex

	int nJ;
	int nM;
	vector<int> list_ind;
	void init_model_and_data();
	void init_list_ind();


	IloArray<IloIntVarArray> C_ij;
	void init_Cij();

	IloExpr sum_CC; //somme des dates de d�part des batch
	void init_sum_CC();


	//var li� au routing 
	IloArray<IloBoolVarArray> x_ij;
	void init_xij();


	IloIntVarArray D_i;
	void init_Di();
	IloIntVarArray T_i;
	void init_Ti();


	//Variable utilisees pour traduire les fonctions lin�aires
	//en programme MILP
	//(Met �gualement � jour l'expression de PPC_M
	IloArray<IloIntVarArray>d_bi;
	IloArray<IloBoolVarArray>x_bi;
	void add_fct_lin_ctr(const Fct_lin& fct, int num_fct, const IloIntVar& C);

	IloExpr IC_WIP;
	void init_IC_WIP();
	IloExpr IC_FIN;
	void init_IC_FIN();
	IloExpr PPC_M;
	void init_PPC_M();
	//IloExpr Retard_incompressible;

	IloExpr RT; //routing cost
	void init_RT();


	IloObjective obj;

	//Contrainte
	void add_capa_machine_ctr();
	void add_gamme_ctr();
	void add_C00_vaut_0();

	//Routing ctr
	IloRangeArray ctr_departure_date;

	void add_simple_routing_ctr(vector<int>  date_depart = vector<int>());
	void add_flot_routing_ctr();
	void add_delivery_date_ctr();

	//ici on consid�re que dans les contraintes sur Ti et de departure date les date de d�part sont 
	//adapter pour simuler un depart � l'instant 0
	void add_departure_date_ctr();

	void modify_departure_date(const vector<int>& date_depart, int date_min_depart);

	void add_Ti_ctr(int depart_au_plus_tot);

	vector<int> extrait_chemin_model();

};