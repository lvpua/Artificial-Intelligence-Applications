#pragma once

#include "sceneObject.h"

enum { /* Bullet joint float params */
    simi_bullet_joint_stoperp=0,
    simi_bullet_joint_stopcfm,
    simi_bullet_joint_normalcfm
};

// enum { /* Bullet joint int params */
//     /* if you add something here, search for bji11032016 */
// };
// enum { /* Bullet joint bit params */
//     /* if you add something here, search for bjb11032016 */
// };

enum { /* Ode joint float params */
    simi_ode_joint_stoperp=0,
    simi_ode_joint_stopcfm,
    simi_ode_joint_bounce,
    simi_ode_joint_fudgefactor,
    simi_ode_joint_normalcfm
};

// enum { /* Ode joint int params */
//     /* if you add something here, search for oji11032016 */
// };
// enum { /* Ode joint bit params */
//     /* if you add something here, search for ojb11032016 */
// };

enum { /* Vortex joint float params */
    simi_vortex_joint_lowerlimitdamping=0,
    simi_vortex_joint_upperlimitdamping,
    simi_vortex_joint_lowerlimitstiffness,
    simi_vortex_joint_upperlimitstiffness,
    simi_vortex_joint_lowerlimitrestitution,
    simi_vortex_joint_upperlimitrestitution,
    simi_vortex_joint_lowerlimitmaxforce,
    simi_vortex_joint_upperlimitmaxforce,
    simi_vortex_joint_motorconstraintfrictioncoeff,
    simi_vortex_joint_motorconstraintfrictionmaxforce,
    simi_vortex_joint_motorconstraintfrictionloss,
    simi_vortex_joint_p0loss,
    simi_vortex_joint_p0stiffness,
    simi_vortex_joint_p0damping,
    simi_vortex_joint_p0frictioncoeff,
    simi_vortex_joint_p0frictionmaxforce,
    simi_vortex_joint_p0frictionloss,
    simi_vortex_joint_p1loss,
    simi_vortex_joint_p1stiffness,
    simi_vortex_joint_p1damping,
    simi_vortex_joint_p1frictioncoeff,
    simi_vortex_joint_p1frictionmaxforce,
    simi_vortex_joint_p1frictionloss,
    simi_vortex_joint_p2loss,
    simi_vortex_joint_p2stiffness,
    simi_vortex_joint_p2damping,
    simi_vortex_joint_p2frictioncoeff,
    simi_vortex_joint_p2frictionmaxforce,
    simi_vortex_joint_p2frictionloss,
    simi_vortex_joint_a0loss,
    simi_vortex_joint_a0stiffness,
    simi_vortex_joint_a0damping,
    simi_vortex_joint_a0frictioncoeff,
    simi_vortex_joint_a0frictionmaxforce,
    simi_vortex_joint_a0frictionloss,
    simi_vortex_joint_a1loss,
    simi_vortex_joint_a1stiffness,
    simi_vortex_joint_a1damping,
    simi_vortex_joint_a1frictioncoeff,
    simi_vortex_joint_a1frictionmaxforce,
    simi_vortex_joint_a1frictionloss,
    simi_vortex_joint_a2loss,
    simi_vortex_joint_a2stiffness,
    simi_vortex_joint_a2damping,
    simi_vortex_joint_a2frictioncoeff,
    simi_vortex_joint_a2frictionmaxforce,
    simi_vortex_joint_a2frictionloss,
    simi_vortex_joint_dependencyfactor,
    simi_vortex_joint_dependencyoffset
};

enum { /* Vortex joint int params */
    simi_vortex_joint_bitcoded=0,
    simi_vortex_joint_relaxationenabledbc,
    simi_vortex_joint_frictionenabledbc,
    simi_vortex_joint_frictionproportionalbc,
    simi_vortex_joint_objectid,
    simi_vortex_joint_dependentobjectid
};

enum { /* Vortex joint bit params */
    simi_vortex_joint_motorfrictionenabled=1,
    simi_vortex_joint_proportionalmotorfriction=2
};

