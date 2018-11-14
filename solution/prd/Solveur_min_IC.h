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

using namespace std;

struct Struct_Retour
{
	int cpl_time_ms;
	int IC;
	int PC;
	int IC_PC;
	int nb_tournee;
	double av_tournee;
	int nb_link;
	double av_link;
	double ecart_a_opti;

};

class Solveur_min_IC
{
public:
	Solveur_min_IC();
	~Solveur_min_IC();

	//retourne le temps de CPLEX en ms 
	//int init_instance_avec_calcul_a_l_optima(Instance* inst, /*vector<int>rm,*/ vector<vector<int>> tab_Batch);

	//******
	//Utiliser pour g�n�rer les instances (fixe les due dates pour les rendre "int�ressante")

	//Modifie les due date des jobs de l'instance au vue de leurs fenetre de temps au depart 
	void ajust_due_date_with_windows_a_b(Instance* inst, /*vector<int>rm,*/ vector<vector<int>> tab_Batch);
	void ajust_due_date_with_approx(Instance* inst, /*vector<int>rm,*/ vector<vector<int>> tab_Batch, int nb_j_batch);
	//******


	static const int heuristic_near = -1;
	static const int EDD = 0;
	static const int EDDx4 = 1;
	static const int near = 2;
	static const int nearx4 = 3;
	static const int EDD_near = 4;
	static const int EDD_nearx4 = 5;
	static const int CPLEX = 6;
	static const int B_and_B = 7;

	static const int init_as_optima = 0;
	static const int comp_with_optima = 1;
	static const int nothing_special = 2;

	//***
	//Cette fonction reprend l'algorithme global de r�solution pr�sent� dans l'article
	//****
	Struct_Retour solve(Instance* inst, vector<vector<int>> tab_Batch, int approx_method = EDD_nearx4, int mode = nothing_special);


	//Creation d'une fonction F_k en utilisant CPLEX
	//evalue exactement le PPCM du batch pour toute date de d�part comprise entre min_a et max_ avec un intervalle d�finie
	Fct_lin eval_exact_with_CPLEX(Instance* inst, const vector<int>& bat, int depart_min_a, int depart_max_b, int intervalle);
	vector<int> solve_routing_1_batch(Instance* inst, const vector<int>& bat, int departure_date);


	//****
	//element stocker d'apres la resolution exact
	vector<vector<int>> tab_Batch_ref;
	vector<Fct_lin> best_fct_par_batch;
	vector<pair<int, int>> a_b_par_batch;
	int global_optima;
	//****

private:

	Instance * inst;
	vector<vector<int>>* tab_Batch;

	int big_M = 1000;
	int depart_au_plus_tot = 0;

	IloEnv env;//variable d'environnement
	IloModel model; //mod�le, on l'associe directement � l'environnement
	IloCplex cplex; //solveur cplex

	int nJ;
	int nM;
	vector<int> list_ind;
	void init_model_and_data(Instance* inst, vector<vector<int>>* tab_Batch = nullptr);
	void init_list_ind(Instance* inst, vector<vector<int>> tab_Batch);


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