#include "simInternal.h"
#include "simStrings.h"
#include "ttUtil.h"
#include "dummy.h"
#include "global.h"
#include "easyLock.h"
#include "app.h"
#include "tt.h"
#include "dummyRendering.h"
#include "pluginContainer.h"

CDummy::CDummy()
{
    _objectType=sim_object_dummy_type;
    _localObjectSpecialProperty=0;

    _dummySize=0.01f;
    _assignedToParentPath=false;
    _assignedToParentPathOrientation=false;
    _linkedDummyHandle=-1;
    _linkType=sim_dummy_linktype_dynamics_loop_closure;

    _visibilityLayer=DUMMY_LAYER;
    _objectAlias=IDSOGL_DUMMY;
    _objectName_old=IDSOGL_DUMMY;
    _objectAltName_old=tt::getObjectAltNameFromObjectName(_objectName_old.c_str());

    _freeOnPathTrajectory=false;
    _virtualDistanceOffsetOnPath=0.0f;
    _virtualDistanceOffsetOnPath_variationWhenCopy=0.0f;

    _dummyColor.setDefaultValues();
    _dummyColor.setColor(1.0f,0.8f,0.55f,sim_colorcomponent_ambient_diffuse);
    computeBoundingBox();
}

CDummy::~CDummy()
{
}

std::string CDummy::getObjectTypeInfo() const
{
    return(IDSOGL_DUMMY);
}

std::string CDummy::getObjectTypeInfoExtended() const
{
    return(IDSOGL_DUMMY);
}

bool CDummy::isPotentiallyCollidable() const
{
    return(true);
}

bool CDummy::isPotentiallyMeasurable() const
{
    return(true);
}

bool CDummy::isPotentiallyDetectable() const
{
    return(true);
}

void CDummy::computeBoundingBox()
{
    C3Vector maxV(_dummySize/2.0f,_dummySize/2.0f,_dummySize/2.0f);
    _setBoundingBox(maxV*-1.0f,maxV);
}

bool CDummy::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices) const
{
    vertices.clear();
    indices.clear();
    return(false); // for now
}

void CDummy::scaleObject(float scalingFactor)
{
    setDummySize(_dummySize*scalingFactor);
    _virtualDistanceOffsetOnPath*=scalingFactor;
    _virtualDistanceOffsetOnPath_variationWhenCopy*=scalingFactor;

    CSceneObject::scaleObject(scalingFactor);
}

void CDummy::scaleObjectNonIsometrically(float x,float y,float z)
{
    scaleObject(cbrt(x*y*z));
}

void CDummy::removeSceneDependencies()
{
    CSceneObject::removeSceneDependencies();
    setLinkedDummyHandle(-1,false);
}

void CDummy::addSpecializedObjectEventData(CInterfaceStackTable* data) const
{
    CInterfaceStackTable* subC=new CInterfaceStackTable();
    data->appendMapObject_stringObject("dummy",subC);
    data=subC;

    data->appendMapObject_stringFloat("size",_dummySize);

    CInterfaceStackTable* colors=new CInterfaceStackTable();
    data->appendMapObject_stringObject("colors",colors);
    float c[9];
    _dummyColor.getColor(c,sim_colorcomponent_ambient_diffuse);
    _dummyColor.getColor(c+3,sim_colorcomponent_specular);
    _dummyColor.getColor(c+6,sim_colorcomponent_emission);
    colors->appendArrayObject_floatArray(c,9);
}

CSceneObject* CDummy::copyYourself()
{   
    CDummy* newDummy=(CDummy*)CSceneObject::copyYourself();
    newDummy->_linkedDummyHandle=_linkedDummyHandle; // important for copy operations connections

    _dummyColor.copyYourselfInto(&newDummy->_dummyColor);
    newDummy->_dummySize=_dummySize;
    newDummy->_linkType=_linkType;
    newDummy->_assignedToParentPath=_assignedToParentPath;
    newDummy->_assignedToParentPathOrientation=_assignedToParentPathOrientation;
    newDummy->_freeOnPathTrajectory=_freeOnPathTrajectory;

    if (App::worldContainer->copyBuffer->isCopyForPasting())
    { // here the original object is not reset (the variation) because it is located in the copy buffer!
        _virtualDistanceOffsetOnPath+=_virtualDistanceOffsetOnPath_variationWhenCopy;
        newDummy->_virtualDistanceOffsetOnPath=_virtualDistanceOffsetOnPath;
        newDummy->_virtualDistanceOffsetOnPath_variationWhenCopy=0.0f; // the new new object's variation is reset!
    }
    else
    {
        newDummy->_virtualDistanceOffsetOnPath=_virtualDistanceOffsetOnPath;
        newDummy->_virtualDistanceOffsetOnPath_variationWhenCopy=_virtualDistanceOffsetOnPath_variationWhenCopy;
        _virtualDistanceOffsetOnPath_variationWhenCopy=0.0f; // we reset the original object!!
    }

    return(newDummy);
}

