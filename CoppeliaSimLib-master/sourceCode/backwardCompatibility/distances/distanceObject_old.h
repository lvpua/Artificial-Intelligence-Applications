#pragma once

#include "colorObject.h"
#include "ser.h"

class CDistanceObject_old
{
public:
    CDistanceObject_old();
    CDistanceObject_old(int entity1Handle,int entity2Handle);
    virtual ~CDistanceObject_old();

    void displayDistanceSegment();
    void initializeInitialValues(bool simulationAlreadyRunning);
    void simulationAboutToStart();
    void simulationEnded();

    bool isSame(int entity1Handle,int entity2Handle) const;
    bool getDistanceResult(float dist[7]) const;
    float getCalculationTime() const;
    float readDistance() const;
    std::string getUniquePersistentIdString() const;
    std::string getObjectDescriptiveName() const;
    void clearDistanceResult();
    float handleDistance();

    void serialize(CSer& ar);
    void performObjectLoadingMapping(const std::vector<int>* map);
    void performCollectionLoadingMapping(const std::vector<int>* map);
    bool announceObjectWillBeErased(int objectHandle,bool copyBuffer);
    bool announceCollectionWillBeErased(int collectionHandle,bool copyBuffer);
    CDistanceObject_old* copyYourself();

    int getObjectHandle() const;
    int getEntity1Handle() const;
    int getEntity2Handle() const;
    std::string getObjectName() const;
    float getTreshhold() const;
    bool getTreshholdEnabled() const;
    bool getDisplaySegment() const;
    bool getExplicitHandling() const;
    int getSegmentWidth() const;

    CColorObject* getSegmentColor();

    bool setObjectHandle(int newHandle);
    bool setThresholdEnabled(bool enabled);
    bool setDisplaySegment(bool display);
    bool setExplicitHandling(bool explicitHandl);
    bool setObjectName(const char* newName,bool check);
    bool setThreshold(float tr);
    bool setSegmentWidth(int w);

private:
    void _commonInit();

    int _entity1Handle;
    int _entity2Handle;
    std::string _objectName;
    int _objectHandle;
    int _segmentWidth;
    float _threshold;
    bool _thresholdEnabled;
    bool _displaySegment;
    bool _explicitHandling;

    CColorObject _segmentColor;
    std::string _uniquePersistentIdString;
    float _distanceResult[7];
    float _distance;
    int _distanceBuffer[4];
    bool _distanceIsValid;
    int _calcTimeInMs;
    bool _initialValuesInitialized;
    bool _initialExplicitHandling;
};
