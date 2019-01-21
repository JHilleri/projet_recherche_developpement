#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>

#include "Instance.h"

using namespace std;

Instance::Instance(std::istream & input, bool mise_en_batch)
{
	input >> nJob >> mMachine >> cV;
	if (mise_en_batch)
	{
		unsigned int job_number_per_batch;
		input >> job_number_per_batch;
		const unsigned int batch_number{ static_cast<unsigned int>(nJob) / job_number_per_batch + (static_cast<unsigned int>(nJob) % job_number_per_batch) ? 1u : 0u };
		tab_batch = std::vector<std::vector<int>>{ batch_number };
		int last_job_index{ 0 };
		std::generate(tab_batch.begin(), tab_batch.end(), [&last_job_index, job_number_per_batch]() {
			std::vector<int> new_batch{ static_cast<int>(job_number_per_batch) };
			std::generate(new_batch.begin(), new_batch.end(), [&last_job_index]() {return last_job_index++; });
			return new_batch;
		});
	}
	list_Job = std::vector<Job_old>{ static_cast<unsigned int>(nJob) };
	int i{ 0 };
	std::generate(list_Job.begin(), list_Job.end(), [&]() {
		Job_old new_job{ input, mMachine };
		new_job.index_lieu = i++;
		return new_job;
	});

	distancier = Distancier(input, nJob);
}


void Instance::reecrire_Instance(bool mise_en_batch)
{

	ofstream fichier = ofstream(nom_fichier, ios::trunc);

	if (fichier)
	{


		fichier << nJob << " " << mMachine << endl << cV << endl;

		if (mise_en_batch) {
			for  (auto& var : tab_batch)
			{
				fichier << var.size() << " ";
			}fichier << endl;
		}

		for (int i = 0; i < nJob; i++)
		{
			fichier << "Jobs " << i << " :" << endl;
			list_Job[i].reecrire_job(&fichier);

		}

		fichier << "Distancier :" << endl;

		distancier.reecrire_distancier(&fichier, nJob);

		fichier.close();
	}
	else { cerr << "Impossible d'ouvrir le fichier !" << endl; }
}
