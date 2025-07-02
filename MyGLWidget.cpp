#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : A3GLWidget(parent)
{
}

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::iniLight() {
    glm::vec3 llumAmbient = glm::vec3(0.1, 0.1, 0.1);
    glUniform3fv(llumAmbientLoc, 1, &llumAmbient[0]);
    glm::vec3 colorFocus = glm::vec3(0.1, 0.6, 0.5);
    glUniform3fv(colorFocusLoc, 1, &colorFocus[0]);
    glm::vec3 posFocusEscena = glm::vec3(0, 50, 0);
    glUniform3fv(posFocusLoc, 1, &posFocusEscena[0]);

    posFocus[0] = glm::vec3( 0.44, 0.85, 3.47);
    posFocus[1] = glm::vec3( 0.88, 0.85, 2.83);
    posFocus[2] = glm::vec3(-0.95, 0.85, 2.83);
    posFocus[3] = glm::vec3(-0.45, 0.85, 3.47);
    posAntenaSub = glm::vec3(0.0, 2.44, 1.37);
    for(int i = 0; i < 4; ++i) {
        colFocusSub[i] = glm::vec3(1.0, 1.0, 1.0);
        glm::vec3 res = colFocusSub[i];
        glUniform3fv(colorFocusSubLoc[i], 1, &res[0]);
        enabled[i] = true;
        glUniform1i(enabledLoc[i], 1);
    }
    pintarMarM = 0;
}

void MyGLWidget::iniEscena ()
{
    creaBuffersModels();
    creaBuffersQuadrat(glm::vec3(1.0,1.0,1.0),
                       &VAO_Cub_Debug, 0.5f);
    //--------------------------------------------------
    centreEsc = glm::vec3 (0, 2, 0);
    radiEsc = 10;
    //--------------------------------------------------
    SubmarinePos = glm::vec3(1.0, 4.0, 2.0);
    A3GLWidget::modelTransformSubmarine();
    iniLight();
    actualitzaPosFocusSub();
}

void MyGLWidget::setIntensitatLlum(int value) {
    makeCurrent(); // activar context OpenGL
    float f = value / 100.0f;  // convertir de 0-100 a 0.0-1.0

    glm::vec3 colBase(0.1, 0.6, 0.5);
    glm::vec3 colEscena = f * colBase;

    glUniform3fv(colorFocusLoc, 1, &colEscena[0]);

    update();
}

void MyGLWidget::actualitzaPosFocusSub() {
    glm::vec4 pos = Submarine_TG * glm::vec4(posAntenaSub, 1.0);
    glm::vec3 posFinal = glm::vec3(pos);
    glUniform3fv(posAntenaLoc, 1, &posFinal[0]);

    for (int i = 0; i < NUM_FOCUS; ++i) {
        glm::vec4 pos = Submarine_TG * glm::vec4(posFocus[i], 1.0);
        glm::vec3 posFinal = glm::vec3(pos);
        glUniform3fv(posFocusSubLoc[i], 1, &posFinal[0]);
    }
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
    makeCurrent();
    float step = 0.2;
    switch (event->key()) {
        case Qt::Key_W:
            SubmarinePos.z -= step;
            break;
        case Qt::Key_S:
            SubmarinePos.z += step;
            break;
        case Qt::Key_A:
            SubmarinePos.x -= step;
            break;
        case Qt::Key_D:
            SubmarinePos.x += step;
            break;
        case Qt::Key_Up:
            SubmarinePos.y += step;
            break;
        case Qt::Key_Down:
            SubmarinePos.y -= step;
            break;
        default: event->ignore(); break;
    }
    A3GLWidget::modelTransformSubmarine();
    actualitzaPosFocusSub();
    update();
}

void MyGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // sòlids
    glBlendFunc(GL_ONE, GL_ZERO);
    pintarVidresSub = 0;
    glUniform1i(pintarVidresLoc, pintarVidresSub);
    pintarMarM = 0;
    glUniform1i(pintarMarLoc, pintarMarM);
    A3GLWidget::paintGL(); // dibuixa tota l'escena, però el fragment shader descarta finestres

    // només VAO submarí amb vidres translúcids
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pintarVidresSub = 1;
    glUniform1i(pintarVidresLoc, pintarVidresSub);
    glBindVertexArray (VAO_models[SUBMARINE]);
    modelTransformSubmarine();
    glDrawArrays(GL_TRIANGLES, 0, models[SUBMARINE].faces().size()*3);
    glBindVertexArray(0);

    // Mar
    pintarVidresSub = 0;
    pintarMarM = 1;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i(pintarVidresLoc, pintarVidresSub);
    glUniform1i(pintarMarLoc, pintarMarM);
    glBindVertexArray(VAO_models[SEA]);
    modelTransformSea();
    glDrawArrays(GL_TRIANGLES, 0, models[SEA].faces().size() * 3);
    glBindVertexArray(0);
}

void MyGLWidget::initializeGL() {
    pintarVidresSub = 0;
    A3GLWidget::initializeGL();
}

