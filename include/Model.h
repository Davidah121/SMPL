#pragma once
#include "MathExt.h"

namespace glib
{

    struct VertexFormat
    {
        unsigned char type = 0;
        unsigned char usage = 0;
    };

    class Model : public Object
    {
    public:
        /**
         * @brief Construct a new Model object
         *      The model object contains data about a shape in either 2D or 3D.
         *      Each vertex can contain more than just position data.
         *      
         *      The model format is adjusted by the function addVertexFormatInfo()
         */
        Model();

        /**
         * @brief Destroy the Model object
         * 
         */
        ~Model();

        //Object and Class Stuff
        const Class* getClass();
        static const Class myClass;

        static const unsigned char TYPE_INT = 0;
        static const unsigned char TYPE_VEC2 = 1;
        static const unsigned char TYPE_VEC3 = 2;
        static const unsigned char TYPE_VEC4 = 3;

        static const unsigned char USAGE_POSITION = 0;
        static const unsigned char USAGE_TEXTURE = 1;
        static const unsigned char USAGE_NORMAL = 2;
        static const unsigned char USAGE_OTHER = 3;

        static const unsigned char POINTS = 0;
        
        static const unsigned char LINES = 1;
        static const unsigned char LINE_STRIP = 2;
        static const unsigned char LINE_LOOP = 3;

        static const unsigned char TRIANGLES = 4;
        static const unsigned char TRIANGLE_STRIP = 5;
        static const unsigned char TRIANGLE_FAN = 6;

        static const unsigned char QUADS = 7;
        static const unsigned char QUAD_STRIP = 8;
        
        /**
         * @brief Adds new format information for the verticies of the model.
         *      Must be called before using the model.
         * 
         * @param type 
         *      Valid types are:
         *          TYPE_INT
         *          TYPE_VEC2
         *          TYPE_VEC3
         *          TYPE_VEC4
         * @param usage 
         *      Valid usages are:
         *          USAGE_POSITION
         *          USAGE_TEXTURE
         *          USAGE_NORMAL
         *          USAGE_OTHER
         */
        void addVertexFormatInfo(unsigned char type, unsigned char usage);

        /**
         * @brief Adds an int to the vertex data.
         * 
         * @param value 
         */
        void addInt(int value);

        /**
         * @brief Adds a Vec2f to the vertex data.
         * 
         * @param value 
         */
        void addVec2f(Vec2f value);

        /**
         * @brief Adds a Vec3f to the vertex data.
         * 
         * @param value 
         */
        void addVec3f(Vec3f value);

        /**
         * @brief Adds a Vec4f to the vertex data.
         * 
         * @param value 
         */
        void addVec4f(Vec4f value);
        
        /**
         * @brief Gets the vertex at the specified location.
         *      Returns all information for the vertex as a list of doubles
         * 
         * @param i 
         * @return std::vector<double> 
         */
        std::vector<double> getVertex(int i);

        /**
         * @brief Gets the Vertex Format Infomation as a list of VertexFormat structures.
         * 
         * @return std::vector<VertexFormat> 
         */
        std::vector<VertexFormat> getVertexFormatInfomation();

        /**
         * @brief Returns all vertex data as a list of doubles.
         *      Specifically returns the internal vector used to store vertex data.
         * 
         * @return std::vector<double>& 
         */
        std::vector<double>& getAllVertices();

        /**
         * @brief Returns the amount of verticies in the model.
         * 
         * @return int 
         */
        int size();

        /**
         * @brief Sets the Model Format
         * 
         * @param format 
         *      Valid formats are:
         *          POINTS
         *          LINES
         *          LINE_STRIP
         *          LINE_LOOP
         *          TRIANGLES
         *          TRIANGLE_STRIP
         *          TRIANGLE_FAN
         *          QUADS
         *          QUAD_STRIP
         */
        void setModelFormat(unsigned char format);

        /**
         * @brief Gets the Model Format
         * 
         * @return unsigned char 
         */
        unsigned char getModelFormat();

        /**
         * @brief Gets the size of a single vertex.
         *      Returns how many values are used for a vertex. 
         *      Not how many bytes are used.
         * @return int 
         */
        int getSizeOfVertex();

        /**
         * @brief Clears all of the data for the model.
         * 
         */
        void clear();

        /**
         * @brief Loads a model from a file.
         *      Not yet implemented
         * 
         * @param filename 
         */
        void loadModel(std::string filename);

        /**
         * @brief Loads a model from a file.
         *      Not yet implemented
         * 
         * @param filename 
         */
        void loadModel(std::wstring filename);
        
    private:
        void loadOBJ(std::wstring filename);

        std::vector<double> vertexInfo = std::vector<double>();
        std::vector<VertexFormat> orderInfo = std::vector<VertexFormat>();
        int sizeOfVertex = 0;
        unsigned char modelFormat = TRIANGLES;
    };

} //NAMESPACE glib END