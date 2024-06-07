#include "ModelBuilder.h"

namespace smpl
{
    Model* ModelBuilder::createSphere(double radius, int steps, bool faceNormals)
    {
        Model* m = new Model();

        int uSteps = steps+1;
        int vSteps = steps+1;

        double du = (2*PI) / (uSteps-1);
        double dv = (PI) / (vSteps-1);

        m->setModelFormat(Model::TRIANGLES);
        
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_POSITION);
        m->addVertexFormatInfo(Model::TYPE_VEC2, Model::USAGE_TEXTURE);
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_NORMAL);

        m->setIndexed(true);

        int indexStuff = 0;

        for(int v = 0; v < vSteps; v++)
        {
            for(int u = 0; u < uSteps; u++)
            {
                Vec3f position;
                Vec2f tex;

                position.x = radius*MathExt::cos(u*du) * MathExt::sin(v*dv);
                position.y = radius*MathExt::sin(u*du) * MathExt::sin(v*dv);
                position.z = radius*MathExt::cos(v*dv);

                tex.x = (float)u/uSteps;
                tex.y = (float)v/vSteps;

                m->addVec3f( position, 0 );
                m->addVec2f( tex, 1);
                if(!faceNormals)
                    m->addVec3f( position.normalize(), 2);

                indexStuff++;
            }
        }

        unsigned int normalIndex = 0;
        std::vector<std::vector<int>> data = m->getRawVertexData();
        float* positionData = (float*)data[0].data();
        for(int v = 0; v < vSteps-1; v++)
        {
            for(int u = 0; u < uSteps; u++)
            {
                unsigned int posIndex1 = u + (v)*(uSteps);
                unsigned int posIndex2 = ((u+1) % (uSteps)) + (v)*(uSteps);
                unsigned int posIndex3 = ((u+1) % (uSteps)) + (v+1)*(uSteps);
                unsigned int posIndex4 = (u) + (v+1)*(uSteps);
                
                
                if(v==0)
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});
                        
                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex3*3], positionData[1 + posIndex3*3], positionData[2 + posIndex3*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
                else if(v==vSteps-2)
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex2, posIndex2, posIndex2});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex2, posIndex2, normalIndex});
                        
                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex2*3], positionData[1 + posIndex2*3], positionData[2 + posIndex2*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
                else
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                        m->addIndicies({posIndex2, posIndex2, posIndex2});

                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});
                        m->addIndicies({posIndex2, posIndex2, normalIndex});

                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex2*3], positionData[1 + posIndex2*3], positionData[2 + posIndex2*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex3*3], positionData[1 + posIndex3*3], positionData[2 + posIndex3*3]);
                        Vec3f p4 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;

                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});

                        normal = MathExt::crossProduct((p4-p1), (p3-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
            }
        }

        return m;
    }

    Model* ModelBuilder::createBox(double xScale, double yScale, double zScale, bool faceNormals)
    {
        Model* m = new Model();

        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_POSITION);
        m->addVertexFormatInfo(Model::TYPE_VEC2, Model::USAGE_TEXTURE);
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_NORMAL);

        m->setIndexed(true);
        m->setModelFormat(Model::TRIANGLES);

        m->addVec3f( Vec3f(-1*xScale, -1*yScale, -1*zScale), 0);
        m->addVec3f( Vec3f(1*xScale, -1*yScale, -1*zScale), 0);
        m->addVec3f( Vec3f(-1*xScale, 1*yScale, -1*zScale), 0);
        m->addVec3f( Vec3f(1*xScale, 1*yScale, -1*zScale), 0);

        m->addVec3f( Vec3f(-1*xScale, -1*yScale, 1*zScale), 0);
        m->addVec3f( Vec3f(1*xScale, -1*yScale, 1*zScale), 0);
        m->addVec3f( Vec3f(-1*xScale, 1*yScale, 1*zScale), 0);
        m->addVec3f( Vec3f(1*xScale, 1*yScale, 1*zScale), 0);

        m->addVec2f( Vec2f(0, 0), 1);
        m->addVec2f( Vec2f(1, 0), 1);
        m->addVec2f( Vec2f(0, 1), 1);
        m->addVec2f( Vec2f(1, 1), 1);

        if(faceNormals)
        {
            m->addVec3f( Vec3f(0, 0, -1), 2);
            m->addVec3f( Vec3f(0, 0, 1), 2);
            m->addVec3f( Vec3f(0, -1, 0), 2);
            m->addVec3f( Vec3f(0, 1, 0), 2);
            m->addVec3f( Vec3f(-1, 0, 0), 2);
            m->addVec3f( Vec3f(1, 0, 0), 2);

            //bottom face
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {3, 3, 0} ); m->addIndicies( {1, 1, 0} );
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {2, 2, 0} ); m->addIndicies( {3, 3, 0} );
            
            //top face
            m->addIndicies( {4, 0, 1} ); m->addIndicies( {5, 1, 1} ); m->addIndicies( {7, 3, 1} );
            m->addIndicies( {4, 0, 1} ); m->addIndicies( {7, 3, 1} ); m->addIndicies( {6, 2, 1} );

            //back face
            m->addIndicies( {0, 0, 2} ); m->addIndicies( {1, 1, 2} ); m->addIndicies( {5, 3, 2} );
            m->addIndicies( {0, 0, 2} ); m->addIndicies( {5, 3, 2} ); m->addIndicies( {4, 2, 2} );
            
            //front face
            m->addIndicies( {2, 0, 3} ); m->addIndicies( {7, 3, 3} ); m->addIndicies( {3, 1, 3} );
            m->addIndicies( {2, 0, 3} ); m->addIndicies( {6, 2, 3} ); m->addIndicies( {7, 3, 3} );

            //left face
            m->addIndicies( {0, 0, 4} ); m->addIndicies( {6, 3, 4} ); m->addIndicies( {2, 1, 4} );
            m->addIndicies( {0, 0, 4} ); m->addIndicies( {4, 2, 4} ); m->addIndicies( {6, 3, 4} );
            
            //right face
            m->addIndicies( {1, 0, 5} ); m->addIndicies( {3, 1, 5} ); m->addIndicies( {7, 3, 5} );
            m->addIndicies( {1, 0, 5} ); m->addIndicies( {7, 3, 5} ); m->addIndicies( {5, 2, 5} );
            
        }
        else
        {
            m->addVec3f( Vec3f(-1*xScale, -1*yScale, -1*zScale).normalize(), 2);
            m->addVec3f( Vec3f(1*xScale, -1*yScale, -1*zScale).normalize(), 2);
            m->addVec3f( Vec3f(-1*xScale, 1*yScale, -1*zScale).normalize(), 2);
            m->addVec3f( Vec3f(1*xScale, 1*yScale, -1*zScale).normalize(), 2);

            m->addVec3f( Vec3f(-1*xScale, -1*yScale, 1*zScale).normalize(), 2);
            m->addVec3f( Vec3f(1*xScale, -1*yScale, 1*zScale).normalize(), 2);
            m->addVec3f( Vec3f(-1*xScale, 1*yScale, 1*zScale).normalize(), 2);
            m->addVec3f( Vec3f(1*xScale, 1*yScale, 1*zScale).normalize(), 2);

            //bottom face
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {3, 3, 3} ); m->addIndicies( {1, 1, 1} );
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {2, 2, 2} ); m->addIndicies( {3, 3, 3} );
            
            //top face
            m->addIndicies( {4, 0, 4} ); m->addIndicies( {5, 1, 5} ); m->addIndicies( {7, 3, 7} );
            m->addIndicies( {4, 0, 4} ); m->addIndicies( {7, 3, 7} ); m->addIndicies( {6, 2, 6} );

            //back face
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {1, 1, 1} ); m->addIndicies( {5, 3, 5} );
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {5, 3, 5} ); m->addIndicies( {4, 2, 4} ); 
            
            //front face
            m->addIndicies( {2, 0, 2} ); m->addIndicies( {7, 3, 7} ); m->addIndicies( {3, 1, 3} );
            m->addIndicies( {2, 0, 2} ); m->addIndicies( {6, 2, 6} ); m->addIndicies( {7, 3, 7} );

            //left face
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {6, 3, 6} ); m->addIndicies( {2, 1, 2} );
            m->addIndicies( {0, 0, 0} ); m->addIndicies( {4, 2, 4} ); m->addIndicies( {6, 3, 6} );
            
            //right face
            m->addIndicies( {1, 0, 1} ); m->addIndicies( {3, 1, 3} ); m->addIndicies( {7, 3, 7} );
            m->addIndicies( {1, 0, 1} ); m->addIndicies( {7, 3, 7} ); m->addIndicies( {5, 2, 5} );
            
        }

        return m;
    }
    
    Model* ModelBuilder::createEllipsoid(double xRadius, double yRadius, double zRadius, int steps, bool faceNormals)
    {
        Model* m = new Model();

        int uSteps = steps+1;
        int vSteps = steps+1;

        double du = (2*PI) / (uSteps-1);
        double dv = (PI) / (vSteps-1);

        m->setModelFormat(Model::TRIANGLES);
        
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_POSITION);
        m->addVertexFormatInfo(Model::TYPE_VEC2, Model::USAGE_TEXTURE);
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_NORMAL);

        m->setIndexed(true);

        int indexStuff = 0;

        for(int v = 0; v < vSteps; v++)
        {
            for(int u = 0; u < uSteps; u++)
            {
                Vec3f position;
                Vec2f tex;

                position.x = xRadius*MathExt::cos(u*du) * MathExt::sin(v*dv);
                position.y = yRadius*MathExt::sin(u*du) * MathExt::sin(v*dv);
                position.z = zRadius*MathExt::cos(v*dv);

                tex.x = (float)u/uSteps;
                tex.y = (float)v/vSteps;

                m->addVec3f( position, 0 );
                m->addVec2f( tex, 1);
                if(!faceNormals)
                    m->addVec3f( position.normalize(), 2);

                indexStuff++;
            }
        }

        unsigned int normalIndex = 0;
        std::vector<std::vector<int>> data = m->getRawVertexData();
        float* positionData = (float*)data[0].data();
        for(int v = 0; v < vSteps-1; v++)
        {
            for(int u = 0; u < uSteps; u++)
            {
                unsigned int posIndex1 = u + (v)*(uSteps);
                unsigned int posIndex2 = ((u+1) % (uSteps)) + (v)*(uSteps);
                unsigned int posIndex3 = ((u+1) % (uSteps)) + (v+1)*(uSteps);
                unsigned int posIndex4 = (u) + (v+1)*(uSteps);
                
                
                if(v==0)
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});
                        
                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex3*3], positionData[1 + posIndex3*3], positionData[2 + posIndex3*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
                else if(v==vSteps-2)
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex2, posIndex2, posIndex2});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex2, posIndex2, normalIndex});
                        
                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex2*3], positionData[1 + posIndex2*3], positionData[2 + posIndex2*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
                else
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                        m->addIndicies({posIndex2, posIndex2, posIndex2});

                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});
                        m->addIndicies({posIndex2, posIndex2, normalIndex});

                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex2*3], positionData[1 + posIndex2*3], positionData[2 + posIndex2*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex3*3], positionData[1 + posIndex3*3], positionData[2 + posIndex3*3]);
                        Vec3f p4 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;

                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});

                        normal = MathExt::crossProduct((p4-p1), (p3-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
            }
        }

        return m;
    }

    Model* ModelBuilder::createCylinder(double radius, double zScale, int steps, bool faceNormals)
    {
        Model* m = new Model();

        int uSteps = steps+1;

        double du = (2*PI) / (uSteps-1);

        m->setModelFormat(Model::TRIANGLES);
        
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_POSITION);
        m->addVertexFormatInfo(Model::TYPE_VEC2, Model::USAGE_TEXTURE);
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_NORMAL);

        m->setIndexed(true);

        unsigned int indexStuff = 0;
        double zVal = -zScale;

        for(int v = 0; v < 2; v++)
        {
            for(int u = 0; u < uSteps; u++)
            {
                Vec3f position;
                Vec2f tex;

                position.x = radius*MathExt::cos(u*du);
                position.y = radius*MathExt::sin(u*du);
                position.z = zVal;

                tex.x = (float)u/uSteps;
                tex.y = (float)v;

                Vec3f normal = Vec3f(position.x, position.y, 0);

                m->addVec3f( position, 0 );
                m->addVec2f( tex, 1);
                if(!faceNormals)
                    m->addVec3f( normal.normalize(), 2);

                indexStuff++;
            }
            zVal = zScale;
        }


        //add bottom midPoint
        m->addVec3f( Vec3f(0, 0, -zScale), 0 );
        m->addVec2f( Vec2f(0.5, 0), 1);
        if(!faceNormals)
            m->addVec3f( Vec3f(0, 0, -1), 2);

        //add top midPoint
        m->addVec3f( Vec3f(0, 0, zScale), 0 );
        m->addVec2f( Vec2f(0.5, 1), 1);
        if(!faceNormals)
            m->addVec3f( Vec3f(0, 0, 1), 2);

        //Cyclinder top and bottom will basically have no texture coordinates

        //connect sides
        unsigned int normalIndex = 0;
        for(int u = 0; u < uSteps; u++)
        {
            unsigned int index1 = u;
            unsigned int index2 = (u+1)%uSteps;
            unsigned int index3 = uSteps + (u+1)%uSteps;
            unsigned int index4 = uSteps + u;

            if(!faceNormals)
            {
                m->addIndicies( {index1, index1, index1});
                m->addIndicies( {index2, index2, index2});
                m->addIndicies( {index3, index3, index3});

                m->addIndicies( {index1, index1, index1});
                m->addIndicies( {index3, index3, index3});
                m->addIndicies( {index4, index4, index4});
            }
            else
            {
                Vec3f v1, v2, v3, v4, normal;
                v1 = m->getVec3f(index1, 0);
                v2 = m->getVec3f(index2, 0);
                v3 = m->getVec3f(index3, 0);
                v4 = m->getVec3f(index4, 0);
                
                normal = MathExt::crossProduct( v2-v1, v3-v1 );
                m->addVec3f(normal.normalize(), 2);

                m->addIndicies( {index1, index1, normalIndex});
                m->addIndicies( {index2, index2, normalIndex});
                m->addIndicies( {index3, index3, normalIndex});
                normalIndex++;

                normal = MathExt::crossProduct( v3-v1, v4-v1 );
                m->addVec3f(normal.normalize(), 2);

                m->addIndicies( {index1, index1, normalIndex});
                m->addIndicies( {index3, index3, normalIndex});
                m->addIndicies( {index4, index4, normalIndex});
                normalIndex++;
            }
        }


        //bottom cap
        for(int u = 0; u < uSteps; u++)
        {
            unsigned int index1 = u;
            unsigned int index2 = (u+1)%uSteps;

            if(!faceNormals)
            {
                m->addIndicies( {index1, index1, index1});
                m->addIndicies( {indexStuff, indexStuff, indexStuff});
                m->addIndicies( {index2, index2, index2});
            }
            else
            {
                m->addIndicies( {index1, index1, normalIndex});
                m->addIndicies( {indexStuff, indexStuff, normalIndex});
                m->addIndicies( {index2, index2, normalIndex});
            }
        }
        if(faceNormals)
        {
            m->addVec3f(Vec3f(0, 0, -1), 2);
            normalIndex++;
        }

        //top cap
        for(int u = 0; u < uSteps; u++)
        {
            unsigned int index1 = uSteps + u;
            unsigned int index2 = uSteps + (u+1)%uSteps;

            if(!faceNormals)
            {
                m->addIndicies( {index1, index1, index1});
                m->addIndicies( {index2, index2, index2});
                m->addIndicies( {indexStuff+1, indexStuff+1, indexStuff+1});
            }
            else
            {
                m->addIndicies( {index1, index1, normalIndex});
                m->addIndicies( {index2, index2, normalIndex});
                m->addIndicies( {indexStuff+1, indexStuff+1, normalIndex});
            }
        }
        if(faceNormals)
        {
            m->addVec3f(Vec3f(0, 0, 1), 2);
            normalIndex++;
        }

        return m;
    }

    Model* ModelBuilder::createCapsule(double radius, double zScale, int steps, bool faceNormals)
    {
        Model* m = new Model();
        
        int uSteps = steps+1;
        int vSteps = steps+1;

        double du = (2*PI) / (uSteps-1);
        double dv = (PI) / (vSteps-1);

        m->setModelFormat(Model::TRIANGLES);
        
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_POSITION);
        m->addVertexFormatInfo(Model::TYPE_VEC2, Model::USAGE_TEXTURE);
        m->addVertexFormatInfo(Model::TYPE_VEC3, Model::USAGE_NORMAL);

        m->setIndexed(true);

        //cylinder stuff
        unsigned int indexStuff = 0;
        double zVal = zScale;
        

        //Generate top and bottom circle
        for(int v = 0; v < vSteps; v++)
        {
            for(int u = 0; u < uSteps; u++)
            {
                Vec3f position;
                Vec2f tex;

                position.x = radius*MathExt::cos(u*du)*MathExt::sin(v*dv);
                position.y = radius*MathExt::sin(u*du)*MathExt::sin(v*dv);
                position.z = radius*MathExt::cos(v*dv);

                tex.x = (float)u/uSteps;
                tex.y = (float)v/vSteps;

                Vec3f normal = position.normalize();
                position.z += zVal;

                m->addVec3f( position, 0 );
                m->addVec2f( tex, 1);
                if(!faceNormals)
                    m->addVec3f( position.normalize(), 2);

                indexStuff++;
            }

            if(v>= vSteps/2)
                zVal = -zScale;
        }

        unsigned int normalIndex = 0;
        std::vector<std::vector<int>> data = m->getRawVertexData();
        float* positionData = (float*)data[0].data();
        for(int v = 0; v < vSteps-1; v++)
        {
            for(int u = 0; u < uSteps; u++)
            {
                unsigned int posIndex1 = u + (v)*(uSteps);
                unsigned int posIndex2 = ((u+1) % (uSteps)) + (v)*(uSteps);
                unsigned int posIndex3 = ((u+1) % (uSteps)) + (v+1)*(uSteps);
                unsigned int posIndex4 = (u) + (v+1)*(uSteps);
                
                
                if(v==0)
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});
                        
                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex3*3], positionData[1 + posIndex3*3], positionData[2 + posIndex3*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
                else if(v==vSteps-2)
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex2, posIndex2, posIndex2});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex2, posIndex2, normalIndex});
                        
                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex2*3], positionData[1 + posIndex2*3], positionData[2 + posIndex2*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
                else
                {
                    if(!faceNormals)
                    {
                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                        m->addIndicies({posIndex2, posIndex2, posIndex2});

                        m->addIndicies({posIndex1, posIndex1, posIndex1});
                        m->addIndicies({posIndex4, posIndex4, posIndex4});
                        m->addIndicies({posIndex3, posIndex3, posIndex3});
                    }
                    else
                    {
                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});
                        m->addIndicies({posIndex2, posIndex2, normalIndex});

                        Vec3f p1 = Vec3f(positionData[posIndex1*3], positionData[1 + posIndex1*3], positionData[2 + posIndex1*3]);
                        Vec3f p2 = Vec3f(positionData[posIndex2*3], positionData[1 + posIndex2*3], positionData[2 + posIndex2*3]);
                        Vec3f p3 = Vec3f(positionData[posIndex3*3], positionData[1 + posIndex3*3], positionData[2 + posIndex3*3]);
                        Vec3f p4 = Vec3f(positionData[posIndex4*3], positionData[1 + posIndex4*3], positionData[2 + posIndex4*3]);
                        
                        Vec3f normal = MathExt::crossProduct((p3-p1), (p2-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;

                        m->addIndicies({posIndex1, posIndex1, normalIndex});
                        m->addIndicies({posIndex4, posIndex4, normalIndex});
                        m->addIndicies({posIndex3, posIndex3, normalIndex});

                        normal = MathExt::crossProduct((p4-p1), (p3-p1));
                        m->addVec3f(normal.normalize(), 2);
                        normalIndex++;
                    }
                }
            }
        }

        return m;
    }

}