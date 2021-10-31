#include "ext/GLShader.h"

namespace glib
{

    GLShader::GLShader(File vertexShaderFile, File fragmentShaderFile)
    {
        std::vector<unsigned char> vData, fData;

        SimpleFile f = SimpleFile(vertexShaderFile, SimpleFile::READ | SimpleFile::ASCII);
        if(f.isOpen())
        {
            vData = f.readFullFileAsBytes();
            f.close();
        }
        else
        {
            //ERROR LOADING VERTEX SHADER
            StringTools::println("ERROR LOADING VERTEX SHADER - %ls", vertexShaderFile.getFullFileName().c_str());
            valid = false;
        }

        SimpleFile f2 = SimpleFile(fragmentShaderFile, SimpleFile::READ | SimpleFile::ASCII);
        if(f2.isOpen())
        {
            fData = f2.readFullFileAsBytes();
            f2.close();
        }
        else
        {
            //ERROR LOADING FRAGMENT SHADER
            StringTools::println("ERROR LOADING FRAGMENT SHADER - %ls", fragmentShaderFile.getFullFileName().c_str());
            valid = false;
        }

        vData.push_back(0);
        fData.push_back(0);

        StringTools::println("SIZES: %lu - %lu", vData.size(), fData.size());
        for(int i=0; i<vData.size(); i++)
        {
            StringTools::print("%c", (char)vData[i]);
        }

        StringTools::println("\n ------------------------------------------------------");

        for(int i=0; i<fData.size(); i++)
        {
            StringTools::print("%c", (char)fData[i]);
        }

        int status = 0;
        char infoLog[512];

        vertexID = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexShaderCode = (const char*)vData.data();
        glShaderSource(vertexID, 1, &vertexShaderCode, NULL);
        glCompileShader(vertexID);

        glGetShaderiv(vertexID, GL_COMPILE_STATUS, &status);

        if(status == 0)
        {
            //ERROR COMPILING
            glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
            StringTools::println("ERROR COMPILING VERTEX SHADER - %ls", vertexShaderFile.getFullFileName().c_str());
            StringTools::println("%s", infoLog);
            valid = false;
        }

        fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentShaderCode = (const char*)fData.data();
        glShaderSource(fragmentID, 1, &fragmentShaderCode, NULL);
        glCompileShader(fragmentID);

        glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &status);

        if(status == 0)
        {
            //ERROR COMPILING
            glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
            StringTools::println("ERROR COMPILING FRAGMENT SHADER - %ls", fragmentShaderFile.getFullFileName().c_str());
            StringTools::println("%s", infoLog);
            valid = false;
        }

        shaderID = glCreateProgram();
        glAttachShader(shaderID, vertexID);
        glAttachShader(shaderID, fragmentID);
        glLinkProgram(shaderID);

        glGetProgramiv(shaderID, GL_LINK_STATUS, &status);

        if(status == 0)
        {
            //ERROR LINKING
            glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
            StringTools::println("ERROR LINKING SHADERS - %ls & %ls", vertexShaderFile.getFullFileName().c_str(), fragmentShaderFile.getFullFileName().c_str());
            StringTools::println("%s", infoLog);
            valid = false;
        }

        if(!valid)
            valid = true;
    }

    GLShader::~GLShader()
    {
        glDetachShader(shaderID, vertexID);
        glDetachShader(shaderID, fragmentID);
        
        glDeleteProgram(shaderID);

        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
        
        valid = false;
    }

    void GLShader::setAsActive()
    {
        if(valid)
            glUseProgram(shaderID);
    }

    void GLShader::setBool(std::string varName, bool value)
    {
        if(valid)
            glUniform1i( glGetUniformLocation(shaderID, varName.c_str()), value);
    }

    void GLShader::setInt(std::string varName, int value)
    {
        if(valid)
            glUniform1i( glGetUniformLocation(shaderID, varName.c_str()), value);
    }

    void GLShader::setFloat(std::string varName, float value)
    {
        if(valid)
            glUniform1f( glGetUniformLocation(shaderID, varName.c_str()), value);
    }

    void GLShader::setVec2(std::string varName, Vec2f value)
    {
        if(valid)
            glUniform2f( glGetUniformLocation(shaderID, varName.c_str()), value.x, value.y);
    }

    void GLShader::setVec3(std::string varName, Vec3f value)
    {
        if(valid)
            glUniform3f( glGetUniformLocation(shaderID, varName.c_str()), value.x, value.y, value.z);
    }

    void GLShader::setVec4(std::string varName, Vec4f value)
    {
        if(valid)
            glUniform4f( glGetUniformLocation(shaderID, varName.c_str()), value.x, value.y, value.z, value.w);
    }

    void GLShader::setMat2(std::string varName, Mat2f value, bool transpose)
    {
        if(valid)
        {
            float data[4] = {(float)value[0][0], (float)value[0][1], (float)value[1][0], (float)value[1][1]};
            glUniformMatrix2fv( glGetUniformLocation(shaderID, varName.c_str()), 1, transpose, data);
        }
    }

    void GLShader::setMat3(std::string varName, Mat3f value, bool transpose)
    {
        if(valid)
        {
            float data[9] = {(float)value[0][0], (float)value[0][1], (float)value[0][2],
                            (float)value[1][0], (float)value[1][1], (float)value[1][2],
                            (float)value[2][0], (float)value[2][1], (float)value[2][2]};
            glUniformMatrix3fv( glGetUniformLocation(shaderID, varName.c_str()), 1, transpose, data);
        }
    }

    void GLShader::setMat4(std::string varName, Mat4f value, bool transpose)
    {
        if(valid)
        {
            float data[16] = {(float)value[0][0], (float)value[0][1], (float)value[0][2], (float)value[0][3],
                            (float)value[1][0], (float)value[1][1], (float)value[1][2], (float)value[1][3],
                            (float)value[2][0], (float)value[2][1], (float)value[2][2], (float)value[2][3],
                            (float)value[3][0], (float)value[3][1], (float)value[3][2], (float)value[3][3]};

            glUniformMatrix4fv( glGetUniformLocation(shaderID, varName.c_str()), 1, transpose, data);
        }
    }

    bool GLShader::getValid()
    {
        return valid;
    }

    unsigned int GLShader::getShaderID()
    {
        return shaderID;
    }
    
    unsigned int GLShader::getVertexID()
    {
        return vertexID;
    }

    unsigned int GLShader::getFragmentID()
    {
        return fragmentID;
    }

}