#ifndef NSC_H
#define NSC_H
/*
 *
 *  program NSC
 *  version 2.0  (April 1995)
 *
 *  Author: Frank Eisenhaber
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
 *  1.F.Eisenhaber, P.Lijnzaad, P.Argos, C.Sander, and M.Scharf
 *    "The Double Cubic Lattice Method: Efficient Approaches to
 *    Numerical Integration of Surface Area and Volume and to Dot
 *    Surface Contouring of Molecular Assemblies"
 *    Journal of Computational Chemistry (1995) v.16, N3, pp.273-284
 *  2.F.Eisenhaber and P.Argos
 *    "Improved Strategy in Analytic Surface Calculation for Molecular
 *    Systems: Handling of Singularities and Computational Efficiency"
 *    Journal of Computational Chemistry (1993) v.14, N11, pp.1272-1280
 *
 */

#define FLAG_DOTS       01
#define FLAG_VOLUME     02
#define FLAG_ATOM_AREA  04

#define NSC nsc_dclm

extern int NSC(
    double * ,	/* atom coordinates xyz0, xyz1, ... */
    double * ,	/* atom radii r0, r1, r2, ... */
    int ,         /* number of atoms */
    int ,		/* number of dots per fully accessible sphere */
    int ,		/* flag : dots, volume and/or area per atom */
    double * ,	/* 1 output: overall area */
    double ** ,   /* 2 output: pointer to list of areas per atom */
    double * ,	/* 3 output: overall volume */
    double ** ,	/* 4 output: pointer to list of surface dots x0, y0, z0, ... */
    int *		/* 5 output: number of surface dots */
);

/*
    User notes :
The input requirements :
  The arrays with atom coordinates and radii are thought to start
  with index 0, i.e., places 0, 1, and 2 are the x-, y-, and z-
  coordinates of the zero-th atom and place 0 in the other array
  is its radius.

  PLEASE TAKE INTO ACCOUNT THAT THE RADII GIVEN HERE ARE DIRECTLY
  USED FOR SURFACE CALCULATION. NSC does not increment with a probe
  radius.

  The user can define any number of dots. The program selects a
  dot density that is the lowest possible with at least the required
  number of dots. The points are distributed in accordance with the
  icosahedron-based or the dodecahedron-based method as described in
  ref. 1.

The output requirements are :
  1 and 3 :  pointer to an existing double
  2 and 4 :  pointer to an existing pointer to double
             NSC allocates memory for an array
  5       :  pointer to an existing integer

The subroutine NSC makes use of variant 2 described in reference 1.
By selecting the necessary output via flags, the requirements for
cpu-time and computer memory can be adapted to the actual needs.

Example : flag = FLAG_VOLUME | FLAG_ATOM_AREA | FLAG_DOTS
          The routine calculates the area, volume and the dot surface. The
          program allocates arrays for the atomwise areas and for the surface
          dots. The addresses are returned in the pointers to pointers to
          double.
          This variant is not recommended because normally the dot surface
          is needed for low point density (e.g.42) at which area and volume
          are inaccurate. The sign "|" is used as binary AND !

          flag = FLAG_VOLUME | FLAG_ATOM_AREA
          In this case the large arrays for storing the surface dots
          are not allocated. A large point number of the fully accessible
          sphere can be selected. Good accuracy is already achieved with
          600-700 points per sphere (accuracy of about 1.5 square Angstrem
          per atomic sphere).
          Output pointers 4 and 5 may be NULL.

          flag = FLAG_DOTS
          Only the dot surface is produced.
          Output pointers 2 and 3 may be NULL.

The output pointer 1 cannot be set to NULL in any circumstances. The
overall area value is returned in every mode.

All files calling NSC should include nsc.h !!


Example for calling NSC (contents of user file):

  ...
#include "nsc.h"

int routine_calling_NSC(int n_atom, double *coordinates, double *radii) {
  double area, volume, *atomwise_area, *surface_dots;
  int    i, density = 300, n_dots;

  ...

  for (i=0; i<n_atom; i++) {
   radii[i]  += 1.4      /# add the probe radius if necessary #/

  if (NSC(coordinates, radii, n_atom, density,
          FLAG_AREA | FLAG_VOLUME | FLAG_DOTS,
          &area, &atomwise_area, &volume, &surface_dots, &n_dots))
    printf("error occured\n");
    return 1;
    }

  ...

/# do something with areas, volume and surface dots #/

  ...

  return 0;
  }

*/

int
nsc_dclm_weighted (double *co, double *radius, int nat, int densit,
                   unsigned int* at_res_p, double* res_p,
                   unsigned int* at_res_index, unsigned int half_window,
                   double *atom_area);

typedef double *point_double;
typedef int *point_int;
extern point_double xpunsp;
extern double del_cube;
extern point_int ico_wk;
extern point_int ico_pt;
extern int n_dot;
extern int ico_cube;
extern int last_n_dot;
extern int last_densit;
extern int last_unsp;
extern int last_cubus;

#define FOURPI (4.*M_PI)
#define TORAD(A)     ((A)*0.017453293)
#define DP_TOL     0.001
#define MAXIMUM(A, B)  (((A) > (B) ? (A) : (B)))
#define MINIMUM(A, B)  (((A) < (B) ? (A) : (B)))

#include <stdarg.h>
#include <stdio.h>

#define UPDATE_FL  __file__=__FILE__,__line__=__LINE__
char * __file__;         /* declared versions of macros */
int  __line__;           /* __FILE__  and __LINE__ */

#define ERROR UPDATE_FL,error
void error(const char *fmt, ...);
#define WARNING UPDATE_FL,warning
void warning (const char *fmt, ...);

#define CALLOC(n, size) mycalloc(__FILE__,__LINE__, n, size)
void* mycalloc (const char *filename, const int linenr, size_t nelem, size_t elsize);

int unsp_type (int densit);
int make_unsp (int densit, int mode, int *num_dot, int cubus);

#endif //NSC_H