bool CDummy::setAssignedToParentPath(bool assigned)
{
    bool diff=(_assignedToParentPath!=assigned);
    if (diff)
    {
        _assignedToParentPath=assigned;
        if (_assignedToParentPath)
            setAssignedToParentPathOrientation(false);
    }
    return(diff);
}

bool CDummy::setAssignedToParentPathOrientation(bool assigned)
{
    bool diff=(_assignedToParentPathOrientation!=assigned);
    if (diff)
    {
        _assignedToParentPathOrientation=assigned;
        if (_assignedToParentPathOrientation)
            setAssignedToParentPath(false);
    }
    return(diff);
}

void CDummy::setFreeOnPathTrajectory(bool isFree)
{
    _freeOnPathTrajectory=isFree;
}

void CDummy::setVirtualDistanceOffsetOnPath(float off)
{
    _virtualDistanceOffsetOnPath=off;
}

void CDummy::setVirtualDistanceOffsetOnPath_variationWhenCopy(float off)
{
    _virtualDistanceOffsetOnPath_variationWhenCopy=off;
}

void CDummy::announceCollectionWillBeErased(int groupID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceCollectionWillBeErased(groupID,copyBuffer);
}

void CDummy::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceCollisionWillBeErased(collisionID,copyBuffer);
}

void CDummy::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceDistanceWillBeErased(distanceID,copyBuffer);
}

void CDummy::performIkLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{
    CSceneObject::performIkLoadingMapping(map,loadingAmodel);
}

void CDummy::performCollectionLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
    CSceneObject::performCollectionLoadingMapping(map,loadingAmodel);
}

void CDummy::performCollisionLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
    CSceneObject::performCollisionLoadingMapping(map,loadingAmodel);
}

void CDummy::performDistanceLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
    CSceneObject::performDistanceLoadingMapping(map,loadingAmodel);
}

void CDummy::performTextureObjectLoadingMapping(const std::vector<int>* map)
{
    CSceneObject::performTextureObjectLoadingMapping(map);
}

void CDummy::performDynMaterialObjectLoadingMapping(const std::vector<int>* map)
{
    CSceneObject::performDynMaterialObjectLoadingMapping(map);
}

void CDummy::initializeInitialValues(bool simulationAlreadyRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
    CSceneObject::initializeInitialValues(simulationAlreadyRunning);
}

void CDummy::simulationAboutToStart()
{
    initializeInitialValues(false);
    CSceneObject::simulationAboutToStart();
}

void CDummy::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
    if (_initialValuesInitialized)
    {
        if ( App::currentWorld->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeModelProperty()&sim_modelproperty_not_reset)==0))
        {
        }
    }
    CSceneObject::simulationEnded();
}

