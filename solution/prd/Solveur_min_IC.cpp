#include "Solveur_min_IC.h"
#include "cost_per_departure_date/edd_near_x4.h"
#include "cost_per_departure_date/edd_near.h"
#include "cost_per_departure_date/branch_and_bound_functor.h"

#include <numeric>
#include <functional>

Solveur_min_IC::Solveur_min_IC(Instance& instacne, batch_list& tab_batch)
	:inst{instacne}, tab_Batch{tab_batch}
{
	model_initialisation(); // pose du mod�le
	cplex.setOut(env.getNullStream());
}


Solveur_min_IC::~Solveur_min_IC()
{
	model.end();
	cplex.end();
	env.end();
}


//Utiliser pour g�n�rer les instances (fixe les due dates pour les rendre "int�ressante")
void Solveur_min_IC::ajust_due_date_with_windows_a_b()
{

	//Copie coller du model standart

	/////////////////////////////
	// Pose du modele
	model_initialisation();
	//Modele pos�
	///////////////////////////

	cplex.setOut(env.getNullStream());


	///////////////////////////
	// Premiere r�solution : min Cmax

	obj = IloMinimize(env, sum_CC, "obj");
	model.add(obj);

	//Capture du temps
	chrono::time_point<chrono::system_clock> current, start_t = chrono::system_clock::now();

	cplex.solve();

	//recup des dates minimums
	vector<pair<int, int>> tab_a_b(tab_Batch.size());

	int num_CC = -1;
	int num_b = 0;
	for (auto bat : tab_Batch)
	{
		num_CC += bat.size();
		tab_a_b[num_b].first = cplex.getValue(C_ij[num_CC][nM - 1]);
		num_b++;
	}
	////////////////////////

	///////////////////////////
	// Deuxi�me r�solution : min IC
	obj.setExpr(IC_WIP + IC_FIN + sum_CC / (cplex.getValue(sum_CC) * 10));

	cplex.solve();

	/**/
	/*std::cout << " !!! min IC !!!" << endl;
	std::cout << "====> Date de fin : " << cplex.getValue(C_ij[nJ - 1][nM - 1]) << endl;
	std::cout << "====> IC : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl;//*/

	//r�cup des dates max
	num_CC = -1;
	num_b = 0;
	for (auto& bat : tab_Batch)
	{
		num_CC += bat.size();
		tab_a_b[num_b].second = cplex.getValue(C_ij[num_CC][nM - 1]);
		num_b++;
	}
	//////////////////////////

	/*std::cout << "Intervalle de d�part pour chaque batch" << endl;
	for each (auto var in tab_a_b)
	{
	std::cout <<"a : " << var.first << " b : " << var.second << endl;;
	}
	std::cout << endl;//*/


	/////////////////////////
	//Modification des due dates au sein des batch de l'instance
	default_random_engine gen;
	gen = default_random_engine(42);
	uniform_int_distribution<int> distri_due_date;
	int max_time_tournee;

	num_b = 0; int a, b;
	for (vector<int> bat : tab_Batch)
	{
		max_time_tournee = inst.distancier.max_edge_on_sub_matrix(bat) * bat.size();

		a = tab_a_b[num_b].first;
		b = tab_a_b[num_b].second;

		distri_due_date = uniform_int_distribution<int>(
			a *0.8, //Quelque date peuvent tomber avant le d�part au plus tot
			a + (b - a + max_time_tournee) / 2);

		//std::cout << "B " << num_b << ": \t";
		for (int index_j : bat)
		{
			int n_dd = (int)distri_due_date(gen);
			inst.list_Job[index_j].due_d = n_dd;
			//std::cout << n_dd << "\t";
		}//std::cout << endl;
		num_b++;
	}//*/

	 //Fin modification des due dates
	 ////////////////////////

	model.end();
	cplex.end();
	env.end();
}

void Solveur_min_IC::model_initialisation()
{
	nM = inst.mMachine;
	nJ = std::accumulate(tab_Batch.begin(), tab_Batch.end(), 0, [](int a, const batch_old &b) {return a + b.size(); });

	this->env = IloEnv();
	model = IloModel(env); //mod�le, on l'associe directement � l'environnement
	cplex = IloCplex(model);//solveur, on l'associe directement au mod�le

	list_ind = vector<int>(nJ);
	int ind = 0;
	for (auto var : tab_Batch)
	{
		for (int x = 0; x < var.size(); x++)
		{
			list_ind[ind] = var[x];
			ind++;
		}
	}

	//init_Cij();
	C_ij = IloArray<IloIntVarArray>(env, nJ);
	for (int j = 0; j < nJ; j++) {
		C_ij[j] = IloIntVarArray(env, nM, 0, INT_MAX);
		for (int k = 0; k < nM; k++) {
			string str = "C_" + to_string(j) + "_" + to_string(k);
			C_ij[j][k].setName(str.c_str());
		}
	}

	//add_capa_machine_ctr();
	for (int i = 1; i < nJ; i++)
	{
		int i_job = list_ind[i];
		Job_old& job = inst.list_Job[i_job];
		for (int j = 0; j < nM; j++)
		{
			model.add(C_ij[i - 1][j] + job.p[j] <= C_ij[i][j]);
		}
	}

	//add_gamme_ctr();
	for (int i = 0; i < nJ; i++)
	{
		int i_job = list_ind[i];
		Job_old& job = inst.list_Job[i_job];
		for (int j = 1; j < nM; j++)
		{
			model.add(C_ij[i][j - 1] + job.p[j] <= C_ij[i][j]);
		}
	}

	//add_C00_vaut_0();
	model.add(C_ij[0][0] == inst.list_Job[list_ind[0]].p[0]);

	//init_IC_WIP();
	IC_WIP = IloExpr(env);
	for (int i = 0; i < nJ; i++)
	{
		int i_job = list_ind[i];
		Job_old& job = inst.list_Job[i_job];
		for (int j = 0; j < nM - 1; j++)
		{
			IC_WIP += job.thWIP[j] * (C_ij[i][j + 1] - job.p[j + 1] - C_ij[i][j]);
		}
	}

	//init_IC_FIN();
	IC_FIN = IloExpr(env);
	int nb_job_avant = 0;
	for (auto var : tab_Batch)
	{
		int i_last_job = var.back();
		Job_old& last_job = inst.list_Job[i_last_job];
		for (int i = 0; i < var.size() - 1; i++)
		{
			int i_job = var[i];
			Job_old& job = inst.list_Job[i_job];

			IC_FIN += job.thFIN *
				// cpl_time du dernier job du batch sur la dernier machine
				(C_ij[nb_job_avant + var.size() - 1][nM - 1] - last_job.p[nM - 1] - C_ij[nb_job_avant + i][nM - 1]);
		}
		nb_job_avant += var.size();
	}
	//init_sum_CC();
	sum_CC = IloExpr(env);

	int num_CC = -1;
	for (auto var : tab_Batch)
	{
		num_CC += var.size();
		sum_CC += C_ij[num_CC][nM - 1];
	}
}

