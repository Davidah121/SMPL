#pragma once
#include "VectorGraphic.h"

namespace glib
{

    class VectorSprite
    {
    public:
        /**
         * @brief Construct a new VectorSprite object
         *      A VectorSprite, much like the Sprite object, contains a list of VectorGraphic objects.
         *      It does not contain data about delay between VectorGraphics.
         * 
         */
        VectorSprite();

        /**
         * @brief Destroy the VectorSprite object
         *      All VectorGraphic objects in the VectorSprite will be deleted and should be considered invalid.
         * 
         */
        ~VectorSprite();

        /**
         * @brief Loads a VectorGraphic from a .svg file.
         * 
         * @param filename 
         */
        void load(std::string filename);

        /**
         * @brief Saves a VectorGraphic into a .svg file.
         * 
         * @param filename 
         */
        void save(std::string filename);

        /**
         * @brief Gets the Transform for the VectorSprite.
         *      The transform should be applied to each VectorGraphic when drawn.
         * 
         * @return Mat3f 
         */
        Mat3f getTransform();

        /**
         * @brief Sets the Transform for the VectorSprite.
         * 
         * @param t 
         */
        void setTransform(Mat3f t);
        
        /**
         * @brief Adds a VectorGraphic to the VectorSprite.
         *      Note that the graphic added will be deleted when the other graphics are deleted.
         * 
         * @param g 
         */
        void addGraphic(VectorGraphic* g);

        /**
         * @brief Gets the amount of VectorGraphic objects in the VectorSprite.
         * 
         * @return int 
         */
        int getSize();

        /**
         * @brief Gets a VectorGraphic specified by the index.
         * 
         * @param index 
         * @return VectorGraphic* 
         */
        VectorGraphic* getGraphic(int index);
    private:
        Mat3f transform;
        std::vector<VectorGraphic*> vectorGraphicList = std::vector<VectorGraphic*>();
    };

} //NAMESPACE glib END