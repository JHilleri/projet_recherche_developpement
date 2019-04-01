#include "Fct_lin.h"
#include "Branch_and_bound.h" 


int Fct_lin::compteur = 0;

Tournee Branch_and_bound::test;
int Branch_and_bound::ind_test;
vector<bool> Branch_and_bound::vec_bool;

Link Fct_lin::create_link(int t, int c, int alpha, const Tournee& tr) {
	Link l; l.t = t; l.c = c; l.alpha = alpha; l.tournee = tr.copy();
	l.is_locally_optimal = false;
	return l;
}

Tournee Tournee::create_tournee(const vector<int>& vect)
{
	Tournee ret;
	ret.nb = vect.size();
	ret.tab_c = new int[ret.nb];
	for (int i = 0; i < ret.nb; i++)
	{
		ret.tab_c[i] = vect[i];
	}
	return ret;
}

Tournee Tournee::create_tournee_vide(int nb_site)
{
	Tournee ret;
	ret.nb = 0;
	ret.tab_c = new int[nb_site];
	return ret;
}

void Tournee::add_job(int job)
{
	tab_c[nb] = job;
	nb++;
}

void Tournee::suppr_job()
{
	if (nb <= 0) {
		cout << "ERREUR: OQNAER" << endl;
	}
	nb--;
}

Tournee Tournee::create_tournee_random(const vector<int>& vect)
{
	Tournee swarzy = create_tournee(vect); 
	swarzy.suffle();
	return swarzy;
}

Tournee Tournee::create_tournee_EDD(const Instance& inst, const vector<int>& vect)
{
	Tournee ret;
	ret.nb = vect.size();
	ret.tab_c = new int[ret.nb];
	for (int i = 0; i < ret.nb; i++)
	{
		ret.tab_c[i] = vect[i];
	}

	int j;
	int due_date; int index;
	for (int i = 1; i < ret.nb; i++)
	{
		j = i;

		index = ret.tab_c[j];
		due_date = inst.list_Job[index].due_d;
		while (j != 0 && due_date < inst.list_Job[ret.tab_c[j - 1]].due_d) {
			ret.tab_c[j] = ret.tab_c[j - 1];
			j--;
		}
		ret.tab_c[j] = index;
	}

	return ret;
}

Tournee Tournee::create_tournee_nearest_insertion(const Instance& inst, const vector<int>& vect)
{
	Tournee ret;
	ret.nb = vect.size();
	ret.tab_c = new int[ret.nb];

	//struct trier par ordre croisant de distance
	vector<pair<int, int>> ind_dist(ret.nb);
	for (int i = 0; i < ret.nb; i++)
	{
		ind_dist[i].first = vect[i];
		ind_dist[i].second = numeric_limits<int>::max();
	}

	const Distancier& dist = inst.distancier;
	int last_index = dist.index_manu();

	int k;
	for (int i = 0; i < ret.nb; i++)
	{

		//mise � jour de la distance avec le dernier clinet atteint
		//et tri de ces distances par ordre decroissant
		for (int j = 0; j < ret.nb - i; j++)
		{
			ind_dist[j].second = dist.dist(last_index, ind_dist[j].first);
			k = j;
			auto tmp = ind_dist[j];
			while (k >= 1 && ind_dist[k].second > ind_dist[k - 1].second)
			{
				ind_dist[k] = ind_dist[k - 1];
				k--;
			}
			ind_dist[k] = tmp;
		}

		//recup du dernier �l�ment (le plus proche
		last_index = ind_dist.back().first;

		//suppr de ce dernier elemnet dans le tableau
		ind_dist.pop_back();

		//incr�mentation du tableau
		ret.tab_c[i] = last_index;
	}

	//for (int i = 0; i < ret.nb; i++) cout << ret.tab_c[i] << " " << endl;


	return ret;
}

void Tournee::inverse_tournee()
{
	int tmp = tab_c[0];
	for (int i = 0; i < nb - 1; i++)
	{
		tab_c[i] = tab_c[i + 1];
	}
	tab_c[nb - 1] = tmp;
}

void Tournee::inverse_half_tournee()
{
	int tmp_mid, tmp;
	int decalage = 0;
	int mid_s = nb / 2;
	if (nb % 2 == 1) {
		mid_s = (nb - 1) / 2;
		tmp_mid = tab_c[mid_s];
		decalage = 1;
	}

	for (int i = 0; i < mid_s; i++)
	{
		tmp = tab_c[i];
		tab_c[i] = tab_c[i + mid_s + decalage];
		tab_c[mid_s + i] = tmp;
	}

	if (nb % 2 == 1) tab_c[nb - 1] = tmp_mid;
}