//Utiliser pour g�n�rer les instances (fixe les due dates pour les rendre "int�ressante")

void Solveur_min_IC::ajust_due_date_with_approx(int nb_j_batch)
{
	double coeff_from_machine_depart = 1.0;

	double coeff_from_machine_length = 1.0;

	switch (inst.mMachine)
	{
	case 5:
		coeff_from_machine_depart = 1.1;

		coeff_from_machine_length = 0.2;
		break;
	case 10:
		coeff_from_machine_depart = 1.20;

		coeff_from_machine_length = 0.3;
		break;
	case 20:
		coeff_from_machine_depart = 1.25;

		coeff_from_machine_length = 0.4;
		break;
	case 40:
		coeff_from_machine_depart = 1.3;

		coeff_from_machine_length = 0.5;
		break;
	default:
		coeff_from_machine_depart = 1.0;

		coeff_from_machine_length = 1.0;
		break;
	}


	double average_pij = 0.0;
	for (int i = 0; i < inst.nJob; i++)
	{
		for (int j = 0; j < inst.mMachine; j++)
		{
			average_pij += inst.list_Job[i].p[j];
		}
	}
	average_pij /= (double)(inst.nJob * inst.mMachine);

	average_pij *= coeff_from_machine_depart;



	/////////////////////////
	//Modification des due dates au sein des batch de l'instance
	default_random_engine gen;
	gen = default_random_engine(42);
	uniform_int_distribution<int> distri_due_date;

	int num_b = 0;

	int min_w = (inst.mMachine - 1)*average_pij;
	int max_w;

	double average_time_tournee = average_pij * ((inst.nJob) / tab_Batch.size()) * 2;
	double windows_size = average_time_tournee;
	//std::cout << "average_pij : " << average_pij << "\t" << "average_time_tournee : " << average_time_tournee << endl;

	for (vector<int> bat : tab_Batch)
	{
		min_w += tab_Batch[num_b].size() * average_pij;

		windows_size += coeff_from_machine_length * average_time_tournee;

		//*** max_v
		max_w = min_w + windows_size;

		distri_due_date = uniform_int_distribution<int>(min_w, max_w);

		//std::cout << "B " << num_b << ": \t";
		for (int index_j : bat)
		{
			int n_dd = (int)distri_due_date(gen);
			inst.list_Job[index_j].due_d = n_dd;
			//std::cout << "min_w : " << min_w << "\t" << "max_w : " << max_w << "\t" << "new_due_d : " << n_dd << endl;
		}//std::cout << endl;
		num_b++;
	}//*/

	 //Fin modification des due dates
	 ////////////////////////
}