enum { /* Newton joint float params */
    simi_newton_joint_dependencyfactor=0,
    simi_newton_joint_dependencyoffset
};

enum { /* Newton joint int params */
    simi_newton_joint_objectid=0,
    simi_newton_joint_dependentobjectid
};

// enum { /* Newton joint bit params */
//     /* if you add something here, search for njb11032016 */
// };

class CJoint : public CSceneObject
{
public:
    CJoint(); // default, use only during serialization!
    CJoint(int jointType);
    virtual ~CJoint();

    // Overridden from CSceneObject:
    void buildUpdateAndPopulateSynchronizationObject(const std::vector<SSyncRoute>* parentRouting);
    void connectSynchronizationObject();

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
    bool announceObjectWillBeErased(int objectHandle,bool copyBuffer);
    void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
    void performObjectLoadingMapping(const std::vector<int>* map,bool loadingAmodel);
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
    C7Vector getFullLocalTransformation() const;

    std::string getObjectTypeInfo() const;
    std::string getObjectTypeInfoExtended() const;

    float getLength() const;
    float getDiameter() const;
    float getPosition() const;
    bool getEnableDynamicMotor() const;
    float getDynamicMotorTargetVelocity() const;
    bool getDynamicMotorLockModeWhenInVelocityControl() const;
    float getDynamicMotorUpperLimitVelocity() const;
    float getDynamicMotorMaximumForce() const;
    bool getEnableDynamicMotorControlLoop() const;
    bool getEnableTorqueModulation() const;
    bool getHybridFunctionality() const;
    float getEngineFloatParam(int what,bool* ok) const;
    int getEngineIntParam(int what,bool* ok) const;
    bool getEngineBoolParam(int what,bool* ok) const;
    void getBulletFloatParams(std::vector<float>& p) const;
    void getBulletIntParams(std::vector<int>& p) const;
    void getOdeFloatParams(std::vector<float>& p) const;
    void getOdeIntParams(std::vector<int>& p) const;
    void getVortexFloatParams(std::vector<float>& p) const;
    void getVortexIntParams(std::vector<int>& p) const;
    void getNewtonFloatParams(std::vector<float>& p) const;
    void getNewtonIntParams(std::vector<int>& p) const;
    int getVortexDependentJointId() const;
    int getNewtonDependentJointId() const;
    float getScrewPitch() const;
    int getJointType() const;
    C4Vector getSphericalTransformation() const;
    bool getPositionIsCyclic() const;
    float getPositionIntervalMin() const;
    float getPositionIntervalRange() const;
    float getIKWeight() const;
    float getMaxStepSize() const;
    int getJointMode() const;
    int getDependencyMasterJointHandle() const;
    float getDependencyJointMult() const;
    float getDependencyJointOffset() const;
    void getDynamicMotorPositionControlParameters(float& p_param,float& i_param,float& d_param) const;
    void getDynamicMotorSpringControlParameters(float& k_param,float& c_param) const;
    float getDynamicMotorPositionControlTargetPosition() const;
    CColorObject* getColor(bool part2);

    C7Vector getIntrinsicTransformation(bool includeDynErrorComponent) const;


    float getMeasuredJointVelocity() const;
    std::string getDependencyJointLoadAlias() const;
    std::string getDependencyJointLoadName_old() const;
    int getJointCallbackCallOrder_backwardCompatibility() const;
    void setDirectDependentJoints(const std::vector<CJoint*>& joints);

    void setPositionIntervalMin(float min);
    void setPositionIntervalRange(float range);
    void setPositionIsCyclic(bool isCyclic);
    void setLength(float l);
    void setDiameter(float d);
    bool setScrewPitch(float pitch);
    void setDependencyMasterJointHandle(int depJointID);
    void setDependencyJointMult(float coeff);
    void setDependencyJointOffset(float off);
    void setIkWeight(float newWeight);
    void setMaxStepSize(float stepS);
    void setPosition(float pos,bool setDirect);
    void setSphericalTransformation(const C4Vector& tr);
    void setJointMode(int theMode);

