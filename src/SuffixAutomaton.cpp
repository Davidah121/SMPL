#include "SuffixAutomaton.h"
#include "System.h"

namespace glib
{
    SuffixAutomaton::SuffixAutomaton(size_t s)
    {
        //create init state. No need to init the mapping array yet. Only needed after adding something to it.
        states = new SFA_State[s*2];
        states[0].map.fill(0);
        states[0].len = 0;
        states[0].link = -1;
        stateSize = 1;
        charsAdded = 0;
        last = 0;
    }

    SuffixAutomaton::~SuffixAutomaton()
    {
        delete states;
        states = nullptr;
        stateSize = 0;
        charsAdded = 0;
        last = -1;
    }

    void SuffixAutomaton::reset()
    {
        states[0].map.fill(0);
        states[0].len = 0;
        states[0].link = -1;
        stateSize = 1;
        last = 0;
        charsAdded = 0;
    }

    void SuffixAutomaton::extend(unsigned char c)
    {
        charsAdded++;
        int curr = stateSize;
        states[curr].map.fill(0);
        states[curr].len = states[last].len + 1;
        states[curr].firstPos = states[last].len;
        stateSize++;

        int p = last;
        while (p != -1 && !states[p].map[c])
        {
            states[p].map[c] = curr;
            p = states[p].link;
        }

        if(p == -1)
        {
            states[curr].link = 0;
        }
        else
        {
            int q = states[p].map[c];
            if(states[p].len + 1 == states[q].len)
            {
                states[curr].link = q;
            }
            else
            {
                //clone
                int cloneIndex = stateSize;
                states[cloneIndex] = states[q];
                states[cloneIndex].len = states[p].len + 1;
                stateSize++;

                //Change things that pointed to stateQ to point to the clone
                while(p != -1 && states[p].map[c] == q)
                {
                    states[p].map[c] = cloneIndex;
                    p = states[p].link;
                }

                //set the links to go to the clone
                states[curr].link = cloneIndex;
                states[q].link = cloneIndex;
            }
        }
    
        last = curr;
    }

    SFA_State* SuffixAutomaton::getRootState()
    {
        return &states[0];
    }
    
    size_t SuffixAutomaton::size()
    {
        return charsAdded;
    }
    
    size_t SuffixAutomaton::getTotalStatesCreated()
    {
        return stateSize;
    }

    SFA_State* SuffixAutomaton::getState(int index)
    {
        return &states[index];
    }

    int SuffixAutomaton::traverse(int startP, unsigned char character)
    {
        return states[startP].map[character];
    }

    ShortSuffixAutomaton::ShortSuffixAutomaton(size_t s)
    {
        //create init state. No need to init the mapping array yet. Only needed after adding something to it.
        states = new SFA_State_Short[s*2];
        states[0].map.fill(0);
        states[0].len = 0;
        states[0].link = -1;
        stateSize = 1;
        charsAdded = 0;
        last = 0;
    }

    ShortSuffixAutomaton::~ShortSuffixAutomaton()
    {
        delete states;
        states = nullptr;
        stateSize = 0;
        charsAdded = 0;
        last = -1;
    }

    void ShortSuffixAutomaton::reset()
    {
        states[0].map.fill(0);
        states[0].len = 0;
        states[0].link = -1;
        stateSize = 1;
        charsAdded = 0;
        last = 0;
    }

    void ShortSuffixAutomaton::extend(unsigned char c)
    {
        charsAdded++;
        int curr = stateSize++;
        states[curr].map.fill(0);
        states[curr].len = states[last].len + 1;
        states[curr].firstPos = states[last].len;

        int p = last;
        while (p != -1 && !states[p].map[c])
        {
            states[p].map[c] = curr;
            p = states[p].link;
        }

        if(p == -1)
        {
            states[curr].link = 0;
        }
        else
        {
            int q = states[p].map[c];
            if(states[p].len + 1 == states[q].len)
            {
                states[curr].link = q;
            }
            else
            {
                //clone
                int cloneIndex = stateSize++;
                states[cloneIndex] = states[q];
                states[cloneIndex].len = states[p].len + 1;

                //Change things that pointed to stateQ to point to the clone
                while(p != -1 && states[p].map[c] == q)
                {
                    states[p].map[c] = cloneIndex;
                    p = states[p].link;
                }

                //set the links to go to the clone
                states[curr].link = cloneIndex;
                states[q].link = cloneIndex;
            }
        }
    
        last = curr;
    }