//***
//Cette fonction reprend l'algorithme global de r�solution pr�sent� dans l'article
//****
Struct_Retour Solveur_min_IC::solve(algorithme_de_resolution approx_method, int mode)
{

	if (mode == init_as_optima) {
		best_fct_par_batch = vector<Fct_lin>(tab_Batch.size());
	}

	std::cout << "----------" << endl << "Chargement du mod�le initial et resolution" << endl << "----------" << endl << endl;

	/////////////////////////////
	// Pose du modele
	model_initialisation();
	//Modele pos�
	///////////////////////////



	vector<pair<int, int>> tab_a_b{ tab_Batch.size() };

	IloObjective obj = IloMinimize(env, sum_CC, "obj");
	///////////////////////////
	// Premiere r�solution : min Cmax
	// calcule de a
	chrono::time_point<chrono::system_clock> start_t = chrono::system_clock::now();
	{
		model.add(obj);

		//Capture du temps

		cplex.solve();
		//recup des dates minimums
		// num_CC equivalent � J_k
		int num_CC = -1;
		int num_b = 0;
		for (auto bat : tab_Batch)
		{
			num_CC += bat.size();
			tab_a_b[num_b].first = cplex.getValue(C_ij[num_CC][nM - 1]);
			num_b++;
		}
	}
	const auto earlyer_departur_date{ cplex.getValue(C_ij[nJ - 1][nM - 1]) };
	const auto earlyer_departur_inventory_cost{ cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) };
	///////////////////////////
	// Deuxi�me r�solution : min IC
	// calcule de b
	{
		obj.setExpr(IC_WIP + IC_FIN + sum_CC / (cplex.getValue(sum_CC) * 5));

		cplex.solve();
		//r�cup des dates max
		int num_CC = -1;
		int num_b = 0;
		for (auto bat : tab_Batch)
		{
			num_CC += bat.size();
			tab_a_b[num_b].second = cplex.getValue(C_ij[num_CC][nM - 1]);
			num_b++;
		}
	}
	const auto minimal_inventory_end_date{ cplex.getValue(C_ij[nJ - 1][nM - 1]) };
	const auto minimal_inventory_inventory_cost{ cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) };


	std::cout << "Date de fin le plus tot possible calculee :" << endl;
	std::cout << "====> Date de fin : " << earlyer_departur_date << endl;
	std::cout << "====> IC : " << earlyer_departur_inventory_cost << endl << endl;
	std::cout << "Date de fin pour inventaire minimum calculee :" << endl;
	std::cout << "====> Date de fin : " << minimal_inventory_end_date << endl;
	std::cout << "====> IC : " << minimal_inventory_inventory_cost << endl << endl;

	//////////////////////////

	if (mode == init_as_optima) {
		a_b_par_batch = tab_a_b;
	}

	print_departure_windows(std::cout, tab_a_b);

	std::cout << "----------" << endl << "Calcul des profils de fonction de livraison F_k pour chaque batch" << endl << "----------" << endl << endl;

	/////////////////////////
	// Estimation du co�t d'une tourn�e en fonction de sa date 
	//pour chaque batch


	//
	// Attention, comprendre comment le fonction F_k sont cr�er en d�tail
	// n'est pas demand�.
	//

	// ==> tra�age d'une fonction lin�aire par morceau
	vector<Fct_lin> list_fct_lin;

	int min_a, max_b;
	int cpt_time_approx = 0;

	int average_time = 0;

	chrono::time_point<chrono::system_clock> current;
	for (int i = 0; i < tab_Batch.size(); i++)
	{
		min_a = tab_a_b[i].first;
		max_b = tab_a_b[i].second;


		//!!!!!!!
		//*******
		Fct_lin best = Fct_lin::create_fct_max(0, 0);
		Fct_lin f_EDD = Fct_lin::create_fct_max(0, 0);
		Fct_lin f_near = Fct_lin::create_fct_max(0, 0);
		Tournee t_EDD = Tournee::create_tournee_vide(0);
		Tournee t_near = Tournee::create_tournee_vide(0);

		Solveur_min_IC solve_CPLEX{ inst, tab_Batch };

		start_t = chrono::system_clock::now();


		switch (approx_method) // par defaut du prototype EDD_nearx4
		{
		case algorithme_de_resolution::heuristic_near:
			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			best = Fct_lin::create_fct_lin(t_near, inst, min_a, max_b);
			break;
		case algorithme_de_resolution::EDD:
			t_EDD = Tournee::create_tournee_EDD(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_local_search(inst, t_EDD, min_a, max_b);
			break;
		case algorithme_de_resolution::EDDx4:
			t_EDD = Tournee::create_tournee_EDD(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_four_extrema(inst, t_EDD, min_a, max_b);
			break;
		case algorithme_de_resolution::near:
			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_local_search(inst, t_near, min_a, max_b);
			break;
		case algorithme_de_resolution::nearx4:
			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_four_extrema(inst, t_near, min_a, max_b);
			break;
		case algorithme_de_resolution::EDD_near: // utile
			best = cost_per_departure_date::edd_near{}(inst, tab_Batch[i], min_a, max_b);
			break;
		case algorithme_de_resolution::EDD_nearx4:// utile
			best = cost_per_departure_date::edd_near_x4{}(inst, tab_Batch[i], min_a, max_b);
			break;
		case algorithme_de_resolution::CPLEX:
			best = solve_CPLEX.eval_exact_with_CPLEX(tab_Batch[i], min_a, max_b, 1);
			break;
		case algorithme_de_resolution::B_and_B: // utile
			best = cost_per_departure_date::branch_and_bound{}(inst, tab_Batch[i], min_a, max_b);
			break;
		default: std::cout << "Method non reconnu" << endl;
			break;
		}

		current = chrono::system_clock::now();
		int time = chrono::duration_cast<chrono::milliseconds>(current - start_t).count();
		average_time += time;

		if (mode == init_as_optima) {
			best_fct_par_batch[i] = best;
		}

		list_fct_lin.push_back(best);

	}
	std::cout << "DONE" << endl << endl;
	//
	/////////////////////////

	int i = 0; int num_b = 0;
	std::cout << "Affichage des fonctions lineaires pour chaque batch." << endl << "La methode utilisee est celle passee en parametre.";
	std::cout << "Legende: " << endl;
	std::cout << "1) Date de depart du segment\n \t2) std::cout initial\n\t\t3) Coeff directeur\n\t\t\t4) Tournee associee" << endl;
	for (auto fct : list_fct_lin)
	{
		std::cout << "--- Batch " << num_b << ":  Size batch :  " << tab_Batch[i].size() << endl; i++;
		fct.print_fct_lin();
		num_b++;
	}
	std::cout << endl;//*/


	std::cout << "----------" << endl << "Ajout des profils au F_k au modele et resolution globale" << endl << "----------" << endl << endl;

	//////////////////////
	//Ajout du co�t des tourn�es aux mod�les
	//(Traduction des fonction lin en mod�le math)

	PPC_M = IloExpr(env);
	//Retard_incompressible = IloExpr(env);

	//on a un jeu de variable par batch / fonction lineaire
	d_bi = IloArray<IloIntVarArray>(env, list_fct_lin.size());
	x_bi = IloArray<IloBoolVarArray>(env, list_fct_lin.size());

	{
		int num_CC = -1;
		int num_b = 0;
		for (auto fct : list_fct_lin)
		{
			num_CC += tab_Batch[num_b].size();
			add_fct_lin_ctr(fct, num_b, C_ij[num_CC][nM - 1]); //<== ajout ici
															   //des contraintes et de valeurs de variable
			num_b++;
		}
	}
	//
	///////////////////////

	// Deuxi�me r�solution : min IC + PPC_M
	obj.setExpr(IC_WIP + IC_FIN + PPC_M);

	cplex.solve();

	//Capture du temps total
	current = chrono::system_clock::now();
	cpt_time_approx += chrono::duration_cast<chrono::milliseconds>(current - start_t).count();

#ifdef DEBUG
	cplex.exportModel("CPLEX.lp");// debug
#endif // DEBUG


	int result = cplex.getValue(PPC_M) + cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN);

	if (mode == init_as_optima) {
		global_optima = result;
	}

	double ecart_a_l_opti = -42.0;
	if (comp_with_optima) {
		ecart_a_l_opti = (result - global_optima)*100.0 / (global_optima);
	}

	//
	// Affichage tourn�e choisit et comparaison batch
	//des fonctions optis
	//

	double moyenne_ecart_a_l_optima = 0;

	{
		int num_CC = -1;
		int num_b = 0;

		std::cout << endl << "Solution optimal trouvee :" << endl << "Date de depart et tournee par batch:" << endl;

		for(auto fct : list_fct_lin)
		{
			num_CC += tab_Batch[num_b].size();

			int date_depart = cplex.getValue(C_ij[num_CC][nM - 1]);
			Tournee t = fct.found_tournee(date_depart);



			std::cout << "Batch " << num_b + 1 << " Date de depart :  " << date_depart << "  Tournee: {"; 
			t.print(); 
			std::cout << "}" << endl <<
				"\tCout du trajet: " << t.eval_routing_cost(inst) << "  Cout du retard: " << fct.eval_fct_lin(date_depart) - t.eval_routing_cost(inst) << endl;

			num_b++;
		}
		std::cout << endl;
	}

	for (auto fct : list_fct_lin)
	{
		for (auto link : fct.tab_link)
		{
			link.tournee.suppr();
		}
	}

	Struct_Retour retour;

	retour.cpl_time_ms = cpt_time_approx;
	retour.IC = cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN);
	retour.PC = cplex.getValue(PPC_M);
	retour.IC_PC = cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) + cplex.getValue(PPC_M);

	std::cout << "Cout d'inventaire total : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl
		<< "Cout de routing toatal : " << cplex.getValue(PPC_M) << endl
		<< "Cout total : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) + cplex.getValue(PPC_M) << endl << endl;

	model.end();
	cplex.end();
	env.end();

	return retour;
}

