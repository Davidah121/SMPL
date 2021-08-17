#pragma once
#include "VectorGraphic.h"

namespace glib
{

    class VectorSprite
    {
    public:
        VectorSprite();
        ~VectorSprite();

        void load(std::string filename);
        void save(std::string filename);

        Mat3f getTransform();
        void setTransform(Mat3f t);
        
        void addGraphic(VectorGraphic* g);

        int getSize();
        VectorGraphic* getGraphic(int index);
    private:
        Mat3f transform;
        std::vector<VectorGraphic*> vectorGraphicList = std::vector<VectorGraphic*>();
    };

} //NAMESPACE glib END