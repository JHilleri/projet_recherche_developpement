#include "Job.h"


Job_old::Job_old()
{
}

Job_old::Job_old(std::istream & input, int mMachine)
{
	string tmp;
	getline(input, tmp);
	getline(input, tmp);
	p = vector<int>(mMachine);

	sum_p = 0;
	for (int i = 0; i < mMachine; i++)
	{
		input >> p[i];
		sum_p += p[i];
	}


	// A decommenter
	//Coordon�es
	input >> x >> y;

	thWIP = vector<int>(mMachine - 1);
	for (int i = 0; i < mMachine - 1; i++)
	{
		input >> thWIP[i];
		sum_p += thWIP[i];
	}
	input >> thFIN;
	//*/

	input >> due_d >> q >> hWIP >> hFIN >> piM >> pi3PL;
}


Job_old::~Job_old()
{
}

void Job_old::reecrire_job(ofstream * fichier)
{
	int mMachine = p.size();
	for (int i = 0; i < mMachine; i++)
	{
		*fichier << p[i] << " ";
	}*fichier << endl;


	// A decommenter
	//Coordon�es
	*fichier << x << " " << y << endl;

	for (int i = 0; i < mMachine - 1; i++)
	{
		*fichier << thWIP[i] << " ";
	}*fichier << thFIN << endl;
	//*/

	*fichier << due_d << " " << q << " " << hWIP << " " << hFIN << " " << piM << " " << pi3PL << endl;
}
