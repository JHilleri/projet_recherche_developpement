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

	/*
		array of batchs, each batch contain the index of his jobs
		! never used
		TODO : use Instance::tab_batch or remove it 
	*/
	vector<vector<int>> tab_batch = vector<vector<int>>();

	/*
		the name of the original file containing the instance
		TODO : extract the file management from the Instance class
	*/
	string nom_fichier;
	void reecrire_Instance(bool mise_en_batch = false);

	//int nbV; 

	// inutilisé
	//TODO : identifier Instance::cV
	int cV;
	//vector<int> Fk;

	//TODO: identifier Instance::T
	int T;


};


