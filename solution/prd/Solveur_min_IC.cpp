#include "Solveur_min_IC.h"


Solveur_min_IC::Solveur_min_IC()
{
}


Solveur_min_IC::~Solveur_min_IC()
{
	cplex.end();
	env.end();
}


//Utiliser pour générer les instances (fixe les due dates pour les rendre "intéressante")
void Solveur_min_IC::ajust_due_date_with_windows_a_b(Instance * inst, vector<vector<int>> tab_Batch)
{

	//Copie coller du model standart

	/////////////////////////////
	// Pose du modele
	init_model_and_data(inst, &tab_Batch); //redefinition du modele
	init_list_ind(inst, tab_Batch);

	init_Cij();

	add_capa_machine_ctr();
	add_gamme_ctr();
	add_C00_vaut_0();

	init_IC_WIP();
	init_IC_FIN();
	init_sum_CC();

	//Modele posé
	///////////////////////////


	cplex.setOut(env.getNullStream());


	///////////////////////////
	// Premiere résolution : min Cmax

	//obj = IloMinimize(env, IC_WIP + IC_FIN  + C_ij[nJ - 1][nM - 1] / 10000, "obj");
	obj = IloMinimize(env, sum_CC, "obj");
	model.add(obj);

	//Capture du temps
	chrono::time_point<chrono::system_clock> current, start_t = chrono::system_clock::now();

	cplex.solve();

	/**/
	/*cout << " !!! min Cmax !!!" << endl;
	cout << "====> Date de fin : " << cplex.getValue(C_ij[nJ - 1][nM - 1]) << endl;
	cout << "====> IC : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl;//*/

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
	// Deuxième résolution : min IC
	obj.setExpr(IC_WIP + IC_FIN + sum_CC / (cplex.getValue(sum_CC) * 10));

	cplex.solve();

	/**/
	/*cout << " !!! min IC !!!" << endl;
	cout << "====> Date de fin : " << cplex.getValue(C_ij[nJ - 1][nM - 1]) << endl;
	cout << "====> IC : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl;//*/

	//récup des dates max
	num_CC = -1;
	num_b = 0;
	for (auto& bat : tab_Batch)
	{
		num_CC += bat.size();
		tab_a_b[num_b].second = cplex.getValue(C_ij[num_CC][nM - 1]);
		num_b++;
	}
	//////////////////////////

	/*cout << "Intervalle de départ pour chaque batch" << endl;
	for each (auto var in tab_a_b)
	{
	cout <<"a : " << var.first << " b : " << var.second << endl;;
	}
	cout << endl;//*/


	/////////////////////////
	//Modification des due dates au sein des batch de l'instance
	default_random_engine gen;
	gen = default_random_engine(42);
	uniform_int_distribution<int> distri_due_date;
	int max_time_tournee;

	num_b = 0; int a, b;
	for (vector<int> bat : tab_Batch)
	{
		max_time_tournee = inst->distancier.max_edge_on_sub_matrix(bat) * bat.size();

		a = tab_a_b[num_b].first;
		b = tab_a_b[num_b].second;

		distri_due_date = uniform_int_distribution<int>(
			a *0.8, //Quelque date peuvent tomber avant le départ au plus tot
			a + (b - a + max_time_tournee) / 2);

		//cout << "B " << num_b << ": \t";
		for (int index_j : bat)
		{
			int n_dd = (int)distri_due_date(gen);
			inst->list_Job[index_j]->due_d = n_dd;
			//cout << n_dd << "\t";
		}//cout << endl;
		num_b++;
	}//*/

	 //Fin modification des due dates
	 ////////////////////////

	model.end();
	cplex.end();
	env.end();
}

//Utiliser pour générer les instances (fixe les due dates pour les rendre "intéressante")

