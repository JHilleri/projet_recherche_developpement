#include "Branch_and_Bound.h"

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

Covering_tree::Covering_tree(Instance * inst, const vector<int>& batch)
{
	this->inst = inst;
	edges = vector<Edge>();

	link_with = map<int, vector<int>>();
	for (int i = 0; i < batch.size(); i++) link_with[batch[i]] = vector<int>();

	vector<vector<int>> sous_ens = vector<vector<int>>();
	for (int i = 0; i < batch.size(); i++) sous_ens.push_back(vector<int>({ batch[i] }));

	////
	connect_ss_ens(sous_ens);
	////


	/*for each (auto& var in sous_ens)
	{
	cout << "{";
	for each (auto var1 in var)
	{
	cout << var1 << ", ";
	}cout << "}";
	}cout << endl;

	for each (auto var in edges)
	{
	cout << "n1: " << var.n_1 << " n2: " << var.n_2 << " cost: " << var.cost << endl;
	}

	for each (auto var in link_with)
	{
	cout << var.first << ": ";
	for each (int jih in var.second)
	{
	cout << jih << ", ";
	}cout << endl;
	}*/
}

void Covering_tree::connect_ss_ens(vector<vector<int>>& sous_ens) {
	int b_ss1; //best sous ensemble 1
	int b_ss2;
	int b_ind_1; // best ind lie a ss1
	int b_ind_2;
	int cost;
	int best_cost;
	//int n_1, n_2;

	while (sous_ens.size() != 1) {

		////
		//recupération de larrete de poid minimum
		best_cost = numeric_limits<int>::max();

		for (int ss_1 = 0; ss_1 < sous_ens.size(); ss_1++)
		{
			for (int ss_2 = ss_1 + 1; ss_2 < sous_ens.size(); ss_2++)
			{
				for (int ind_1 = 0; ind_1 < sous_ens[ss_1].size(); ind_1++)
				{
					for (int ind_2 = 0; ind_2 < sous_ens[ss_2].size(); ind_2++)
					{
						cost = inst->distancier.dist(sous_ens[ss_1][ind_1], sous_ens[ss_2][ind_2]);

						if (best_cost > cost) {
							best_cost = cost;

							b_ss1 = ss_1;
							b_ss2 = ss_2;
							b_ind_1 = ind_1;
							b_ind_2 = ind_2;
						}
					}
				}
			}
		}
		// Recup done
		////

		edges.push_back(create_edge(sous_ens[b_ss1][b_ind_1], sous_ens[b_ss2][b_ind_2], best_cost));

		////
		// mise à jour des links with
		link_with[sous_ens[b_ss1][b_ind_1]].push_back(sous_ens[b_ss2][b_ind_2]);
		link_with[sous_ens[b_ss2][b_ind_2]].push_back(sous_ens[b_ss1][b_ind_1]);
		//
		////

		////
		//agglomeration des differents job

		for(auto var : sous_ens[b_ss2])
		{
			sous_ens[b_ss1].push_back(var);
		}
		sous_ens.erase(sous_ens.begin() + b_ss2);
		//agglo done
		////
	}
}

void Covering_tree::delete_node(int indexnode)
{

	//suppr du job dans link with
	for (auto it = link_with.begin(); it != link_with.end(); ++it)
	{
		int i = (*it).second.size() - 1;
		while (i >= 0) {
			if ((*it).second[i] == indexnode) {
				(*it).second.erase((*it).second.begin() + i);
			}
			i--;
		}
	}

	//suppr des edges liées à ce job
	int i = edges.size() - 1;
	while (i >= 0) {
		if (edges[i].n_1 == indexnode
			|| edges[i].n_2 == indexnode) {
			edges.erase(edges.begin() + i);
		}
		i--;
	}

	//suppr la ligne lié à ce job
	link_with.erase(link_with.find(indexnode));

	auto sous_ens = reconstruct_ss_ens_from_tab_link_with();

	connect_ss_ens(sous_ens);
}

int Covering_tree::sum_edge()
{
	int swarzy = 0;
	for(auto& var : edges) swarzy += var.cost;
	return swarzy;
}


vector<vector<int>> Covering_tree::reconstruct_ss_ens_from_tab_link_with()
{
	vector<vector<int>> swarzy = vector<vector<int>>();
	set<int> already_assigned = set<int>();

	for (auto it = link_with.begin(); it != link_with.end(); ++it) {
		vector<int> current_set = vector<int>();
		rec_reconstruct(current_set, (*it).first, already_assigned);

		if (current_set.size() != 0) swarzy.push_back(current_set);
	}


	return swarzy;
}


