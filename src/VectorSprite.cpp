#include "VectorSprite.h"
#include "StringTools.h"

namespace smpl
{

    VectorSprite::VectorSprite()
    {

    }

    VectorSprite::~VectorSprite()
    {
        for(int i=0; i<vectorGraphicList.size(); i++)
        {
            delete vectorGraphicList[i];
        }
        vectorGraphicList.clear();
    }

    void VectorSprite::load(std::string filename)
    {
        SimpleXml file = SimpleXml();
        bool valid = file.load(StringTools::toWideString(filename));
        if(valid)
        {
            for(XmlNode* n : file.getNodes())
            {
                if(StringTools::equalsIgnoreCase<char>(n->getTitle(), "svg"))
                {
                    VectorGraphic* g = new VectorGraphic();
                    g->load(n);
                    vectorGraphicList.push_back(g);
                }
            }
        }
    }

    void VectorSprite::save(std::string filename)
    {
        SimpleXml f = SimpleXml();
        for(int i=0; i<vectorGraphicList.size(); i++)
        {
            XmlNode* n = vectorGraphicList[i]->writeAsXmlNode();
            f.addNode(n);
        }
        f.save(StringTools::toWideString(filename));
    }

    Mat3f VectorSprite::getTransform()
    {
        return transform;
    }

    void VectorSprite::setTransform(Mat3f t)
    {
        transform = t;
    }

    int VectorSprite::getSize()
    {
        return vectorGraphicList.size();
    }

    void VectorSprite::addGraphic(VectorGraphic* g)
    {
        vectorGraphicList.push_back(g);
    }

    VectorGraphic* VectorSprite::getGraphic(int index)
    {
        if(index < vectorGraphicList.size())
            return vectorGraphicList[index];
        return nullptr;
    }

} //NAMESPACE glib END