void MyGLWidget::carregaUniformLocations(){

    // Aquí podeu afegir tot el codi relatiu a
    // la inicialització dels uniforms locators: glGetUniformLocation....
    // sense necessitat de sobreescriure la funció A3GLWidget::carregaShaders()
    // sencera.
    llumAmbientLoc = glGetUniformLocation(program->programId(), "llumAmbient");
    colorFocusLoc = glGetUniformLocation(program->programId(), "colorFocus");
    posFocusLoc = glGetUniformLocation(program->programId(), "posFocus");
    posAntenaLoc = glGetUniformLocation(program->programId(), "posAntena");
    pintarVidresLoc = glGetUniformLocation(program->programId(), "pintarVidres");
    pintarMarLoc = glGetUniformLocation(program->programId(), "pintarMar");
    for (int i = 0; i < NUM_FOCUS; ++i) {
        std::string name = "posFocusSub[" + std::to_string(i) + "]";
        posFocusSubLoc[i] = glGetUniformLocation(program->programId(), name.c_str());

        name = "colorFocusSub[" + std::to_string(i) + "]";
        colorFocusSubLoc[i] = glGetUniformLocation(program->programId(), name.c_str());

        name = "enabledFocus[" + std::to_string(i) + "]";
        enabledLoc[i] = glGetUniformLocation(program->programId(), name.c_str());
    }
}

void MyGLWidget::setRColorFocus1(int value) {
    makeCurrent();
    colFocusSub[0].x = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[0];
    glUniform3fv(colorFocusSubLoc[0], 1, &res[0]);
    update();
}

void MyGLWidget::setGColorFocus1(int value) {
    makeCurrent();
    colFocusSub[0].y = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[0];
    glUniform3fv(colorFocusSubLoc[0], 1, &res[0]);
    update();
}

void MyGLWidget::setBColorFocus1(int value) {
    makeCurrent();
    colFocusSub[0].z = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[0];
    glUniform3fv(colorFocusSubLoc[0], 1, &res[0]);
    update();
}

void MyGLWidget::setRColorFocus2(int value) {
    makeCurrent();
    colFocusSub[1].x = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[1];
    glUniform3fv(colorFocusSubLoc[1], 1, &res[0]);
    update();
}

void MyGLWidget::setGColorFocus2(int value) {
    makeCurrent();
    colFocusSub[1].y = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[1];
    glUniform3fv(colorFocusSubLoc[1], 1, &res[0]);
    update();
}

void MyGLWidget::setBColorFocus2(int value) {
    makeCurrent();
    colFocusSub[1].z = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[1];
    glUniform3fv(colorFocusSubLoc[1], 1, &res[0]);
    update();
}

void MyGLWidget::setRColorFocus3(int value) {
    makeCurrent();
    colFocusSub[2].x = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[2];
    glUniform3fv(colorFocusSubLoc[2], 1, &res[0]);
    update();
}

void MyGLWidget::setGColorFocus3(int value) {
    makeCurrent();
    colFocusSub[2].y = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[2];
    glUniform3fv(colorFocusSubLoc[2], 1, &res[0]);
    update();
}

void MyGLWidget::setBColorFocus3(int value) {
    makeCurrent();
    colFocusSub[2].z = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[2];
    glUniform3fv(colorFocusSubLoc[2], 1, &res[0]);
    update();
}

void MyGLWidget::setRColorFocus4(int value) {
    makeCurrent();
    colFocusSub[3].x = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[3];
    glUniform3fv(colorFocusSubLoc[3], 1, &res[0]);
    update();
}

void MyGLWidget::setGColorFocus4(int value) {
    makeCurrent();
    colFocusSub[3].y = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[3];
    glUniform3fv(colorFocusSubLoc[3], 1, &res[0]);
    update();
}

void MyGLWidget::setBColorFocus4(int value) {
    makeCurrent();
    colFocusSub[3].z = float(value) / 100.0f;
    glm::vec3 res = colFocusSub[3];
    glUniform3fv(colorFocusSubLoc[3], 1, &res[0]);
    update();
}

void MyGLWidget::toggleFocus1() {
    makeCurrent();
    if(enabled[0] == true)
        enabled[0] = false;
    else
        enabled[0] = true;
    glUniform1i(enabledLoc[0], enabled[0]);
    update();
}

void MyGLWidget::toggleFocus2() {
    makeCurrent();
    if(enabled[1] == true)
        enabled[1] = false;
    else
        enabled[1] = true;
    glUniform1i(enabledLoc[1], enabled[1]);
    update();
}

void MyGLWidget::toggleFocus3() {
    makeCurrent();
    if(enabled[2] == true)
        enabled[2] = false;
    else
        enabled[2] = true;
    glUniform1i(enabledLoc[2], enabled[2]);
    update();
}

void MyGLWidget::toggleFocus4() {
    makeCurrent();
    if(enabled[3] == true)
        enabled[3] = false;
    else
        enabled[3] = true;
    glUniform1i(enabledLoc[3], enabled[3]);
    update();
}