void Tournee::suffle()
{
	int temp, index, i;
	for (i = 0; i < nb; i++)
	{
		index = (int)(rand() % (nb - i)) + i;

		temp = tab_c[i];
		tab_c[i] = tab_c[index];
		tab_c[index] = temp;
	}
}

Tournee Tournee::copy() const {
	Tournee ret;
	ret.nb = nb;

	ret.tab_c = new int[nb];
	for (int i = 0; i < nb; i++) {
		ret.tab_c[i] = tab_c[i];
	}
	return ret;
}

void Tournee::suppr()
{
	nb = 0;
	delete tab_c;
}

void Tournee::print()const
{
	for (int i = 0; i < nb; i++) {
		cout << tab_c[i] << ", ";
	}
}

int Tournee::eval_routing_cost(const Instance& inst) const
{
	int routing_cost = 0;


	//!!!!!!!
	//Cr�er un distancier de co�t
	//!!!!!!!
	const Distancier& dist = inst.distancier; 

	int old_index = dist.index_manu();
	int index;
	for (int i = 0; i < nb; i++)
	{
		index = tab_c[i];
		routing_cost += dist.dist(old_index, index);
		old_index = index;
	}

	return routing_cost;
}

//create a function lin for specific travel on specific intervalle
Fct_lin Fct_lin::create_fct_lin(const Tournee& road, Instance& inst, int m_a, int m_b)
{
	Fct_lin fct;

	fct.min_a = m_a;
	fct.max_b = m_b;

	fct.routing_cost = road.eval_routing_cost(inst);
	std::vector<pair<int, int>> ahead; //{ [alpha_i, avance_i], [alpha_j, avance_j] }

	Distancier& dist = inst.distancier;

	int old_index = dist.index_manu();
	int index;
	//int old_time;
	int arrival_date = m_a;
	for (int i = 0; i < road.nb; i++)
	{
		index = road.tab_c[i];
		arrival_date += dist.dist(old_index, index);

		ahead.push_back(pair < int, int>(inst.list_Job[index].piM, inst.list_Job[index].due_d - arrival_date));

		old_index = index;

		//cout << index << " : " << arrival_date << "   " << inst->list_Job[index]->due_d << "   " << inst->list_Job[index]->due_d - arrival_date <<endl;
	}


	//tri du plus en avance vers le moins
	sort(ahead.begin(), ahead.end(),
		[](pair<int, int> a, pair<int, int> b) {
		return a.second < b.second;
	});

	//Edition of the link

	int alpha = 0;
	int cost = fct.routing_cost;
	int start_time = m_a;
	int index_a = 0;

	//first link creation
	while (index_a < ahead.size() && ahead[index_a].second <= 0) {

		alpha += ahead[index_a].first;
		cost += ahead[index_a].first * (-ahead[index_a].second); // here .second is negatif
																 //cout de retard fourni par ce job

		index_a++;
	}

	fct.tab_link.push_back(create_link(start_time, cost, alpha, road));  // !!! &road


																		 //***
																		 //[first, second] = [alpha, avance(ahead)]
																		 //***

																		 //next index
	int old_time = 0;

	while (start_time < m_b && index_a < ahead.size()) {

		start_time += (ahead[index_a].second - old_time);

		if (start_time < m_b) {
			cost += alpha * (ahead[index_a].second - old_time);
			//on pars plus tard, le retard augmente proportionnellement en fonction des job deja en retard 

			old_time = ahead[index_a].second;

			alpha += ahead[index_a].first;


			while (index_a + 1 < ahead.size() && ahead[index_a].second == ahead[index_a + 1].second) {
				//si deux jobs on exactement la m�me avance (.second), on les consid�rent ensemble pour evit� d'avoir deux maillons distings
				index_a++;
				alpha += ahead[index_a].first;
			}


			fct.tab_link.push_back(create_link(start_time, cost, alpha, road));  // !!! &road

			index_a++;

		}
	}

	//add of the point in b
	//fct.tab_link.push_back(create_link(m_b , cost + (m_b - fct.tab_link.back().t)*alpha  , alpha, &road));

	//for each (auto var in fct.tab_link){cout << var.t << "  " << var.alpha << "  " << var.c << endl;}

	fct.eval_max_b = eval_link(fct.tab_link.back(), fct.max_b);

	fct.aire_sous_la_courbe = fct.calcul_aire_sous_la_courbe();

	return fct;
}


Fct_lin Fct_lin::create_fct_max(int min_a, int min_b)
{
	Fct_lin fct;
	fct.min_a = min_a;
	fct.max_b = min_b;

	Link l;
	l.alpha = 0;
	l.t = min_a;
	l.c = numeric_limits<int>::max();
	l.tournee = Tournee();
	l.tournee.tab_c = new int[0];
	l.tournee.nb = 0;

	fct.tab_link.push_back(l);
	fct.eval_max_b = numeric_limits<int>::max();
	return fct;
}

