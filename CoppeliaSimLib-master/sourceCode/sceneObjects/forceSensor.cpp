#include "forceSensor.h"
#include "simInternal.h"
#include "tt.h"
#include "simStrings.h"
#include <algorithm>
#include "ttUtil.h"
#include "easyLock.h"
#include "app.h"
#include "forceSensorRendering.h"

CForceSensor::CForceSensor()
{
    commonInit();
}

void CForceSensor::commonInit()
{
    _objectType=sim_object_forcesensor_type;
    _forceThreshold=100.0f;
    _torqueThreshold=10.0f;
    _forceThresholdEnabled=false;
    _torqueThresholdEnabled=false;
    _consecutiveThresholdViolationsForBreaking=10;
    _currentThresholdViolationCount=0;
    _stillAutomaticallyBreaking=false;

    // Dynamic values:
    _intrinsicTransformationError.setIdentity();
    _filteredDynamicForces.clear();
    _filteredDynamicTorques.clear();

    _cumulativeForcesTmp.clear();
    _cumulativeTorquesTmp.clear();

    _lastForce_dynStep.clear();
    _lastTorque_dynStep.clear();
    _lastForceAndTorqueValid_dynStep=false;


    _forceSensorSize=0.05f;

    _valueCountForFilter=1;
    _filterType=0; // average
    _filteredValuesAreValid=false;

    _color.setDefaultValues();
    _color.setColor(0.22f,0.9f,0.45f,sim_colorcomponent_ambient_diffuse);
    _color_removeSoon.setDefaultValues();
    _color_removeSoon.setColor(0.22f,0.22f,0.22f,sim_colorcomponent_ambient_diffuse);
    _visibilityLayer=FORCE_SENSOR_LAYER;
    _localObjectSpecialProperty=0;
    _objectAlias=IDSOGL_FORCE_SENSOR;
    _objectName_old=IDSOGL_FORCE_SENSOR;
    _objectAltName_old=tt::getObjectAltNameFromObjectName(_objectName_old.c_str());
    computeBoundingBox();
}

CForceSensor::~CForceSensor()
{

}

CColorObject* CForceSensor::getColor(bool part2)
{
    if (part2)
        return(&_color_removeSoon);
    return(&_color);
}

void CForceSensor::setValueCountForFilter(int c)
{
    _valueCountForFilter=tt::getLimitedInt(1,1000,c);
}

int CForceSensor::getValueCountForFilter() const
{
    return(_valueCountForFilter);
}

void CForceSensor::setFilterType(int t)
{
    _filterType=t;
}

int CForceSensor::getFilterType() const
{
    return(_filterType);
}
bool CForceSensor::getStillAutomaticallyBreaking()
{
    bool retVal=_stillAutomaticallyBreaking;
    _stillAutomaticallyBreaking=false;
    return(retVal);
}
void CForceSensor::setForceThreshold(float t)
{
    _forceThreshold=tt::getLimitedFloat(0.0f,10000000000.0f,t);
}
float CForceSensor::getForceThreshold() const
{
    return(_forceThreshold);
}
void CForceSensor::setTorqueThreshold(float t)
{
    _torqueThreshold=tt::getLimitedFloat(0.0f,10000000000.0f,t);
}
float CForceSensor::getTorqueThreshold() const
{
    return(_torqueThreshold);
}
void CForceSensor::setEnableForceThreshold(bool e)
{
    _forceThresholdEnabled=e;
}
bool CForceSensor::getEnableForceThreshold() const
{
    return(_forceThresholdEnabled);
}
void CForceSensor::setEnableTorqueThreshold(bool e)
{
    _torqueThresholdEnabled=e;
}
bool CForceSensor::getEnableTorqueThreshold() const
{
    return(_torqueThresholdEnabled);
}
void CForceSensor::setConsecutiveThresholdViolationsForBreaking(int count)
{
    _consecutiveThresholdViolationsForBreaking=tt::getLimitedInt(1,10000,count);
}
int CForceSensor::getConsecutiveThresholdViolationsForBreaking() const
{
    return(_consecutiveThresholdViolationsForBreaking);
}

