#pragma once
#include "LinkedList.h"
#include <vector>

namespace smpl
{
    template<typename T>
    class Queue
    {
    public:
        Queue() {}
        ~Queue() {}

        void add(T data)
        {
            buffer.addNode(data);
            totalSize++;
        }

        /**
         * @brief Removes from the front of the list.
         * 
         */
        void popFront()
        {
            if(buffer.getRootNode() != nullptr)
            {
                buffer.removeFirst();
                totalSize--;
            }
            // if(buffer.getRootNode() != nullptr)
            // {
            //     buffer.removeNode(buffer.getRootNode());
            //     totalSize--;
            // }
        }

        /**
         * @brief Removes from the end of the list.
         * 
         */
        void popBack()
        {
            if(buffer.getLastNode() != nullptr)
            {
                buffer.removeNode(buffer.getLastNode());
                totalSize--;
            }
        }

        /**
         * @brief Finds the data in the list if it exists.
         *      Returns true if it was found.
         * 
         * @param data 
         * @return true 
         * @return false 
         */
        bool find(T data)
        {
            LinkNode<T>* currNode = buffer.getRootNode();
            while(currNode != nullptr)
            {
                if(currNode->value == data)
                    break;
                currNode = currNode->nextNode;
            }
            return currNode != nullptr;
        }

        /**
         * @brief Removes the first instance of the data found.
         *      Returns if successful.
         * 
         * @param data 
         * @return true 
         * @return false 
         */
        bool erase(T data)
        {
            LinkNode<T>* nodeToRemove = nullptr;
            LinkNode<T>* currNode = buffer.getRootNode();
            while(currNode != nullptr)
            {
                if(currNode->value == data)
                    break;
                currNode = currNode->nextNode;
            }
            buffer.removeNode(currNode);
            if(currNode != nullptr)
                totalSize--;
            return currNode != nullptr;
        }

        /**
         * @brief Removes all instances of the data found.
         *      Returns if successful.
         * 
         * @param data 
         * @return true 
         * @return false 
         */
        bool eraseAll(T data)
        {
            std::vector<LinkNode<T>*> nodesToRemove;
            LinkNode<T>* currNode = buffer.getRootNode();
            while(currNode != nullptr)
            {
                if(currNode->value == data)
                {
                    nodesToRemove.push_back(currNode);
                }
                currNode = currNode->nextNode;
            }
            for(int i=0; i<nodesToRemove.size(); i++)
            {
                buffer.removeNode(nodesToRemove[i]);
            }
            
            totalSize -= nodesToRemove.size();
            return nodesToRemove.size() > 0;
        }
        T get()
        {
            return buffer.getRootNode()->value;
        }
        void clear()
        {
            buffer.clear();
        }
        size_t size()
        {
            return totalSize;
        }
    private:
        LinkedList<T> buffer;
        size_t totalSize = 0;
    };
}
