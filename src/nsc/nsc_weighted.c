//Modified nsc_dclm to calculate residue weighted areas
//Chris Oldfield (1/26/16)
//Original nsc.c header follows:
/*
 *
 *  program NSC
 *  version 2.0  (April 1995)
 *
 *  Author: Frank Eisenhaber
 *
 *  For user notes see file nsc.h and readme.nsc !!
 *
 *  Copyright Notice:
 *  All rights reserved, whether the whole or part of the program is
 *  concerned. The software may not be used without specific, prior
 *  written permission of the author.
 *
 *  An academic licence agreement for the package ASC/GM or its parts
 *  is granted if you make the following commitments:
 *  1) In using this software, the user will respect the interests of
 *     the author.
 *  2) The use of the software in commercial activities is not allowed
 *     without a prior written commercial licence agreement.
 *  3) Other interested research groups will be redirected
 *     to the author. The user will not redistribute the code outside
 *     his immediate research group.
 *  4) The copyright messages will not be modified or suppressed.
 *  5) The reference given below will be cited in any publication
 *     of scientific results based in part or completely on use of the
 *     program.
 *  6) Bugs will be reported to the author.
 *
 *  Permission to use, copy, and modify this software and
 *  its documentation is hereby granted without fee for
 *  academic use, provided
 *  that the above copyright notices and this permission notice appear in
 *  all copies of the software and related documentation.
 *
 *  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
 *  ANY KIND,
 *  EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 *  WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 *  LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 *  OF THIS SOFTWARE.
 *
 *
 *  contact address :    European Molecular Biology Laboratory
 *                       Biocomputing Unit
 *                       Meyerhofstr. 1
 *                       Postfach 10.2209
 *                       D-69012 Heidelberg
 *                       Federal Republic of Germany
 *
 *
 *
 *  E-mail : IN%"EISENHABER@EMBL-Heidelberg.DE"
 *  Please send your contact address to get information on updates and
 *  new features. Questions will be answered as soon as possible.
 *
 *
 *  references :
 *  1.F.Eisenhaber, P.Lijnzaad, P.Argos, M.Scharf
 *    "The Double Cubic Lattice Method: Efficient Approaches to
 *    Numerical Integration of Surface Area and Volume and to Dot
 *    Surface Contouring of Molecular Assemblies"
 *    Journal of Computational Chemistry (1995) v.16, N3, pp.273-284
 *  2.F.Eisenhaber, P.Argos
 *    "Improved Strategy in Analytic Surface Calculation for Molecular
 *    Systems: Handling of Singularities and Computational Efficiency"
 *    Journal of Computational Chemistry (1993) v.14, N11, pp.1272-1280
 *
 */

//#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "nsc.h"

typedef struct _stwknb
{
	double x;
	double y;
	double z;
	double dot;
	int index;
} Neighb;

inline
int
abs_unint_diff(unsigned int i, unsigned int j)
{
	return i > j ? i - j : j - i;
}

int 
add_wknb_res_index(int index, int* wknb_res_index, int length)
{
	int i;
	for(i=0; i<=length; ++i)
	{
		if(wknb_res_index[i] == index)
		{
			return length;
		}
	}
	wknb_res_index[length] = index;
	return length+1;
}