Struct_Retour Solveur_min_IC::operator()(cost_per_departure_date::algorithm &algoritm, int mode)
{
	chrono::time_point<chrono::system_clock> start_t = chrono::system_clock::now();
	IloObjective obj = IloMinimize(env, sum_CC, "obj");

	vector<pair<int, int>> tab_a_b = get_departure_windows(obj);
	print_departure_windows(std::cout, tab_a_b);
	
	std::cout << "----------" << endl << "Calcul des profils de fonction de livraison F_k pour chaque batch" << endl << "----------" << endl << endl;

	/////////////////////////
	// Estimation du co�t d'une tourn�e en fonction de sa date 
	//pour chaque batch
	// ==> tra�age d'une fonction lin�aire par morceau
	vector<Fct_lin> list_fct_lin{ tab_Batch.size() };

	for (int i = 0; i < tab_Batch.size(); i++)
	{
		auto [min_a, max_b] = tab_a_b[i];
		list_fct_lin[i] = algoritm(inst, tab_Batch[i], min_a, max_b);
		if (mode == init_as_optima) 
			best_fct_par_batch[i] = list_fct_lin[i];
	}
	//
	/////////////////////////

	std::cout << "Affichage des fonctions lineaires pour chaque batch." << endl << "La methode utilisee est celle passee en parametre.";
	std::cout << "Legende: " << endl;
	std::cout << "1) Date de depart du segment\n \t2) std::cout initial\n\t\t3) Coeff directeur\n\t\t\t4) Tournee associee" << endl;
	for (int i{ 0 }; i < list_fct_lin.size(); ++i)
	{
		std::cout << "--- Batch " << i << ":  Size batch :  " << tab_Batch[i].size() << endl; i++;
		list_fct_lin[i].print_fct_lin();
	}
	std::cout << endl;

	std::cout << "----------" << endl << "Ajout des profils au F_k au modele et resolution globale" << endl << "----------" << endl << endl;

	//////////////////////
	//Ajout du co�t des tourn�es aux mod�les
	//(Traduction des fonction lin en mod�le math)

	PPC_M = IloExpr(env);
	//Retard_incompressible = IloExpr(env);

	//on a un jeu de variable par batch / fonction lineaire
	d_bi = IloArray<IloIntVarArray>(env, list_fct_lin.size());
	x_bi = IloArray<IloBoolVarArray>(env, list_fct_lin.size());

	for (int num_b{ 0 }, num_CC{ -1 }; num_b < list_fct_lin.size(); ++num_b)
	{
		num_CC += tab_Batch[num_b].size();
		add_fct_lin_ctr(list_fct_lin[num_b], num_b, C_ij[num_CC][nM - 1]); //<== ajout ici
															//des contraintes et de valeurs de variable
	}
	//
	///////////////////////

	// Deuxi�me r�solution : min IC + PPC_M
	obj.setExpr(IC_WIP + IC_FIN + PPC_M);

	cplex.solve();

	//Capture du temps total
	chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();
	int cpt_time_approx = chrono::duration_cast<chrono::milliseconds>(current - start_t).count();

#ifdef DEBUG
	cplex.exportModel("CPLEX.lp");// debug
#endif // DEBUG

	//
	// Affichage tourn�e choisit et comparaison batch
	//des fonctions optis
	//
	std::cout << endl << "Solution optimal trouvee :" << endl << "Date de depart et tournee par batch:" << endl;

	for (int num_b{ 0 }, num_CC{ -1 }; num_b < list_fct_lin.size(); ++num_b)
	{
		num_CC += tab_Batch[num_b].size();

		int date_depart = cplex.getValue(C_ij[num_CC][nM - 1]);
		Tournee t = list_fct_lin[num_b].found_tournee(date_depart);

		std::cout << "Batch " << num_b + 1 << " Date de depart :  " << date_depart << "  Tournee: {";
		t.print();
		std::cout << "}" << endl <<
			"\tCout du trajet: " << t.eval_routing_cost(inst) << "  Cout du retard: " << list_fct_lin[num_b].eval_fct_lin(date_depart) - t.eval_routing_cost(inst) << endl;
	}
	std::cout << endl;

	Struct_Retour retour;

	retour.cpl_time_ms = cpt_time_approx;
	retour.IC = cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN);
	retour.PC = cplex.getValue(PPC_M);
	retour.IC_PC = cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) + cplex.getValue(PPC_M);

	return retour;
}

