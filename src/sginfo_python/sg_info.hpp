#include <string>
#include <vector>

// Shamelessly ripped off from sginfo source!!!
extern "C" {
#include <sginfo_1_01/sginfo.h>
}
int sgToTransforms(std::string spaceGroup, 
                   std::vector<double>& matArrayPtr, 
                   std::vector<double>& vecArrayPtr);
int getSpaceGroupTransforms(const T_SgInfo *SgInfo, 
                            std::vector<double>& matArrayPtr,
                            std::vector<double>& vecArrayPtr);
int BuildSgInfo(T_SgInfo *SgInfo, const char *SgName);
//End shameless rip off
void DestroySgInfo(T_SgInfo *SgInfo);

