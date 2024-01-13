#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <algorithm>
#include <cmath>
#include "MathExt.h"

namespace glib
{
    template<typename T>
    struct HANode
    {
        std::array<uint32_t, 256> connections;
        uint64_t counter = 0;
    };

    template<typename T>
    class StaticHAIndex
    {
    public:

        /**
         * @brief Construct a new Static HA Index object
         *      Based on http://merlintang.github.io/paper/EDBT-Hamming-V2.pdf
         *          or http://openproceedings.org/2015/conf/edbt/paper-263.pdf
         *      
         *      Designed specifically around Hamming-Distance
         * 
         * @param segmentSize
         *      The size of each split used when traversing the graph.
         *          Larger sizes reduce redundacy at the cost of more storage and search time.
         *          Smaller sizes can increase redundacy but lower storage and search time.
         *      No larger than 8. Should be even
         */
        StaticHAIndex(uint8_t segmentSize);

        /**
         * @brief Destroy the Static HA Index object
         *      Assume that all functions will no longer work and cause memory errors.
         *      Assume that all data from the object is no longer valid and deleted.
         */
        ~StaticHAIndex();

        /**
         * @brief Inserts data into the StaticHAIndex
         * 
         * @param data 
         * @param hash 
         */
        void insert(T data, uint64_t hash);

        /**
         * @brief Removes data from the StaticHAIndex
         * 
         * @param data 
         * @param hash 
         */
        void remove(T data, uint64_t hash);

        /**
         * @brief Gets Similar hashes based on the hamming distance from the given hash.
         *      The max distance is how different a hash can be before it is rejected.
         *          The distance should be chosen based on the hash used.
         * 
         *      The hashes returned are sorted by hamming distance where the front has the lowest distance.
         *      
         * @param hash 
         * @param maxDistance 
         * @return std::vector<uint64_t> 
         */
        std::vector<uint64_t> getSimilarHashes(uint64_t hash, unsigned char maxDistance);

        /**
         * @brief Gets Similar Data based on the hamming distance from the given hash.
         *      The max distance is how different a hash can be before it is rejected.
         *          The distance should be chosen based on the hash used.
         * 
         *      The data returned is sorted by hamming distance where the front has the lowest distance.
         *          This returns all data which may be desired. If only say the top 10 is desired,
         *          use getSimilarHashes() and grab the first 10 from the multimap from getRawDataList()
         *      
         * 
         * @param hash 
         * @param maxDistance 
         * @return std::vector<T> 
         */
        std::vector<T> getSimilarData(uint64_t hash, unsigned char maxDistance);

        /**
         * @brief Set whether to Prevent Duplicate Insertions
         *      Does not affect previous insertions that may have duplicates.
         * 
         *      Default is true
         * 
         * @param b
         */
        void setPreventDuplicateInsert(bool b);

        std::multimap<uint64_t, T>& getRawDataList();

    private:
        void recursiveTraverse(uint64_t hash, unsigned char maxDistance, int bitsProc, uint64_t runningHash, std::vector<uint64_t>& retResult, HANode<T>* currNode);

        bool checkForDuplicateInsert = true;
        uint8_t segmentSize;
        int layers;
        int width;
        std::vector<HANode<T>*> nodes;
        std::multimap<uint64_t, T> dataList;
        std::set<T> dataListToCheckForDuplicates;
    };


    template<typename T>
    inline StaticHAIndex<T>::StaticHAIndex(uint8_t segmentSize)
    {
        this->segmentSize = segmentSize;
        layers = (int)std::ceil(64.0 / segmentSize);
        width = 1<<segmentSize;

        nodes = std::vector<HANode<T>*>((width * layers) + 1);

        //create root
        nodes[0] = new HANode<T>();
    }

    template<typename T>
    inline StaticHAIndex<T>::~StaticHAIndex()
    {
        for(HANode<T>* n : nodes)
        {
            if(n!=nullptr)
                delete n;
        }
        nodes.clear();
    }

    template<typename T>
    inline void StaticHAIndex<T>::setPreventDuplicateInsert(bool b)
    {
        checkForDuplicateInsert = b;
    }

    template<typename T>
    inline void StaticHAIndex<T>::insert(T data, uint64_t hash)
    {
        //traverse till can't anymore.
        //Add nodes if they don't exist.
        //Add to last node moved to.
        bool shouldInsert = true;
        if(checkForDuplicateInsert)
        {
            //check if already there.
            auto it = dataListToCheckForDuplicates.find(data);
            if(it != dataListToCheckForDuplicates.end())
                return; //duplicate. Don't insert. Must remove first if replacing
        }

        if(shouldInsert)
        {
            dataList.insert( {hash, data} );
            dataListToCheckForDuplicates.insert(data);
        }

        HANode<T>* currNode = nodes.front();
        int bitsProc = 0;
        int bitwiseAndValue = (1<<segmentSize)-1;
        int currLayer = 0;

        if(currNode == nullptr)
        {
            //problem.
            return;
        }
        
        while(bitsProc < 64)
        {
            uint8_t cmpValue = (hash >> bitsProc) & bitwiseAndValue;
            uint64_t indexToGoto = currNode->connections[cmpValue];

            if(indexToGoto == 0)
            {
                //create a new connection
                int nIndex = 1 + (currLayer * width) + cmpValue;
                currNode->connections[cmpValue] = nIndex;

                //check if the node exist before creating a new node
                if(nodes[nIndex] == nullptr)
                {
                    HANode<T>* newHANode = new HANode<T>();
                    nodes[nIndex] = newHANode;
                }

                currNode = nodes[nIndex];
            }
            else
            {
                currNode = nodes[indexToGoto];
            }
            
            currNode->counter++;
            currLayer++;

            bitsProc += segmentSize;
        }
    }