vector<pair<int, int>> Solveur_min_IC::get_departure_windows(IloObjective & obj)
{
	vector<pair<int, int>> tab_a_b{ tab_Batch.size() };
	///////////////////////////
	// Premiere r�solution : min Cmax
	// calcule de a
	model.add(obj);
	cplex.solve();
	//recup des dates minimums
	// num_CC equivalent � J_k
	for (int num_b{ 0 }, num_CC{ -1 }; num_b < tab_Batch.size(); ++num_b)
	{
		num_CC += tab_Batch[num_b].size();
		tab_a_b[num_b].first = cplex.getValue(C_ij[num_CC][nM - 1]);
	}
	const auto earlyer_departur_date{ cplex.getValue(C_ij[nJ - 1][nM - 1]) };
	const auto earlyer_departur_inventory_cost{ cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) };
	///////////////////////////
	// Deuxi�me r�solution : min IC
	// calcule de b
	obj.setExpr(IC_WIP + IC_FIN + sum_CC / (cplex.getValue(sum_CC) * 5));
	cplex.solve();
	//r�cup des dates max
	for (int num_b{ 0 }, num_CC{ -1 }; num_b < tab_Batch.size(); ++num_b)
	{
		num_CC += tab_Batch[num_b].size();
		tab_a_b[num_b].second = cplex.getValue(C_ij[num_CC][nM - 1]);
	}
	const auto minimal_inventory_end_date{ cplex.getValue(C_ij[nJ - 1][nM - 1]) };
	const auto minimal_inventory_inventory_cost{ cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) };


	std::cout << "Date de fin le plus tot possible calculee :" << endl;
	std::cout << "====> Date de fin : " << earlyer_departur_date << endl;
	std::cout << "====> IC : " << earlyer_departur_inventory_cost << endl << endl;
	std::cout << "Date de fin pour inventaire minimum calculee :" << endl;
	std::cout << "====> Date de fin : " << minimal_inventory_end_date << endl;
	std::cout << "====> IC : " << minimal_inventory_inventory_cost << endl << endl;

	return tab_a_b;
}

void print_departure_windows(std::ostream & output, std::vector<std::pair<int, int>> &tab_a_b)
{
	int num_b = 0;
	output << "Pour chaque batch, intervalle des dates de depart interressante:" << endl;
	for (auto var : tab_a_b)
	{
		output << "Batch " << num_b + 1 << ": [\t" << var.first << ",\t" << var.second << "\t]";

		num_b++;
		output << endl;
	}
	output << endl;
}




void Solveur_min_IC::init_model_and_data()
{
	nM = inst.mMachine;
	//nJ = std::accumulate(tab_Batch.begin(), tab_Batch.end(), 0);

	this->env = IloEnv();
	model = IloModel(env); //mod�le, on l'associe directement � l'environnement
	cplex = IloCplex(model);//solveur, on l'associe directement au mod�le
}

void Solveur_min_IC::init_list_ind()
{
	int size = 0;
	for (auto var : tab_Batch)
	{
		size += var.size();
	}

	list_ind = vector<int>(size);
	int ind = 0;
	for (auto var : tab_Batch)
	{
		for (int x = 0; x < var.size(); x++)
		{
			list_ind[ind] = var[x];
			ind++;
		}
	}
}


void Solveur_min_IC::init_Cij()
{
	string str;
	C_ij = IloArray<IloIntVarArray>(env, nJ);
	for (int j = 0; j < nJ; j++) {
		C_ij[j] = IloIntVarArray(env, nM, 0, INT_MAX);
		for (int k = 0; k < nM; k++) {
			str = "C_" + to_string(j) + "_" + to_string(k);
			C_ij[j][k].setName(str.c_str());
		}
	}
}

void Solveur_min_IC::init_xij()
{
	string str;
	x_ij = IloArray<IloBoolVarArray>(env, nJ + 2);

	for (int i = 0; i < nJ + 2; i++)
	{
		x_ij[i] = IloBoolVarArray(env, nJ + 2);
		for (int j = 0; j < nJ + 2; j++)
		{
			str = "x_" + to_string(i) + "_" + to_string(j);
			x_ij[i][j].setName(str.c_str());
		}
	}
}