int Fct_lin::eval_fct_lin(int t)const
{
	if (t < min_a || t > max_b) {
		cout << "ERREUR : cade ALSUF" << endl;
	}

	int index = 0;
	while (index + 1 < tab_link.size() && t >= tab_link[index + 1].t) index++;

	return  tab_link[index].c + tab_link[index].alpha * (t - tab_link[index].t);
}

Tournee Fct_lin::found_tournee(int t)const
{
	if (t < min_a || t > max_b) {
		cout << "ERREUR : cade ODISX" << endl;
	}

	int index = 0;
	while (index + 1 < tab_link.size() && t >= tab_link[index + 1].t) index++;

	return  tab_link[index].tournee;
}

int Fct_lin::eval_link(const Link & l, int t)
{
	return l.c + l.alpha*(t - l.t);
}

int Fct_lin::eval_alpha_lin(int t)const
{
	if (t < min_a || t > max_b) {
		cout << "ERREUR : cade AUDSVSRF" << endl;
	}

	int index = 0;
	while (index + 1 < tab_link.size() && t >= tab_link[index + 1].t) index++;

	return  tab_link[index].alpha;
}

double Fct_lin::intersection_t(const Link & l1, const Link & l2)
{
	if (l1.alpha == l2.alpha)return -999999;

	int c1 = l1.c - (l1.t * l1.alpha);
	int c2 = l2.c - (l2.t * l2.alpha);

	double t = (double)(c1 - c2) / (double)(l2.alpha - l1.alpha);

	return t;
}

int Fct_lin::calcul_aire_sous_la_courbe(int new_min_a, int new_min_b) const
{
	int m_a = min_a, m_b = max_b;

	if (max_b == numeric_limits<int>::min()) {
		m_a = new_min_a;
		m_b = new_min_b;

		if (m_a < min_a || m_a > max_b) {
			cout << "ISBEF" << endl;
		}
		if (m_b < min_a || m_b > max_b) {
			cout << "ISBEF" << endl;
		}
	}

	auto& t_link = tab_link;

	int aire = 0;
	int index_i = -1;
	int value;
	int current_alpha;

	for (int i = m_a; i < m_b; i++)
	{
		if (index_i + 1 < t_link.size()
			&& t_link[index_i + 1].t == i) {
			index_i++;
			value = t_link[index_i].c;
			current_alpha = t_link[index_i].alpha;
		}
		else {
			value += current_alpha;
		}

		aire += value;
	}
	aire += eval_max_b;

	return aire;
}

Link Fct_lin::copy_link(const Link& l) {
	Link r = l;
	r.is_locally_optimal = l.is_locally_optimal;
	r.tournee = l.tournee.copy();
	return r;
}

