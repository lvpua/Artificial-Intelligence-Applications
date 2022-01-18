#pragma once

#include <vector>
#include "7Vector.h"
#include "4X4Matrix.h"

//FULLY STATIC CLASS
class CAlgos  
{
public:
    static C7Vector alignAndCenterGeometryAndGetTransformation(float* vert,int vertLength,int* ind,int indLength,float* norm,int normLength,bool alignWithGeomMainAxis);
    static C4X4Matrix getMainAxis(const std::vector<float>* vertices,const std::vector<int>* triangles,const std::vector<int>* trianglesIndices,bool useAllVerticesForce,bool veryPreciseWithTriangles);
    static C4X4Matrix getMainAxis(const float* vertices,int verticesLength,const int* indices,int indicesLength,const int* triIndices,int triIndicesLength,bool useAllVerticesForce,bool veryPreciseWithTriangles);
    static bool isBoxOutsideVolumeApprox(const C4X4Matrix& tr,const C3Vector& s,const std::vector<float>* planes);
};
