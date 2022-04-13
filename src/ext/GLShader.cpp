#ifdef USE_OPENGL

    #include "ext/GLShader.h"

    namespace glib
    {
        GLShader* GLShader::activeShader = nullptr;

        GLShader::GLShader()
        {

        }

        GLShader GLShader::createShaderFromStringLiteral(std::string vertexString, std::string fragmentString)
        {
            if(!GLSingleton::getInit())
            {
                return GLShader();
            }
            
            GLShader shader = GLShader();
            shader.init(vertexString, fragmentString);

            return shader;
        }

        GLShader::GLShader(File vertexShaderFile, File fragmentShaderFile)
        {
            if(!GLSingleton::getInit())
            {
                return;
            }

            std::vector<unsigned char> vData, fData;
            bool error = false;

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
                error = true;
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
                error = true;
            }

            vData.push_back(0);
            fData.push_back(0);

            init((char*)vData.data(), (char*)fData.data());
        }

        void GLShader::init(std::string vertexString, std::string fragmentString)
        {
            if(!GLSingleton::getInit())
            {
                return;
            }
            bool error = false;

            int status = 0;
            char infoLog[512];

            vertexID = glCreateShader(GL_VERTEX_SHADER);
            const char* vertexShaderCode = vertexString.c_str();
            glShaderSource(vertexID, 1, &vertexShaderCode, NULL);
            glCompileShader(vertexID);

            glGetShaderiv(vertexID, GL_COMPILE_STATUS, &status);

            if(status == 0)
            {
                //ERROR COMPILING
                glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
                StringTools::println("ERROR COMPILING VERTEX SHADER");
                StringTools::println("%s", infoLog);
                valid = false;
                error = true;
            }

            fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fragmentShaderCode = fragmentString.c_str();
            glShaderSource(fragmentID, 1, &fragmentShaderCode, NULL);
            glCompileShader(fragmentID);

            glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &status);

            if(status == 0)
            {
                //ERROR COMPILING
                glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
                StringTools::println("ERROR COMPILING FRAGMENT SHADER");
                StringTools::println("%s", infoLog);
                valid = false;
                error = true;
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
                StringTools::println("ERROR LINKING SHADERS");
                StringTools::println("%s", infoLog);
                valid = false;
                error = true;
            }

            if(!error)
                valid = true;
        }

        GLShader::~GLShader()
        {
            if(!GLSingleton::getInit())
            {
                return;
            }
            glDetachShader(shaderID, vertexID);
            glDetachShader(shaderID, fragmentID);
            
            glDeleteProgram(shaderID);

            glDeleteShader(vertexID);
            glDeleteShader(fragmentID);
            
            valid = false;

            if(activeShader == this)
                activeShader = nullptr;
        }

        void GLShader::setAsActive()
        {
            if(valid)
            {
                glUseProgram(shaderID);
                activeShader = nullptr;
            }
        }

        void GLShader::setBool(std::string varName, bool value)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                glUniform1i( uniformLocation, value);
            }
        }

        void GLShader::setInt(std::string varName, int value)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                glUniform1i( uniformLocation, value);
            }
        }

        void GLShader::setFloat(std::string varName, float value)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                glUniform1f( uniformLocation, value);
            }
        }

        void GLShader::setVec2(std::string varName, Vec2f value)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                glUniform2f( uniformLocation, value.x, value.y);
            }
        }

        void GLShader::setVec3(std::string varName, Vec3f value)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                glUniform3f( uniformLocation, value.x, value.y, value.z);
            }
        }

        void GLShader::setVec4(std::string varName, Vec4f value)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                glUniform4f( uniformLocation, value.x, value.y, value.z, value.w);
            }
        }

        void GLShader::setMat2(std::string varName, Mat2f value, bool transpose)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                float data[4] = {(float)value[0][0], (float)value[0][1], (float)value[1][0], (float)value[1][1]};
                glUniformMatrix2fv( uniformLocation, 1, transpose, data);
            }
        }

        void GLShader::setMat3(std::string varName, Mat3f value, bool transpose)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                float data[9] = {(float)value[0][0], (float)value[0][1], (float)value[0][2],
                                (float)value[1][0], (float)value[1][1], (float)value[1][2],
                                (float)value[2][0], (float)value[2][1], (float)value[2][2]};
                glUniformMatrix3fv( uniformLocation, 1, transpose, data);
            }
        }

        void GLShader::setMat4(std::string varName, Mat4f value, bool transpose)
        {
            if(valid)
            {
                int uniformLocation = glGetUniformLocation(shaderID, varName.c_str());
                float data[16] = {(float)value[0][0], (float)value[0][1], (float)value[0][2], (float)value[0][3],
                                (float)value[1][0], (float)value[1][1], (float)value[1][2], (float)value[1][3],
                                (float)value[2][0], (float)value[2][1], (float)value[2][2], (float)value[2][3],
                                (float)value[3][0], (float)value[3][1], (float)value[3][2], (float)value[3][3]};

                glUniformMatrix4fv( uniformLocation, 1, transpose, data);
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

        void GLShader::deactivateCurrentShader()
        {
            glUseProgram(0);
        }
        
        GLShader* GLShader::getActiveShader()
        {
            return activeShader;
        }
    }

#endif