void Covering_tree::rec_reconstruct(vector<int>& current_set, int node, set<int>& already_assigned)
{
	if (already_assigned.find(node) == already_assigned.end()) {
		already_assigned.insert(node);
		current_set.push_back(node);

		for (int node_link : link_with[node])
		{
			rec_reconstruct(current_set, node_link, already_assigned);
		}
	}
}

Fct_lin Branch_and_bound::generate_fct_with_branch_and_bound(Instance * inst, const vector<int>& batch, int min_a, int max_b)
{
	this->inst = inst;

	vec_bool = vector<bool>(max_b - min_a + 1, false);

	//generation d'une upper bound avec des heuristic
	upper_bound = init_upper_bound(inst, batch, min_a, max_b);

	nb_resting_job = batch.size();
	//index des jobs restant a assigné lors de la descante de l'arbre
	job_resting_for_assignement = list<int>();
	for (int var : batch) job_resting_for_assignement.push_back(var);

	//liste ordoné des jobs déjà fixé
	tournee = Tournee::create_tournee_vide(batch.size());

	//index du dernier site visiter, départ du depot
	last_visited_site_index = inst->distancier.index_manu();

	//arbre couvrant de point minimum, initialiser avec tous les noeud du batch
	Covering_tree covering_tree = Covering_tree(inst, batch);

	//cout de la solution en fonction de la date de départ
	//à un état d'avancement du B&B => LowerBound
	Vec_date_cost tab_date_cost(min_a, max_b);

	int tmp;
	for (int i = 0; i < nb_resting_job; i++)
	{
		tmp = job_resting_for_assignement.front();
		job_resting_for_assignement.pop_front();

		tournee.add_job(tmp);
		nb_resting_job--;

		rec_branch_and_bound(covering_tree, tmp, tab_date_cost, 0);

		nb_resting_job++;
		tournee.suppr_job();
		job_resting_for_assignement.push_back(tmp);
	}

	/*for (int date = tab_date_cost.min_a; date < tab_date_cost.max_b; date++) {
	cout << date << "\t" << vec_bool[date - tab_date_cost.min_a] << endl;
	}cout << endl;//*/

	/*int val_from_opt;
	int val_from_tournee_opt;
	for (int i = min_a; i <= max_b; i++) {
	val_from_opt = upper_bound.eval_fct_lin(i);

	Fct_lin& fct_tmp = Fct_lin::create_fct_lin(upper_bound.found_tournee(i), inst, min_a, max_b);
	val_from_tournee_opt = fct_tmp.eval_fct_lin(i);

	cout << "Date " << i << ":\t" << val_from_opt << "\t" << val_from_tournee_opt << "\t" <<
	(val_from_opt <= val_from_tournee_opt) << "\t" << (val_from_opt == val_from_tournee_opt) << "\t";
	//best.found_tournee(i).print();
	cout << endl;
	}cout << endl;//*/

	return upper_bound;
}

void Branch_and_bound::evaluation_of_instance_file(string path, string prefix)
{

	ifstream memo = ifstream(path + "1Memo_inst_" + prefix + ".txt");

	int nb_inst;
	int duree_tournee;

	memo >> nb_inst >> duree_tournee;
	memo.close();

	Instance* inst;
	vector<int> batch;
	Branch_and_bound b_and_b;
	Fct_lin fct; int aire;

	ofstream of_file = ofstream(path + "1Opti_" + prefix + ".txt", ios::out | ios::trunc);


	/*double min_a = duree_tournee * 0;
	double max_b = duree_tournee * 2;//*/

	double min_a = duree_tournee * 1.5;
	double max_b = duree_tournee * 1.6;//*/


	for (int i = 0; i < nb_inst; i++)
	{
		//cout << path + "Inst_" + prefix + "_" + to_string(i) + ".txt" << endl;
		//string file_inst = "C:/Users/21709633t/source/repos/Manu_dominates_1/Sonja-Rohmer-heuristique/Instance/";
		//inst = new Instance(file_inst + "Instance_1batch_10job/I_1bat_10job" + to_string(i) + ".txt");

		inst = new Instance(path + "Inst_" + prefix + "_" + to_string(i) + ".txt");

		batch = vector<int>();
		for (int i = 0; i < inst->nJob; i++) batch.push_back(i);

		fct = b_and_b.generate_fct_with_branch_and_bound(inst, batch, min_a, max_b);

		//fct.print_fct_lin();

		aire = fct.calcul_aire_sous_la_courbe();
		cout << aire << endl;
		of_file << aire << endl;

		inst->~Instance();
	}

	of_file.close();

}

