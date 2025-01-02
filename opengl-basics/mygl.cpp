#include "mygl.h"
#include <QDir>
#include <string.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

MyGL::MyGL(QWidget *parent)
    : QOpenGLWidget(parent),
      timer(), currTime(0.f),
      vao(),
      vertShader(), fragShader(), shaderProgram(),
      shaderAttribVariableHandles(), shaderUniformVariableHandles(),
      bufferPosition(), bufferIndex(),
      indexBufferLength(-1)
{
    // Allow Qt to trigger mouse events
    // even when a mouse button is not held.
    setMouseTracking(true);

    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
}


MyGL::~MyGL(){}

// As noted in mygl.h, use this function in order to get
// the direct file path to this Qt project so that you
// can read the contents of your GLSL files.
QString MyGL::getCurrentPath() const {
    QString path = QDir::currentPath();
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
#ifdef __APPLE__
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
#endif
    return path;
}

// A helper function for createAndCompileShaderProgram.
// It reads the contents of a file into a char*.
char* textFileRead(const char* fileName) {
    char* text = nullptr;

    if (fileName != nullptr) {
        FILE *file = fopen(fileName, "rt");

        if (file != nullptr) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol
            }
            fclose(file);
        }
    }
    return text;
}

void MyGL::createAndCompileShaderProgram(GLuint &vertHandle,
                              GLuint &fragHandle,
                              GLuint &shaderProgramHandle,
                              QString vertFile,
                              QString fragFile) {

    // Set up the direct filepath to the vertex and fragment
    // shader files.
    QString projectPath = getCurrentPath();
    projectPath.append("/glsl/");
    QString vertPath = projectPath + vertFile;
    QString fragPath = projectPath + fragFile;

    // Make the OpenGL function calls that create a
    // vertex shader object, fragment shader object,
    // and shader program object on the GPU. We store
    // CPU-side handles to these GPU-side objects in
    // the GLuints passed into this function.
    vertHandle = glCreateShader(GL_VERTEX_SHADER);
    fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgramHandle = glCreateProgram();

    // Parse the plain-text contents of the vertex
    // and fragment shader files, storing them in C-style
    // char* strings (how OpenGL expects strings to be formatted).
    char *vertexShaderSource = textFileRead(vertPath.toStdString().c_str());
    char *fragmentShaderSource = textFileRead(fragPath.toStdString().c_str());

    // Send the contents of the shader files to the GPU
    glShaderSource(vertHandle, 1, &vertexShaderSource, 0);
    glShaderSource(fragHandle, 1, &fragmentShaderSource, 0);
    // Manually de-allocate the heap memory used to store the
    // shader contents. We don't need it now that it's been sent
    // to the GPU.
    free(vertexShaderSource);
    free(fragmentShaderSource);

    // Tell OpenGL on the GPU to try compiling the shader code
    // we just sent.
    glCompileShader(vertHandle);
    glCompileShader(fragHandle);
    // Check if everything compiled OK. If not, print out
    // any errors sent to us from the GPU-side shader compiler.
    GLint compiled;
    glGetShaderiv(vertHandle, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        std::cout << "Errors from " << vertFile.toStdString() << ":" << std::endl;
        printShaderInfoLog(vertHandle);
    }
    glGetShaderiv(fragHandle, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        std::cout << "Errors from " << fragFile.toStdString() << ":" << std::endl;
        printShaderInfoLog(fragHandle);
    }

    // Tell shaderProgramHandle that it manages
    // these particular vertex and fragment shaders
    glAttachShader(shaderProgramHandle, vertHandle);
    glAttachShader(shaderProgramHandle, fragHandle);
    glLinkProgram(shaderProgramHandle);

    // Check for linking success
    GLint linked;
    glGetProgramiv(shaderProgramHandle, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(shaderProgramHandle);
    }
}

void MyGL::getHandlesForShaderVariables(GLuint shaderProgramHandle) {
    glUseProgram(shaderProgramHandle);

    GLint posHandle = glGetAttribLocation(shaderProgramHandle, "vs_pos");
    shaderAttribVariableHandles["vs_pos"] = posHandle;

    GLint timeHandle = glGetUniformLocation(shaderProgramHandle, "u_Time");
    shaderUniformVariableHandles["u_Time"] = timeHandle;

    GLint modelHandle = glGetUniformLocation(shaderProgramHandle, "u_Model");
    shaderUniformVariableHandles["u_Model"] = modelHandle;

    // GLint screenHandle = glGetUniformLocation(shaderProgramHandle, "u_ScreenDimensions");
    // shaderUniformVariableHandles["u_ScreenDimensions"] = screenHandle;



    // TODO: Write code that gets the handle IDs for each "in"
    // and "uniform" variable used in your shader program,
    // and put them in shaderAttribVariableHandles and
    // shaderUniformVariableHandles. The key for each ID handle
    // should be the string representation of its name, e.g.
    // "vs_Pos" or "u_ScreenDimensions", etc.
    // You will need to make use of the following OpenGL API calls:
    // - glGetUniformLocation
    // - glGetAttribLocation
}