Fct_lin Fct_lin::minimum_fct(const Fct_lin& f1, const Fct_lin& f2) {

	if (f1.min_a != f2.min_a || f2.max_b != f2.max_b) {
		cout << "ERREUR : code ICMSTO" << endl;
	}

	Fct_lin function;
	function.min_a = f1.min_a;
	function.max_b = f1.max_b;
	auto& fun_l = function.tab_link;

	auto& f1_l = f1.tab_link;
	int i_link_f1 = -1;
	Link link_f1;
	int end_l_f1 = 0;


	auto& f2_l = f2.tab_link;
	int i_link_f2 = -1;
	Link link_f2;
	int end_l_f2 = 0;

	int eval_link_f1;
	int eval_link_f2;

	bool f1_is_current_best_link_else_is_f2 = true;//val bidon en init
	bool insert_f1;
	bool insert_f2;

	bool new_link_is_f1;
	bool new_link_is_f2;

	while (end_l_f1 != f1.max_b || end_l_f2 != f1.max_b) {

		insert_f1 = false;
		insert_f2 = false;

		//*********
		//selection du / des nouveaux segments + evaluation
		new_link_is_f1 = false;
		new_link_is_f2 = false;

		if (end_l_f1 <= end_l_f2)
			new_link_is_f1 = true;
		if (end_l_f1 >= end_l_f2)
			new_link_is_f2 = true;

		if (new_link_is_f1) {
			i_link_f1++;
			if (i_link_f1 < f1_l.size()) {
				link_f1 = f1_l[i_link_f1];

				eval_link_f1 = link_f1.c;
				if (!new_link_is_f2) eval_link_f2 = eval_link(link_f2, link_f1.t);

				if (i_link_f1 < f1_l.size() - 1) {
					end_l_f1 = f1_l[i_link_f1 + 1].t;
				}
				else { end_l_f1 = f1.max_b; }
			}
		}


		if (new_link_is_f2) {
			i_link_f2++;
			if (i_link_f2 < f2_l.size()) {
				link_f2 = f2_l[i_link_f2];

				eval_link_f2 = link_f2.c;
				if (!new_link_is_f1) eval_link_f1 = eval_link(link_f1, link_f2.t);

				if (i_link_f2 < f2_l.size() - 1) {
					end_l_f2 = f2_l[i_link_f2 + 1].t;
				}
				else { end_l_f2 = f1.max_b; }
			}
		}

		//fin selection
		//**********


		//*******
		// gestion de l'insertion des liens par rapport � leur origine
		if (new_link_is_f1 == new_link_is_f2) { //cas o� les deux points d'inflexion sont aux m�mes endroits
			if (eval_link_f1 < eval_link_f2) { // ici, equivalent � : link_f1.c < link_f2.c
				insert_f1 = true;
			}
			else if (eval_link_f1 > eval_link_f2) {
				insert_f2 = true;
			}
			else if (link_f1.alpha < link_f2.alpha) {
				insert_f1 = true;
			}
			else if (link_f1.alpha > link_f2.alpha) {
				insert_f2 = true;

				//�gualit� stricte, on met le maillon qui dure de plus longtemps
			}
			else if (end_l_f1 > end_l_f2) {
				insert_f1 = true;
			}
			else {
				insert_f2 = true;
			}
		}
		else {
			if (new_link_is_f1 || !f1_is_current_best_link_else_is_f2) {

				//test placement segment
				if (eval_link_f2 > eval_link_f1 || (eval_link_f2 == eval_link_f1 && link_f1.alpha < link_f2.alpha)
					|| (eval_link_f2 == eval_link_f1 && link_f1.alpha == link_f2.alpha && end_l_f1 >= end_l_f2)) {
					insert_f1 = true;
				}
			}

			if (new_link_is_f2 || f1_is_current_best_link_else_is_f2) {

				//test placement segment
				if (eval_link_f1 > eval_link_f2 || (eval_link_f1 == eval_link_f2 && link_f2.alpha < link_f1.alpha)
					|| (eval_link_f1 == eval_link_f2 && link_f1.alpha == link_f2.alpha && end_l_f1 < end_l_f2)) {
					insert_f2 = true;
				}
			}
		}

		if (insert_f1 == true && insert_f2 == true) {
			cout << endl;
		}

		if (insert_f1) {
			fun_l.push_back(Fct_lin::copy_link(link_f1));
			if (!new_link_is_f1) {
				fun_l.back().c = eval_link_f1;
				fun_l.back().t = link_f2.t;
			}

			f1_is_current_best_link_else_is_f2 = true;
		}
		else if (insert_f2)
		{
			fun_l.push_back(Fct_lin::copy_link(link_f2));
			if (!new_link_is_f2) {
				fun_l.back().c = eval_link_f2;
				fun_l.back().t = link_f1.t;
			}

			f1_is_current_best_link_else_is_f2 = false;
		}
		//FIN gestion
		//*******

		double intersection;
		int intersection_entier_sup;

		//test intersection
		intersection = Fct_lin::intersection_t(link_f1, link_f2);
		insert_f1 = false;
		insert_f2 = false;

		if (intersection > link_f1.t  //intersection en dehors des limites des segments
			&& intersection > link_f2.t
			&& intersection <= end_l_f1 - 1
			&& intersection <= end_l_f2 - 1) {
			//s'il y a une intersection, c'est que le plus grand deviens le plus petit et vice-versa

			intersection_entier_sup = (int)intersection;
			if ((double)intersection_entier_sup < intersection) {
				intersection_entier_sup++;
			}

			//le meilleur lien change
			f1_is_current_best_link_else_is_f2 = !f1_is_current_best_link_else_is_f2;

			if (f1_is_current_best_link_else_is_f2) {
				fun_l.push_back(Fct_lin::copy_link(link_f1));
				fun_l.back().t = intersection_entier_sup;
				fun_l.back().c = Fct_lin::eval_link(link_f1, intersection_entier_sup);
			}
			else {
				fun_l.push_back(Fct_lin::copy_link(link_f2));
				fun_l.back().t = intersection_entier_sup;
				fun_l.back().c = Fct_lin::eval_link(link_f2, intersection_entier_sup);
			}

		}

		//***
		//rustine pour la local optimalite
		if (fun_l.back().alpha == link_f1.alpha &&
			fun_l.back().c == Fct_lin::eval_link(link_f1, fun_l.back().t) &&
			link_f1.is_locally_optimal) {
			fun_l.back().is_locally_optimal = true;
		}

		if (fun_l.back().alpha == link_f2.alpha &&
			fun_l.back().c == Fct_lin::eval_link(link_f2, fun_l.back().t) &&
			link_f2.is_locally_optimal) {
			fun_l.back().is_locally_optimal = true;
		}
		//***

		//*** //dans de rare cas, j'ai des points redondants que je n'arrive pas a enlever
		int size = fun_l.size();
		if (size > 2 && fun_l[size - 1].t == fun_l[size - 2].t) {
			if (fun_l[size - 1].c < fun_l[size - 2].c
				|| (fun_l[size - 1].c == fun_l[size - 2].c && fun_l[size - 1].alpha < fun_l[size - 2].alpha)) {
				fun_l[size - 2].tournee.suppr();
				fun_l[size - 2] = fun_l[size - 1];
			}
			else {
				fun_l[size - 1].tournee.suppr();
			}

			function.print_fct_lin(); cout << endl;

			fun_l.pop_back();
		}
	}

	function.eval_max_b = min(f1.eval_max_b, f2.eval_max_b);

	//function.aire_sous_la_courbe = function.calcul_aire_sous_la_courbe();

	//****
	//Suppression des tournees des fonction utilis� (save de memoire);
	//****
	for(auto var : f1.tab_link) { var.tournee.suppr(); }
	for(auto var : f2.tab_link) { var.tournee.suppr(); }

	return function;
}