    void setIntrinsicTransformationError(const C7Vector& tr);

    void setHybridFunctionality(bool h);
    void setDynamicMotorTargetVelocity(float v);
    void setDynamicMotorUpperLimitVelocity(float v);
    void setDynamicMotorPositionControlTargetPosition(float pos);
    void setDynamicMotorPositionControlParameters(float p_param,float i_param,float d_param);
    void setDynamicMotorSpringControlParameters(float k_param,float c_param);
    void setDynamicMotorMaximumForce(float f);

    void setBulletFloatParams(const std::vector<float>& p);
    void setOdeFloatParams(const std::vector<float>& p);
    void setVortexFloatParams(const std::vector<float>& p);
    void setNewtonFloatParams(const std::vector<float>& p);
    void setBulletIntParams(const std::vector<int>& p);
    void setOdeIntParams(const std::vector<int>& p);
    void setVortexIntParams(const std::vector<int>& p);
    void setNewtonIntParams(const std::vector<int>& p);

    bool setJointMode_noDynMotorTargetPosCorrection(int theMode);

    void setEnableDynamicMotor(bool e);
    void setEnableDynamicMotorControlLoop(bool p);
    void setEnableTorqueModulation(bool p);
    void setDynamicMotorLockModeWhenInVelocityControl(bool e);


    void measureJointVelocity(float dt); // should be called just after the main script was called!!!

    void getDynamicJointErrors(float& linear,float& angular) const;
    void getDynamicJointErrorsFull(C3Vector& linear,C3Vector& angular) const;

    void handleDynJointControl(bool init,int loopCnt,int totalLoops,float currentPos,float effort,float dynStepSize,float errorV,float& velocity,float& forceTorque);

    void setDynamicMotorReflectedPosition_useOnlyFromDynamicPart(float rfp);

    void addCumulativeForceOrTorque(float forceOrTorque,int countForAverage);
    bool getDynamicForceOrTorque(float& forceOrTorque,bool dynamicStepValue) const;
    void setForceOrTorqueNotValid();


    bool setEngineFloatParam(int what,float v);
    bool setEngineIntParam(int what,int v);
    bool setEngineBoolParam(int what,bool v);



    void copyEnginePropertiesTo(CJoint* target);


    // DEPRECATED:
    void setExplicitHandling_DEPRECATED(bool explicitHandl);
    bool getExplicitHandling_DEPRECATED();
    void resetJoint_DEPRECATED();
    void handleJoint_DEPRECATED(float deltaTime);
    void setUnlimitedAcceleration_DEPRECATED(bool unlimited);
    bool getUnlimitedAcceleration_DEPRECATED();
    void setInvertTargetVelocityAtLimits_DEPRECATED(bool invert);
    bool getInvertTargetVelocityAtLimits_DEPRECATED();
    void setMaxAcceleration_DEPRECATED(float maxAccel);
    float getMaxAcceleration_DEPRECATED();
    void setVelocity_DEPRECATED(float vel);
    float getVelocity_DEPRECATED();
    void setTargetVelocity_DEPRECATED(float vel);
    float getTargetVelocity_DEPRECATED();

protected:
    void _rectifyDependentJoints();
    void _commonInit();

    void _setPositionIntervalMin_sendOldIk(float min) const;
    void _setPositionIntervalRange_sendOldIk(float range) const;
    void _setPositionIsCyclic_sendOldIk(bool isCyclic) const;
    void _setScrewPitch_sendOldIk(float pitch) const;
    void _setDependencyJointHandle_sendOldIk(int depJointID) const;
    void _setDependencyJointMult_sendOldIk(float coeff) const;
    void _setDependencyJointOffset_sendOldIk(float off) const;
    void _setIkWeight_sendOldIk(float newWeight) const;
    void _setMaxStepSize_sendOldIk(float stepS) const;
    void _setPosition_sendOldIk(float pos) const;
    void _setSphericalTransformation_sendOldIk(const C4Vector& tr) const;
    void _setJointMode_sendOldIk(int theMode) const;