void CDummy::serialize(CSer& ar)
{
    CSceneObject::serialize(ar);
    if (ar.isBinary())
    {
        if (ar.isStoring())
        {       // Storing
            ar.storeDataName("Dy2");
            ar << _dummySize;
            ar.flush();

            ar.storeDataName("Cl0");
            ar.setCountingMode();
            _dummyColor.serialize(ar,0);
            if (ar.setWritingMode())
                _dummyColor.serialize(ar,0);

            ar.storeDataName("Lli");
            ar << _linkedDummyHandle;
            ar.flush();

            ar.storeDataName("Var");
            unsigned char dummy=0;
    // removed on 2010/01/26        SIM_SET_CLEAR_BIT(dummy,0,_targetDummy);
            SIM_SET_CLEAR_BIT(dummy,1,_assignedToParentPath);
            SIM_SET_CLEAR_BIT(dummy,2,_freeOnPathTrajectory);
            SIM_SET_CLEAR_BIT(dummy,3,_assignedToParentPathOrientation);
    // removed on 17/6/2011.. probably always 0     SIM_SET_CLEAR_BIT(dummy,4,_removeAfterAssembly);
            ar << dummy;
            ar.flush();

            ar.storeDataName("Po5");
            ar << _virtualDistanceOffsetOnPath << _virtualDistanceOffsetOnPath_variationWhenCopy;
            ar.flush();

            ar.storeDataName("Dl2");
            ar << _linkType;
            ar.flush();

            ar.storeDataName(SER_END_OF_OBJECT);
        }
        else
        {       // Loading
            int byteQuantity;
            std::string theName="";
            bool before2009_12_16=false;
            while (theName.compare(SER_END_OF_OBJECT)!=0)
            {
                theName=ar.readDataName();
                if (theName.compare(SER_END_OF_OBJECT)!=0)
                {
                    bool noHit=true;
                    if (theName.compare("Dy2")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _dummySize;
                    }
                    if (theName.compare("Lli")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _linkedDummyHandle;
                    }
                    if (theName.compare("Cl0")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        _dummyColor.serialize(ar,0);
                    }
                    if (theName.compare("Var")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        unsigned char dummy;
                        ar >> dummy;
                        _assignedToParentPath=SIM_IS_BIT_SET(dummy,1);
                        _freeOnPathTrajectory=SIM_IS_BIT_SET(dummy,2);
                        _assignedToParentPathOrientation=SIM_IS_BIT_SET(dummy,3);
                    }
    //************* for backward compatibility (23/02/2011) ****************
                    if (theName.compare("Po4")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _virtualDistanceOffsetOnPath;
                    }
    //************************************************************************
                    if (theName.compare("Po5")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _virtualDistanceOffsetOnPath >> _virtualDistanceOffsetOnPath_variationWhenCopy;
                    }

                    if (theName.compare("Dl2")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _linkType;
                    }
    //************** for backward compatibility (17/6/2011) **********************
                    if (theName.compare("Ack")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        ar >> _modelAcknowledgement; // this is now the CSceneObject's variable!!! (was Dummy's variable before)
                    }
    //****************************************************************************
                    if (noHit)
                        ar.loadUnknownData();
                }
            }
            if (before2009_12_16)
            {
                if (_linkedDummyHandle==-1)
                    _linkType=sim_dummy_linktype_dynamics_loop_closure;
            }
            if (ar.getSerializationVersionThatWroteThisFile()<17)
            { // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
                CTTUtil::scaleColorUp_(_dummyColor.getColorsPtr());
            }
            computeBoundingBox();
        }
    }
    else
    {
        bool exhaustiveXml=( (ar.getFileType()!=CSer::filetype_csim_xml_simplescene_file)&&(ar.getFileType()!=CSer::filetype_csim_xml_simplemodel_file) );
        if (ar.isStoring())
        {
            ar.xmlAddNode_float("size",_dummySize);

            if (exhaustiveXml)
                ar.xmlAddNode_int("linkedDummyHandle",_linkedDummyHandle);
            else
            {
                std::string str;
                CDummy* it=App::currentWorld->sceneObjects->getDummyFromHandle(_linkedDummyHandle);
                if (it!=nullptr)
                    str=it->getObjectName_old();
                ar.xmlAddNode_comment(" 'linkedDummy' tag only used for backward compatibility, use instead 'linkedDummyAlias' tag",exhaustiveXml);
                ar.xmlAddNode_string("linkedDummy",str.c_str());
                if (it!=nullptr)
                {
                    str=it->getObjectAlias()+"*";
                    str+=std::to_string(it->getObjectHandle());
                }
                ar.xmlAddNode_string("linkedDummyAlias",str.c_str());
            }

            ar.xmlAddNode_comment(" 'linkType' tag: can be 'dynamics_loopClosure'' ",exhaustiveXml);
            ar.xmlAddNode_enum("linkType",_linkType,sim_dummy_linktype_dynamics_loop_closure,"dynamics_loopClosure",sim_dummy_linktype_dynamics_force_constraint,"dynamics_forceConstraint",sim_dummy_linktype_ik_tip_target,"ik_tipTarget");

            if (exhaustiveXml)
            {
                ar.xmlPushNewNode("switches");
                ar.xmlAddNode_bool("assignedToParentPath",_assignedToParentPath);
                ar.xmlAddNode_bool("freeOnPathTrajectory",_freeOnPathTrajectory);
                ar.xmlAddNode_bool("assignedToParentPathOrientation",_assignedToParentPathOrientation);
                ar.xmlPopNode();

                ar.xmlAddNode_float("virtualDistanceOffsetOnPath",_virtualDistanceOffsetOnPath);
                ar.xmlAddNode_float("virtualDistanceOffsetOnPath_whenCopy",_virtualDistanceOffsetOnPath_variationWhenCopy);
            }

            ar.xmlPushNewNode("color");
            if (exhaustiveXml)
                _dummyColor.serialize(ar,0);
            else
            {
                int rgb[3];
                for (size_t l=0;l<3;l++)
                    rgb[l]=int(_dummyColor.getColorsPtr()[l]*255.1f);
                ar.xmlAddNode_ints("object",rgb,3);
            }
            ar.xmlPopNode();
        }
        else
        {
            ar.xmlGetNode_float("size",_dummySize,exhaustiveXml);

            if (exhaustiveXml)
                ar.xmlGetNode_int("linkedDummyHandle",_linkedDummyHandle);
            else
            {
                ar.xmlGetNode_string("linkedDummyAlias",_linkedDummyLoadAlias,exhaustiveXml);
                ar.xmlGetNode_string("linkedDummy",_linkedDummyLoadName_old,exhaustiveXml);
            }
            ar.xmlGetNode_enum("linkType",_linkType,exhaustiveXml,"dynamics_loopClosure",sim_dummy_linktype_dynamics_loop_closure,"dynamics_forceConstraint",sim_dummy_linktype_dynamics_force_constraint,"ik_tipTarget",sim_dummy_linktype_ik_tip_target);

            if (exhaustiveXml&&ar.xmlPushChildNode("switches"))
            {
                ar.xmlGetNode_bool("assignedToParentPath",_assignedToParentPath);
                ar.xmlGetNode_bool("freeOnPathTrajectory",_freeOnPathTrajectory);
                ar.xmlGetNode_bool("assignedToParentPathOrientation",_assignedToParentPathOrientation);
                ar.xmlPopNode();
            }

            if (exhaustiveXml)
            {
                ar.xmlGetNode_float("virtualDistanceOffsetOnPath",_virtualDistanceOffsetOnPath,exhaustiveXml);
                ar.xmlGetNode_float("virtualDistanceOffsetOnPath_whenCopy",_virtualDistanceOffsetOnPath_variationWhenCopy,exhaustiveXml);
            }

            if (ar.xmlPushChildNode("color",exhaustiveXml))
            {
                if (exhaustiveXml)
                    _dummyColor.serialize(ar,0);
                else
                {
                    int rgb[3];
                    if (ar.xmlGetNode_ints("object",rgb,3,exhaustiveXml))
                        _dummyColor.setColor(float(rgb[0])/255.1f,float(rgb[1])/255.1f,float(rgb[2])/255.1f,sim_colorcomponent_ambient_diffuse);
                }
                ar.xmlPopNode();
            }
            computeBoundingBox();
        }
    }
}

