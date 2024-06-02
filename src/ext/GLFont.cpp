#include "ext/GLFont.h"

#ifdef USE_OPENGL

    namespace smpl
    {
        const RootClass GLFont::globalClass = CREATE_ROOT_CLASS(GLFont, &Font::globalClass);
        const RootClass* GLFont::getClass()
        {
            return &GLFont::globalClass;
        }

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
                GLTexture* nTexture = new GLTexture(otherImgs->getImage(i), true, {GLTexture::BEHAVIOR_CLAMP_TO_EDGE, GLTexture::FILTER_LINEAR});
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
                float xMultVal = 1.0 / img.getTexture(imgPage[i])->getWidth();
                float yMultVal = 1.0 / img.getTexture(imgPage[i])->getHeight();

                std::vector<float> textures = {
                    xMultVal * fci.x, yMultVal * fci.y,
                    xMultVal * fci.x, yMultVal * (fci.y+fci.height),
                    xMultVal * (fci.x+fci.width), yMultVal * (fci.y+fci.height),
                    xMultVal * (fci.x+fci.width), yMultVal * fci.y
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