Fct_lin Branch_and_bound::init_upper_bound(Instance * inst, const vector<int>& batch, int min_a, int max_b)
{
	Tournee t_EDD = Tournee::create_tournee_EDD(inst, batch);
	Tournee t_near = Tournee::create_tournee_nearest_insertion(inst, batch);


	Fct_lin f_EDDx4 = Fct_lin::generate_fct_with_four_extrema(inst, t_EDD, min_a, max_b);
	Fct_lin f_nearx4 = Fct_lin::generate_fct_with_four_extrema(inst, t_near, min_a, max_b);


	Fct_lin f_EDD_nearx4 = Fct_lin::minimum_fct(f_EDDx4, f_nearx4);

	return f_EDD_nearx4;//*/

						//return Fct_lin::create_fct_max(min_a,max_b);
}


void Branch_and_bound::rec_branch_and_bound(Covering_tree covering_tree/*passage par copie*/,
	int branch_node, Vec_date_cost tab_date_cost, int time_travel_for_last_job)
{
	if (nb_resting_job > 1) { //si pas une feuille

							  //*****
							  //élément de la LB ne variant pas avec la date de départ
		Job* job = inst->list_Job[branch_node];

		int min_routing_cost = covering_tree.sum_edge();

		int dist = inst->distancier.dist(last_visited_site_index, branch_node);
		time_travel_for_last_job += dist;
		// tournee incrémentée dans la branch precedente
		//*****		

		//*****
		//mise à jour du tableau de cout pour toute les date de départ encore valide
		for (int date = tab_date_cost.min_a; date <= tab_date_cost.max_b; date++)
		{
			if (tab_date_cost.date_is_valide(date)) {
				tab_date_cost.add_cost(date, dist /* cout de routing == distance */
					+ (max(0, date + time_travel_for_last_job - job->due_d)*job->piM)); // cout de retard du job
			}
		}

		/*if (branch_node == 3 && covering_tree.edges.size() == 4) {
		cout << endl;
		}
		if (branch_node == 2) {
		cout << endl;
		}*/

		//appliquation des bornes sur l'ensemble de l'intervalle

		//compute the assignation matrix
		//assignement de cout mini
		Vec_date_cost min_assigement_cost_per_date = min_assigment(covering_tree, tab_date_cost, time_travel_for_last_job, branch_node);

		bool il_y_a_encore_une_date_valide = false;
		bool pas_de_cut_sur_date;
		int up_eval;

		//On checke si la lower bound et inférieure à la upper bound pour au moins une date
		for (int date = tab_date_cost.min_a; date <= tab_date_cost.max_b; date++)
		{


			if (tab_date_cost.date_is_valide(date)) {

				/*if (tournee.tab_c[0] == 3 &&
				tournee.tab_c[1] == 2 &&
				tournee.tab_c[2] == 1 &&
				date == 110) {
				cout << "3,2,1\tDate: " << date << " " <<
				tab_date_cost.get_cost(date) << " " <<
				min_assigement_cost_per_date.get_cost(date) << " " <<
				min_routing_cost;
				cout << endl;
				}else if (tournee.tab_c[0] == 3 &&
				tournee.tab_c[1] == 2 &&
				date == 110) {
				cout << "3,2\tDate: " << date << " " <<
				tab_date_cost.get_cost(date) << " " <<
				min_assigement_cost_per_date.get_cost(date) << " " <<
				min_routing_cost;
				cout << endl;
				}else if (tournee.tab_c[0] == 3 &&
				date == 110 ) {
				cout << "3 \tDate: " << date << " " <<
				tab_date_cost.get_cost(date) << " " <<
				min_assigement_cost_per_date.get_cost(date) << " " <<
				min_routing_cost;
				cout << endl;
				}//*/

				up_eval = upper_bound.eval_fct_lin(date);

				//current_cost + min_assigement_cost + min_routing_cost < up_eval

				//!!! < ou <=  !!! < par défault
				pas_de_cut_sur_date =
					(tab_date_cost.get_cost(date) + min_assigement_cost_per_date.get_cost(date) + min_routing_cost
						/*ICI =>*/ < up_eval);

				il_y_a_encore_une_date_valide = il_y_a_encore_une_date_valide || pas_de_cut_sur_date;

				if (!pas_de_cut_sur_date) {
					tab_date_cost.put_date_to_minus_one(date);
				}
			}
		}


		if (il_y_a_encore_une_date_valide) {

			int tmp_old_last_visited_site_index = last_visited_site_index;
			last_visited_site_index = branch_node;

			covering_tree.delete_node(branch_node);

			int tmp;
			for (int i = 0; i < nb_resting_job; i++)
			{
				tmp = job_resting_for_assignement.front();
				job_resting_for_assignement.pop_front();
				tournee.add_job(tmp);
				nb_resting_job--;

				rec_branch_and_bound(covering_tree, tmp, tab_date_cost, time_travel_for_last_job);

				nb_resting_job++;
				tournee.suppr_job();
				job_resting_for_assignement.push_back(tmp);
			}

			//retablissement de l'ancien index
			last_visited_site_index = tmp_old_last_visited_site_index;
		}
		else {//rien, on coupe
			  //cout << "cut "; tournee.print(); cout << endl;
		}
	}
	else { //si une feuille, update de la upperbound
		tournee.add_job(job_resting_for_assignement.front());

		Fct_lin fct = Fct_lin::create_fct_lin(tournee, inst, upper_bound.min_a, upper_bound.max_b);
		upper_bound = Fct_lin::minimum_fct(upper_bound, fct);

		//cout << "feuille "; tournee.print(); cout << endl;

		for (int date = tab_date_cost.min_a; date <= tab_date_cost.max_b; date++) {
			if (tab_date_cost.get_cost(date) != -1) {
				vec_bool[date - tab_date_cost.min_a] = true;
			}
		}

		tournee.suppr_job();
	}
}