// SPECIAL FUNCTION TO GUARANTEE FORWARD COMPATIBILITY WHEN LOADING OBJECT TYPES THAT DON'T EXIST YET!
void CDummy::loadUnknownObjectType(CSer& ar)
{
    CSceneObject::serialize(ar);
    // Now finish reading the object! (but we throw its content away!)
    std::string theName="";
    while (theName.compare(SER_END_OF_OBJECT)!=0)
    {
        theName=ar.readDataName();
        if (theName.compare(SER_END_OF_OBJECT)!=0)
            ar.loadUnknownData();
    }
}

void CDummy::performObjectLoadingMapping(const std::vector<int>* map,bool loadingAmodel)
{ // New_Object_ID=map[Old_Object_ID]
    CSceneObject::performObjectLoadingMapping(map,loadingAmodel);
    _linkedDummyHandle=CWorld::getLoadingMapping(map,_linkedDummyHandle);
}

void CDummy::setLinkedDummyHandle(int handle,bool check)
{
    int _linkedDummyHandleOld=_linkedDummyHandle;
    CDummy* thisObject=nullptr;
    if (check)
        thisObject=App::currentWorld->sceneObjects->getDummyFromHandle(_objectHandle);
    if (thisObject!=this)
        _linkedDummyHandle=handle;
    else
    {
        CDummy* linkedDummy=App::currentWorld->sceneObjects->getDummyFromHandle(_linkedDummyHandle);
        if (handle==-1)
        { // we unlink this dummy and its partner:
            if (linkedDummy!=nullptr)
                linkedDummy->setLinkedDummyHandle(-1,false);
            _linkedDummyHandle=-1;
        }
        else if (_linkedDummyHandle!=handle)
        { // We link this dummy to another dummy
            CDummy* newLinkedDummy=App::currentWorld->sceneObjects->getDummyFromHandle(handle);
            if (linkedDummy!=nullptr)
                linkedDummy->setLinkedDummyHandle(-1,false); // we first detach it from its old partner
            if (newLinkedDummy!=nullptr)
            { // we detach the new dummy from its original linking:
                newLinkedDummy->setLinkedDummyHandle(-1,true);
                _linkedDummyHandle=handle;
                newLinkedDummy->setLinkedDummyHandle(getObjectHandle(),false);

                if (_linkType==sim_dummy_linktype_gcs_tip)
                    newLinkedDummy->setLinkType(sim_dummy_linktype_gcs_target,false);
                if (_linkType==sim_dummy_linktype_gcs_target)
                    newLinkedDummy->setLinkType(sim_dummy_linktype_gcs_tip,false);
                if ( (_linkType==sim_dummy_linktype_ik_tip_target)||(_linkType==sim_dummy_linktype_gcs_loop_closure)||(_linkType==sim_dummy_linktype_dynamics_loop_closure)||(_linkType==sim_dummy_linktype_dynamics_force_constraint) )
                    newLinkedDummy->setLinkType(_linkType,false);
            }
            else
                _linkedDummyHandle=-1;
        }
    }
    if (_linkedDummyHandleOld!=_linkedDummyHandle)
    {
        if (getObjectCanSync())
            _setLinkedDummyHandle_sendOldIk(_linkedDummyHandle);
        App::setRefreshHierarchyViewFlag();
        App::setFullDialogRefreshFlag();
    }
}