void CForceSensor::addCumulativeForcesAndTorques(const C3Vector& f,const C3Vector& t,int countForAverage)
{ // the countForAverage mechanism is needed because we need to average all values in a simulation time step (but this is called every dynamic simulation time step!!)
    _cumulativeForcesTmp+=f;
    _cumulativeTorquesTmp+=t;
    _lastForce_dynStep=f;
    _lastTorque_dynStep=t;
    _lastForceAndTorqueValid_dynStep=true;
    if (countForAverage>0)
    {
        _cumulatedForces.push_back(_cumulativeForcesTmp/float(countForAverage));
        _cumulatedTorques.push_back(_cumulativeTorquesTmp/float(countForAverage));
        _cumulativeForcesTmp.clear();
        _cumulativeTorquesTmp.clear();
        if (int(_cumulatedForces.size())>_valueCountForFilter)
        { // we have too many values for the filter. Remove the first entry:
            _cumulatedForces.erase(_cumulatedForces.begin(),_cumulatedForces.begin()+1);
            _cumulatedTorques.erase(_cumulatedTorques.begin(),_cumulatedTorques.begin()+1);
        }
        _computeFilteredValues();
        _handleSensorBreaking();
    }
}

void CForceSensor::setForceAndTorqueNotValid()
{
    _filteredValuesAreValid=false;
    _lastForceAndTorqueValid_dynStep=false;
}

void CForceSensor::_computeFilteredValues()
{
    if (int(_cumulatedForces.size())>=_valueCountForFilter)
    {
        _filteredValuesAreValid=true;
        if (_filterType==0)
        { // average filter
            C3Vector fo;
            C3Vector to;
            fo.clear();
            to.clear();
            for (int i=0;i<_valueCountForFilter;i++)
            {
                fo+=_cumulatedForces[i];
                to+=_cumulatedTorques[i];
            }
            _filteredDynamicForces=fo/float(_valueCountForFilter);
            _filteredDynamicTorques=to/float(_valueCountForFilter);
        }
        if (_filterType==1)
        {
            std::vector<float> fx;
            std::vector<float> fy;
            std::vector<float> fz;
            std::vector<float> tx;
            std::vector<float> ty;
            std::vector<float> tz;
            for (int i=0;i<_valueCountForFilter;i++)
            {
                fx.push_back(_cumulatedForces[i](0));
                fy.push_back(_cumulatedForces[i](1));
                fz.push_back(_cumulatedForces[i](2));
                tx.push_back(_cumulatedTorques[i](0));
                ty.push_back(_cumulatedTorques[i](1));
                tz.push_back(_cumulatedTorques[i](2));
            }
            std::sort(fx.begin(),fx.end());
            std::sort(fy.begin(),fy.end());
            std::sort(fz.begin(),fz.end());
            std::sort(tx.begin(),tx.end());
            std::sort(ty.begin(),ty.end());
            std::sort(tz.begin(),tz.end());
            int ind=_valueCountForFilter/2;
            _filteredDynamicForces(0)=fx[ind];
            _filteredDynamicForces(1)=fy[ind];
            _filteredDynamicForces(2)=fz[ind];
            _filteredDynamicTorques(0)=tx[ind];
            _filteredDynamicTorques(1)=ty[ind];
            _filteredDynamicTorques(2)=tz[ind];
        }
    }
    else
        _filteredValuesAreValid=false;
}

bool CForceSensor::getDynamicForces(C3Vector& f,bool dynamicStepValue) const
{
    if (dynamicStepValue)
    {
        if (App::currentWorld->dynamicsContainer->getCurrentlyInDynamicsCalculations())
        {
            if (!_lastForceAndTorqueValid_dynStep)
                return(false);
            f=_lastForce_dynStep;
            return(true);
        }
        return(false);
    }
    else
    {
        if ( (!_filteredValuesAreValid) )//||(!_dynamicSecondPartIsValid) )
            return(false);
        f=_filteredDynamicForces;
        return(true);
    }
}

