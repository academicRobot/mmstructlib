#include "sg_info.hpp"
#include <stdexcept>
#include <cstdlib>
#include <cstdio>

int sgToTransforms(std::string spaceGroup, 
                   std::vector<double>& matArrayPtr, 
                   std::vector<double>& vecArrayPtr){
   T_SgInfo SgInfo;

   if(BuildSgInfo(&SgInfo, spaceGroup.c_str()) != 0){
      char buff[2000];
      std::snprintf(buff, sizeof(buff), "problem with space group, dying now '%s'\n", SgError);
      throw std::runtime_error(buff);
   }

   int ret = getSpaceGroupTransforms(&SgInfo, matArrayPtr, vecArrayPtr);
   DestroySgInfo(&SgInfo);
   return ret;
}

int getSpaceGroupTransforms(const T_SgInfo *SgInfo, //returns number of transforms
                            std::vector<double>& matArrayPtr, 
                            std::vector<double>& vecArrayPtr){
  int           iList, f, i, numTransforms, currentTransform;
  int           currT, currI, nTrV, iTrV, nLoopInv, iLoopInv;
  const int     *TrV;
  T_RTMx        SMx;
  const T_RTMx  *lsmx;

  nLoopInv = Sg_nLoopInv(SgInfo);

  nTrV = SgInfo->LatticeInfo->nTrVector;
  TrV = SgInfo->LatticeInfo->TrVector;

  numTransforms = SgInfo->nList*nTrV*nLoopInv;
  matArrayPtr.resize(numTransforms*9);
  vecArrayPtr.resize(numTransforms*3);

  currT=0;
  for (iTrV = 0; iTrV < nTrV; iTrV++, TrV += 3)
  {
    for (iLoopInv = 0; iLoopInv < nLoopInv; iLoopInv++)
    {
      if (iLoopInv == 0) f =  1;
      else               f = -1;

      lsmx = SgInfo->ListSeitzMx;

      for (iList = 0; iList < SgInfo->nList; iList++, lsmx++)
      {
	  	currI = currT*9;
        for(int i=0; i<9; ++i){
          matArrayPtr[currI+i] = (double) f * lsmx->s.R[i];
        }
	  	currI = currT*3;
        for(int i=0; i<3; ++i){
          vecArrayPtr[currI+i] = 
            (double) iModPositive(f * lsmx->s.T[i] + TrV[i], STBF) / STBF;
        }
        currT++;
      }
    }
  }
  if(currT != numTransforms){
     return 0;
  }
  return numTransforms;
}

static int str_ibegin(const char *s1, const char *s2) /* string ignore-case */
{                                                     /* begin              */
  char     u1, u2;

  while (*s1 && *s2)
  {
    u1 = toupper(*s1++);
    u2 = toupper(*s2++);
    if      (u1 < u2) return -1;
    else if (u1 > u2) return  1;
  }
  if (*s2) return -1;
  return 0;
}

int BuildSgInfo(T_SgInfo *SgInfo, const char *SgName)
{
  int                VolLetter;
  const T_TabSgName  *tsgn;


  /* look for "VolA", "VolI", or "Hall"
   */

  while (*SgName && isspace(*SgName)) SgName++;

  VolLetter = -1;

  if      (isdigit(*SgName))
    VolLetter = 'A';
  else if (str_ibegin(SgName, "VolA") == 0)
  {
    VolLetter = 'A';
    SgName += 4;
  }
  else if (   str_ibegin(SgName, "VolI") == 0
           || str_ibegin(SgName, "Vol1") == 0)
  {
    VolLetter = 'I';
    SgName += 4;
  }
  else if (str_ibegin(SgName, "Hall") == 0)
  {
    VolLetter = 0;
    SgName += 4;
  }

  while (*SgName && isspace(*SgName)) SgName++;

  /* default is "VolA"
   */

  if (VolLetter == -1)
    VolLetter = 'A';

  /* if we do not have a Hall symbol do a table look-up
   */

  tsgn = NULL;

  if (VolLetter)
  {
    tsgn = FindTabSgNameEntry(SgName, VolLetter);
    if (tsgn == NULL) return -1; /* no matching table entry */
    SgName = tsgn->HallSymbol;
  }

  /* Allocate memory for the list of Seitz matrices and
     a supporting list which holds the characteristics of
     the rotation parts of the Seitz matrices
   */

  SgInfo->MaxList = 192; /* absolute maximum number of symops */

  SgInfo->ListSeitzMx
    = (T_RTMx*)malloc(SgInfo->MaxList * sizeof (*SgInfo->ListSeitzMx));

  if (SgInfo->ListSeitzMx == NULL) {
    SetSgError("Not enough core");
    return -1;
  }

  SgInfo->ListRotMxInfo
    = (T_RotMxInfo*)malloc(SgInfo->MaxList * sizeof (*SgInfo->ListRotMxInfo));

  if (SgInfo->ListRotMxInfo == NULL) {
    SetSgError("Not enough core");
    return -1;
  }

  /* Initialize the SgInfo structure
   */

  InitSgInfo(SgInfo);
  SgInfo->TabSgName = tsgn; /* in case we know the table entry */

  /* Translate the Hall symbol and generate the whole group
   */

  ParseHallSymbol(SgName, SgInfo);
  if (SgError != NULL) return -1;

  /* Do some book-keeping and derive crystal system, point group,
     and - if not already set - find the entry in the internal
     table of space group symbols
   */

  return CompleteSgInfo(SgInfo);
}

void DestroySgInfo(T_SgInfo *SgInfo){
  free(SgInfo->ListSeitzMx);
  free(SgInfo->ListRotMxInfo);
}

