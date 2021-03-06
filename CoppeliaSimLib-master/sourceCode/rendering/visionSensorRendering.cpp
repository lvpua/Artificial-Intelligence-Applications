#include "visionSensorRendering.h"

#ifdef SIM_WITH_OPENGL
#include "pluginContainer.h"

void displayVisionSensor(CVisionSensor* visionSensor,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(visionSensor,renderingObject,displayAttrib);

    // Bounding box display:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(visionSensor,displayAttrib,true,0.0);

    C3Vector normalVectorForLinesAndPoints(visionSensor->getFullCumulativeTransformation().Q.getInverse()*C3Vector::unitZVector);

    // Object display:
    if (visionSensor->getShouldObjectBeDisplayed(renderingObject->getObjectHandle(),displayAttrib))
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE_OLD)==0)
        {
            if (visionSensor->getObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(visionSensor->getModelSelectionHandle());
            else
                glLoadName(visionSensor->getObjectHandle());
        }
        else
            glLoadName(-1);

        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

        glPushMatrix();

        if (visionSensor->getShowVolume())
        {
            visionSensor->getColor()->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
            C3Vector c,f;
            visionSensor->getVolumeVectors(c,f);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(-f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(-f(0),+f(1),f(2));
            ogl::addBuffer3DPoints(+f(0),+f(1),f(2));
            ogl::addBuffer3DPoints(+f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(-f(0),-f(1),f(2));
            ogl::drawRandom3dLines(&ogl::buffer[0],(int)ogl::buffer.size()/3,true,normalVectorForLinesAndPoints.data);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(-c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(-c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(+c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(+c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(-c(0),-c(1),c(2));
            ogl::drawRandom3dLines(&ogl::buffer[0],(int)ogl::buffer.size()/3,true,normalVectorForLinesAndPoints.data);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(-c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(-f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(-c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(-f(0),+f(1),f(2));
            ogl::addBuffer3DPoints(+c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(+f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(+c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(+f(0),+f(1),f(2));
            ogl::drawRandom3dLines(&ogl::buffer[0],(int)ogl::buffer.size()/3,false,normalVectorForLinesAndPoints.data);
            ogl::buffer.clear();
        }

        visionSensor->getColor()->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        float _size(visionSensor->getVisionSensorSize());
        float dx=_size*0.5f;
        float dy=_size*0.5f;
        glBegin(GL_QUADS);
        glNormal3f(-1.0f,0.0f,0.0f);
        glVertex3f(-dx,-dy,0.0f);
        glVertex3f(-dx,dy,0.0f);
        glVertex3f(-dx,dy,-_size*2.0f);
        glVertex3f(-dx,-dy,-_size*2.0f);
        glNormal3f(0.0f,0.0f,-1.0f);
        glVertex3f(-dx,-dy,-_size*2.0f);
        glVertex3f(-dx,dy,-_size*2.0f);
        glVertex3f(dx,dy,-_size*2.0f);
        glVertex3f(dx,-dy,-_size*2.0f);
        glNormal3f(1.0f,0.0f,0.0f);
        glVertex3f(dx,-dy,-_size*2.0f);
        glVertex3f(dx,dy,-_size*2.0f);
        glVertex3f(dx,dy,0.0f);
        glVertex3f(dx,-dy,0.0f);
        glNormal3f(0.0f,-1.0f,0.0f);
        glVertex3f(-dx,-dy,0.0f);
        glVertex3f(-dx,-dy,-_size*2.0f);
        glVertex3f(dx,-dy,-_size*2.0f);
        glVertex3f(dx,-dy,0.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(-dx,dy,0.0f);
        glVertex3f(dx,dy,0.0f);
        glVertex3f(dx,dy,-_size*2.0f);
        glVertex3f(-dx,dy,-_size*2.0f);
        ogl::setMaterialColor(ogl::colorBlack,ogl::colorBlack,ogl::colorBlack);
        glNormal3f(0.0f,0.0f,1.0f);
        glVertex3f(dx,-dy,0.0f);
        glVertex3f(dx,dy,0.0f);
        glVertex3f(-dx,dy,0.0f);
        glVertex3f(-dx,-dy,0.0f);
        glEnd();
        glPopMatrix();

        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(visionSensor,renderingObject);
}

#else

void displayVisionSensor(CVisionSensor* visionSensor,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