bool CForceSensor::getDynamicTorques(C3Vector& t,bool dynamicStepValue) const
{
    if (dynamicStepValue)
    {
        if (App::currentWorld->dynamicsContainer->getCurrentlyInDynamicsCalculations())
        {
            if (!_lastForceAndTorqueValid_dynStep)
                return(false);
            t=_lastTorque_dynStep;
            return(true);
        }
        return(false);
    }
    else
    {
        if ( (!_filteredValuesAreValid) )//||(!_dynamicSecondPartIsValid) )
            return(false);
        t=_filteredDynamicTorques;
        return(true);
    }
}

void CForceSensor::_handleSensorBreaking()
{
    if (_filteredValuesAreValid)
    {
        bool trigger=false;
        if (_forceThresholdEnabled&&(_filteredDynamicForces.getLength()>=_forceThreshold))
            trigger=true;
        if (_torqueThresholdEnabled&&(_filteredDynamicTorques.getLength()>=_torqueThreshold))
            trigger=true;
        if (trigger)
            _currentThresholdViolationCount++;
        else
            _currentThresholdViolationCount=0;
        if (_currentThresholdViolationCount>=_consecutiveThresholdViolationsForBreaking)
        { // we need to break something!
            CScriptObject* script=App::currentWorld->embeddedScriptContainer->getScriptFromObjectAttachedTo_child(_objectHandle);
            if ( (script!=nullptr)&&(!script->getContainsTriggerCallbackFunction()) )
                script=nullptr;
            CScriptObject* cScript=App::currentWorld->embeddedScriptContainer->getScriptFromObjectAttachedTo_customization(_objectHandle);
            if ( (cScript!=nullptr)&&(!cScript->getContainsTriggerCallbackFunction()) )
                cScript=nullptr;
            if ( (script!=nullptr)||(cScript!=nullptr) )
            {
                CInterfaceStack* inStack=App::worldContainer->interfaceStackContainer->createStack();
                inStack->pushTableOntoStack();
                inStack->insertKeyInt32IntoStackTable("handle",getObjectHandle());
                inStack->insertKeyFloatArrayIntoStackTable("force",_lastForce_dynStep.data,3);
                inStack->insertKeyFloatArrayIntoStackTable("torque",_lastTorque_dynStep.data,3);
                inStack->insertKeyFloatArrayIntoStackTable("filteredForce",_filteredDynamicForces.data,3);
                inStack->insertKeyFloatArrayIntoStackTable("filteredTorque",_filteredDynamicTorques.data,3);
                // we are in the main simulation thread. Call only scripts that live in the same thread
                if ( (script!=nullptr)&&(!script->getThreadedExecution_oldThreads()) )
                    script->systemCallScript(sim_syscb_trigger,inStack,nullptr);
                if (cScript!=nullptr)
                    cScript->systemCallScript(sim_syscb_trigger,inStack,nullptr);
                App::worldContainer->interfaceStackContainer->destroyStack(inStack);
            }
            _currentThresholdViolationCount=0;
        }
    }
}

C7Vector CForceSensor::getIntrinsicTransformation(bool includeDynErrorComponent) const
{
    C7Vector retVal;
    retVal.setIdentity();
    if (includeDynErrorComponent)
        retVal=_intrinsicTransformationError;
    return(retVal);
}

C7Vector CForceSensor::getFullLocalTransformation() const
{ // Overridden from _CSceneObject_
    return(_localTransformation*getIntrinsicTransformation(true));
}

void CForceSensor::setIntrinsicTransformationError(const C7Vector& tr)
{
    bool diff=(_intrinsicTransformationError!=tr);
    if (diff)
    {
        _intrinsicTransformationError=tr;
        if ( _isInScene&&App::worldContainer->getEventsEnabled() )
        {
            const char* cmd="intrinsicPose";
            auto [event,data]=App::worldContainer->prepareSceneObjectChangedEvent(this,false,cmd,true);
            float p[7]={tr.X(0),tr.X(1),tr.X(2),tr.Q(1),tr.Q(2),tr.Q(3),tr.Q(0)};
            data->appendMapObject_stringFloatArray(cmd,p,7);
            App::worldContainer->pushEvent(event);
        }
    }
}