void Solveur_min_IC::init_Di()
{
	string str;
	D_i = IloIntVarArray(env, nJ, 0, INT_MAX);
	for (int j = 0; j < nJ; j++) {
		str = "D_" + to_string(j);
		D_i[j].setName(str.c_str());
	}

}

void Solveur_min_IC::init_Ti()
{
	string str;
	T_i = IloIntVarArray(env, nJ, 0, INT_MAX);
	for (int j = 0; j < nJ; j++) {
		str = "T_" + to_string(j);
		T_i[j].setName(str.c_str());
	}
}


void Solveur_min_IC::init_IC_WIP()
{
	int i_job;

	IC_WIP = IloExpr(env);
	for (int i = 0; i < nJ; i++)
	{
		i_job = list_ind[i];
		Job_old& job = inst.list_Job[i_job];
		for (int j = 0; j < nM - 1; j++)
		{
			IC_WIP += job.thWIP[j] * (C_ij[i][j + 1] - job.p[j + 1] - C_ij[i][j]);
		}
	}
}

void Solveur_min_IC::init_IC_FIN()
{
	int i_job;

	IC_FIN = IloExpr(env);
	int i_last_job;
	
	int nb_job_avant = 0;
	for (auto var : tab_Batch)
	{
		i_last_job = var.back();
		Job_old& last_job = inst.list_Job[i_last_job];
		for (int i = 0; i < var.size() - 1; i++)
		{
			i_job = var[i];
			Job_old& job = inst.list_Job[i_job];

			IC_FIN += job.thFIN *
				// cpl_time du dernier job du batch sur la dernier machine
				(C_ij[nb_job_avant + var.size() - 1][nM - 1] - last_job.p[nM - 1] - C_ij[nb_job_avant + i][nM - 1]);
		}
		nb_job_avant += var.size();
	}
}

void Solveur_min_IC::init_PPC_M()
{
	int i_job;

	PPC_M = IloExpr(env);
	for (int i = 0; i < nJ; i++)
	{
		i_job = list_ind[i];
		Job_old &job = inst.list_Job[i_job];
		PPC_M += T_i[i] * job.piM;
	}

}

void Solveur_min_IC::init_RT()
{
	RT = IloExpr(env);

	int index_depot_distancier = inst.distancier.index_manu();
	for (int i = 1; i <= nJ; i++)
	{
		RT += x_ij[0][i] * inst.distancier.dist(index_depot_distancier, list_ind[i - 1]);

		//on ne prend pas en compte l'aller au d�p�t
		//RT = x_ij[i][nJ+1] * inst->distancier.dist(index_depot_distancier, list_ind[i-1]);
	}

	for (int i = 1; i <= nJ; i++)
	{
		for (int j = 1; j <= nJ; j++)
		{
			if (i != j) {
				RT += x_ij[i][j] * inst.distancier.dist(list_ind[i - 1], list_ind[j - 1]);
			}
		}
	}
}

void Solveur_min_IC::init_sum_CC()
{
	sum_CC = IloExpr(env);

	int num_CC = -1;
	for (auto var : tab_Batch)
	{
		num_CC += var.size();
		sum_CC += C_ij[num_CC][nM - 1];
	}
}

void Solveur_min_IC::add_capa_machine_ctr()
{
	int i_job;
	for (int i = 1; i < nJ; i++)
	{
		i_job = list_ind[i];
		Job_old& job = inst.list_Job[i_job];
		for (int j = 0; j < nM; j++)
		{
			model.add(C_ij[i - 1][j] + job.p[j] <= C_ij[i][j]);
		}
	}
}

void Solveur_min_IC::add_gamme_ctr()
{
	int i_job;
	for (int i = 0; i < nJ; i++)
	{
		i_job = list_ind[i];
		Job_old& job = inst.list_Job[i_job];
		for (int j = 1; j < nM; j++)
		{
			model.add(C_ij[i][j - 1] + job.p[j] <= C_ij[i][j]);
		}
	}
}

void Solveur_min_IC::add_C00_vaut_0()
{
	model.add(C_ij[0][0] == inst.list_Job[list_ind[0]].p[0]);
}

void Solveur_min_IC::add_simple_routing_ctr(vector<int> date_depart)
{

	add_flot_routing_ctr();
	add_delivery_date_ctr();
	add_departure_date_ctr();
}

void Solveur_min_IC::add_flot_routing_ctr()
{
	//
	// L'index 0 (x_ij[0][-]) dans le mod�le correspond au lieu de d�part de la tourn�e
	// L'index nJ+1 (x_ij[-][nJ+1]) dans le mod�le correspond au lieu de d'arriv�e de la tourn�e
	// (Note : c'est deux lieux sont en fait le m�me d�pot
	//

	Distancier& dist = inst.distancier;

	IloExpr nb_depart = IloExpr(env);
	IloExpr nb_arrivee = IloExpr(env);

	for (int i = 1; i <= nJ; i++)
	{
		nb_depart += x_ij[0][i];
		nb_arrivee += x_ij[i][nJ + 1];
	}

	//Cas avec batch connu
	model.add(nb_depart == tab_Batch.size());
	model.add(nb_arrivee == tab_Batch.size());


	// un successeur et un pr�d�cesseur
	IloExpr pred_i = IloExpr(env);
	IloExpr succ_i = IloExpr(env);

	//cas batch connu
	int start_batch = 1;
	int size_batch;
	for (auto bat : tab_Batch)
	{

		size_batch = bat.size();
		for (int i = start_batch; i < start_batch + size_batch; i++)
		{
			pred_i = x_ij[0][i];
			succ_i = x_ij[i][nJ + 1];

			for (int j = start_batch; j < start_batch + size_batch; j++)
			{
				if (i != j) {
					pred_i += x_ij[j][i];
					succ_i += x_ij[i][j];
				}
			}

			model.add(pred_i == 1);
			model.add(succ_i == 1);

		}
		start_batch += size_batch;
	}
}