Branch_and_bound::Vec_date_cost Branch_and_bound::min_assigment(Covering_tree& covering_tree, Branch_and_bound::Vec_date_cost& tab_date_cost, int travel_time_to_last_job, int branch_node, vector<vector<int>>& test)
{
	Vec_date_cost swarzy(tab_date_cost.min_a, tab_date_cost.max_b, -1);

	int nb_livraison = covering_tree.edges.size(); //n noeud dans l'arbre, n-1 arete, mais on ne compte pas le noeud de branchement d'ou pars la tournee

												   /*if (branch_node == 3 && covering_tree.edges.size() == 4) {
												   cout << endl;
												   }*/

												   //*******
												   //Début initialization

												   //calcul de la suite des dates de livraison en utilisant
												   //les aretes du covering tree par ordre croissant
	vector<int> delivery_date = vector<int>(nb_livraison);
	delivery_date[0] = tab_date_cost.min_a + travel_time_to_last_job + covering_tree.edges[0].cost;
	for (int i = 1; i < nb_livraison; i++)
	{
		delivery_date[i] = delivery_date[i - 1] + covering_tree.edges[i].cost;
	}


	//initialisation de la matrice de l'algo hongrois
	vector<vector<int>> tab_job_assignement_initial = vector<vector<int>>(nb_livraison);
	for (int i = 0; i < nb_livraison; i++) tab_job_assignement_initial[i] = vector<int>(nb_livraison);


	Job* job;
	int i_job = 0;
	//pour chacun des jobs restant à assigner
	//on calcul ses pénalités de retard associé si le job
	//et livré en i^eme position
	//==> 1 ligne: 1 job
	//==> et pour chaque colonne: le retard du job associé à cette position dans la livraison
	for (int index_job : job_resting_for_assignement)
	{
		if (index_job != branch_node) {
			job = inst->list_Job[index_job];
			for (int i = 0; i < nb_livraison; i++)
			{
				// présence de négatif, mais qui compte comme 0 dans l'algo hongrois
				tab_job_assignement_initial[i_job][i] = (delivery_date[i] - job->due_d)*job->piM;
			}
			i_job++;
		}
	}

	// ==> cette première initialisation se fait en min_a
	//FIN INITIALISATION
	//*******

	vector<vector<int>> tab_job_assignement = tab_job_assignement_initial;

	int old_valide_date = tab_date_cost.min_a;
	int increase_cost;
	int reste_job_en_avance = true;
	int min_affect_a_date;

	vector<int> zero_couvrant = vector<int>(nb_livraison);
	vector<vector<int>> zero_barre = vector<vector<int>>(nb_livraison);

	for (int date = tab_date_cost.min_a; date <= tab_date_cost.max_b; date++)
	{
		if (tab_date_cost.date_is_valide(date)) {



			//*****
			//MISE A JOUR DE LA MATRICE DE L'ALGO HONGROIS POUR CETTE DATE
			//il n'est pas nécéssaire d'augmenter les lignes de l'algo hongrois
			//pour lequelle le job est en retard à tous les postes de livraison
			//(et donc en retard pour le premier)

			//===> Conclusion: si tout les jobs sont en retard, pas besoin de mettre à jour
			//la matrice et donc de réapliqué l'algo hongrois

			//l'affectation reste la même si tous les jobs sont en retard, il suffi juste de les appliquer à la nouvelle
			//matrice initial (dont les coût continue de grimper)
			if (reste_job_en_avance) {

				// SUPPRESSION DE LA MISE A JOUR POUR TEST
				//==> RECALCUL A CHAQUE FOIS

				//*****
				//DEBUT MISE A JOUR DE LA MATRICE DE L'ALGO HONGROIS POUR CETTE DATE

				if (date != old_valide_date) {

					reste_job_en_avance = false;

					i_job = 0;
					for (int index_job : job_resting_for_assignement)
					{
						//il n'est pas nécéssaire d'augmenter les lignes de l'algo hongrois
						//pour lequelle le job est en retard à tous les postes de livraison
						//(et donc en retard (ou juste à l'heure) pour le premier)
						//d'après le tableau d'assignement initial
						if (index_job != branch_node) {

							if (tab_job_assignement_initial[i_job][0] < 0) {

								reste_job_en_avance = true;

								job = inst->list_Job[index_job];

								increase_cost = (date - old_valide_date)*job->piM;

								for (int i = 0; i < nb_livraison; i++)
								{
									tab_job_assignement[i_job][i] += increase_cost;
								}
							}
							i_job++;
						}
					}
				}
				//FIN MISE A JOUR DE LA MATRICE POUR CETTE DATE
				//*****

				//*/


				//==> mise a jour systhematique par recopie
				/*i_job = 0;
				for each (int index_job in job_resting_for_assignement)
				{
				if (index_job != branch_node) {
				job = inst->list_Job[index_job];
				for (int i = 0; i < nb_livraison; i++)
				{
				// présence de négatif, mais qui compte comme 0 dans l'algo hongrois
				tab_job_assignement[i_job][i] = tab_job_assignement_initial[i_job][i];
				}
				i_job++;
				}
				}//*/

				//*****
				//DEBUT ALGO HONGROIS 
				//Note : considérer les vals négatives comme des zéros 
				//représente des changement minimie
				////
				//Etape 0 - Etape de réduction
				////

				//Pour chaque ligne de la matrice, on retire à l'ensemble de la ligne la valeur minimale de celle-ci. La matrice a alors au moins un zéro par ligne. On répète la même opération sur les colonnes. 
				//On obtient alors un problème équivalent avec une matrice ayant au moins un zéro par ligne et par colonne. (WIKIPEDIA)

				//==> pour les lignes
				//on sait que le plus petit element d'une ligne est son premier element
				//(correspond au cout associer si le job est livré en premier
				int cout_min;
				for (int i = 0; i < nb_livraison; i++)
				{
					cout_min = tab_job_assignement[i][0];
					if (cout_min > 0) {
						for (int j = 0; j < nb_livraison; j++)
						{
							tab_job_assignement[i][j] -= cout_min;
						}
					}
				}

				//==> pour les colonnes
				int min;
				for (int j = 0; j < nb_livraison; j++)
				{
					//recherche du min
					min = numeric_limits<int>::max();
					for (int i = 0; i < nb_livraison; i++)
					{
						if (min > tab_job_assignement[i][j]) min = tab_job_assignement[i][j];
					}

					//soustraction du min
					if (min > 0) {
						for (int i = 0; i < nb_livraison; i++)
						{
							tab_job_assignement[i][j] -= min;
						}
					}
				}

				/*if (date == 55) {
				cout << "";
				}*/

				bool fini = false;
				while (!fini) {

					////
					//Etape 1
					////

					//Selectionner les 0 couvrants
					aux_0_couvrant_non_couvrant(tab_job_assignement, zero_couvrant, zero_barre);

					int in = 0;

					//test matrice couverte
					fini = true; int cpt = 0;
					while (cpt < nb_livraison && fini) {
						fini = fini && zero_couvrant[cpt] != -1;
						cpt++;
					}

					if (!fini) {

						////
						// Etape 2
						////
						//Marquer ligne et barre colonne

						vector<bool> ligne_marque = vector<bool>(nb_livraison);
						vector<bool> colonne_marque = vector<bool>(nb_livraison);

						Branch_and_bound::aux_ligne_marquee_colonne_barree(zero_couvrant, zero_barre, ligne_marque, colonne_marque);


						////
						// Etape 3
						////

						//retrait et addition
						Branch_and_bound::aux_retrait_addition(tab_job_assignement, ligne_marque, colonne_marque);
					}
				}
				//FIN ALGO HONGROIS
				//******
			}//fin reste des jobs en avance

			 //******
			 //MISE A JOUR DE LA MATRICE INITIALE POUR CETTE DATE
			 //à chaque fois que la date avance, 
			 //les couts associés dans la matrice initial augmentent
			i_job = 0;
			for (int index_job : job_resting_for_assignement)
			{
				if (index_job != branch_node) {
					job = inst->list_Job[index_job];

					increase_cost = (date - old_valide_date)*job->piM;

					for (int i = 0; i < nb_livraison; i++)
					{
						tab_job_assignement_initial[i_job][i] += increase_cost;
					}
					i_job++;
				}
			}
			//FIN MISE A JOUR DE LA MATRICE INITIALE POUR CETTE DATE
			//******

			//*****
			//RECUPERATION DU COUT ASSOCIé A CETTE DATE
			min_affect_a_date = 0;
			for (int i = 0; i < tab_job_assignement_initial.size(); i++)
			{
				//compte uniquement des valeurs positives, zéro sinon
				min_affect_a_date += max(0, tab_job_assignement_initial[i][zero_couvrant[i]]);
			}


			/*i_job = 0;
			cout << "Date : " << date << endl;
			for each (int index_job in job_resting_for_assignement)
			{
			if (index_job != branch_node) {
			job = inst->list_Job[index_job];

			increase_cost = (date - old_valide_date)*job->piM;

			for (int i = 0; i < nb_livraison; i++)
			{
			cout << tab_job_assignement_initial[i_job][i] << "\t";
			}

			cout << "\t";

			for (int i = 0; i < nb_livraison; i++)
			{
			cout << tab_job_assignement[i_job][i] << "\t";
			}

			i_job++;
			}cout << endl;
			}cout << endl;*/

			//incrémentation du tab de renvoie swarzy
			//(initialisé a -1 pour chaque date)
			swarzy.add_cost(date, min_affect_a_date + 1);
			//FIN Récupération
			//*****

			old_valide_date = date;

		}//fin date valide 

	}//FIN for pour toute date 
	 //for (int date = tab_date_cost.min_a; date <= tab_date_cost.max_b; date++)

	return swarzy;
}