void CForceSensor::getDynamicErrorsFull(C3Vector& linear,C3Vector& angular) const
{
    linear=_intrinsicTransformationError.X;
    angular=_intrinsicTransformationError.Q.getEulerAngles();
}


float CForceSensor::getDynamicPositionError() const
{
    return(_intrinsicTransformationError.X.getLength());
}

float CForceSensor::getDynamicOrientationError() const
{
    return(_intrinsicTransformationError.Q.getAngleBetweenQuaternions(C4Vector::identityRotation));
}

void CForceSensor::initializeInitialValues(bool simulationAlreadyRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
    CSceneObject::initializeInitialValues(simulationAlreadyRunning);
    _filteredDynamicForces.clear();
    _filteredDynamicTorques.clear();
    _currentThresholdViolationCount=0;
    _filteredValuesAreValid=false;
    _lastForceAndTorqueValid_dynStep=false;
    _cumulatedForces.clear();
    _cumulatedTorques.clear();
    _cumulativeForcesTmp.clear();
    _cumulativeTorquesTmp.clear();
    setIntrinsicTransformationError(C7Vector::identityTransformation);
}

void CForceSensor::simulationAboutToStart()
{
    initializeInitialValues(false);
    CSceneObject::simulationAboutToStart();
}

void CForceSensor::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
    if (_initialValuesInitialized)
    {
        if ( App::currentWorld->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeModelProperty()&sim_modelproperty_not_reset)==0))
        {
        }
    }
    _filteredValuesAreValid=false;
    _lastForceAndTorqueValid_dynStep=false;
    _cumulatedForces.clear();
    _cumulatedTorques.clear();
    _cumulativeForcesTmp.clear();
    _cumulativeTorquesTmp.clear();
    setIntrinsicTransformationError(C7Vector::identityTransformation);
    CSceneObject::simulationEnded();
}

std::string CForceSensor::getObjectTypeInfo() const
{
    return(IDSOGL_FORCE_SENSOR);
}
std::string CForceSensor::getObjectTypeInfoExtended() const
{
    return(IDSOGL_FORCE_SENSOR);
}
bool CForceSensor::isPotentiallyCollidable() const
{
    return(false);
}
bool CForceSensor::isPotentiallyMeasurable() const
{
    return(false);
}
bool CForceSensor::isPotentiallyDetectable() const
{
    return(false);
}
bool CForceSensor::isPotentiallyRenderable() const
{
    return(false);
}

void CForceSensor::computeBoundingBox()
{
    C3Vector maxV(_forceSensorSize/2.0f,_forceSensorSize/2.0f,_forceSensorSize/2.0f);
    _setBoundingBox(maxV*-1.0f,maxV);
}

bool CForceSensor::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices) const
{
    vertices.clear();
    indices.clear();
    return(false); // for now
}

void CForceSensor::setForceSensorSize(float s)
{
    tt::limitValue(0.001f,10.0f,s);
    if (_forceSensorSize!=s)
    {
        _forceSensorSize=s;
        computeBoundingBox();
        if ( _isInScene&&App::worldContainer->getEventsEnabled() )
        {
            const char* cmd="size";
            auto [event,data]=App::worldContainer->prepareSceneObjectChangedEvent(this,false,cmd,true);
            data->appendMapObject_stringFloat(cmd,_forceSensorSize);
            App::worldContainer->pushEvent(event);
        }
    }
}

float CForceSensor::getForceSensorSize() const
{
    return(_forceSensorSize);
}

void CForceSensor::scaleObject(float scalingFactor)
{
    setForceSensorSize(_forceSensorSize*scalingFactor);
    _forceThreshold*=scalingFactor*scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change
    _torqueThreshold*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change

    CSceneObject::scaleObject(scalingFactor);
    _filteredValuesAreValid=false;
    _lastForceAndTorqueValid_dynStep=false;
}

void CForceSensor::scaleObjectNonIsometrically(float x,float y,float z)
{
    scaleObject(cbrt(x*y*z));
}