void Solveur_min_IC::ajust_due_date_with_approx(Instance * inst, vector<vector<int>> tab_Batch, int nb_j_batch)
{
	double coeff_from_machine_depart = 1.0;

	double coeff_from_machine_length = 1.0;

	switch (inst->mMachine)
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
	for (int i = 0; i < inst->nJob; i++)
	{
		for (int j = 0; j < inst->mMachine; j++)
		{
			average_pij += inst->list_Job[i]->p[j];
		}
	}
	average_pij /= (double)(inst->nJob * inst->mMachine);

	average_pij *= coeff_from_machine_depart;



	/////////////////////////
	//Modification des due dates au sein des batch de l'instance
	default_random_engine gen;
	gen = default_random_engine(42);
	uniform_int_distribution<int> distri_due_date;

	int num_b = 0;

	int min_w = (inst->mMachine - 1)*average_pij;
	int max_w;

	double average_time_tournee = average_pij * ((inst->nJob) / tab_Batch.size()) * 2;
	double windows_size = average_time_tournee;
	//cout << "average_pij : " << average_pij << "\t" << "average_time_tournee : " << average_time_tournee << endl;

	for (vector<int> bat : tab_Batch)
	{
		min_w += tab_Batch[num_b].size() * average_pij;

		windows_size += coeff_from_machine_length * average_time_tournee;

		//*** max_v
		max_w = min_w + windows_size;

		distri_due_date = uniform_int_distribution<int>(min_w, max_w);

		//cout << "B " << num_b << ": \t";
		for (int index_j : bat)
		{
			int n_dd = (int)distri_due_date(gen);
			inst->list_Job[index_j]->due_d = n_dd;
			//cout << "min_w : " << min_w << "\t" << "max_w : " << max_w << "\t" << "new_due_d : " << n_dd << endl;
		}//cout << endl;
		num_b++;
	}//*/

	 //Fin modification des due dates
	 ////////////////////////
}