void Branch_and_bound::aux_0_couvrant_non_couvrant(const vector<vector<int>>& matrix, vector<int>& zero_couvrant, vector<vector<int>>& zero_barre)
{
	int taille = matrix.size();

	for (int i = 0; i < taille; i++) zero_couvrant[i] = -1;
	for (int i = 0; i < taille; i++) zero_barre[i].clear();

	vector<vector<int>> zero_ligne = vector<vector<int>>(taille);
	vector<vector<int>> zero_colonne = vector<vector<int>>(taille);

	vector<int> zero_couvrant_col = vector<int>(taille);
	for (int i = 0; i < taille; i++) zero_couvrant_col[i] = -1;

	//repertorier l'essemble des zeros
	for (int i = 0; i < taille; i++) {
		for (int j = 0; j < taille; j++) {
			//les valeurs négative compte comme des 0
			if (matrix[i][j] <= 0) {
				zero_ligne[i].push_back(j);
				zero_colonne[j].push_back(i);
			}
		}
	}


	int ind_ligne_du_zero;
	int ind_colonne_du_zero;

	int nb_min_zero;

	for (int nb_zero_trouvee = 0; nb_zero_trouvee < taille; nb_zero_trouvee++)
	{
		nb_min_zero = numeric_limits<int>::max();

		int index = 0;
		//parcours des lignes
		while (index < taille && nb_min_zero != 1)
		{
			if (zero_ligne[index].size() != 0 && zero_ligne[index].size() < nb_min_zero) {
				nb_min_zero = zero_ligne[index].size();
				ind_ligne_du_zero = index;
				ind_colonne_du_zero = zero_ligne[index][0]; //on considère que l'on enlève le premier 0 de la ligne / de la colonne
			} index++;
		}

		//cout << ind_ligne_du_zero << "\t" << ind_colonne_du_zero << endl;

		index = 0;
		//parcours des colonnes
		while (index < taille && nb_min_zero != 1)
		{
			if (zero_colonne[index].size() != 0 && zero_colonne[index].size() < nb_min_zero) {
				nb_min_zero = zero_colonne[index].size();
				ind_colonne_du_zero = index;
				ind_ligne_du_zero = zero_colonne[index][0]; //on considère que l'on enlève le premier 0 de la ligne / de la colonne
			} index++;
		}

		//remplissage de zero couvrant et zero barré
		zero_couvrant[ind_ligne_du_zero] = ind_colonne_du_zero;
		zero_couvrant_col[ind_colonne_du_zero] = ind_ligne_du_zero;

		//suppression dans zero_ligne
		for (int  num_ligne : zero_colonne[ind_colonne_du_zero])
		{
			if (num_ligne != ind_ligne_du_zero) {

				//!!! ajout de zero barre ici
				zero_barre[num_ligne].push_back(ind_colonne_du_zero);

				index = zero_ligne[num_ligne].size() - 1;
				while (zero_ligne[num_ligne][index] != ind_colonne_du_zero) index--; //l'index que l'on cherche existe
				zero_ligne[num_ligne].erase(zero_ligne[num_ligne].begin() + index);
			}
		}

		//suppression dans zero_colonne
		for (int  num_colonne : zero_ligne[ind_ligne_du_zero])
		{
			if (num_colonne != ind_colonne_du_zero) {

				//!!! ajout de zero barre ici
				zero_barre[ind_ligne_du_zero].push_back(num_colonne);

				index = zero_colonne[num_colonne].size() - 1;
				while (zero_colonne[num_colonne][index] != ind_ligne_du_zero) index--; //l'index que l'on cherche existe
				zero_colonne[num_colonne].erase(zero_colonne[num_colonne].begin() + index);
			}
		}

		//suppression de la ligne et de la colonne contenant le 0
		zero_colonne[ind_colonne_du_zero].clear();
		zero_ligne[ind_ligne_du_zero].clear();
	}

	aux_approfondisement_allocation_de_ligne(zero_barre, zero_couvrant, zero_couvrant_col);


	/*for each (auto& var in matrix)
	{
	for each(int i in var) {
	cout << i << " ";
	}cout << endl;
	}cout << endl;

	for each (auto& var in zero_barre)
	{
	for each(int i in var) {
	cout << i << " ";
	}cout << endl;
	}cout << endl;

	for each (int var in zero_couvrant)
	{
	cout << var << endl;
	}cout << endl;*/





}