void Fct_lin::print_fct_lin()const {
	for(auto var : tab_link)
	{
		cout << var.t << "\t" << var.c << "\t" << var.alpha << "\t";

		var.tournee.print();

		cout << endl;
	}
	cout << max_b << "\t" << eval_max_b << endl;
}


double Fct_lin::ratio_courbe_inf(const Fct_lin & fct_1, const Fct_lin & fct_2)
{
	int m_a = min(fct_1.tab_link[1].t, fct_2.tab_link[1].t); //premier t avec un alpha non nulle
	int m_b = max(fct_1.tab_link.back().t, fct_2.tab_link.back().t); // dernier t

	int min_RT = min(fct_1.tab_link[1].c, fct_2.tab_link[1].c);

	double aire_fct_1 = fct_1.calcul_aire_sous_la_courbe(m_a, m_b);

	double aire_fct_2 = fct_2.calcul_aire_sous_la_courbe(m_a, m_b);

	aire_fct_1 -= min_RT * (m_b - m_a);
	aire_fct_2 -= min_RT * (m_b - m_a);

	if (aire_fct_1 == 0) aire_fct_1 = 1;

	double swarzy = (aire_fct_2 - aire_fct_1) / aire_fct_1;

	return swarzy;
}

pair<int, int> Fct_lin::a_b_inf(Instance& inst, const vector<int>& batch_old)
{
	auto& jobs = inst.list_Job;
	int min_dd_date = jobs[batch_old.front()].due_d;
	int max_dd_date = jobs[batch_old.front()].due_d;

	int due_date;
	for (int i = 1; i < batch_old.size(); i++)
	{
		due_date = jobs[i].due_d;

		if (due_date < min_dd_date) min_dd_date = due_date;
		if (due_date > max_dd_date) max_dd_date = due_date;
	}

	auto& dist = inst.distancier;
	int max_dist = 0;
	int distance;
	for (int i = 0; i < batch_old.size(); i++)
	{
		distance = dist.dist(dist.index_manu(), batch_old[i]);
		if (distance > max_dist) {
			max_dist = distance;
		}

		for (int j = i + 1; j < batch_old.size(); j++)
		{
			distance = dist.dist(batch_old[i], batch_old[j]);
			if (distance > max_dist) {
				max_dist = distance;
			}
		}
	}

	int duree_tournee = max_dist * batch_old.size();

	return pair<int, int>(min_dd_date - duree_tournee, max_dd_date);
}