//***
//Cette fonction reprend l'algorithme global de résolution présenté dans l'article
//****
Struct_Retour Solveur_min_IC::solve(Instance* inst, vector<vector<int>> tab_Batch, int approx_method, int mode)
{

	if (mode == init_as_optima) {
		best_fct_par_batch = vector<Fct_lin>(tab_Batch.size());
	}

	cout << "----------" << endl << "Chargement du modèle initial et resolution" << endl << "----------" << endl << endl;

	/////////////////////////////
	// Pose du modele
	init_model_and_data(inst, &tab_Batch); //redefinition du modele
	init_list_ind(inst, tab_Batch);

	init_Cij();

	add_capa_machine_ctr();
	add_gamme_ctr();
	add_C00_vaut_0();

	init_IC_WIP();
	init_IC_FIN();
	init_sum_CC();

	//Modele posé
	///////////////////////////


	cplex.setOut(env.getNullStream());


	///////////////////////////
	// Premiere résolution : min Cmax

	//obj = IloMinimize(env, IC_WIP + IC_FIN  + C_ij[nJ - 1][nM - 1] / 10000, "obj");
	obj = IloMinimize(env, sum_CC, "obj");
	model.add(obj);

	//Capture du temps
	chrono::time_point<chrono::system_clock> current, start_t = chrono::system_clock::now();

	cplex.solve();

	/**/
	cout << "Date de fin le plus tot possible calculee :" << endl;
	cout << "====> Date de fin : " << cplex.getValue(C_ij[nJ - 1][nM - 1]) << endl;
	cout << "====> IC : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl << endl;//*/

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
	// Deuxième résolution : min IC
	obj.setExpr(IC_WIP + IC_FIN + sum_CC / (cplex.getValue(sum_CC) * 5));

	cplex.solve();

	/**/
	cout << "Date de fin pour inventaire minimum calculee :" << endl;
	cout << "====> Date de fin : " << cplex.getValue(C_ij[nJ - 1][nM - 1]) << endl;
	cout << "====> IC : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl << endl;//*/

																							 //récup des dates max
	num_CC = -1;
	num_b = 0;
	for (auto bat : tab_Batch)
	{
		num_CC += bat.size();
		tab_a_b[num_b].second = cplex.getValue(C_ij[num_CC][nM - 1]);
		num_b++;
	}
	//////////////////////////

	if (mode == init_as_optima) {
		a_b_par_batch = tab_a_b;
	}

	num_b = 0;
	cout << "Pour chaque batch, intervalle des dates de depart interressante:" << endl;
	for (auto var : tab_a_b)
	{
		cout << "Batch " << num_b + 1 << ": [\t" << var.first << ",\t" << var.second << "\t]";

		num_b++;
		cout << endl;
	}cout << endl;//*/

	cout << "----------" << endl << "Calcul des profils de fonction de livraison F_k pour chaque batch" << endl << "----------" << endl << endl;

	/////////////////////////
	// Estimation du coût d'une tournée en fonction de sa date 
	//pour chaque batch


	//
	// Attention, comprendre comment le fonction F_k sont créer en détail
	// n'est pas demandé.
	//

	// ==> traçage d'une fonction linéaire par morceau
	vector<Fct_lin> list_fct_lin;

	int min_a, max_b;
	int cpt_time_approx = 0;

	int average_time = 0;

	for (int i = 0; i < tab_Batch.size(); i++)
	{
		min_a = tab_a_b[i].first;
		max_b = tab_a_b[i].second;


		//!!!!!!!
		//*******
		Fct_lin& best = Fct_lin::create_fct_max(0, 0);
		Fct_lin& f_EDD = Fct_lin::create_fct_max(0, 0);
		Fct_lin& f_near = Fct_lin::create_fct_max(0, 0);
		Tournee & t_EDD = Tournee::create_tournee_vide(0);
		Tournee & t_near = Tournee::create_tournee_vide(0);

		Solveur_min_IC solve_CPLEX;
		Branch_and_bound b_and_b;

		chrono::time_point<chrono::system_clock> current, start_t = chrono::system_clock::now();



		switch (approx_method) // par defaut du prototype EDD_nearx4
		{
		case heuristic_near:
			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			best = Fct_lin::create_fct_lin(t_near, inst, min_a, max_b);
			break;

		case EDD:
			t_EDD = Tournee::create_tournee_EDD(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_local_search(inst, t_EDD, min_a, max_b);

			//cout << "EDD  ";
			break;
		case EDDx4:
			t_EDD = Tournee::create_tournee_EDD(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_four_extrema(inst, t_EDD, min_a, max_b);

			//cout << "EDD_x4 ";
			break;
		case near:
			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_local_search(inst, t_near, min_a, max_b);

			// << "near ";
			break;
		case nearx4:
			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			best = Fct_lin::generate_fct_with_four_extrema(inst, t_near, min_a, max_b);

			//cout << "near_x4 ";
			break;
		case EDD_near:
			t_EDD = Tournee::create_tournee_EDD(inst, tab_Batch[i]);
			f_EDD = Fct_lin::generate_fct_with_local_search(inst, t_EDD, min_a, max_b);
			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			f_near = Fct_lin::generate_fct_with_local_search(inst, t_near, min_a, max_b);

			best = Fct_lin::minimum_fct(f_EDD, f_near);

			//cout << "EDD_near ";
			break;
		case EDD_nearx4:
			t_EDD = Tournee::create_tournee_EDD(inst, tab_Batch[i]);
			f_EDD = Fct_lin::generate_fct_with_four_extrema(inst, t_EDD, min_a, max_b);

			t_near = Tournee::create_tournee_nearest_insertion(inst, tab_Batch[i]);
			f_near = Fct_lin::generate_fct_with_four_extrema(inst, t_near, min_a, max_b);

			best = Fct_lin::minimum_fct(f_EDD, f_near);

			//cout << "EDD_near_x4 ";

			break;

		case CPLEX:
			best = solve_CPLEX.eval_exact_with_CPLEX(inst, tab_Batch[i], min_a, max_b, 1);

			//cout << "CPLEX ";
			break;

		case B_and_B:

			best = b_and_b.generate_fct_with_branch_and_bound(inst, tab_Batch[i], min_a, max_b);

			//cout << "B&B  ";
			break;

		default: cout << "Method non reconnu" << endl;
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
	cout << "DONE" << endl << endl;
	//
	/////////////////////////

	//cout << "Average time: " << average_time / tab_Batch.size() << endl;

	int i = 0; num_b = 0;
	cout << "Affichage des fonctions lineaires pour chaque batch." << endl << "La methode utilisee est celle passee en parametre.";
	cout << "Legende: " << endl;
	cout << "1) Date de depart du segment\n \t2) Cout initial\n\t\t3) Coeff directeur\n\t\t\t4) Tournee associee" << endl;
	for (auto fct : list_fct_lin)
	{
		cout << "--- Batch " << num_b << ":  Size batch :  " << tab_Batch[i].size() << endl; i++;
		fct.print_fct_lin();
		num_b++;
	}
	cout << endl;//*/


	cout << "----------" << endl << "Ajout des profils au F_k au modele et resolution globale" << endl << "----------" << endl << endl;

	//////////////////////
	//Ajout du coût des tournées aux modèles
	//(Traduction des fonction lin en modèle math)

	PPC_M = IloExpr(env);
	//Retard_incompressible = IloExpr(env);

	//on a un jeu de variable par batch / fonction lineaire
	d_bi = IloArray<IloIntVarArray>(env, list_fct_lin.size());
	x_bi = IloArray<IloBoolVarArray>(env, list_fct_lin.size());

	num_CC = -1;
	num_b = 0;
	for (auto fct : list_fct_lin)
	{
		num_CC += tab_Batch[num_b].size();
		add_fct_lin_ctr(fct, num_b, C_ij[num_CC][nM - 1]); //<== ajout ici
														   //des contraintes et de valeurs de variable
		num_b++;
	}
	//
	///////////////////////

	// Deuxième résolution : min IC + PPC_M

	//cout << " !!! min IC + PC + RT !!!" << endl;
	obj.setExpr(IC_WIP + IC_FIN + PPC_M);

	cplex.solve();

	//Capture du temps total
	current = chrono::system_clock::now();
	cpt_time_approx += chrono::duration_cast<chrono::milliseconds>(current - start_t).count();

	cplex.exportModel("CPLEX.lp");

	/**/
	//cout << "====> Date de fin : " << cplex.getValue(C_ij[nJ - 1][nM - 1]) << endl;
	//cout << "====> IC : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl;
	//cout << "====> PPC_M : " << cplex.getValue(PPC_M) << endl;
	//cout << "====> Cout total : " << cplex.getValue(PPC_M) + cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl;
	//cout << "Ratio IC / PPC_M: " << (cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN)) / cplex.getValue(PPC_M) << endl;
	//*/

	int result = cplex.getValue(PPC_M) + cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN);

	if (mode == init_as_optima) {
		global_optima = result;
	}

	double ecart_a_l_opti = -42.0;
	if (comp_with_optima) {

		ecart_a_l_opti = (result - global_optima)*100.0 / (global_optima);

		//cout << "  => Ecart a loptima: " << ecart_a_l_opti << " %" << endl;
	}



	//
	// Affichage tournée choisit et comparaison batch
	//des fonctions optis
	//

	double moyenne_ecart_a_l_optima = 0;

	num_CC = -1;
	num_b = 0;

	cout << endl << "Solution optimal trouvee :" << endl << "Date de depart et tournee par batch:" << endl;

	for(auto fct : list_fct_lin)
	{
		num_CC += tab_Batch[num_b].size();

		int date_depart = cplex.getValue(C_ij[num_CC][nM - 1]);
		Tournee& t = fct.found_tournee(date_depart);



		cout << "Batch " << num_b + 1 << " Date de depart :  " << date_depart << "  Tournee: {"; t.print(); cout << "}" << endl <<
			"\tCout du trajet: " << t.eval_routing_cost(inst) << "  Cout du retard: " << fct.eval_fct_lin(date_depart) - t.eval_routing_cost(inst) << endl;

		/*if (comp_with_optima) {
		int result = fct.eval_fct_lin( date_depart );

		int opti_a_cette_date = best_fct_par_batch[num_b].eval_fct_lin(date_depart);

		double ecart_a_l_opti = (result - opti_a_cette_date)*100.0 / (opti_a_cette_date);

		int aire_opti = best_fct_par_batch[num_b].calcul_aire_sous_la_courbe();
		int aire_approx = fct.calcul_aire_sous_la_courbe();

		cout << "  => Ecart a loptima: " << ecart_a_l_opti << " %" <<
		"\t\t=> Ecart d'aire:"<< (aire_approx - aire_opti)*100.0 / (aire_opti) << " %" << endl;

		moyenne_ecart_a_l_optima += ecart_a_l_opti;
		}//*/
		num_b++;
	}cout << endl;//*/
				  //Suppr fct
	for (auto fct : list_fct_lin)
	{
		for (auto link : fct.tab_link)
		{
			link.tournee.suppr();
		}
	}

	Struct_Retour retour;

	retour.ecart_a_opti = ecart_a_l_opti;
	retour.cpl_time_ms = cpt_time_approx;
	retour.IC = cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN);
	retour.PC = cplex.getValue(PPC_M);
	retour.IC_PC = cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) + cplex.getValue(PPC_M);

	cout << "Cout d'inventaire total : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) << endl
		<< "Cout de routing toatal : " << cplex.getValue(PPC_M) << endl
		<< "Cout total : " << cplex.getValue(IC_WIP) + cplex.getValue(IC_FIN) + cplex.getValue(PPC_M) << endl << endl;

	model.end();
	cplex.end();
	env.end();

	return retour;
}

void Solveur_min_IC::init_model_and_data(Instance* inst, vector<vector<int>>* tab_Batch)
{

	this->inst = inst;
	nM = inst->mMachine;

	if (tab_Batch != nullptr) {
		this->tab_Batch = tab_Batch;
		nJ = 0;
		for (auto var : *tab_Batch) {
			nJ += var.size();
		}
	}
	else { nJ = inst->nJob; }

	nM = inst->mMachine;

	this->env = IloEnv();
	model = IloModel(env); //modèle, on l'associe directement à l'environnement
	cplex = IloCplex(model);//solveur, on l'associe directement au modèle
}

void Solveur_min_IC::init_list_ind(Instance * inst, vector<vector<int>> tab_Batch)
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
	Job* job;

	IC_WIP = IloExpr(env);
	for (int i = 0; i < nJ; i++)
	{
		i_job = list_ind[i];
		job = inst->list_Job[i_job];
		for (int j = 0; j < nM - 1; j++)
		{
			IC_WIP += job->thWIP[j] * (C_ij[i][j + 1] - job->p[j + 1] - C_ij[i][j]);
		}
	}
}