bool CDummy::setLinkType(int lt,bool check)
{
    bool diff=(_linkType!=lt);
    if (diff)
    {
        _linkType=lt;
        if (getObjectCanSync())
            _setLinkType_sendOldIk(lt);
    }
    if ( (_linkedDummyHandle!=-1)&&check )
    {
        CDummy* it=App::currentWorld->sceneObjects->getDummyFromHandle(_linkedDummyHandle);
        CDummy* thisObject=thisObject=App::currentWorld->sceneObjects->getDummyFromHandle(_objectHandle);
        if ( (thisObject==this)&&(it!=nullptr) )
        { // dummy is in the scene
            if (lt==sim_dummy_linktype_gcs_tip)
                it->setLinkType(sim_dummy_linktype_gcs_target,false);
            if (lt==sim_dummy_linktype_gcs_target)
                it->setLinkType(sim_dummy_linktype_gcs_tip,false);
            if ( (lt==sim_dummy_linktype_ik_tip_target)||(lt==sim_dummy_linktype_gcs_loop_closure)||(lt==sim_dummy_linktype_dynamics_loop_closure)||(lt==sim_dummy_linktype_dynamics_force_constraint) )
                it->setLinkType(lt,false);
        }
        App::setRefreshHierarchyViewFlag();
        App::setFullDialogRefreshFlag();
    }
    return(diff);
}

bool CDummy::announceObjectWillBeErased(int objectHandle,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    // This routine can be called for objCont-objects, but also for objects
    // in the copy-buffer!! So never make use of any 
    // 'ct::objCont->getObject(id)'-call or similar
    // Return value true means 'this' has to be erased too!
    bool retVal=CSceneObject::announceObjectWillBeErased(objectHandle,copyBuffer);
    if (_linkedDummyHandle==objectHandle)
        setLinkedDummyHandle(-1,!copyBuffer);
    return(retVal);
}