Fct_lin Fct_lin::generate_fct(Instance & inst, const vector<int>& batch_old, int method, int min_a, int max_b)
{
	if (min_a == numeric_limits<int>::min() || max_b == numeric_limits<int>::min()) {

		auto a_b = a_b_inf(inst, batch_old);

		min_a = a_b.first;
		max_b = a_b.second;
	}

	Fct_lin best = Fct_lin::create_fct_max(0, 0);
	Fct_lin f_EDD = Fct_lin::create_fct_max(0, 0);
	Fct_lin f_near = Fct_lin::create_fct_max(0, 0);
	Tournee  t_EDD = Tournee::create_tournee_vide(0);
	Tournee  t_near = Tournee::create_tournee_vide(0);
	Branch_and_bound b_and_b{ inst };

	switch (method) // par defaut du prototype EDD_nearx4
	{
	case EDD:
		t_EDD = Tournee::create_tournee_EDD(inst, batch_old);
		best = Fct_lin::generate_fct_with_local_search(inst, t_EDD, min_a, max_b);
		break;
	case EDDx4:
		t_near = Tournee::create_tournee_nearest_insertion(inst, batch_old);
		best = Fct_lin::generate_fct_with_local_search(inst, t_near, min_a, max_b);
		break;
	case near:
		t_EDD = Tournee::create_tournee_EDD(inst, batch_old);
		best = Fct_lin::generate_fct_with_four_extrema(inst, t_EDD, min_a, max_b);
		break;
	case nearx4:
		t_near = Tournee::create_tournee_nearest_insertion(inst, batch_old);
		best = Fct_lin::generate_fct_with_four_extrema(inst, t_near, min_a, max_b);
		break;
	case EDD_near:
		t_EDD = Tournee::create_tournee_EDD(inst, batch_old);
		f_EDD = Fct_lin::generate_fct_with_local_search(inst, t_EDD, min_a, max_b);
		t_near = Tournee::create_tournee_nearest_insertion(inst, batch_old);
		f_near = Fct_lin::generate_fct_with_local_search(inst, t_near, min_a, max_b);

		best = Fct_lin::minimum_fct(f_EDD, f_near);
		break;
	case EDD_nearx4:
		t_EDD = Tournee::create_tournee_EDD(inst, batch_old);
		f_EDD = Fct_lin::generate_fct_with_four_extrema(inst, t_EDD, min_a, max_b);

		t_near = Tournee::create_tournee_nearest_insertion(inst, batch_old);
		f_near = Fct_lin::generate_fct_with_four_extrema(inst, t_near, min_a, max_b);

		best = Fct_lin::minimum_fct(f_EDD, f_near);
		break;
	case B_and_B:
		best = b_and_b.generate_fct_with_branch_and_bound(batch_old, min_a, max_b);

		break;

	default: cout << "Method non reconnu" << endl;
		break;
	}

	return best;
}

Fct_lin Fct_lin::generate_pure_random_fct(Instance & inst, const vector<int>& batch_old, int min_a, int max_b)
{
	Tournee t = Tournee::create_tournee(batch_old);

	Fct_lin fct = Fct_lin::create_fct_lin(t, inst, min_a, max_b); compteur++;
	Fct_lin best = fct;

	for (int j = 0; j < 100000; j++)
	{
		t.suffle();
		fct = Fct_lin::create_fct_lin(t, inst, min_a, max_b); compteur++;

		best = Fct_lin::minimum_fct(best, fct);
	}
	return best;
}

