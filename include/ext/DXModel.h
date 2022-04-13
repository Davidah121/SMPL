#pragma once

#ifdef USE_DIRECTX

    #include "MathExt.h"
    #include "ext/DXSingleton.h"
    #include "Model.h"

    /**
     * Things that still need work.
     *      Setting data dynamically inside of a buffer instead of creating a new one.
     *      Enable and disable attributes.
     */

    namespace glib
    {
        class DXModel
        {
        public:
            static const bool TYPE_ARRAY = false;
            static const bool TYPE_INDEXED = true;

            DXModel();
            DXModel(DXModel& other);
            void operator=(DXModel& other);
            ~DXModel();

            void draw();

            static DXModel convertModel(Model* m);
            static DXModel generateTestModel();

            bool getModelType();
            int getDrawType();

            size_t getSize();

            void storeDataInt(int attributeID, std::vector<int> data, int valuesPerVertex, bool forceReallocate = false);
            void storeDataFloat(int attributeID, std::vector<float> data, int valuesPerVertex, bool forceReallocate = false);
            void storeDataFloat(int attributeID, std::vector<double> data, int valuesPerVertex, bool forceReallocate = false);
            
            void setAttributeEnabled(int attributeID, bool v);

            void disableAllAttributes();

            void setDrawType(int value);
        private:
            std::vector<ID3D11Buffer*> vertexData = std::vector<ID3D11Buffer*>();
            std::vector<unsigned int> offsets = std::vector<unsigned int>();
            std::vector<unsigned int> strides = std::vector<unsigned int>();
            std::vector<bool> attributeEnabled;

            size_t size = 0;
            ID3D11Buffer* indexData = nullptr;
            
            bool modType = TYPE_ARRAY;
            int drawType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            bool shouldDelete = true;
        };
    }

#endif