void Branch_and_bound::aux_ligne_marquee_colonne_barree(vector<int>& zero_couvrant, vector<vector<int>>& zero_barre, vector<bool>& ligne_marque, vector<bool>& colonne_marque)
{
	int taille = zero_couvrant.size();

	for (int i = 0; i < taille; i++) ligne_marque[i] = zero_couvrant[i] == -1;

	//for each (bool var in ligne_marque)cout << var << endl;

	bool new_ligne_marque = true;

	while (new_ligne_marque) {
		new_ligne_marque = false;
		int cpt_c;
		//on barre les colonne avec des zero barrés sur les ligne marque
		for (int j = 0; j < taille; j++)
		{
			if (!colonne_marque[j]) {
				for (int i = 0; i < taille; i++)
				{
					if (ligne_marque[i]) {
						cpt_c = 0;
						while (cpt_c < zero_barre[i].size()) {//&& zero_barre[i][cpt_c] <= j FAUX pas forcement barré dans l'ordre
							if (zero_barre[i][cpt_c] == j) {
								colonne_marque[j] = true;
								cpt_c = zero_barre[i].size();
							}
							cpt_c++;
						}
					}
				}
			}
		}

		for (int i = 0; i < taille; i++) {
			if (zero_couvrant[i] != -1 && !ligne_marque[i]) {
				if (colonne_marque[zero_couvrant[i]]) {
					ligne_marque[i] = true;
					new_ligne_marque = true;
				}
			}
		}
	}
}

