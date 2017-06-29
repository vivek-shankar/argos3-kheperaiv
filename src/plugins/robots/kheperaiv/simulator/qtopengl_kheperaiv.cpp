/**
 * @file <argos3/plugins/robots/kheperaiv/simulator/qtopengl_kheperaiv.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "qtopengl_kheperaiv.h"
#include "kheperaiv_entity.h"
#include "kheperaiv_measures.h"
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/plugins/simulator/entities/led_equipped_entity.h>
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_widget.h>

namespace argos {

   /****************************************/
   /****************************************/

   CQTOpenGLKheperaIV::CQTOpenGLKheperaIV() :
      m_unVertices(40) {
      /* Reserve the needed display lists */
      m_unLists = glGenLists(2);

      /* Assign indices for better referencing (later) */
      m_unBaseList    = m_unLists;
      m_unLEDList     = m_unLists + 1;

      /* Create the base display list */
      glNewList(m_unBaseList, GL_COMPILE);
      RenderBase();
      glEndList();

      /* Create the LED display list */
      glNewList(m_unLEDList, GL_COMPILE);
      RenderLED();
      glEndList();
   }

   /****************************************/
   /****************************************/

   CQTOpenGLKheperaIV::~CQTOpenGLKheperaIV() {
      glDeleteLists(m_unLists, 2);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLKheperaIV::Draw(CKheperaIVEntity& c_entity) {
      /* Place the body */
      glCallList(m_unBaseList);
      /* Place the LEDs */
      glPushMatrix();
      CLEDEquippedEntity& cLEDEquippedEntity = c_entity.GetLEDEquippedEntity();
      for(UInt32 i = 0; i < 3; i++) {
         const CColor& cColor = cLEDEquippedEntity.GetLED(i).GetColor();
         const CVector3& cOffset = cLEDEquippedEntity.GetLEDOffset(i);
         SetLEDMaterial(cColor.GetRed(), cColor.GetGreen(), cColor.GetBlue());
         glTranslatef(cOffset.GetX(), cOffset.GetY(), cOffset.GetZ());
         glCallList(m_unLEDList);
      }
      glPopMatrix();
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLKheperaIV::SetWhitePlasticMaterial() {
      const GLfloat pfColor[]     = {   1.0f, 1.0f, 1.0f, 1.0f };
      const GLfloat pfSpecular[]  = {   0.9f, 0.9f, 0.9f, 1.0f };
      const GLfloat pfShininess[] = { 100.0f                   };
      const GLfloat pfEmission[]  = {   0.0f, 0.0f, 0.0f, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLKheperaIV::SetBluePlasticMaterial() {
      const GLfloat pfColor[]     = {   0.0f, 0.0f, 1.0f, 1.0f };
      const GLfloat pfSpecular[]  = {   0.9f, 0.9f, 0.9f, 1.0f };
      const GLfloat pfShininess[] = { 100.0f                   };
      const GLfloat pfEmission[]  = {   0.0f, 0.0f, 0.0f, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLKheperaIV::SetLEDMaterial(GLfloat f_red, GLfloat f_green, GLfloat f_blue) {
      const GLfloat fEmissionFactor = 10.0f;
      const GLfloat pfColor[]     = {                   f_red,                   f_green,                   f_blue, 1.0f };
      const GLfloat pfSpecular[]  = {                    0.0f,                      0.0f,                     0.0f, 1.0f };
      const GLfloat pfShininess[] = {                    0.0f                                                            };
      const GLfloat pfEmission[]  = { f_red * fEmissionFactor, f_green * fEmissionFactor, f_blue * fEmissionFactor, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLKheperaIV::RenderBase() {
      SetBluePlasticMaterial();
      /* Used to precalculate the rotation */
      const CVector2 cRot(1.0, -CRadians::TWO_PI / m_unVertices);
      /* Bottom face is a disk */
      glBegin(GL_TRIANGLE_FAN);
      glNormal3f(0.0, 0.0, -1.0);
      glVertex3f(0.0, 0.0, 0.0);
      CVector2 cVert(KHEPERAIV_BASE_RADIUS, 0.0);
      glVertex3f(cVert.GetX(), cVert.GetY(), 0.0);
      for(GLuint i = 1; i < m_unVertices; ++i) {
         cVert.Set(
            cVert.GetX() * cRot.GetX() - cVert.GetY() * cRot.GetY(),
            cVert.GetX() * cRot.GetY() + cVert.GetY() * cRot.GetX());
         glVertex3f(cVert.GetX(), cVert.GetY(), 0.0);
      }
      glEnd();
      /* Top face is a disk */
      glBegin(GL_TRIANGLE_FAN);
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(0.0, 0.0, KHEPERAIV_BASE_TOP);
      cVert.Set(KHEPERAIV_BASE_RADIUS, 0.0);
      glVertex3f(cVert.GetX(), cVert.GetY(), KHEPERAIV_BASE_TOP);
      for(GLuint i = 1; i < m_unVertices; ++i) {
         cVert.Set(
            cVert.GetX() * cRot.GetX() - cVert.GetY() * cRot.GetY(),
            cVert.GetX() * cRot.GetY() + cVert.GetY() * cRot.GetX());
         glVertex3f(cVert.GetX(), cVert.GetY(), KHEPERAIV_BASE_TOP);
      }
      glEnd();
      /* Side face is a cylinder */
      glBegin(GL_QUAD_STRIP);
      cVert.Set(1.0, 0.0);
      glNormal3f(cVert.GetX(), cVert.GetY(), 0.0);
      glVertex3f(KHEPERAIV_BASE_RADIUS * cVert.GetX(), KHEPERAIV_BASE_RADIUS * cVert.GetY(), 0.0);
      glVertex3f(KHEPERAIV_BASE_RADIUS * cVert.GetX(), KHEPERAIV_BASE_RADIUS * cVert.GetY(), KHEPERAIV_BASE_TOP);
      for(GLuint i = 1; i < m_unVertices; ++i) {
         cVert.Set(
            cVert.GetX() * cRot.GetX() - cVert.GetY() * cRot.GetY(),
            cVert.GetX() * cRot.GetY() + cVert.GetY() * cRot.GetX());
         glNormal3f(cVert.GetX(), cVert.GetY(), 0.0);
         glVertex3f(KHEPERAIV_BASE_RADIUS * cVert.GetX(), KHEPERAIV_BASE_RADIUS * cVert.GetY(), 0.0);
         glVertex3f(KHEPERAIV_BASE_RADIUS * cVert.GetX(), KHEPERAIV_BASE_RADIUS * cVert.GetY(), KHEPERAIV_BASE_TOP);
      }
      glEnd();
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLKheperaIV::RenderLED() {
      /* Draw it as a tiny pyramid pointing to Z upwards
         (with no base, cause it's not visible anyway) */
      glBegin(GL_TRIANGLE_FAN);
      /* Top */
      glVertex3f( 0.00,  0.00, 0.01);
      /* North */
      glVertex3f( 0.01,  0.00, 0.00);
      /* West */
      glVertex3f( 0.00, -0.01, 0.00);
      /* South */
      glVertex3f(-0.01,  0.00, 0.00);
      /* East */
      glVertex3f( 0.00,  0.01, 0.00);
      glEnd();
   }

   /****************************************/
   /****************************************/

   class CQTOpenGLOperationDrawKheperaIVNormal : public CQTOpenGLOperationDrawNormal {
   public:
      void ApplyTo(CQTOpenGLWidget& c_visualization,
                   CKheperaIVEntity& c_entity) {
         static CQTOpenGLKheperaIV m_cModel;
         c_visualization.DrawRays(c_entity.GetControllableEntity());
         c_visualization.DrawEntity(c_entity.GetEmbodiedEntity());
         m_cModel.Draw(c_entity);
      }
   };

   class CQTOpenGLOperationDrawKheperaIVSelected : public CQTOpenGLOperationDrawSelected {
   public:
      void ApplyTo(CQTOpenGLWidget& c_visualization,
                   CKheperaIVEntity& c_entity) {
         c_visualization.DrawBoundingBox(c_entity.GetEmbodiedEntity());
      }
   };

   REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawNormal, CQTOpenGLOperationDrawKheperaIVNormal, CKheperaIVEntity);

   REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawSelected, CQTOpenGLOperationDrawKheperaIVSelected, CKheperaIVEntity);

   /****************************************/
   /****************************************/

}