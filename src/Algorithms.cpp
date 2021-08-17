#include "Algorithms.h"
#include <System.h>

namespace glib
{

    std::vector<Grouping> Algorithms::packageMergeAlgorithm(std::vector<Grouping> baseItems, size_t desiredTotal)
    {
        std::vector<Grouping*> nL = std::vector<Grouping*>(baseItems.size());
        std::vector<Grouping> results;
        for(int i=0; i<baseItems.size(); i++)
        {
            nL[i] = new Grouping();
            nL[i]->weight = baseItems[i].weight;
            nL[i]->width = baseItems[i].width;
            nL[i]->valid = true;
            nL[i]->ids = baseItems[i].ids;
        }

        std::function<bool(Grouping*, Grouping*)> cmpFunc = [](Grouping* a, Grouping* b) -> bool{
            if(a->width == b->width)
            {
                return a->weight < b->weight;
            }
            else
            {
                return a->width < b->width;
            }
        };

        Sort::mergeSort<Grouping*>(nL.data(), nL.size(), cmpFunc);

        size_t X = desiredTotal;
        size_t minDiadicValue = 0;
        int startBitIndex = 0;
        for(int i=0; i<64; i++)
        {
            size_t expVal = 1;
            expVal <<= i;
            size_t bitValue = X & expVal;
            if(bitValue != 0)
            {
                minDiadicValue = expVal;
                startBitIndex = i;
                break;
            }
        }

        size_t totalRemoved = 0;

        //find minDiadic value
        while(X > 0)
        {
            if(totalRemoved == nL.size())
            {
                StringTools::println("No solution no items. TotalRemoved = %llu", totalRemoved);
                return std::vector<Grouping>(); // No solution
            }
            
            int location = -1;
            for(int i=0; i<nL.size(); i++)
            {
                if(nL[i]->valid == true)
                {
                    if(nL[i]->width <= minDiadicValue)
                    {
                        if(location==-1)
                        {
                            location = i;
                        }
                        else
                        {
                            if(nL[i]->width == nL[location]->width)
                            {
                                if(nL[i]->weight < nL[location]->weight)
                                {
                                    location = i;
                                }
                            }
                            else
                            {
                                if(nL[i]->width < nL[location]->width)
                                    location = i;
                            }
                        }
                    }
                }
            }

            size_t baseWidthValue = nL[location]->width;

            if(location < 0)
            {
                //couldn't find required width or a value lower than it.
                //no solution
                StringTools::println("No solution, Couldn't find location");
                return std::vector<Grouping>();
            }

            if(nL[location]->width == minDiadicValue)
            {
                //add to solution
                results.push_back(*nL[location]);

                //remove from list
                nL[location]->valid = false; //remove 
                totalRemoved++;

                X -= minDiadicValue;

                for(int i=startBitIndex; i<64; i++)
                {
                    size_t expVal = 1;
                    expVal <<= i;
                    size_t bitValue = X & expVal;
                    if(bitValue != 0)
                    {
                        minDiadicValue = expVal;
                        startBitIndex = i;
                        break;
                    }
                }
            }

            //package and merge

            //Package
            std::vector<Grouping*> packs = std::vector<Grouping*>();
            
            int p1Index = -1;
            int p2Index = -1;

            for(int i=0; i<nL.size(); i++)
            {
                if(nL[i]->width == baseWidthValue && nL[i]->valid == true)
                {
                    if(p1Index == -1)
                    {
                        p1Index = i;
                    }
                    else
                    {
                        p2Index = i;

                        //group
                        Grouping* g = new Grouping();
                        g->width = baseWidthValue * 2;
                        g->weight = nL[p1Index]->weight + nL[p2Index]->weight;
                        
                        for(int& id : nL[p1Index]->ids)
                        {
                            g->ids.push_back(id);
                        }
                        for(int& id : nL[p2Index]->ids)
                        {
                            g->ids.push_back(id);
                        }

                        g->valid = true;
                        packs.push_back(g);
                        
                        //remove those items
                        nL[p1Index]->valid = false;
                        nL[p2Index]->valid = false;
                        totalRemoved+=2;

                        p1Index = -1;
                        p2Index = -1;
                    }
                }
            }

            if(p1Index!=-1)
            {
                nL[p1Index]->valid = false;
                totalRemoved++;
            }

            //merge
            for(Grouping* m : packs)
            {
                nL.push_back(m);
            }

            Sort::mergeSort<Grouping*>(nL.data(), nL.size(), cmpFunc);
        }
        
        for(int i=0; i<nL.size(); i++)
        {
            delete nL[i];
            nL[i] = nullptr;
        }

        nL.clear();
        return results;
    }

} //NAMESPACE glib END
