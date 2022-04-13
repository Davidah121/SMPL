#ifdef USE_DIRECTX

    #include "ext/DXFont.h"

    namespace glib
    {
        const Class DXFont::myClass = Class("DXFont", {&Font::myClass});
        const Class* DXFont::getClass()
        {
            return &DXFont::myClass;
        }

        DXFont::DXFont(File f)
        {
            BitmapFont k = BitmapFont(f);
            convertBitmapFont(k);
        }

        DXFont::~DXFont()
        {
            for(int i=0; i<models.size(); i++)
            {
                delete models[i];
            }
            models.clear();
        }

        DXTexture* DXFont::getTexture(int index)
        {
            if(index>=0 && index<imgPage.size())
                return img.getTexture( imgPage[index] );
            else
            {
                #ifdef USE_EXCEPTIONS
                throw OutOfBoundsError();
                #endif
            }

            return nullptr;
        }

        DXModel* DXFont::getModel(int index)
        {
            if(index>=0 && index<models.size())
                return models[ index ];
            else
            {
                #ifdef USE_EXCEPTIONS
                throw OutOfBoundsError();
                #endif
            }

            return nullptr;
        }

        void DXFont::convertBitmapFont(BitmapFont& p)
        {
            copyFont(p);

            //convert each sprite to a texture
            Sprite* otherImgs = p.getSprite();
            for(int i=0; i<otherImgs->getSize(); i++)
            {
                DXTexture* nTexture = new DXTexture(otherImgs->getImage(i));
                img.addTexture(nTexture);
            }

            //copy image page locations
            imgPage = p.getImagePages();

            //add models for each character
            //already copied to charInfoList
            for(int i=0; i<charInfoList.size(); i++)
            {
                FontCharInfo fci = charInfoList[i];
                DXModel* m = new DXModel();
                
                //setup position values
                std::vector<float> positions = {
                    0, 0,
                    (float)fci.width, (float)fci.height,
                    0, (float)fci.height,

                    0, 0,
                    (float)fci.width, 0,
                    (float)fci.width, (float)fci.height
                };

                //setup texture values
                float xMultVal = 1.0 / img.getTexture(imgPage[i])->getWidth();
                float yMultVal = 1.0 / img.getTexture(imgPage[i])->getHeight();

                std::vector<float> textures = {
                    xMultVal * fci.x, yMultVal * fci.y,
                    xMultVal * (fci.x+fci.width), yMultVal * (fci.y+fci.height),
                    xMultVal * fci.x, yMultVal * (fci.y+fci.height),
                    
                    xMultVal * fci.x, yMultVal * fci.y,
                    xMultVal * (fci.x+fci.width), yMultVal * fci.y,
                    xMultVal * (fci.x+fci.width), yMultVal * (fci.y+fci.height)
                };
                

                m->storeDataFloat(0, positions, 2);
                m->storeDataFloat(1, textures, 2);
                m->setAttributeEnabled(0, true);
                m->setAttributeEnabled(1, true);
                m->setDrawType(Model::TRIANGLES);

                models.push_back(m);
            }
        }
    }

#endif