void Solveur_min_IC::add_delivery_date_ctr()
{
	Distancier& dist = inst.distancier;

	int start_batch = 1;
	int size_batch;

	int dist_i_j;

	start_batch = 1;
	//size_batch;
	int i_CC = -1;
	int num_b = 0;
	IloIntVar* CC;
	for (auto bat : tab_Batch)
	{
		size_batch = bat.size();

		for (int i = start_batch; i < start_batch + size_batch; i++)
		{
			//std::cout << dist.dist(dist.index_manu(), list_ind[i - 1]) << "\t";
			for (int j = start_batch; j < start_batch + size_batch; j++)
			{
				if (i != j) {

					//std::cout << dist.dist(list_ind[i - 1], list_ind[j - 1]) << "\t";

					dist_i_j = dist.dist(list_ind[i - 1], list_ind[j - 1]);

					model.add(D_i[j - 1] >= D_i[i - 1] + dist_i_j - big_M * (1 - x_ij[i][j]));

					//brisage de sous tours si les jobs sont au m�me endroit
					if (dist_i_j == 0 && i < j) {
						model.add(x_ij[j][i] == 0);
					}
				}
				//else { std::cout << "-\t"; }
			}//std::cout << endl;
		}
		start_batch += size_batch;
		num_b++;
	}
}

//on fait l'hypoth�se que valeurs des instances sont adapt�s pour que le 
//l'on puisse faire comme si le v�hicule part� � t = 0
void Solveur_min_IC::add_departure_date_ctr()
{
	Distancier& dist = inst.distancier;


	ctr_departure_date = IloRangeArray(env, nJ);


	int start_batch = 1;
	int size_batch;

	int i_CC = -1;
	int num_b = 0;
	IloIntVar* CC;
	for (auto bat : tab_Batch)
	{
		size_batch = bat.size();

		for (int i = start_batch; i < start_batch + size_batch; i++)
		{
			ctr_departure_date[i - 1] = IloRange(env, /*date_depart[num_b]*/ 0 + dist.dist(dist.index_manu(), list_ind[i - 1]),
				D_i[i - 1], IloInfinity);

			model.add(ctr_departure_date[i - 1]);

		}
		start_batch += size_batch;
		num_b++;
	}
}

void Solveur_min_IC::modify_departure_date(const vector<int>& date_depart, int date_min_depart)
{
	Distancier& dist = inst.distancier;

	int start_batch = 1;
	int size_batch;

	int i_CC = -1;
	int num_b = 0;

	for (auto bat : tab_Batch)
	{
		size_batch = bat.size();

		for (int i = start_batch; i < start_batch + size_batch; i++)
		{

			ctr_departure_date[i - 1].setLb(date_depart[num_b] - date_min_depart + dist.dist(dist.index_manu(), list_ind[i - 1]));

		}
		start_batch += size_batch;
		num_b++;
	}
}


void Solveur_min_IC::add_Ti_ctr(int depart_au_plus_tot)
{
	int i_job;
	Job_old* job;

	for (int i = 0; i < nJ; i++)
	{
		model.add(T_i[i] >= D_i[i] + depart_au_plus_tot - inst.list_Job[list_ind[i]].due_d);
	}
}

vector<int> Solveur_min_IC::extrait_chemin_model()
{
	auto swarzy = vector<int>();

	int val2;
	int source = 0, arrive;
	int size = -1;
	//std::cout << "D ";
	while (source != x_ij.getSize() - 1) {

		arrive = 0;

		do {
			arrive++;
			try
			{
				val2 = cplex.getValue(x_ij[source][arrive]);
			}
			catch (const IloException &e)
			{
				val2 = 0;
			}
		} while (val2 != 1);

		size++;

		if (arrive != x_ij.getSize() - 1) swarzy.push_back(list_ind[arrive - 1]);// std::cout << arrive - 1 << " ";

		source = arrive;
	}
	/*std::cout << "A\n  ";
	if (size != x_ij.getSize() - 2) {
	std::cout << "Size: " << size << " !!! ERREUR !!! " << endl;
	}
	else {
	std::cout << "Size: " << size << " OK " << endl;
	}*/

	return swarzy;
}

