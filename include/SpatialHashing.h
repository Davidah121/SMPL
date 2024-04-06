#include <vector>
#include "Object.h"
#include "Shape.h"

namespace smpl
{
    template<typename T>
    class SpatialHashing;

    template<typename T>
    class SpatialHashingObject : public Object
    {
    public:
        SpatialHashingObject(T data, Shape shape)
        {
            this->data = data;
            this->collisionShape = shape;
        }
        ~SpatialHashingObject()
        {
        }

        void setData(T data)
        {
            this->data = data;
        }
        T getData()
        {
            return data;
        }

        void setShape(Shape p)
        {
            collisionShape = p;
        }
        Shape getShape()
        {
            return collisionShape;
        }

    private:
        friend SpatialHashing<T>;
        T data;
        Shape collisionShape;
        int id=-1;
    };

    template<typename T>
    class SpatialHashing : public Object
    {
    public:
        SpatialHashing(int xGrids, int yGrids, double width, double height)
        {
            this->xGrids = xGrids;
            this->yGrids = yGrids;
            this->width = width;
            this->height = height;

            for(int i=0; i<xGrids*yGrids; i++)
            {
                objects.push_back( std::vector<SpatialHashingObject<T>>() );
            }
        }
        ~SpatialHashing()
        {
            objects.clear();
            width = 0;
            height = 0;
            xGrids = 0;
            yGrids = 0;
        }

        void addObject(SpatialHashingObject<T> obj)
        {
            //determine what grids the object collides with.
            double radius = obj.getShape().generateBoundingRadius() + 1e-8; //Add some small offset to handle boundary conditions. Also handles points since they have no radius

            double x1 = obj.getShape().getPosition().x - radius;
            double y1 = obj.getShape().getPosition().y - radius;
            double x2 = obj.getShape().getPosition().x + radius;
            double y2 = obj.getShape().getPosition().y + radius;

            int startXGrid = MathExt::clamp( (int)MathExt::floor((x1/width) * xGrids), 0, xGrids);
            int startYGrid = MathExt::clamp( (int)MathExt::floor((y1/height) * yGrids), 0, yGrids);
            
            int endXGrid = MathExt::clamp( (int)MathExt::ceil((x2/width) * xGrids), 0, xGrids);
            int endYGrid = MathExt::clamp( (int)MathExt::ceil((y2/height) * yGrids), 0, yGrids);

            double xInc = width/xGrids;
            double yInc = height/yGrids;
            Box2D boxShape;

            for(int y=startYGrid; y<endYGrid; y++)
            {
                boxShape.setTopBound(y*yInc);
                boxShape.setBottomBound((y+1) * yInc);

                for(int x=startXGrid; x<endXGrid; x++)
                {
                    boxShape.setLeftBound(x*xInc);
                    boxShape.setRightBound((x+1) * xInc);

                    if( CollisionMaster::getCollision( &boxShape, &obj.collisionShape, true ) )
                    {
                        if(obj.id == -1)
                        {
                            obj.id = s;
                            s++;
                        }
                        objects[x + y*xGrids].push_back(obj);
                    }
                }
            }
        }

        void addObject(T data, Shape colShape)
        {
            SpatialHashingObject<T> tempObject = SpatialHashingObject<T>(data, colShape);
            addObject(tempObject);
        }

        std::vector<SpatialHashingObject<T>> getObjects(int xGrid, int yGrid)
        {
            if(xGrid < xGrids && xGrid >= 0)
            {
                if(yGrid < yGrids && yGrid >= 0)
                {
                    return objects[xGrid + yGrid*xGrids];
                }
            }

            return {};
        }

        std::vector<SpatialHashingObject<T>> getObjects(Shape* shape)
        {
            std::vector<SpatialHashingObject<T>> finalObjects;
            std::vector<bool> included = std::vector<bool>(s);

            //determine what grids the object collides with.
            double radius = shape->generateBoundingRadius() + 1e-8; //Add some small offset to handle boundary conditions. Also handles points since they have no radius

            double x1 = shape->getPosition().x - radius;
            double y1 = shape->getPosition().y - radius;
            double x2 = shape->getPosition().x + radius;
            double y2 = shape->getPosition().y + radius;

            int startXGrid = MathExt::clamp( (int)MathExt::floor((x1/width) * xGrids), 0, xGrids);
            int startYGrid = MathExt::clamp( (int)MathExt::floor((y1/height) * yGrids), 0, yGrids);
            
            int endXGrid = MathExt::clamp( (int)MathExt::ceil((x2/width) * xGrids), 0, xGrids);
            int endYGrid = MathExt::clamp( (int)MathExt::ceil((y2/height) * yGrids), 0, yGrids);

            double xInc = width/xGrids;
            double yInc = height/yGrids;
            Box2D boxShape;

            for(int y=startYGrid; y<endYGrid; y++)
            {
                boxShape.setTopBound(startYGrid*yInc);
                boxShape.setBottomBound((startYGrid+1) * yInc);

                for(int x=startXGrid; x<endXGrid; x++)
                {
                    boxShape.setLeftBound(startXGrid*xInc);
                    boxShape.setRightBound((startXGrid+1) * xInc);

                    if( CollisionMaster::getCollision( &boxShape, shape ) )
                    {
                        int gridIndex = x + y*xGrids;

                        for(SpatialHashingObject<T>& obj : objects[gridIndex])
                        {
                            if(obj.id < 0)
                                continue; //Error probably
                            
                            if(obj.id > s)
                                continue; //Error probably
                            
                            if(!included[obj.id])
                            {
                                finalObjects.push_back(obj);
                                included[obj.id] = true;
                            }
                        }
                    }
                }
            }

            return finalObjects;
        }

        void removeObject(int id)
        {
            //1 pass
            for(int i=0; i<yGrids; i++)
            {
                for(int j=0; j<xGrids; j++)
                {
                    std::vector<SpatialHashingObject<T>> newObjects;
                    for(SpatialHashingObject<T>& obj : objects[j + i*xGrids])
                    {
                        if(obj.id != id)
                        {
                            if(obj.id > id)
                            {
                                obj.id -= 1;
                            }
                            newObjects.push_back(obj);
                        }
                    }

                    if(newObjects.size() != objects.size())
                        objects[j + i*xGrids] = newObjects;
                }
            }
        }

        size_t size()
        {
            return s;
        }

    private:
        std::vector<std::vector<SpatialHashingObject<T>>> objects = std::vector<std::vector<SpatialHashingObject<T>>>();
        size_t s = 0;
        double width = 0;
        double height = 0;
        int xGrids = 0;
        int yGrids = 0;
    };
}