#pragma once

#include "ser.h"
#include "sceneObject.h"
#include "3Vector.h"
#include "7Vector.h"

class CDummy;
class COctree;

class CPointCloud : public CSceneObject
{
public:

    CPointCloud();
    virtual ~CPointCloud();

    // Following functions are inherited from CSceneObject
    void display(CViewableBase* renderingObject,int displayAttrib);
    void addSpecializedObjectEventData(CInterfaceStackTable* data) const;
    CSceneObject* copyYourself();
    void removeSceneDependencies();
    void scaleObject(float scalingFactor);
    void scaleObjectNonIsometrically(float x,float y,float z);
    void serialize(CSer& ar);
    void announceCollectionWillBeErased(int groupID,bool copyBuffer);
    void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
    void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
    void performIkLoadingMapping(const std::vector<int>* map,bool loadingAmodel);
    void performCollectionLoadingMapping(const std::vector<int>* map,bool loadingAmodel);
    void performCollisionLoadingMapping(const std::vector<int>* map,bool loadingAmodel);
    void performDistanceLoadingMapping(const std::vector<int>* map,bool loadingAmodel);
    void performTextureObjectLoadingMapping(const std::vector<int>* map);
    void performDynMaterialObjectLoadingMapping(const std::vector<int>* map);
    void simulationAboutToStart();
    void simulationEnded();
    void initializeInitialValues(bool simulationAlreadyRunning);
    void computeBoundingBox();
    bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices) const;
    std::string getObjectTypeInfo() const;
    std::string getObjectTypeInfoExtended() const;
    bool isPotentiallyCollidable() const;
    bool isPotentiallyMeasurable() const;
    bool isPotentiallyDetectable() const;
    bool isPotentiallyRenderable() const;
    bool announceObjectWillBeErased(int objectHandle,bool copyBuffer);
    void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);

    void performObjectLoadingMapping(const std::vector<int>* map,bool loadingAmodel);

    // Various functions
    void setCellSize(float theNewSize);
    float getCellSize() const;
    void setMaxPointCountPerCell(int cnt);
    int getMaxPointCountPerCell() const;
    void insertPoints(const float* pts,int ptsCnt,bool ptsAreRelativeToPointCloud,const unsigned char* optionalColors3,bool colorsAreIndividual);
    void insertShape(CShape* shape);
    void insertOctree(const COctree* octree);
    void insertDummy(const CDummy* dummy);
    void insertPointCloud(const CPointCloud* pointCloud);
    void insertObjects(const std::vector<int>& sel);
    void insertObject(const CSceneObject* obj);

    int removePoints(const float* pts,int ptsCnt,bool ptsAreRelativeToPointCloud,float distanceTolerance);
    void subtractOctree(const COctree* octree);
    void subtractDummy(const CDummy* dummy,float distanceTolerance);
    void subtractPointCloud(const CPointCloud* pointCloud,float distanceTolerance);
    void subtractOctree(const void* octree2Info,const C7Vector& octree2Tr);
    void subtractObjects(const std::vector<int>& sel);
    void subtractObject(const CSceneObject* obj,float distanceTolerance);

    int intersectPoints(const float* pts,int ptsCnt,bool ptsAreRelativeToPointCloud,float distanceTolerance);

    void clear();
    bool getShowOctree() const;
    void setShowOctree(bool show);
    float getAveragePointCountInCell();
    int getPointSize() const;
    void setPointSize(int s);
    float getBuildResolution() const;
    void setBuildResolution(float r);
    float getRemovalDistanceTolerance() const;
    void setRemovalDistanceTolerance(float t);
    float getInsertionDistanceTolerance() const;
    void setInsertionDistanceTolerance(float t);
    bool getUseRandomColors() const;
    void setUseRandomColors(bool r);
    bool getColorIsEmissive() const;
    void setColorIsEmissive(bool e);
    bool getSaveCalculationStructure() const;
    void setSaveCalculationStructure(bool s);
    bool getDoNotUseCalculationStructure() const;
    void setDoNotUseCalculationStructure(bool s);
    float getPointDisplayRatio() const;
    void setPointDisplayRatio(float r);
    const std::vector<float>* getPoints() const;
    std::vector<float>* getPoints();
    const void* getPointCloudInfo() const;
    void* getPointCloudInfo();
    void getTransfAndHalfSizeOfBoundingBox(C7Vector& tr,C3Vector& hs) const;

    CColorObject* getColor();
    std::vector<float>* getColors();
    std::vector<float>* getDisplayPoints();
    std::vector<float>* getDisplayColors();

protected:
    void _updatePointCloudEvent() const;
    void _readPositionsAndColorsAndSetDimensions();
    void _getCharRGB3Colors(const std::vector<float>& floatRGBA,std::vector<unsigned char>& charRGB);

    // Variables which need to be serialized & copied
    CColorObject color;
    float _cellSize;
    int _maxPointCountPerCell;
    void* _pointCloudInfo;
    std::vector<float> _points;
    std::vector<float> _colors;
    std::vector<float> _displayPoints;
    std::vector<float> _displayColors;
    std::vector<unsigned char> _displayColorsByte;
    bool _showOctreeStructure;
    bool _useRandomColors;
    bool _saveCalculationStructure;
    int _pointSize;
    int _nonEmptyCells;
    float _buildResolution;
    float _removalDistanceTolerance;
    float _insertionDistanceTolerance;
    float _pointDisplayRatio;
    bool _doNotUseOctreeStructure;
    bool _colorIsEmissive;
};
