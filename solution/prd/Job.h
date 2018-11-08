#pragma once

#include <vector>
#include <fstream>
#include <string>

using namespace std;

class Job
{
public:
	Job();
	Job(ifstream* fichier, int mMachine);
	~Job();

	void reecrire_job(ofstream* fichier);

	int index_lieu;

	vector<int> p;
	int sum_p;

	int due_d;
	int q;

	int piM; // cout de penalité producteur utilisé dans le problème
	int pi3PL;

	//coordonee
	int x;
	int y;

	//*** Coût d'inventaire variable
	vector<int> thWIP;
	int thFIN;


	//*** Coût d'inventaire fixe
	int hWIP;
	int hFIN;

};

