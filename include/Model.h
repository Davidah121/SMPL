#pragma once
#include "MathExt.h"
#include "File.h"

namespace glib
{

    struct VertexFormat
    {
        unsigned char type = 0;
        unsigned char usage = 0;
        unsigned char size = 0;
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
        static const Class globalClass;

        static const unsigned char TYPE_INT = 0;
        static const unsigned char TYPE_FLOAT = 1;
        static const unsigned char TYPE_VEC2 = 2;
        static const unsigned char TYPE_VEC3 = 3;
        static const unsigned char TYPE_VEC4 = 4;

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
         * @brief Adds a set of index information for a new vertex.
         *      If the model is indexed, these values will be used for drawing the model.
         * 
         * @param indexInfo 
         */
        void addIndicies(std::vector<unsigned int> indexInfo);

        /**
         * @brief Set the Indicies for a vertex.
         *      If the model is indexed, these values will be used for drawing the model.
         * 
         * @param vertexLocation 
         * @param indexInfo 
         */
        void setIndicies(size_t vertexLocation, std::vector<unsigned int> indexInfo);

        /**
         * @brief Adds an int to the vertex data.
         * 
         * @param value 
         * @param list
         *      Which list to stick the value into.
         */
        void addInt(int value, size_t list);

        /**
         * @brief Adds an int to the vertex data.
         * 
         * @param value 
         * @param list
         *      Which list to stick the value into.
         */
        void addFloat(float value, size_t list);

        /**
         * @brief Adds a Vec2f to the vertex data.
         * 
         * @param value 
         * @param list
         *      Which list to stick the value into.
         */
        void addVec2f(Vec2f value, size_t list);

        /**
         * @brief Adds a Vec3f to the vertex data.
         * 
         * @param value 
         * @param list
         *      Which list to stick the value into.
         */
        void addVec3f(Vec3f value, size_t list);

        /**
         * @brief Adds a Vec4f to the vertex data.
         * 
         * @param value 
         * @param list
         *      Which list to stick the value into.
         */
        void addVec4f(Vec4f value, size_t list);
        
        /**
         * @brief Gets the vertex at the specified location.
         *      Returns all information for the vertex as a list of list
         *      Each list should be cast to the appropriate data type.
         *          Either int* or float*
         * 
         * @param i 
         * @return std::vector<std::vector<int>>
         */
        std::vector<std::vector<int>> getVertex(size_t i);

        int getInt(size_t index, size_t list);
        float getFloat(size_t index, size_t list);
        Vec2f getVec2f(size_t index, size_t list);
        Vec3f getVec3f(size_t index, size_t list);
        Vec4f getVec4f(size_t index, size_t list);
        

        /**
         * @brief Gets the Vertex Format Infomation as a list of VertexFormat structures.
         * 
         * @return std::vector<VertexFormat> 
         */
        std::vector<VertexFormat> getVertexFormatInfomation();

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
         * @brief Returns the amount of verticies in the model.
         * 
         * @return size_t 
         */
        size_t size();

        /**
         * @brief Returns the amount of values in a list
         * 
         * @param index 
         * @return size_t 
         */
        size_t sizeOfList(size_t index);

        /**
         * @brief Gets the size of a single vertex.
         *      Returns how many values are used for a vertex. 
         *      Not how many bytes are used.
         * @return size_t 
         */
        size_t getSizeOfVertex();

        /**
         * @brief Clears all of the data for the model.
         * 
         */
        void clear();

        /**
         * @brief Gets the internal vertex data for each list.
         * 
         * @return std::vector<std::vector<int>> 
         */
        std::vector<std::vector<int>> getRawVertexData();

        /**
         * @brief Gets the internal index data for each vertex.
         * 
         * @return std::vector<std::vector<int>> 
         */
        std::vector<std::vector<unsigned int>> getRawIndexData();
        

        /**
         * @brief Loads a model from a file.
         *      Not yet implemented
         * 
         * @param file 
         */
        void loadModel(File file);
        
        /**
         * @brief Sets whether the model uses index values to refer to vertex data.
         *      Good for saving space. By default, the model is not indexed.
         * 
         * @param indexed 
         */
        void setIndexed(bool indexed);

        /**
         * @brief Gets whether the model uses index values to refer to vertex data.
         * 
         * @return true 
         * @return false 
         */
        bool getIndexed();

    private:
        void loadOBJ(File file);
        void loadSTL(File file);
        void loadCollada(File file);

        std::vector<std::vector<int>> vertexData = std::vector<std::vector<int>>();
        std::vector<std::vector<unsigned int>> vertexIndexInfo = std::vector<std::vector<unsigned int>>();
        std::vector<VertexFormat> formatInfo = std::vector<VertexFormat>();

        unsigned char modelFormat = TRIANGLES;
        bool indexed = false;
    };

} //NAMESPACE glib END