Fct_lin Fct_lin::generate_fct_with_local_search(Instance & inst, Tournee& tournee_init, int min_a, int max_b)
{
	////////
	//Initialisation tourn�e courant EDD

	Tournee tournee_c = tournee_init.copy();

	Fct_lin fct = create_fct_lin(tournee_c, inst, min_a, max_b); compteur++;
	Fct_lin best = fct;

	//End Initialisation
	////////

	int nJ = tournee_c.nb;

	int t_ref = min_a; //date d'evaluation courante des fonction
					   //augment au cours de l'algo
	int min_ref = best.eval_fct_lin(t_ref); // valeur en t_ref de la meilleur tourne (pour t_ref)

	int best_i; //mouvement vers le meilleur voisin
	int best_j;
	int min_tmp;
	bool is_local_optima;

	t_ref = best.tab_link[0].t;

	tournee_c.suppr();
	tournee_c = best.tab_link[0].tournee.copy();

	min_ref = best.eval_fct_lin(t_ref);


	while (t_ref < max_b) {

		best_i = 0; best_j = 0;
		is_local_optima = true;

		/////////
		//recherche de voisinage (quadratique) de niveau 1
		//operateur swap et EBFSR
		for (int i = 0; i < nJ; i++)
		{
			//for (int j = i; j < nJ; j++) //operateur SWAP
			for (int j = 0; j < nJ; j++) //operateur EBFSR
			{
				if (i != j) {

					//tournee_c.swap(i, j); //operateur SWAP
					tournee_c.ebfsr(i, j); //operateur EBFSR

										   //ajout systh�matique de toute fonction cr�er
										   //� la fonction de referance en construction
										   /////
					fct = create_fct_lin(tournee_c, inst, min_a, max_b); compteur++;
					best = minimum_fct(best, fct);

					/////

					//evaluation de la qualit� du voisin
					min_tmp = fct.eval_fct_lin(t_ref);
					if (min_tmp < min_ref) {
						is_local_optima = false;
						min_ref = min_tmp;
						best_i = i;
						best_j = j;
					}

					//tournee_c.reverse_swap(i, j);//operateur SWAP
					tournee_c.reverse_ebfsr(i, j); //operateur EBFSR
				}
			}
		}
		//fin recherche de voisinage
		////////////

		if (!is_local_optima) {
			//transfert vers le meilleur voisin, la recherche continue

			//tournee_c.swap(best_i,best_j);//operateur SWAP
			tournee_c.ebfsr(best_i, best_j);//operateur EBFSR

											//is_all_local_optima = false;
		}
		else {

			int i_link = 0;
			// 1 seul passage

			//d�tection sur la fct best du prochain point d'inflexion (apr�s t_ref)...
			/*while (i_link < best.tab_link.size() && best.tab_link[i_link].t <= t_ref) i_link++;
			// n'impliquant pas la meilleure tournee courante
			while (i_link < best.tab_link.size() && tournee_c.equal(best.tab_link[i_link].tournee)) i_link++;
			///*/


			//Tous local optimal
			//cout << "---" << endl; tournee_c.print(); cout << endl << "---" << endl;
			for (int i = 0; i < best.tab_link.size(); i++)//la tournee issue de la recherche local est localement optimale
			{
				if (best.tab_link[i].tournee.equal(tournee_c)) {
					best.tab_link[i].is_locally_optimal = true;
				}
			}


			while (i_link < best.tab_link.size() && best.tab_link[i_link].t < t_ref) i_link++;
			if (i_link < best.tab_link.size()) {
				tournee_c.suppr();
				tournee_c = best.tab_link[i_link].tournee.copy();
				for (int i = 0; i < best.tab_link.size(); i++)//la tournee � t_ref apr�s recherche local est localement optimale
				{
					if (best.tab_link[i].tournee.equal(tournee_c)) {
						best.tab_link[i].is_locally_optimal = true;
					}
				}
			}

			i_link = 0;
			while (i_link < best.tab_link.size() && best.tab_link[i_link].is_locally_optimal) i_link++;

			if (i_link != best.tab_link.size()) {

				t_ref = best.tab_link[i_link].t;

				tournee_c.suppr();
				tournee_c = best.tab_link[i_link].tournee.copy();

				min_ref = best.eval_fct_lin(t_ref);

			}
			else {
				t_ref = max_b;
			}

		}
	}

	tournee_c.suppr();
	return best;
}

Fct_lin Fct_lin::generate_fct_with_four_extrema(Instance & inst, Tournee & tournee, int min_a, int max_b)
{
	Fct_lin swarzy = Fct_lin::generate_fct_with_local_search(inst, tournee, min_a, max_b);

	tournee.inverse_tournee();
	Fct_lin tmp_fct = Fct_lin::generate_fct_with_local_search(inst, tournee, min_a, max_b);
	swarzy = Fct_lin::minimum_fct(swarzy, tmp_fct);

	tournee.inverse_half_tournee();
	tmp_fct = Fct_lin::generate_fct_with_local_search(inst, tournee, min_a, max_b);
	swarzy = Fct_lin::minimum_fct(swarzy, tmp_fct);

	tournee.inverse_tournee();
	tmp_fct = Fct_lin::generate_fct_with_local_search(inst, tournee, min_a, max_b);
	swarzy = Fct_lin::minimum_fct(swarzy, tmp_fct);

	return swarzy;
}

Fct_lin Fct_lin::rec_generate_fct_with_total_enumeration(Instance & inst, Fct_lin& best, Tournee& tournee, list<int>& batch_old, int size)
{
	if (size != 0) {
		int tmp;
		for (int i = 0; i < size; i++)
		{
			tmp = batch_old.front();
			batch_old.pop_front();

			tournee.add_job(tmp);

			rec_generate_fct_with_total_enumeration(inst, best, tournee, batch_old, size - 1);

			tournee.suppr_job();
			batch_old.push_back(tmp);
		}
	}
	else {
		Fct_lin fct = create_fct_lin(tournee, inst, best.min_a, best.max_b); compteur++;
		best = Fct_lin::minimum_fct(best, fct);
	}

	return best;
}

Fct_lin Fct_lin::generate_fct_with_total_enumeration(Instance & inst, vector<int>& batch_old, int min_a, int max_b)
{
	Fct_lin best = create_fct_max(min_a, max_b);

	list<int> bat;
	for (int i = 0; i < batch_old.size(); i++)
	{
		bat.push_back(batch_old[i]);
	}

	Tournee tournee = Tournee::create_tournee_vide(batch_old.size());

	best = Fct_lin::rec_generate_fct_with_total_enumeration(inst, best, tournee, bat, batch_old.size());

	return best;
}

