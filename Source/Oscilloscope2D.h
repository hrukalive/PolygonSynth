#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "RingBuffer.h"

class Oscilloscope2D :  public Component,
                        public OpenGLRenderer
{
    
public:
    
    Oscilloscope2D (std::shared_ptr<RingBuffer<GLfloat>>& ringBuffer)
    : ringBuffer(ringBuffer)
    {
        openGLContext.setOpenGLVersionRequired (OpenGLContext::OpenGLVersion::openGL3_2);
        
        openGLContext.setRenderer(this);
        openGLContext.attachTo(*this);
    }
    
    ~Oscilloscope2D()
    {
        openGLContext.setContinuousRepainting (false);
        openGLContext.detach();
        
        ringBuffer = nullptr;
        delete[] vertices;
    }
    
    void start()
    {
        openGLContext.setContinuousRepainting (true);
    }
    
    void stop()
    {
        openGLContext.setContinuousRepainting (false);
    }
    
    
    //==========================================================================
    // OpenGL Callbacks
    
    void newOpenGLContextCreated() override
    {
        // Setup Shaders
        createShaders();
        
        // Setup Buffer Objects
        openGLContext.extensions.glGenBuffers (1, &VBO); // Vertex Buffer Object
        openGLContext.extensions.glGenBuffers (1, &EBO); // Element Buffer Object
    }
    
    void openGLContextClosing() override
    {
        shader->release();
        shader = nullptr;
    }
    
    
    /** The OpenGL rendering callback.
     */
    void renderOpenGL() override
    {
        jassert (OpenGLHelpers::isContextActive());
        const int readSize = blockSize.get();
        
        // Setup Viewport
        const float renderingScale = (float) openGLContext.getRenderingScale();
        glViewport (0, 0, roundToInt (renderingScale * getWidth()), roundToInt (renderingScale * getHeight()));
        
        // Set background Color
        OpenGLHelpers::clear (Colours::black);
        
        // Enable Alpha Blending
        glEnable (GL_BLEND);
        glEnable(GL_POINT_SMOOTH);
        glPointSize(5.0);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Use Shader Program that's been defined
        shader->use();
        
        // Read in samples from ring buffer
        ringBuffer->readSamples (readBuffer, readSize);
        auto xPtr = readBuffer.getReadPointer(0, 0), yPtr = readBuffer.getReadPointer(1, 0);
        for (int i = 0; i < readSize; i++)
        {
            vertices[i * 2] = xPtr[i];
            vertices[i * 2 + 1] = yPtr[i];
        }
        
        // VBO (Vertex Buffer Object) - Bind and Write to Buffer
        openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, VBO);
        openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER, readSize * 2 * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
        
        // Setup Vertex Attributes
        openGLContext.extensions.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        openGLContext.extensions.glEnableVertexAttribArray (0);
    
        // Draw Vertices
        glDrawArrays(GL_POINTS, 0, readSize);


        // Reset the element buffers so child Components draw correctly
        openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
        openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    
    //==========================================================================
    // JUCE Callbacks
    
    void paint (Graphics& g) override
    {
    }
    
    void resized () override
    {
    }

    void updateBlocksize(float bs)
    {
        stop();
        blockSize.set(bs);
        if (vertices != nullptr)
            delete[] vertices;
        vertices = new GLfloat[blockSize.get() * 2];
        readBuffer = AudioBuffer<GLfloat>(2, blockSize.get());
    }
    
private:
    
    //==========================================================================
    // OpenGL Functions
    
    
    /** Loads the OpenGL Shaders and sets up the whole ShaderProgram
    */
    void createShaders()
    {
        vertexShader =
        "attribute vec2 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "}\n";
        
        fragmentShader =
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(1, 1, 0, 1);\n"
        "}\n";
        
        ScopedPointer<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
        String statusText;
        
        if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
            && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
            && newShader->link())
        {
            shader = newShader;
            shader->use();
            
            statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
        }
        else
        {
            statusText = newShader->getLastError();
            AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon, "Shader Error", statusText, "OK");
        }
    }
    

    OpenGLContext openGLContext;
    GLuint VBO, VAO, EBO;

    Atomic<int> blockSize{ 256 };
    GLfloat* vertices{ nullptr };
    
    ScopedPointer<OpenGLShaderProgram> shader;
    
    const char* vertexShader;
    const char* fragmentShader;

    std::shared_ptr<RingBuffer<GLfloat>> ringBuffer;
    AudioBuffer<GLfloat> readBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscilloscope2D)
};
