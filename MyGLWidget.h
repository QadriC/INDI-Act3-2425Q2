#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"
#include "A3GLWidget.h"


class MyGLWidget : public A3GLWidget
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:

    void iniEscena () override;
    void carregaUniformLocations () override;
    virtual void keyPressEvent (QKeyEvent *event) override;
    void paintGL() override;
    void initializeGL() override;
    void actualitzaPosFocusSub();

    void iniLight();

    GLuint llumAmbientLoc, colorFocusLoc, posFocusLoc;
    GLuint posFocusSubLoc[NUM_FOCUS], colorFocusSubLoc[NUM_FOCUS];
    GLint enabledLoc[NUM_FOCUS], pintarVidresLoc, pintarMarLoc;
    GLuint posAntenaLoc;
    glm::vec3 posAntenaSub, colFocusSub[NUM_FOCUS];
    bool enabled[NUM_FOCUS];
    int pintarVidresSub, pintarMarM;

  public slots:
    void setIntensitatLlum(int value);
    void setRColorFocus1(int value);
    void setGColorFocus1(int value);
    void setBColorFocus1(int value);
    void setRColorFocus2(int value);
    void setGColorFocus2(int value);
    void setBColorFocus2(int value);
    void setRColorFocus3(int value);
    void setGColorFocus3(int value);
    void setBColorFocus3(int value);
    void setRColorFocus4(int value);
    void setGColorFocus4(int value);
    void setBColorFocus4(int value);
    void toggleFocus1();
    void toggleFocus2();
    void toggleFocus3();
    void toggleFocus4();
};