int Fct_lin::search_optima_with_big_neiborhood(Instance& inst, Tournee& tournee_c, int depart) {

	Fct_lin fct = Fct_lin::create_fct_lin(tournee_c, inst, depart, depart); compteur++;

	//End Initialisation
	////////

	int nJ = tournee_c.nb;

	int min_ref = fct.eval_fct_lin(depart);
	int min_tmp;

	int b_i1; int b_j1;
	int b_i2; int b_j2;
	int b_i3; int b_j3;
	int b_i4; int b_j4;
	bool is_local_optima = false;

	while (!is_local_optima) {

		b_i1 = 0; b_j1 = 0;
		b_i2 = 0; b_j2 = 0;
		b_i3 = 0; b_j3 = 0;
		b_i4 = 0; b_j4 = 0;

		is_local_optima = true;

		/////////
		//recherche de voisinage (quadratique) de niveau 1
		//operateur swap et EBFSR
		for (int i1 = 0; i1 < nJ; i1++)
		{
			for (int j1 = 0; j1 < nJ; j1++) //operateur EBFSR
			{
				if (i1 != j1) {
					for (int i2 = i1 + 1; i2 < nJ; i2++)
					{
						for (int j2 = 0; j2 < nJ; j2++)
						{
							//if (i2 != j1) { //pour �viter de red�placer le premier job
							for (int i3 = i2 + 1; i3 < nJ; i3++)
							{
								for (int j3 = 0; j3 < nJ; j3++)
								{
									/*for (int i4 = i3 + 1; i4 < nJ; i4++)
									{
									for (int j4 = 0; j4 < nJ; j4++)
									{*/
									//le deuxi�me job peut faire du sur place
									tournee_c.ebfsr(i1, j1);
									tournee_c.ebfsr(i2, j2);
									tournee_c.ebfsr(i3, j3);
									//tournee_c.ebfsr(i4, j4);

									fct = Fct_lin::create_fct_lin(tournee_c, inst, depart, depart); compteur++;

									//evaluation de la qualit� du voisin
									min_tmp = fct.eval_fct_lin(depart);
									if (min_tmp < min_ref) {
										is_local_optima = false;
										min_ref = min_tmp;
										b_i1 = i1;
										b_j1 = j1;
										b_i2 = i2;
										b_j2 = j2;
										b_i3 = i3;
										b_j3 = j3;
										//b_i4 = i4;
										//b_j4 = j4;
									}

									fct.tab_link[0].tournee.suppr();

									//tournee_c.reverse_ebfsr(i4, j4);
									tournee_c.reverse_ebfsr(i3, j3);
									tournee_c.reverse_ebfsr(i2, j2);
									tournee_c.reverse_ebfsr(i1, j1);
									//}
									//}
								}
							}
							//}
						}
					}
				}
			}
		}
		//fin recherche de voisinage
		////////////

		if (!is_local_optima) {
			//transfert vers le meilleur voisin, la recherche continue

			tournee_c.ebfsr(b_i1, b_j1);//operateur EBFSR
			tournee_c.ebfsr(b_i2, b_j2);
			tournee_c.ebfsr(b_i3, b_j3);
			//tournee_c.ebfsr(b_i4, b_j4);
		}
		else {
			return min_ref;
		}
	}

	return 0;
}

batch_list Fct_lin::build_regular_batch_repartition(int nb_job, int nb_j_batch)
{
	batch_list batch_j_njb;
	int cpt_nb_job = 0;
	while (cpt_nb_job < nb_job)
	{
		batch_j_njb.push_back(vector<int>());
		for (int j = 0; j < nb_j_batch; j++)
		{
			if (cpt_nb_job < nb_job) {
				batch_j_njb.back().push_back(cpt_nb_job);
			}

			cpt_nb_job++;
		}
	}
	return batch_j_njb;
}

void Tournee::swap(int i, int j)
{
	int tmp = tab_c[i];
	tab_c[i] = tab_c[j];
	tab_c[j] = tmp;

}

void Tournee::reverse_swap(int i, int j) { swap(i, j); }

void Tournee::ebfsr(int i, int j)
{
	int val = tab_c[i];

	while (i != j) {
		if (i < j) {
			tab_c[i] = tab_c[i + 1];
			i++;
		}
		else {
			tab_c[i] = tab_c[i - 1];
			i--;
		}
	}
	tab_c[j] = val;
}

void Tournee::reverse_ebfsr(int i, int j)
{
	ebfsr(j, i);
}

bool Tournee::equal(const Tournee & t) const
{
	if (nb != t.nb) return false;

	int i = 0;
	while (i < nb) {
		if (tab_c[i] != t.tab_c[i]) return false;
		i++;
	}
	return true;
}















