void Branch_and_bound::aux_retrait_addition(vector<vector<int>>& matrix, vector<bool>& ligne_marque, vector<bool>& colonne_marque) {
	int min_value = numeric_limits<int>::max();
	int taille = ligne_marque.size();

	for (int i = 0; i < taille; i++)
	{
		if (ligne_marque[i]) {
			for (int j = 0; j < taille; j++)
			{
				if (!colonne_marque[j]) {
					if (min_value > matrix[i][j]) min_value = matrix[i][j];
				}
			}
		}
	}

	for (int i = 0; i < taille; i++)
	{
		if (ligne_marque[i]) {
			for (int j = 0; j < taille; j++)
			{
				if (!colonne_marque[j]) {
					matrix[i][j] -= min_value;
				}
			}
		}
		else {
			for (int j = 0; j < taille; j++)
			{
				if (colonne_marque[j]) {
					matrix[i][j] += min_value;
				}
			}
		}
	}


}

void Branch_and_bound::aux_approfondisement_allocation_de_ligne(vector<vector<int>>& zero_barre, vector<int>& zero_couvrant, vector<int>& zero_couvrant_col)
{
	int taille = zero_couvrant.size();
	vector<int> colonne_i_acces_via_lin = vector<int>(taille);

	/*int index = 0;
	for (int j = 0; j < taille; j++)
	{
	if (zero_couvrant[j] != -1) {
	cout << zero_couvrant[j] << "\t";
	}
	else { cout << "-\t"; }

	index = 0;
	sort(zero_barre[j].begin(), zero_barre[j].end());

	for (int k = 0; k < taille; k++)
	{
	if (zero_couvrant[j] == k) {
	cout << "X ";
	}else if (index < zero_barre[j].size() && k == zero_barre[j][index]) {
	cout << "0 "; index++;
	}
	else { cout << "- ";  }
	}cout << endl;

	}cout << endl;*/

	for (int ligne_non_couv = 0; ligne_non_couv < taille; ligne_non_couv++)
	{
		if (zero_couvrant[ligne_non_couv] == -1) {//cette ligne est belle est bien non allouée

			for (int i = 0; i < taille; i++) //mise à nulle des possibilité d'accés
			{
				//ligne_i_acces_via_col[i] = -1;
				colonne_i_acces_via_lin[i] = -1;
			}

			int est_couv = rec_approfondissement_de_ligne(ligne_non_couv, zero_barre, zero_couvrant, zero_couvrant_col, colonne_i_acces_via_lin);


			if (est_couv != -1) {
				int i = 0; //suppression du zero couvrant au besion
				while (zero_barre[ligne_non_couv][i] != -1) i++;
				zero_barre[ligne_non_couv].erase(zero_barre[ligne_non_couv].begin() + i);
			}
		}
	}
}

