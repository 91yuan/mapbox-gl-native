#include <llmr/renderer/shader.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdlib>
#include <cstdio>

using namespace llmr;

Shader::Shader(const GLchar *vertSource, const GLchar *fragSource)
    : valid(false),
      program(0) {

    GLuint vertShader;
    if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertSource)) {
        fprintf(stderr, "Vertex shader failed to compile: %s\n", vertSource);
        return;
    }

    GLuint fragShader;
    if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragSource)) {
        fprintf(stderr, "Fragment shader failed to compile: %s\n", fragSource);
        return;
    }

    program = glCreateProgram();

    // Attach shaders
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);


    {
        // Link program
        GLint status;
        glLinkProgram(program);

#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, &logLength, log);
            fprintf(stderr, "Program link log:\n%s", log);
            free(log);
        }
#endif

        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == 0) {
            fprintf(stderr, "Program failed to link\n");
            glDeleteShader(vertShader);
            vertShader = 0;
            glDeleteShader(fragShader);
            fragShader = 0;
            glDeleteProgram(program);
            program = 0;
            return;
        }
    }

    {
        // Validate program
        GLint status;
        glValidateProgram(program);

#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, &logLength, log);
            fprintf(stderr, "Program validate log:\n%s", log);
            free(log);
        }
#endif

        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        if (status == 0) {
            fprintf(stderr, "Program failed to validate\n");
            glDeleteShader(vertShader);
            vertShader = 0;
            glDeleteShader(fragShader);
            fragShader = 0;
            glDeleteProgram(program);
            program = 0;
        }

    }

    // Remove the compiled shaders; they are now part of the program.
    glDetachShader(program, vertShader);
    glDeleteShader(vertShader);
    glDetachShader(program, fragShader);
    glDeleteShader(fragShader);


    fprintf(stderr, "successfully compiled shader\n");
    valid = true;
}


bool Shader::compileShader(GLuint *shader, GLenum type, const GLchar *source) {
    GLint status;

    *shader = glCreateShader(type);

#ifdef EMSCRIPTEN
    // Add WebGL GSLSL precision premable
    const char *preamble = "precision mediump float;\n\n";
    int preamble_length = strlen(preamble);
    int source_length = strlen(source);
    char *modified_source = (char *)malloc(preamble_length + source_length);
    strncpy(&modified_source[0], preamble, preamble_length);
    strncpy(&modified_source[preamble_length], source, source_length);
    glShaderSource(*shader, 1, (const GLchar **)(&modified_source), NULL);
#else
    glShaderSource(*shader, 1, &source, NULL);
#endif

    glCompileShader(*shader);

// #if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        fprintf(stderr, "Shader compile log:\n%s", log);
        free(log);
    }
// #endif

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        *shader = 0;
        return false;
    }

    return true;
}

Shader::~Shader() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
        valid = false;
    }
}
