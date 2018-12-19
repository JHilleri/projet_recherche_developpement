#include "edd_near_x4.h"

namespace cost_per_departure_date
{
	Fct_lin edd_near_x4::operator()(Instance & inst, const batch & batch, int min_a, int max_b)
	{
		Tournee t_EDD = Tournee::create_tournee_EDD(inst, batch);
		Fct_lin f_EDD = Fct_lin::generate_fct_with_four_extrema(inst, t_EDD, min_a, max_b);

		Tournee t_near = Tournee::create_tournee_nearest_insertion(inst, batch);
		Fct_lin f_near = Fct_lin::generate_fct_with_four_extrema(inst, t_near, min_a, max_b);

		return Fct_lin::minimum_fct(f_EDD, f_near);
	}
}