void Solveur_min_IC::init_IC_FIN()
{
	int i_job;
	Job* job;

	IC_FIN = IloExpr(env);
	int i_last_job;
	Job* last_job;

	int nb_job_avant = 0;
	for (auto var : *tab_Batch)
	{
		i_last_job = var.back();
		last_job = inst->list_Job[i_last_job];
		for (int i = 0; i < var.size() - 1; i++)
		{
			i_job = var[i];
			job = inst->list_Job[i_job];

			IC_FIN += job->thFIN *
				// cpl_time du dernier job du batch sur la dernier machine
				(C_ij[nb_job_avant + var.size() - 1][nM - 1] - last_job->p[nM - 1] - C_ij[nb_job_avant + i][nM - 1]);
		}
		nb_job_avant += var.size();
	}
}

void Solveur_min_IC::init_PPC_M()
{
	int i_job;
	Job* job;

	PPC_M = IloExpr(env);
	for (int i = 0; i < nJ; i++)
	{
		i_job = list_ind[i];
		job = inst->list_Job[i_job];
		PPC_M += T_i[i] * job->piM;
	}

}

void Solveur_min_IC::init_RT()
{
	RT = IloExpr(env);

	int index_depot_distancier = inst->distancier.index_manu();
	for (int i = 1; i <= nJ; i++)
	{
		RT += x_ij[0][i] * inst->distancier.dist(index_depot_distancier, list_ind[i - 1]);

		//on ne prend pas en compte l'aller au dépôt
		//RT = x_ij[i][nJ+1] * inst->distancier.dist(index_depot_distancier, list_ind[i-1]);
	}

	for (int i = 1; i <= nJ; i++)
	{
		for (int j = 1; j <= nJ; j++)
		{
			if (i != j) {
				RT += x_ij[i][j] * inst->distancier.dist(list_ind[i - 1], list_ind[j - 1]);
			}
		}
	}
}

