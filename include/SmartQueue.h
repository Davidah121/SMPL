#pragma once
#include "SmartMemory.h"
#include "SmartLinkedList.h"

namespace smpl
{

    template<typename T>
    class SmartQueue
    {
    public:
        SmartQueue()
        {

        }

        ~SmartQueue()
        {
            sList.clear();
        }

        SmartMemory<SLinkNode<T>> add(T d)
        {
            knownSize++;
            return sList.addNode(d);
        }

        SmartMemory<SLinkNode<T>> get()
        {
            return sList.getRootNode();
        }

        void pop()
        {
            if(sList.getRootNode().getPointer() != nullptr)
            {
                sList.removeNode( sList.getRootNode() );
                knownSize--;
            }
        }

        void erase(SmartMemory<SLinkNode<T>> node)
        {
            if(sList.removeNode(node))
                knownSize--;
        }

        void clear()
        {
            sList.clear();
        }

        size_t size()
        {
            return knownSize;
        }

        bool empty()
        {
            return knownSize == 0;
        }
    private:
        SmartLinkedList<T> sList;
        size_t knownSize = 0;

    };
}