void MyGL::initializeAndBufferGeometryData() {
    // TODO: Populate these two std::vectors
    // with data representing the vertex positions
    // of your shape, and the indices that will
    // triangulate that shape using the fan method.

    std::vector<glm::vec3> pos;
    std::vector<GLuint> idx;

    //create vertes position
    int polygonSize = 36;
    pos.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    float radius = 0.5f;
    float perRadians = 2*M_PI/polygonSize;

    for(int i=0; i< polygonSize; i++){
        float radians = i * perRadians;
        float x = radius * cos(radians);
        float y = radius * sin(radians);

        pos.push_back(glm::vec3(x, y, 1.0f));
    }

    //array vertex index to triangles
    for(int i =1; i< polygonSize; i++){
        idx.push_back(0);
        idx.push_back(i);
        idx.push_back(i+1);
    }

    //add the last triangle
    idx.push_back(0);
    idx.push_back(polygonSize);
    idx.push_back(1);



    // Your code here

    // Set indexBufferLength to the total number of
    // indices used to draw your polygon so that it
    // can be passed to glDrawElements in the
    // drawGeometry function.

    indexBufferLength = idx.size();
    //VBO
    glGenBuffers(1, &bufferPosition); //store all vertex position
    glBindBuffer(GL_ARRAY_BUFFER, bufferPosition); //tell space use to store bufferposition
    //glBufferData(GL_ARRAY_BUFFER, sizeInBytes, dataPointer, usageFlag);
    glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(glm::vec3),pos.data(), GL_STATIC_DRAW);

    //EBO
    glGenBuffers(1, &bufferIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(GLuint), idx.data(), GL_STATIC_DRAW);


    // TODO: Use glGenBuffers, glBindBuffer, and glBufferData
    // to set up the GPU-side storage for your geometry data
    // via bufferPosition and bufferIndex.
    // Don't forget to use the GL_ARRAY_BUFFER flag for bufferPosition,
    // but use GL_ELEMENT_ARRAY_BUFFER for bufferIndex since it represents
    // triangle indices rather than mesh vertices.



}

void MyGL::initializeGL() {
    // Create an OpenGL context using Qt's
    // QOpenGLFunctions_3_2_Core class.
    // If you were programming in a non-Qt context
    // you might use GLEW (GL Extension Wrangler)
    // or GLFW (Graphics Library Framework) instead.
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();
    // Create a Vertex Array Object so that we can render
    // our geometry using Vertex Buffer Objects.
    glGenVertexArrays(1, &vao);
    // We have to have a VAO bound in OpenGL 3.2 Core.
    // But if we're not using multiple VAOs, we
    // can just bind one once.
    glBindVertexArray(vao);

    createAndCompileShaderProgram(this->vertShader, this->fragShader, this->shaderProgram,
                       "passthrough.vert.glsl", "coloring.frag.glsl");
    printGLErrorLog();
    getHandlesForShaderVariables(this->shaderProgram);
    printGLErrorLog();

    initializeAndBufferGeometryData();
    printGLErrorLog();
}

void MyGL::resizeGL(int w, int h) {
    w *= this->devicePixelRatio();
    h *= this->devicePixelRatio();

    // TODO: Add code here to pass your screen dimensions to
    // your vertex shader's uniform variable that stores that information.
    // You'll need to use the glUniform2i OpenGL API function.
    glUseProgram(shaderProgram);
    // GLint screenHandle = glGetUniformLocation(shaderProgram, "u_ScreenDimensions");
    // shaderUniformVariableHandles["u_ScreenDimensions"] = screenHandle;
    GLint screenDimensionHandle = shaderUniformVariableHandles["u_ScreenDimensions"];
    glUniform2i(screenDimensionHandle, w, h);
}

void MyGL::drawGeometry() {
    glUseProgram(shaderProgram);

    glBindBuffer(GL_ARRAY_BUFFER, bufferPosition);

    glEnableVertexAttribArray(shaderAttribVariableHandles["vs_Pos"]);
    glVertexAttribPointer(shaderAttribVariableHandles["vs_Pos"], 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glDrawElements(GL_TRIANGLES, indexBufferLength, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(shaderAttribVariableHandles["vs_Pos"]);


    // TODO: Use the functions listed below to
    // associate bufferPosition with the shader
    // "in" variable named "vs_Pos", and tell
    // OpenGL that every three consecutive floats
    // in the buffer represent one vec3.
    // - glBindBuffer
    // - glEnableVertexAttribArray
    // - glVertexAttribPointer
    // TODO: Use the functions listed below to
    // draw your mesh using the triangle indices stored
    // in bufferIndex.
    // - glBindBuffer
    // - glDrawElements
    // TODO: Use glDisableVertexAttribArray
    // after drawing your geometry to "lock"
    // the GPU-side variable named vs_Pos.
}

void MyGL::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawGeometry();
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    // Extra credit: Pass the mouse event's position
    // as an X and Y coordinate to the shader program's
    // mouse position uniform variable. Then, use the mouse
    // position to affect your scene in some way.
    // Don't forget to call glUseProgram first!
    glm::ivec2 pos(e->pos().x(), e->pos().y());


    update();
}

void MyGL::tick() {
    // TODO: Add code to update your shader's
    // u_Time variable.

    // Do not delete this function call! This
    // repaints the GL viewport every time the timer
    // invokes tick().
    update();
}
