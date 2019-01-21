#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

class Job_old
{
public:
	Job_old();
	Job_old(std::istream &input, int mMachine);
	~Job_old();

	void reecrire_job(ofstream* fichier);

	int index_lieu;

	vector<int> p;
	int sum_p;

	int due_d;
	int q;

	int piM; // cout de penalit� producteur utilis� dans le probl�me
	int pi3PL;

	//coordonee
	int x;
	int y;

	//*** Co�t d'inventaire variable
	vector<int> thWIP;
	int thFIN;


	//*** Co�t d'inventaire fixe
	int hWIP;
	int hFIN;

};

