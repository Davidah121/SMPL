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
			for(auto& ptr : models)
			{
				if(ptr.second != nullptr)
					delete ptr.second;
			}
			models.clear();
		}

		SmartMemory<GLTexture> GLFont::getTexture(const FontCharInfo& fci)
		{
			if(fci.reserved < img.getSize())
				return img.getTexture( imgPage[fci.reserved] );
			else
			{
				#ifdef USE_EXCEPTIONS
				throw OutOfBoundsError();
				#endif
			}

			return nullptr;
		}

		GLModel* GLFont::getModel(const FontCharInfo& fci)
		{
			auto it = models.find(fci.reserved);
			if(it != models.end())
				return it->second;
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
			Font::operator=(p); //this should copy the font over this one.

			//convert each sprite to a texture
			Sprite& otherImgs = p.getSprite();
			for(size_t i=0; i<otherImgs.getSize(); i++)
			{
				SmartMemory<GLTexture> nTexture = new GLTexture(otherImgs.getImage(i).getRawPointer(), true, {GLTexture::BEHAVIOR_CLAMP_TO_EDGE, GLTexture::FILTER_NEAREST});
				img.addTexture(nTexture);
			}

			//copy image page locations
			imgPage = p.getImagePages();

			//add models for each character
			//already copied to charInfoList
			size_t index = 0;
			for(FontCharInfo& fci : charInfoSet)
			{
				GLModel* m = new GLModel();
				
				//setup position values
				std::vector<float> positions = {
					0, 0,
					0, (float)fci.height,
					(float)fci.width, (float)fci.height,
					(float)fci.width, 0
				};

				//setup texture values
				double xMultVal = 1.0 / img.getTexture(imgPage[fci.reserved])->getWidth();
				double yMultVal = 1.0 / img.getTexture(imgPage[fci.reserved])->getHeight();

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

				models.insert({index, m});
				index++;
			}
		}
	}
#endif