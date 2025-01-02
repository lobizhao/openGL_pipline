# OpenGL Pipeline

## Vertex data
- OpenGL connection 

        initializeOpenGLFunctions();
- Compiling shader code
- Assemble VBO data

    - VBO/ EBO/ CBO

            //tell OpenGL to give us a handle to a VBO it create on the GPU
            glGenBuffer(GLsizei n, GLunit *buffers);

            //tell OpenGL that we want to put data into the buffer called vbo
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            //glBufferData requires a pointer to the first element of the float array
            glBufferData(GL_ARRAY_BUFFER, N*sizeof(vec3), vertices.data(), GL_STATIC_DRAW);

## Vertex Shader


        in per vertex local info: pos v_color uv normal..

        uniform 

        out 
## Tessellation Control
## Geometry Shader
## Rasterization
## Fragment Shader
## Framebuffer