int Branch_and_bound::rec_approfondissement_de_ligne(int index_ligne, vector<vector<int>>& zero_barre, vector<int>& zero_couvrant, vector<int>& zero_couvrant_col, vector<int>& colonne_i_acces_via_lin)
{
	int ind_new_ligne;
	int ind_colonne_libre = -1;

	int ind_col;
	int i = 0;
	while (i < zero_barre[index_ligne].size() && ind_colonne_libre == -1)
	{
		ind_col = zero_barre[index_ligne][i];

		if (colonne_i_acces_via_lin[ind_col] == -1) { //si la colonne n'est pas déjà accedé

			colonne_i_acces_via_lin[ind_col] = index_ligne; //pour la condition d'arret => colonne_i_acces_via_lin est "plein" 
															// de val != 1

			if (zero_couvrant_col[ind_col] == -1) {
				//cette colonne n'est pas couverte, on peut établir une correspondance ligne colonne non couvert

				ind_colonne_libre = ind_col;

			}
			else {
				ind_new_ligne = zero_couvrant_col[ind_col]; //sinon on propage
				if (-1 != rec_approfondissement_de_ligne(ind_new_ligne, zero_barre, zero_couvrant, zero_couvrant_col, colonne_i_acces_via_lin))
				{
					ind_colonne_libre = ind_col;
				}
			}

		}
		else {
			i++;
		}

	}

	// Propagation
	if (ind_colonne_libre != -1) {
		zero_couvrant_col[ind_colonne_libre] = index_ligne; //on couvre la colonne
		zero_barre[index_ligne][i] = zero_couvrant[index_ligne]; //on barre l'actuel 0 couvrant
		zero_couvrant[index_ligne] = ind_colonne_libre; //on couvre la ligne

	}

	//Type de renvoie
	if (ind_colonne_libre != -1) {
		return zero_couvrant[index_ligne];
	}
	else {
		return -1;
	}
}
