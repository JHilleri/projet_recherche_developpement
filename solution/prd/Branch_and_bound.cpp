#include "Job.h"


Job::Job()
{
}

Job::Job(ifstream* fichier, int mMachine)
{
	string tmp;
	getline(*fichier, tmp);
	getline(*fichier, tmp);
	p = vector<int>(mMachine);

	sum_p = 0;
	for (int i = 0; i < mMachine; i++)
	{
		*fichier >> p[i];
		sum_p += p[i];
	}


	// A decommenter
	//Coordonées
	*fichier >> x >> y;

	thWIP = vector<int>(mMachine - 1);
	for (int i = 0; i < mMachine - 1; i++)
	{
		*fichier >> thWIP[i];
		sum_p += thWIP[i];
	}
	*fichier >> thFIN;
	//*/

	*fichier >> due_d >> q >> hWIP >> hFIN >> piM >> pi3PL;
}


Job::~Job()
{
}

void Job::reecrire_job(ofstream * fichier)
{
	int mMachine = p.size();
	for (int i = 0; i < mMachine; i++)
	{
		*fichier << p[i] << " ";
	}*fichier << endl;


	// A decommenter
	//Coordonées
	*fichier << x << " " << y << endl;

	for (int i = 0; i < mMachine - 1; i++)
	{
		*fichier << thWIP[i] << " ";
	}*fichier << thFIN << endl;
	//*/

	*fichier << due_d << " " << q << " " << hWIP << " " << hFIN << " " << piM << " " << pi3PL << endl;
}
