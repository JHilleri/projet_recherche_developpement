#include "stdafx.h"
#include "Instance.h"
#include <iostream>
#include <fstream>

using namespace std;

Instance::Instance()
{
}

Instance::Instance(string file_name, bool mise_en_batch)
{

	nom_fichier = file_name;

	ifstream fichier = ifstream(file_name, ios::in);

	if (fichier)
	{
		fichier >> nJob >> mMachine >> cV;

		if (mise_en_batch) {
			int nb_job_batch = 0;
			int cpt = 0;

			while (cpt < nJob) {
				fichier >> nb_job_batch;

				tab_batch.push_back(vector<int>());
				for (int i = 0; i < nb_job_batch; i++)
				{
					tab_batch.back().push_back(cpt + i);
				}

				cpt += nb_job_batch;
			}
		}
		else {
			//suat de ligne
			//string tmp;
			//getline(fichier, tmp);
		}


		list_Job = vector<Job*>(nJob);
		for (int i = 0; i < nJob; i++)
		{
			list_Job[i] = new Job(&fichier, mMachine);
			list_Job[i]->index_lieu = i;
		}

		distancier = Distancier(&fichier, nJob);


		fichier.close();
	}
	else { cerr << "Impossible d'ouvrir le fichier !" << endl; }
}




Instance::~Instance()
{

	for each (Job* var in list_Job)
	{
		delete var;
	}

}

void Instance::reecrire_Instance(bool mise_en_batch)
{

	ofstream fichier = ofstream(nom_fichier, ios::trunc);

	if (fichier)
	{


		fichier << nJob << " " << mMachine << endl << cV << endl;

		if (mise_en_batch) {
			for each (auto& var in tab_batch)
			{
				fichier << var.size() << " ";
			}fichier << endl;
		}

		for (int i = 0; i < nJob; i++)
		{
			fichier << "Jobs " << i << " :" << endl;
			list_Job[i]->reecrire_job(&fichier);

		}

		fichier << "Distancier :" << endl;

		distancier.reecrire_distancier(&fichier, nJob);

		fichier.close();
	}
	else { cerr << "Impossible d'ouvrir le fichier !" << endl; }
}