    std::vector<CJoint*> _directDependentJoints;

    float _dynamicMotorPIDCumulativeErrorForIntegralParameter;
    float _dynamicMotorPIDLastErrorForDerivativeParameter;

    std::string _dependencyJointLoadAlias;
    std::string _dependencyJointLoadName_old;

    float _initialPosition;
    C4Vector _initialSphericalJointTransformation;
    float _initialTargetPosition;
    bool _initialDynamicMotorEnabled;
    float _initialDynamicMotorTargetVelocity;
    bool _initialDynamicMotorLockModeWhenInVelocityControl;
    float _initialDynamicMotorUpperLimitVelocity;
    float _initialDynamicMotorMaximumForce;

    bool _initialDynamicMotorControlLoopEnabled;
    float _initialDynamicMotorPositionControl_P;
    float _initialDynamicMotorPositionControl_I;
    float _initialDynamicMotorPositionControl_D;
    float _initialDynamicMotorSpringControl_K;
    float _initialDynamicMotorSpringControl_C;
    int _initialJointMode;
    bool _initialHybridOperation;
    int _initialJointCallbackCallOrder;

    float _measuredJointVelocity_velocityMeasurement;
    float _previousJointPosition_velocityMeasurement;
    bool _previousJointPositionIsValid;

    float _cumulatedForceOrTorque;
    float _cumulativeForceOrTorqueTmp;
    bool _averageForceOrTorqueValid;

    int _jointType;
    float _length;
    float _diameter;
    C4Vector _sphericalTransformation;
    bool _positionIsCyclic;
    float _screwPitch;
    float _jointMinPosition;
    float _jointPositionRange;
    float _jointPosition;
    float _maxStepSize;
    float _ikWeight;
    int _jointMode;
    int _dependencyMasterJointHandle;
    float _dependencyJointMult;
    float _dependencyJointOffset;

    CColorObject _color;
    CColorObject _color_removeSoon;

    bool _dynamicMotorEnabled;
    float _dynamicMotorTargetVelocity;
    bool _dynamicLockModeWhenInVelocityControl;
    float _dynamicMotorUpperLimitVelocity;
    float _dynamicMotorMaximumForce;
    bool _dynamicMotorControlLoopEnabled;
    bool _dynamicMotorPositionControl_torqueModulation;
    float _dynamicMotorPositionControl_targetPosition;
    float _dynamicMotorPositionControl_P;
    float _dynamicMotorPositionControl_I;
    float _dynamicMotorPositionControl_D;
    float _dynamicMotorSpringControl_K;
    float _dynamicMotorSpringControl_C;
    bool _jointHasHybridFunctionality;

    std::vector<float> _bulletFloatParams;
    std::vector<int> _bulletIntParams;

    std::vector<float> _odeFloatParams;
    std::vector<int> _odeIntParams;

    std::vector<float> _vortexFloatParams;
    std::vector<int> _vortexIntParams;

    std::vector<float> _newtonFloatParams;
    std::vector<int> _newtonIntParams;

    C7Vector _intrinsicTransformationError; // from physics engine

    // Following is the force/torque acquired during a single dyn. calculation step:
    float _lastForceOrTorque_dynStep;
    bool _lastForceOrTorqueValid_dynStep;

    // DEPRECATED:
    float _jointPositionForMotionHandling_DEPRECATED;
    float _velocity_DEPRECATED;
    float _targetVelocity_DEPRECATED;
    bool _explicitHandling_DEPRECATED;
    bool _unlimitedAcceleration_DEPRECATED;
    bool _invertTargetVelocityAtLimits_DEPRECATED;
    float _maxAcceleration_DEPRECATED;
    int _jointCallbackCallOrder_backwardCompatibility;
    float _initialVelocity_DEPRECATED;
    float _initialTargetVelocity_DEPRECATED;
    bool _initialExplicitHandling_DEPRECATED;
};
