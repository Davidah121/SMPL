#include "ext/GLFont.h"

#ifdef USE_OPENGL

    namespace smpl
    {
        GLFont::GLFont(File f)
        {
            BitmapFont k = BitmapFont(f);
            convertBitmapFont(k);
        }

        GLFont::~GLFont()
        {
            for(int i=0; i<models.size(); i++)
            {
                delete models[i];
            }
            models.clear();
        }

        GLTexture* GLFont::getTexture(int index)
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

        GLModel* GLFont::getModel(int index)
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

        void GLFont::convertBitmapFont(BitmapFont& p)
        {
            copyFont(p);

            //convert each sprite to a texture
            Sprite* otherImgs = p.getSprite();
            for(int i=0; i<otherImgs->getSize(); i++)
            {
                GLTexture* nTexture = new GLTexture(otherImgs->getImage(i), true, {GLTexture::BEHAVIOR_CLAMP_TO_EDGE, GLTexture::FILTER_NEAREST});
                img.addTexture(nTexture);
            }

            //copy image page locations
            imgPage = p.getImagePages();

            //add models for each character
            //already copied to charInfoList
            for(int i=0; i<charInfoList.size(); i++)
            {
                FontCharInfo fci = charInfoList[i];
                GLModel* m = new GLModel();
                
                //setup position values
                std::vector<float> positions = {
                    0, 0,
                    0, (float)fci.height,
                    (float)fci.width, (float)fci.height,
                    (float)fci.width, 0
                };

                //setup texture values
                double xMultVal = 1.0 / img.getTexture(imgPage[i])->getWidth();
                double yMultVal = 1.0 / img.getTexture(imgPage[i])->getHeight();

                std::vector<float> textures = {
                    (float)(xMultVal * fci.x), (float)(yMultVal * fci.y),
                    (float)(xMultVal * fci.x), (float)(yMultVal * (fci.y+fci.height)),
                    (float)(xMultVal * (fci.x+fci.width)), (float)(yMultVal * (fci.y+fci.height)),
                    (float)(xMultVal * (fci.x+fci.width)), (float)(yMultVal * fci.y)
                };
                
                m->storeDataFloat(0, positions, 2);
                m->storeDataFloat(1, textures, 2);
                m->setAttributeEnabled(0, true);
                m->setAttributeEnabled(1, true);
                m->setDrawType(Model::QUADS);

                models.push_back(m);
            }
        }
    }
#endif