void Solveur_min_IC::init_sum_CC()
{
	sum_CC = IloExpr(env);

	int num_CC = -1;
	for (auto var : *tab_Batch)
	{
		num_CC += var.size();
		sum_CC += C_ij[num_CC][nM - 1];
	}
}

void Solveur_min_IC::add_capa_machine_ctr()
{
	int i_job;
	Job* job;
	for (int i = 1; i < nJ; i++)
	{
		i_job = list_ind[i];
		job = inst->list_Job[i_job];
		for (int j = 0; j < nM; j++)
		{
			model.add(C_ij[i - 1][j] + job->p[j] <= C_ij[i][j]);
		}
	}
}

void Solveur_min_IC::add_gamme_ctr()
{
	int i_job;
	Job* job;
	for (int i = 0; i < nJ; i++)
	{
		i_job = list_ind[i];
		job = inst->list_Job[i_job];
		for (int j = 1; j < nM; j++)
		{
			model.add(C_ij[i][j - 1] + job->p[j] <= C_ij[i][j]);
		}
	}
}

void Solveur_min_IC::add_C00_vaut_0()
{
	model.add(C_ij[0][0] == inst->list_Job[list_ind[0]]->p[0]);
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
	// L'index 0 (x_ij[0][-]) dans le modèle correspond au lieu de départ de la tournée
	// L'index nJ+1 (x_ij[-][nJ+1]) dans le modèle correspond au lieu de d'arrivée de la tournée
	// (Note : c'est deux lieux sont en fait le même dépot
	//

	Distancier& dist = inst->distancier;

	IloExpr nb_depart = IloExpr(env);
	IloExpr nb_arrivee = IloExpr(env);

	for (int i = 1; i <= nJ; i++)
	{
		nb_depart += x_ij[0][i];
		nb_arrivee += x_ij[i][nJ + 1];
	}

	//Cas avec batch connu
	model.add(nb_depart == tab_Batch->size());
	model.add(nb_arrivee == tab_Batch->size());


	// un successeur et un prédécesseur
	IloExpr pred_i = IloExpr(env);
	IloExpr succ_i = IloExpr(env);

	//cas batch connu
	int start_batch = 1;
	int size_batch;
	for (auto bat : *tab_Batch)
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
	Distancier& dist = inst->distancier;

	int start_batch = 1;
	int size_batch;

	int dist_i_j;

	start_batch = 1;
	//size_batch;
	int i_CC = -1;
	int num_b = 0;
	IloIntVar* CC;
	for (auto bat : *tab_Batch)
	{
		size_batch = bat.size();

		for (int i = start_batch; i < start_batch + size_batch; i++)
		{
			//cout << dist.dist(dist.index_manu(), list_ind[i - 1]) << "\t";
			for (int j = start_batch; j < start_batch + size_batch; j++)
			{
				if (i != j) {

					//cout << dist.dist(list_ind[i - 1], list_ind[j - 1]) << "\t";

					dist_i_j = dist.dist(list_ind[i - 1], list_ind[j - 1]);

					model.add(D_i[j - 1] >= D_i[i - 1] + dist_i_j - big_M * (1 - x_ij[i][j]));

					//brisage de sous tours si les jobs sont au même endroit
					if (dist_i_j == 0 && i < j) {
						model.add(x_ij[j][i] == 0);
					}
				}
				//else { cout << "-\t"; }
			}//cout << endl;
		}
		start_batch += size_batch;
		num_b++;
	}
}