void CForceSensor::removeSceneDependencies()
{
    CSceneObject::removeSceneDependencies();
}

void CForceSensor::addSpecializedObjectEventData(CInterfaceStackTable* data) const
{
    CInterfaceStackTable* subC=new CInterfaceStackTable();
    data->appendMapObject_stringObject("forceSensor",subC);
    data=subC;

    data->appendMapObject_stringFloat("size",_forceSensorSize);

    CInterfaceStackTable* colors=new CInterfaceStackTable();
    data->appendMapObject_stringObject("colors",colors);
    float c[9];
    _color.getColor(c,sim_colorcomponent_ambient_diffuse);
    _color.getColor(c+3,sim_colorcomponent_specular);
    _color.getColor(c+6,sim_colorcomponent_emission);
    colors->appendArrayObject_floatArray(c,9);
    _color_removeSoon.getColor(c,sim_colorcomponent_ambient_diffuse);
    _color_removeSoon.getColor(c+3,sim_colorcomponent_specular);
    _color_removeSoon.getColor(c+6,sim_colorcomponent_emission);
    colors->appendArrayObject_floatArray(c,9);
    C7Vector tr(getIntrinsicTransformation(true));
    float p[7]={tr.X(0),tr.X(1),tr.X(2),tr.Q(1),tr.Q(2),tr.Q(3),tr.Q(0)};
    data->appendMapObject_stringFloatArray("intrinsicPose",p,7);
    // todo
}

CSceneObject* CForceSensor::copyYourself()
{   
    CForceSensor* newForceSensor=(CForceSensor*)CSceneObject::copyYourself();

    newForceSensor->_forceSensorSize=_forceSensorSize;
    newForceSensor->_forceThreshold=_forceThreshold;
    newForceSensor->_torqueThreshold=_torqueThreshold;
    newForceSensor->_forceThresholdEnabled=_forceThresholdEnabled;
    newForceSensor->_torqueThresholdEnabled=_torqueThresholdEnabled;
    newForceSensor->_consecutiveThresholdViolationsForBreaking=_consecutiveThresholdViolationsForBreaking;
    newForceSensor->_currentThresholdViolationCount=_currentThresholdViolationCount;

    newForceSensor->_valueCountForFilter=_valueCountForFilter;
    newForceSensor->_filterType=_filterType;

    _color.copyYourselfInto(&newForceSensor->_color);

    return(newForceSensor);
}

bool CForceSensor::announceObjectWillBeErased(int objectHandle, bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    // This routine can be called for objCont-objects, but also for objects
    // in the copy-buffer!! So never make use of any 
    // 'ct::objCont->getObject(id)'-call or similar
    // Return value true means 'this' has to be erased too!
    bool retVal=CSceneObject::announceObjectWillBeErased(objectHandle,copyBuffer);
    return(retVal);
}

void CForceSensor::announceCollectionWillBeErased(int groupID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceCollectionWillBeErased(groupID,copyBuffer);
}
void CForceSensor::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceCollisionWillBeErased(collisionID,copyBuffer);
}
void CForceSensor::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceDistanceWillBeErased(distanceID,copyBuffer);
}
void CForceSensor::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceIkObjectWillBeErased(ikGroupID,copyBuffer);
}
void CForceSensor::performObjectLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // New_Object_ID=map[Old_Object_ID]
    CSceneObject::performObjectLoadingMapping(map,loadingAmodel);
}
void CForceSensor::performCollectionLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
    CSceneObject::performCollectionLoadingMapping(map,loadingAmodel);
}
void CForceSensor::performCollisionLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
    CSceneObject::performCollisionLoadingMapping(map,loadingAmodel);
}
void CForceSensor::performDistanceLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
    CSceneObject::performDistanceLoadingMapping(map,loadingAmodel);
}
void CForceSensor::performIkLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{
    CSceneObject::performIkLoadingMapping(map,loadingAmodel);
}

void CForceSensor::performTextureObjectLoadingMapping(const std::vector<int>* map)
{
    CSceneObject::performTextureObjectLoadingMapping(map);
}