void Solveur_min_IC::add_fct_lin_ctr(const Fct_lin& fct, int num_fct, const IloIntVar& C)
{
	int nb_link = fct.tab_link.size();
	d_bi[num_fct] = IloIntVarArray(env, nb_link, 0, INT_MAX);
	x_bi[num_fct] = IloBoolVarArray(env, nb_link + 1); // permet de generer le max_b de mani�re propre

	for (size_t i = 0; i < nb_link; i++)
	{
		d_bi[num_fct][i].setName(("d_" + to_string(num_fct) + to_string(i)).c_str());
		x_bi[num_fct][i].setName(("x_" + to_string(num_fct) + to_string(i)).c_str());
	}
	x_bi[num_fct][nb_link].setName(("x_" + to_string(num_fct) + "_maxb").c_str());

	IloExpr sum_xt_d = IloExpr(env);
	// C <= Somme( x_ks * t_ks + d_ks )
	for (int i = 0; i < nb_link; i++)
	{
		sum_xt_d += x_bi[num_fct][i] * fct.tab_link[i].t + d_bi[num_fct][i];
	}
	sum_xt_d += x_bi[num_fct][nb_link] * fct.max_b;


	model.add(C <= sum_xt_d);

	//Somme des x_bi == 1
	model.add(IloSum(x_bi[num_fct]) == 1);

	int up_b;
	for (int i = 0; i < nb_link - 1; i++)
	{
		up_b = fct.tab_link[i + 1].t - fct.tab_link[i].t /*Ajout -1 ici*/ - 1;

		//borne de d_bi et activation par xbi = 1
		//d_ks <= x_ks( t_ks+1 - t_ks - 1)
		d_bi[num_fct][i].setUb(up_b);
		model.add(d_bi[num_fct][i] <= up_b * x_bi[num_fct][i]);

	}
	d_bi[num_fct][nb_link - 1].setUb(fct.max_b - fct.tab_link[nb_link - 1].t);
	model.add(d_bi[num_fct][nb_link - 1] <= (fct.max_b - fct.tab_link[nb_link - 1].t)
		* x_bi[num_fct][nb_link - 1]);

	//additional cut
	//***
	model.add(C >= fct.min_a);
	model.add(C <= fct.max_b);
	for (int i = 0; i < nb_link; i++)
	{
		model.add(C >= fct.tab_link[i].t * x_bi[num_fct][i]);
	}
	//***

	//*********
	//Construction de la variable PPC_M
	for (int i = 0; i < nb_link; i++)
	{
		PPC_M += x_bi[num_fct][i] * fct.tab_link[i].c
			+ d_bi[num_fct][i] * fct.tab_link[i].alpha;
	}PPC_M += x_bi[num_fct][nb_link] * fct.eval_max_b;
	//*********

}

vector<int>  Solveur_min_IC::solve_routing_1_batch(const vector<int>& bat, int depart_au_plus_tot)
{
	nM = inst.mMachine;
	nJ = bat.size();

	list_ind = bat;

	tab_Batch = batch_list{};// TODO : this will overide the original tab_batch 
	tab_Batch.push_back(bat);

	this->env = IloEnv();
	model = IloModel(env); //mod�le, on l'associe directement � l'environnement
	cplex = IloCplex(model);//solveur, on l'associe directement au mod�le

	init_xij();
	init_Di();
	init_Ti();

	add_simple_routing_ctr(vector<int>({ depart_au_plus_tot }));
	add_Ti_ctr(depart_au_plus_tot);

	init_PPC_M();

	init_RT();

	IloObjective obj = IloMinimize(env, PPC_M + RT, "obj");

	model.add(obj);

	//ofstream fichier("sortie_CPLEX.txt", ios::out | ios::trunc);

	cplex.setOut(env.getNullStream());

	cplex.exportModel("cplex_imodif.lp");

	cplex.solve();

	//std::cout << "Departure date : " << departure_date << " ==> PPC_M : " << cplex.getObjValue() <<endl;
	//std::cout << departure_date << "\t" << cplex.getObjValue() << endl;

	return extrait_chemin_model();
}

//
//VERSION utilisant rellement CPLEX
//
//
Fct_lin Solveur_min_IC::eval_exact_with_CPLEX(const vector<int>& bat, int depart_min_a, int depart_max_b, int intervalle)
{
	nJ = bat.size();

	//***
	//R�glage du Big_M
	int max_dist = 0;
	for (int i = 0; i < inst.distancier.tab.size(); i++)
	{
		for (int j = 0; j < inst.distancier.tab[i].size(); j++)
		{
			if (inst.distancier.tab[i][j] > max_dist) {
				max_dist = inst.distancier.tab[i][j];
			}
		}
	}
	big_M = depart_max_b - depart_min_a + 1 + max_dist * nJ;
	//***

	depart_au_plus_tot = depart_min_a;



	//cette fonction se charge de construire le modele (entre autre)
	vector<int> chemin = solve_routing_1_batch(bat, depart_au_plus_tot);




	std::cout << cplex.getValue(RT) + cplex.getValue(PPC_M) << endl;

	Fct_lin fct_lin;
	fct_lin.min_a = depart_min_a;
	fct_lin.max_b = depart_max_b;

	//pour l'instant on ne recup�re pas les tourn�e du modele
	fct_lin.tab_link.push_back(Fct_lin::create_link(depart_min_a, cplex.getObjValue(), 0, Tournee::create_tournee(chemin)));


	cplex.exportModel("cplex_imodif.lp");

	vector<int> vect(1);
	int depart = depart_min_a + intervalle - ((depart_min_a + intervalle) % intervalle);
	for (depart; depart < depart_max_b; depart += intervalle)
	{
		//modification de la date de d�part dans les contraintes
		vect[0] = depart;
		modify_departure_date(vect, depart_min_a);



		cplex.solve();
		//std::cout << "Departure date : " << depart << " ==> PPC_M : " << cplex.getValue(PPC_M)
		//<< "   ==> RT : " << cplex.getValue(RT) << endl;
		//std::cout << depart << "\t" << cplex.getObjValue() << endl;

		//pour l'instant on ne recup�re pas les tourn�e du modele
		fct_lin.tab_link.push_back(Fct_lin::create_link(depart, cplex.getObjValue(), 0, Tournee::create_tournee(chemin)));
	}

	vect[0] = depart_max_b;
	modify_departure_date(vect, depart_min_a);
	cplex.solve();

	fct_lin.eval_max_b = cplex.getObjValue();

	cplex.end();
	model.end();
	env.end();


	//cplex.setWarning(std::cout);
	return fct_lin;
}//*/