//on fait l'hypothèse que valeurs des instances sont adaptés pour que le 
//l'on puisse faire comme si le véhicule parté à t = 0
void Solveur_min_IC::add_departure_date_ctr()
{
	Distancier& dist = inst->distancier;


	ctr_departure_date = IloRangeArray(env, nJ);


	int start_batch = 1;
	int size_batch;

	int i_CC = -1;
	int num_b = 0;
	IloIntVar* CC;
	for (auto bat : *tab_Batch)
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
	Distancier& dist = inst->distancier;

	int start_batch = 1;
	int size_batch;

	int i_CC = -1;
	int num_b = 0;

	for (auto bat : *tab_Batch)
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
	Job* job;

	for (int i = 0; i < nJ; i++)
	{
		model.add(T_i[i] >= D_i[i] + depart_au_plus_tot - inst->list_Job[list_ind[i]]->due_d);
	}
}

vector<int> Solveur_min_IC::extrait_chemin_model()
{
	auto swarzy = vector<int>();

	int val2;
	int source = 0, arrive;
	int size = -1;
	//cout << "D ";
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

		if (arrive != x_ij.getSize() - 1) swarzy.push_back(list_ind[arrive - 1]);// cout << arrive - 1 << " ";

		source = arrive;
	}
	/*cout << "A\n  ";
	if (size != x_ij.getSize() - 2) {
	cout << "Size: " << size << " !!! ERREUR !!! " << endl;
	}
	else {
	cout << "Size: " << size << " OK " << endl;
	}*/

	return swarzy;
}

