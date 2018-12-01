#pragma once

#include<string>
#include <iostream>

#include "Job.h"
#include "Distancier.h"


class Instance
{
public:
	Instance() = default;
	Instance(std::istream& fichier, bool mise_en_batch = false);

	int nJob;
	int mMachine;

	Distancier distancier;
	vector<Job> list_Job;

	vector<vector<int>> tab_batch = vector<vector<int>>();

	string nom_fichier;

	void reecrire_Instance(bool mise_en_batch = false);

	//int nbV;
	int cV;
	//vector<int> Fk;

	int T;


};