int
nsc_dclm_weighted (double *co, double *radius, int nat, int densit,
                   unsigned int* at_res_p, double* res_p,
                   unsigned int* at_res_index, unsigned int half_window,
                   double *atom_area)
{

	int iat, i, ii, iii, ix, iy, iz, ixe, ixs, iye, iys, ize, izs; 
	double i_ac, temp_i_ac;
	int jat, j, jj, jjj, jx, jy, jz;
	int distribution;
	int l;
	int maxnei, nnei, last, maxdots;
	point_int wkbox = NULL, wkat1 = NULL, wkatm = NULL;
	Neighb *wknb, *ctnb;
	int* wknb_res_index, ri;
	int local_window_flag;
	int iii1, iii2, iiat, lfnr, i_at, j_at;
	double dx, dy, dz, dd, ai, aisq, ajsq, aj, as;
	double xi, yi, zi, xs = 0., ys = 0., zs = 0.;
	point_double xus, dots = NULL;

	int nxbox, nybox, nzbox, nxy, nxyz;
	double xmin, ymin, zmin, xmax, ymax, zmax, ra2max, d, *pco;

	distribution = unsp_type (densit);
	if (distribution != -last_unsp || last_cubus != 4 ||
	        (densit != last_densit && densit != last_n_dot))
	{
		if (make_unsp (densit, (-distribution), &n_dot, 4))
			return 1;
	}
	xus = xpunsp;

	const double dotarea = FOURPI / (double) n_dot;

	/* start with neighbour list */
	/* calculate neighbour list with the box algorithm */
	if (nat == 0)
	{
        WARNING ("nsc_dclm: no surface atoms selected");
		return 1;
	}

	/* dimensions of atomic set, cell edge is 2*ra_max */
	xmin = co[0];
	xmax = xmin;
	xs = xmin;
	ymin = co[1];
	ymax = ymin;
	ys = ymin;
	zmin = co[2];
	zmax = zmin;
	zs = zmin;
	ra2max = radius[0];

	for (iat = 1; iat < nat; iat++)
	{
		pco = co + 3 * iat;
		xmin = MINIMUM (xmin, *pco);
		xmax = MAXIMUM (xmax, *pco);
		ymin = MINIMUM (ymin, *(pco + 1));
		ymax = MAXIMUM (ymax, *(pco + 1));
		zmin = MINIMUM (zmin, *(pco + 2));
		zmax = MAXIMUM (zmax, *(pco + 2));
		xs = xs + *pco;
		ys = ys + *(pco + 1);
		zs = zs + *(pco + 2);
		ra2max = MAXIMUM (ra2max, radius[iat]);
	}
	xs = xs / (double) nat;
	ys = ys / (double) nat;
	zs = zs / (double) nat;
	ra2max = 2. * ra2max;

	d = xmax - xmin;
	nxbox = (int) MAXIMUM (ceil (d / ra2max), 1.);
	d = (((double) nxbox) * ra2max - d) / 2.;
	xmin = xmin - d;
	xmax = xmax + d;
	d = ymax - ymin;
	nybox = (int) MAXIMUM (ceil (d / ra2max), 1.);
	d = (((double) nybox) * ra2max - d) / 2.;
	ymin = ymin - d;
	ymax = ymax + d;
	d = zmax - zmin;
	nzbox = (int) MAXIMUM (ceil (d / ra2max), 1.);
	d = (((double) nzbox) * ra2max - d) / 2.;
	zmin = zmin - d;
	zmax = zmax + d;
	nxy = nxbox * nybox;
	nxyz = nxy * nzbox;

	/* box number of atoms */
	wkatm = (int *) CALLOC (3 * nat, sizeof (int));
	wkat1 = wkatm + nat;
	wkbox = (int *) CALLOC (nxyz + 1, sizeof (int));

	for (iat = 0; iat < nat; iat++)
	{
		pco = co + 3 * iat;
		i = (int) MAXIMUM (floor ((*pco - xmin) / ra2max), 0);
		i = MINIMUM (i, nxbox - 1);
		j = (int) MAXIMUM (floor ((*(pco + 1) - ymin) / ra2max), 0);
		j = MINIMUM (j, nybox - 1);
		l = (int) MAXIMUM (floor ((*(pco + 2) - zmin) / ra2max), 0);
		l = MINIMUM (l, nzbox - 1);
		i = i + j * nxbox + l * nxy;
		wkat1[iat] = i;
		wkbox[i]++;
	}

	/* sorting of atoms in accordance with box numbers */
	j = wkbox[0];
	for (i = 1; i < nxyz; i++)
	{
		j = MAXIMUM (wkbox[i], j);
	}
	for (i = 1; i <= nxyz; i++)
	{
		wkbox[i] += wkbox[i - 1];
	}

//	maxnei = (int) floor(ra2max*ra2max*ra2max*0.5);
	maxnei = MINIMUM (nat, 27 * j);
	wknb = (Neighb *) CALLOC (maxnei, sizeof (Neighb));
	for (iat = 0; iat < nat; iat++)
	{
		wkatm[--wkbox[wkat1[iat]]] = iat;
	}
	wknb_res_index = (int *) CALLOC(maxnei, sizeof(int));

	/* calculate surface for all atoms, step cube-wise */
	for (iz = 0; iz < nzbox; iz++)
	{
		iii = iz * nxy;
		izs = MAXIMUM (iz - 1, 0);
		ize = MINIMUM (iz + 2, nzbox);
		for (iy = 0; iy < nybox; iy++)
		{
			ii = iy * nxbox + iii;
			iys = MAXIMUM (iy - 1, 0);
			iye = MINIMUM (iy + 2, nybox);
			for (ix = 0; ix < nxbox; ix++)
			{
				i = ii + ix;
				iii1 = wkbox[i];
				iii2 = wkbox[i + 1];
				if (iii1 >= iii2)
					continue;
				ixs = MAXIMUM (ix - 1, 0);
				ixe = MINIMUM (ix + 2, nxbox);

				iiat = 0;
				/* make intermediate atom list */
				for (jz = izs; jz < ize; jz++)
				{
					jjj = jz * nxy;
					for (jy = iys; jy < iye; jy++)
					{
						jj = jy * nxbox + jjj;
						for (jx = ixs; jx < ixe; jx++)
						{
							j = jj + jx;
							for (jat = wkbox[j]; jat < wkbox[j + 1]; jat++)
							{
								wkat1[iiat] = wkatm[jat];
								iiat++;
							}	/* end of cycle "jat" */
						}	/* end of cycle "jx" */
					}		/* end of cycle "jy" */
				}		/* end of cycle "jz" */
				for (iat = iii1; iat < iii2; iat++)
				{
					i_at = wkatm[iat];
					ai = radius[i_at];
					aisq = ai * ai;
					pco = co + 3 * i_at;
					xi = *pco;
					yi = *(pco + 1);
					zi = *(pco + 2);

					ctnb = wknb;
					nnei = 0;
					for (j = 0; j < iiat; j++)
					{
						j_at = *(wkat1 + j);
						if (j_at == i_at)
						{
							continue;
						}

						aj = radius[j_at];
						ajsq = aj * aj;
						pco = co + 3 * j_at;
						dx = *pco - xi;
						dy = *(pco + 1) - yi;
						dz = *(pco + 2) - zi;
						dd = dx * dx + dy * dy + dz * dz;

						as = ai + aj;
						if (dd > as * as)
						{
							continue;
						}
						nnei++;
						ctnb->x = dx;
						ctnb->y = dy;
						ctnb->z = dz;
						ctnb->dot = (dd + aisq - ajsq) / (2. * ai);	/* reference dot product */
						ctnb->index = j_at;
						ctnb++;
					}

					/* check points on accessibility */
					if (nnei)
					{
						i_ac = 0.0;
						for (l = 0; l < n_dot; l++)
						{
							ri = 0;
							local_window_flag = 0;
							for (j = 0, ctnb = wknb; j < nnei; j++, ctnb++)
							{
								if (xus[3 * l] * ctnb->x +
										xus[1 + 3 * l] * ctnb->y + 
										xus[2 + 3 * l] * ctnb->z > 
										ctnb->dot)
								{
//printf("local check %i %i %i %i\n", at_res_index[i_at], at_res_index[ctnb->index], abs_unint_diff(at_res_index[i_at], at_res_index[ctnb->index]), half_window);
									if(abs_unint_diff(at_res_index[i_at], 
													  at_res_index[ctnb->index]) <=
									   half_window)
									{
										local_window_flag = 1;
										break;
									}
									ri = add_wknb_res_index(ctnb->index,wknb_res_index,ri);
								}
							}
							if (!local_window_flag){ //local atoms alway get 100% weight
								temp_i_ac = 1.0;
								for (j = 0; j < ri; ++j)
								{
                                    //res_p is probability residue is present, temp_i_ac is probability point is exposed
									temp_i_ac *= (1.0 - res_p[at_res_p[wknb_res_index[j]]]);  
								}
								i_ac += temp_i_ac;
//printf("nonlocal dot burial %f\n", temp_i_ac);
							}
						}
					}
					else
					{
						i_ac = (double)n_dot;
					}

					atom_area[i_at] = aisq * dotarea * i_ac;
				}		/* end of cycle "iat" */
			}			/* end of cycle "ix" */
		}			/* end of cycle "iy" */
	}				/* end of cycle "iz" */

	free (wkbox);
	free (wkatm);
	free (wknb);
	free (wknb_res_index);

	return 0;
}