    SFA_State_Short* ShortSuffixAutomaton::getRootState()
    {
        return &states[0];
    }
    
    size_t ShortSuffixAutomaton::size()
    {
        return charsAdded;
    }
    
    size_t ShortSuffixAutomaton::getTotalStatesCreated()
    {
        return stateSize;
    }

    SFA_State_Short* ShortSuffixAutomaton::getState(int index)
    {
        return &states[index];
    }

    int ShortSuffixAutomaton::traverse(int startP, unsigned char character)
    {
        return states[startP].map[character];
    }

    ChainedSuffixAutomaton::ChainedSuffixAutomaton(int maxSize, int individualSize, int overlap)
    {
        this->maxSize = maxSize;
        this->individualSize = individualSize;
        this->overlap = overlap;
        
        totalAmount = (maxSize/individualSize) - 1;

        //create first
        SAs.push_back( new ShortSuffixAutomaton(individualSize+overlap) );
        searchStates.push_back(0);
        offsets.push_back(0);
        currIndex = 0;
    }

    ChainedSuffixAutomaton::~ChainedSuffixAutomaton()
    {
        for(int i=0; i<SAs.size(); i++)
            delete SAs[i];
    }

    int ChainedSuffixAutomaton::extend(unsigned char c)
    {
        int i = currIndex;

        SAs[i]->extend(c);
        if(lastIndex >= 0 && currOffset < offsets[lastIndex] + individualSize + overlap)
            SAs[lastIndex]->extend(c);
        
        currOffset++;
        if((currOffset % individualSize) == 0)
        {
            lastIndex = currIndex;
            currIndex = (currIndex + 1) % totalAmount;

            if(currIndex >= SAs.size())
            {
                //extend
                offsets.push_back(currOffset);
                SAs.push_back(new ShortSuffixAutomaton(individualSize+overlap));
                searchStates.push_back(-1);
                return searchStates.size()-1;
            }
            else
            {
                //replace
                offsets[currIndex] = currOffset;
                SAs[currIndex]->reset();
                searchStates[currIndex] = -1;
                return currIndex;
            }
        }
        return -1;
    }

    bool ChainedSuffixAutomaton::searchNext(unsigned char c)
    {
        //do it for every Suffix Automaton
        bool ok = false;
        for(int i=0; i<SAs.size(); i++)
        {
            int sState = searchStates[i];

            if (sState == -1)
                continue;

            sState = SAs[i]->traverse(sState, c);
            if (sState > 0)
            {
                int actualPos = offsets[i] + (SAs[i]->getState(sState)->firstPos - currSearchLen);
                // bool shouldUpdate = (currMatch.second < currSearchLen+1) || (currMatch.first < actualPos);
                bool shouldUpdate = true;
                if(shouldUpdate)
                {
                    currMatch = { i, actualPos, currSearchLen + 1 };
                    lastGoodState = i;
                }
                ok = true;
            }
            else
                sState = -1;

            searchStates[i] = sState;
        }
        if(ok == true)
        {
            currSearchLen++;
        }
        if(currMatch.length >= overlap)
            ok = false;

        return ok;
    }

    void ChainedSuffixAutomaton::resetSearch()
    {
        currMatch = {0, 0};
        currSearchLen = 0;
        for(int i=0; i<SAs.size(); i++)
        {
            searchStates[i] = 0;
        }
    }

    SearchState ChainedSuffixAutomaton::extractSearch()
    {
        return currMatch;
    }

    void ChainedSuffixAutomaton::printStuff()
    {
        StringTools::println("CurrOffset: %d", currOffset);
        StringTools::println("CurrIndex: %d", currIndex);
        StringTools::println("LastIndex: %d", lastIndex);
        StringTools::println("Last State Index Thing: %d", lastGoodState);
        
        for(int i=0; i<SAs.size(); i++)
        {
            StringTools::println("Size of SA[%d] = %llu", i, SAs[i]->size());
            StringTools::println("OFFSET = %d", offsets[i]);
            StringTools::println("\tRootState = %p", SAs[i]->getRootState());
            StringTools::println("\tCurrentState = %p", searchStates[i]);
        }
    }

}