void Solveur_min_IC::add_fct_lin_ctr(const Fct_lin& fct, int num_fct, const IloIntVar& C)
{
	int nb_link = fct.tab_link.size();
	d_bi[num_fct] = IloIntVarArray(env, nb_link, 0, INT_MAX);
	x_bi[num_fct] = IloBoolVarArray(env, nb_link + 1); // permet de generer le max_b de manière propre

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

}//*/

vector<int>  Solveur_min_IC::solve_routing_1_batch(Instance* inst, const vector<int>& bat, int depart_au_plus_tot)
{
	this->inst = inst;

	nM = inst->mMachine;
	nJ = bat.size();

	list_ind = bat;

	tab_Batch = new vector<vector<int>>();
	tab_Batch->push_back(bat);

	this->env = IloEnv();
	model = IloModel(env); //modèle, on l'associe directement à l'environnement
	cplex = IloCplex(model);//solveur, on l'associe directement au modèle

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

	//cout << "Departure date : " << departure_date << " ==> PPC_M : " << cplex.getObjValue() <<endl;
	//cout << departure_date << "\t" << cplex.getObjValue() << endl;

	return extrait_chemin_model();
}

//
//VERSION utilisant rellement CPLEX
//
//
Fct_lin Solveur_min_IC::eval_exact_with_CPLEX(Instance * inst, const vector<int>& bat, int depart_min_a, int depart_max_b, int intervalle)
{
	nJ = bat.size();

	//***
	//Réglage du Big_M
	int max_dist = 0;
	for (int i = 0; i < inst->distancier.tab.size(); i++)
	{
		for (int j = 0; j < inst->distancier.tab[i].size(); j++)
		{
			if (inst->distancier.tab[i][j] > max_dist) {
				max_dist = inst->distancier.tab[i][j];
			}
		}
	}
	big_M = depart_max_b - depart_min_a + 1 + max_dist * nJ;
	//***

	depart_au_plus_tot = depart_min_a;



	//cette fonction se charge de construire le modele (entre autre)
	vector<int>& chemin = solve_routing_1_batch(inst, bat, depart_au_plus_tot);




	cout << cplex.getValue(RT) + cplex.getValue(PPC_M) << endl;

	Fct_lin fct_lin;
	fct_lin.min_a = depart_min_a;
	fct_lin.max_b = depart_max_b;

	//pour l'instant on ne recupère pas les tournée du modele
	fct_lin.tab_link.push_back(Fct_lin::create_link(depart_min_a, cplex.getObjValue(), 0, Tournee::create_tournee(chemin)));


	cplex.exportModel("cplex_imodif.lp");

	vector<int> vect(1);
	int depart = depart_min_a + intervalle - ((depart_min_a + intervalle) % intervalle);
	for (depart; depart < depart_max_b; depart += intervalle)
	{
		//modification de la date de départ dans les contraintes
		vect[0] = depart;
		modify_departure_date(vect, depart_min_a);



		cplex.solve();
		//cout << "Departure date : " << depart << " ==> PPC_M : " << cplex.getValue(PPC_M)
		//<< "   ==> RT : " << cplex.getValue(RT) << endl;
		//cout << depart << "\t" << cplex.getObjValue() << endl;

		//pour l'instant on ne recupère pas les tournée du modele
		fct_lin.tab_link.push_back(Fct_lin::create_link(depart, cplex.getObjValue(), 0, Tournee::create_tournee(chemin)));
	}

	vect[0] = depart_max_b;
	modify_departure_date(vect, depart_min_a);
	cplex.solve();

	fct_lin.eval_max_b = cplex.getObjValue();

	cplex.end();
	model.end();
	env.end();


	//cplex.setWarning(cout);
	return fct_lin;
}//*/