    template<typename T>
    inline void StaticHAIndex<T>::remove(T data, uint64_t hash)
    {
        //check if already there.
        bool exist = false;
        auto existIterator = dataListToCheckForDuplicates.find(data);
        if(existIterator != dataListToCheckForDuplicates.end())
            exist = true;

        if(exist)
        {
            dataListToCheckForDuplicates.erase( data );

            auto iteratorPair = dataList.equal_range(hash);
            auto it = iteratorPair.first;
            for(it = iteratorPair.first; it!=iteratorPair.second; it++)
            {
                if(it->second == data)
                {
                    break;
                }
            }
            dataList.erase( it );

            HANode<T>* currNode = nodes.front();
            HANode<T>* lastNode = nodes.front();
            int bitsProc = 0;
            uint64_t bitwiseAndValue = (1<<segmentSize)-1;
            std::vector<uint32_t> nodesToDelete;

            if(currNode == nullptr)
            {
                //problem.
                return;
            }

            while(bitsProc < 64)
            {
                lastNode = currNode;
                uint8_t cmpValue = (hash >> bitsProc) & bitwiseAndValue;
                uint64_t indexToGoto = currNode->connections[cmpValue];

                if(indexToGoto == 0)
                {
                    //not found
                    currNode = nullptr;
                    break;
                }
                else
                {
                    currNode = nodes[indexToGoto];
                }

                currNode->counter--;
                if(currNode->counter <= 0)
                {
                    //remove connection from previous node
                    nodesToDelete.push_back(lastNode->connections[cmpValue]);
                    lastNode->connections[cmpValue] = 0;
                }
                bitsProc += segmentSize;
            }

            for(uint32_t index : nodesToDelete)
            {
                if(nodes[index] != nullptr)
                    delete nodes[index];
                nodes[index] = nullptr;
            }
        }
    }

    template<typename T>
    std::vector<uint64_t> StaticHAIndex<T>::getSimilarHashes(uint64_t hash, unsigned char maxDistance)
    {
        std::vector<uint64_t> validHashes;
        HANode<T>* rootNode = nodes.front();

        recursiveTraverse(hash, maxDistance, 0, 0, validHashes, rootNode);

        //sort the hashes
        std::sort(validHashes.begin(), validHashes.end(), [hash](uint64_t a, uint64_t b) { return MathExt::hammingDistance(hash, a) < MathExt::hammingDistance(hash, b); } );

        return validHashes;
    }

    template<typename T>
    std::vector<T> StaticHAIndex<T>::getSimilarData(uint64_t hash, unsigned char maxDistance)
    {
        std::vector<T> similarDataList;
        std::vector<uint64_t> validHashes = getSimilarHashes(hash, maxDistance);
        for(uint64_t h : validHashes)
        {
            auto iteratorPair = dataList.equal_range(h);
            for(auto it = iteratorPair.first; it != iteratorPair.second; it++)
            {
                similarDataList.push_back(it->second);
            }
        }

        return similarDataList;
    }

    template<typename T>
    void StaticHAIndex<T>::recursiveTraverse(uint64_t hash, unsigned char maxDistance, int bitsProc, uint64_t runningHash, std::vector<uint64_t>& retResult, HANode<T>* currNode)
    {
        if(currNode != nullptr && bitsProc < 64)
        {
            uint64_t bitwiseAndValue = (1<<segmentSize)-1;
            uint8_t cmpValue = (hash >> bitsProc) & bitwiseAndValue;

            for(uint64_t i=0; i<currNode->connections.size(); i++)
            {
                if(currNode->connections[i] != 0)
                {
                    //valid connection. Check hamming distance
                    int dis = MathExt::hammingDistance(cmpValue, (uint8_t)i);
                    uint64_t newRunningHash = runningHash + (i << bitsProc);

                    if(dis > maxDistance)
                        continue; //skip this node
                    
                    HANode<T>* nextNode = nodes[ currNode->connections[i] ];
                    recursiveTraverse(hash, maxDistance-dis, bitsProc+segmentSize, newRunningHash, retResult, nextNode);
                }
            }
        }
        else if(currNode != nullptr && bitsProc >= 64)
        {
            //probably an end node
            retResult.push_back(runningHash);
        }
    }

    template<typename T>
    std::multimap<uint64_t, T>& StaticHAIndex<T>::getRawDataList()
    {
        return dataList;
    }
};