void CForceSensor::performDynMaterialObjectLoadingMapping(const std::vector<int>* map)
{
    CSceneObject::performDynMaterialObjectLoadingMapping(map);
}

void CForceSensor::serialize(CSer& ar)
{
    CSceneObject::serialize(ar);
    if (ar.isBinary())
    {
        if (ar.isStoring())
        {   // Storing.
            ar.storeDataName("Siz");
            ar << _forceSensorSize;
            ar.flush();

            ar.storeDataName("Vab");
            unsigned char dummy=0;
            SIM_SET_CLEAR_BIT(dummy,0,_forceThresholdEnabled);
            SIM_SET_CLEAR_BIT(dummy,1,_torqueThresholdEnabled);
    //      SIM_SET_CLEAR_BIT(dummy,2,_breaksPositionConstraints); // removed on 2010/02/06
    //      SIM_SET_CLEAR_BIT(dummy,3,_breaksOrientationConstraints); // removed on 2010/02/06
            ar << dummy;
            ar.flush();

            ar.storeDataName("Tri");
            ar << _forceThreshold << _torqueThreshold << _consecutiveThresholdViolationsForBreaking;
            ar.flush();

            ar.storeDataName("Fil");
            ar << _valueCountForFilter << _filterType;
            ar.flush();

    // "Bus" is reserved keyword since 2010/10/09       ar.storeDataName("Bus");

            ar.storeDataName("Cl1");
            ar.setCountingMode();
            _color.serialize(ar,0);
            if (ar.setWritingMode())
                _color.serialize(ar,0);

            ar.storeDataName(SER_END_OF_OBJECT);
        }
        else
        {       // Loading
            int byteQuantity;
            std::string theName="";
            while (theName.compare(SER_END_OF_OBJECT)!=0)
            {
                theName=ar.readDataName();
                if (theName.compare(SER_END_OF_OBJECT)!=0)
                {
                    bool noHit=true;
                    if (theName.compare("Siz")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _forceSensorSize;
                    }
                    if (theName.compare("Cl1")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity; 
                        _color.serialize(ar,0);
                    }
                    if (theName.compare("Vab")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        unsigned char dummy;
                        ar >> dummy;
                        _forceThresholdEnabled=SIM_IS_BIT_SET(dummy,0);
                        _torqueThresholdEnabled=SIM_IS_BIT_SET(dummy,1);
                    }
                    if (theName.compare("Tri")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _forceThreshold >> _torqueThreshold >> _consecutiveThresholdViolationsForBreaking;
                    }
                    if (theName.compare("Tre")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _forceThreshold >> _torqueThreshold >> _consecutiveThresholdViolationsForBreaking;
                        _stillAutomaticallyBreaking=true;
                    }
                    if (theName.compare("Fil")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _valueCountForFilter >> _filterType;
                    }
                    if (noHit)
                        ar.loadUnknownData();
                }
            }
            if (ar.getSerializationVersionThatWroteThisFile()<17)
                CTTUtil::scaleColorUp_(_color.getColorsPtr());
            computeBoundingBox();
        }
    }
    else
    {
        bool exhaustiveXml=( (ar.getFileType()!=CSer::filetype_csim_xml_simplescene_file)&&(ar.getFileType()!=CSer::filetype_csim_xml_simplemodel_file) );
        if (ar.isStoring())
        {
            ar.xmlAddNode_float("size",_forceSensorSize);

            ar.xmlPushNewNode("filter");
            ar.xmlAddNode_int("sampleSize",_valueCountForFilter);
            ar.xmlAddNode_bool("averageFilter",_filterType==0); // 0=average, 1=median
            ar.xmlPopNode();

            ar.xmlPushNewNode("force");
            ar.xmlAddNode_bool("thresholdEnabled",_forceThresholdEnabled);
            ar.xmlAddNode_float("threshold",_forceThreshold);
            ar.xmlPopNode();

            ar.xmlPushNewNode("torque");
            ar.xmlAddNode_bool("threasholdEnabled",_torqueThresholdEnabled);
            ar.xmlAddNode_float("threshold",_torqueThreshold);
            ar.xmlPopNode();

            ar.xmlAddNode_int("consecutiveThresholdViolationsForTrigger",_consecutiveThresholdViolationsForBreaking);

            if (exhaustiveXml)
            {
                ar.xmlPushNewNode("objectColor");
                _color.serialize(ar,0);
                ar.xmlPopNode();
            }
            else
            {
                int rgb[3];
                for (size_t l=0;l<3;l++)
                    rgb[l]=int(_color.getColorsPtr()[l]*255.1f);
                ar.xmlAddNode_ints("objectColor",rgb,3);
            }
        }
        else
        {
            if (ar.xmlGetNode_float("size",_forceSensorSize,exhaustiveXml))
                setForceSensorSize(_forceSensorSize);

            if (ar.xmlPushChildNode("filter",exhaustiveXml))
            {
                if (ar.xmlGetNode_int("sampleSize",_valueCountForFilter,exhaustiveXml))
                    setValueCountForFilter(_valueCountForFilter);
                bool avg;
                if (ar.xmlGetNode_bool("averageFilter",avg,exhaustiveXml))
                {
                    _filterType=0;
                    if (!avg)
                        _filterType=1;
                }
                ar.xmlPopNode();
            }
            if (ar.xmlPushChildNode("force",exhaustiveXml))
            {
                ar.xmlGetNode_bool("thresholdEnabled",_forceThresholdEnabled,exhaustiveXml);
                if (ar.xmlGetNode_float("threshold",_forceThreshold,exhaustiveXml))
                    setForceThreshold(_forceThreshold);
                ar.xmlPopNode();
            }
            if (ar.xmlPushChildNode("torque",exhaustiveXml))
            {
                ar.xmlGetNode_bool("threasholdEnabled",_torqueThresholdEnabled);
                if (ar.xmlGetNode_float("threshold",_torqueThreshold,exhaustiveXml))
                    setTorqueThreshold(_torqueThreshold);
                ar.xmlPopNode();
            }

            ar.xmlGetNode_int("consecutiveThresholdViolationsForTrigger",_consecutiveThresholdViolationsForBreaking,exhaustiveXml);

            if (exhaustiveXml)
            {
                if (ar.xmlPushChildNode("objectColor",false))
                {
                    _color.serialize(ar,0);
                    ar.xmlPopNode();
                }
            }
            else
            {
                int rgb[3];
                if (ar.xmlGetNode_ints("objectColor",rgb,3,false))
                    _color.setColor(float(rgb[0])/255.1f,float(rgb[1])/255.1f,float(rgb[2])/255.1f,sim_colorcomponent_ambient_diffuse);
            }

            if (ar.xmlPushChildNode("color",false))
            { // for backward compatibility
                if (exhaustiveXml)
                {
                    if (ar.xmlPushChildNode("part1"))
                    {
                        _color.serialize(ar,0);
                        ar.xmlPopNode();
                    }
                    if (ar.xmlPushChildNode("part2"))
                    {
                        _color_removeSoon.serialize(ar,0);
                        ar.xmlPopNode();
                    }
                }
                else
                {
                    int rgb[3];
                    if (ar.xmlGetNode_ints("part1",rgb,3,exhaustiveXml))
                        _color.setColor(float(rgb[0])/255.1f,float(rgb[1])/255.1f,float(rgb[2])/255.1f,sim_colorcomponent_ambient_diffuse);
                    if (ar.xmlGetNode_ints("part2",rgb,3,exhaustiveXml))
                        _color_removeSoon.setColor(float(rgb[0])/255.1f,float(rgb[1])/255.1f,float(rgb[2])/255.1f,sim_colorcomponent_ambient_diffuse);
                }
                ar.xmlPopNode();
            }
            computeBoundingBox();
        }
    }
}

void CForceSensor::display(CViewableBase* renderingObject,int displayAttrib)
{
    EASYLOCK(_objectMutex);
    displayForceSensor(this,renderingObject,displayAttrib);
}