void CDummy::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{   // copyBuffer is false by default (if true, we are 'talking' to objects
    // in the copyBuffer)
    CSceneObject::announceIkObjectWillBeErased(ikGroupID,copyBuffer);
}

void CDummy::display(CViewableBase* renderingObject,int displayAttrib)
{
    EASYLOCK(_objectMutex);
    displayDummy(this,renderingObject,displayAttrib);
}

void CDummy::_setLinkedDummyHandle_sendOldIk(int h) const
{
    // Synchronize with IK plugin:
    if (_ikPluginCounterpartHandle!=-1)
    {
        int hh=-1;
        if (h!=-1)
        {
            if (_linkType==sim_dummy_linktype_ik_tip_target)
                hh=App::currentWorld->sceneObjects->getObjectFromHandle(h)->getIkPluginCounterpartHandle();
        }
        CPluginContainer::ikPlugin_setLinkedDummy(_ikPluginCounterpartHandle,hh);
    }
}

void CDummy::_setLinkType_sendOldIk(int t) const
{
    // Synchronize with IK plugin:
    if (_ikPluginCounterpartHandle!=-1)
    {
        int hh=-1;
        if (_linkedDummyHandle!=-1)
        {
            if (t==sim_dummy_linktype_ik_tip_target)
                hh=App::currentWorld->sceneObjects->getObjectFromHandle(_linkedDummyHandle)->getIkPluginCounterpartHandle();
        }
        CPluginContainer::ikPlugin_setLinkedDummy(_ikPluginCounterpartHandle,hh);
    }
}

bool CDummy::getFreeOnPathTrajectory() const
{
    return(_freeOnPathTrajectory);
}

float CDummy::getVirtualDistanceOffsetOnPath() const
{
    return(_virtualDistanceOffsetOnPath);
}

float CDummy::getVirtualDistanceOffsetOnPath_variationWhenCopy() const
{
    return(_virtualDistanceOffsetOnPath_variationWhenCopy);
}

std::string CDummy::getLinkedDummyLoadName_old() const
{
    return(_linkedDummyLoadName_old);
}

std::string CDummy::getLinkedDummyLoadAlias() const
{
    return(_linkedDummyLoadAlias);
}

void CDummy::buildUpdateAndPopulateSynchronizationObject(const std::vector<SSyncRoute>* parentRouting)
{ // Overridden from CSceneObject
    if (setObjectCanSync(true))
    {
        // Set routing:
        SSyncRoute r;
        r.objHandle=_objectHandle;
        r.objType=sim_syncobj_dummy;
        setSyncMsgRouting(parentRouting,r);

        // Update the remote sceneObject:
        CSceneObject::buildUpdateAndPopulateSynchronizationObject(parentRouting);
    }
}

void CDummy::connectSynchronizationObject()
{ // Overridden from CSceneObject
    if (getObjectCanSync())
    {
        CSceneObject::connectSynchronizationObject();

        _setLinkedDummyHandle_sendOldIk(_linkedDummyHandle);
        _setLinkType_sendOldIk(_linkType);
    }
}

bool CDummy::getAssignedToParentPath() const
{
    return(_assignedToParentPath);
}

bool CDummy::getAssignedToParentPathOrientation() const
{
    return(_assignedToParentPathOrientation);
}

float CDummy::getDummySize() const
{
    return(_dummySize);
}

int CDummy::getLinkType() const
{
    return(_linkType);
}

int CDummy::getLinkedDummyHandle() const
{
    return(_linkedDummyHandle);
}

CColorObject* CDummy::getDummyColor()
{
    return(&_dummyColor);
}

void CDummy::setDummySize(float s)
{
    bool diff=(_dummySize!=s);
    if (diff)
    {
        _dummySize=s;
        computeBoundingBox();
        if ( _isInScene&&App::worldContainer->getEventsEnabled() )
        {
            const char* cmd="size";
            auto [event,data]=App::worldContainer->prepareSceneObjectChangedEvent(this,false,cmd,true);
            data->appendMapObject_stringFloat(cmd,_dummySize);
            App::worldContainer->pushEvent(event);
        }
    }
}
