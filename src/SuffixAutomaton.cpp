#include "SuffixAutomaton.h"
#include "System.h"

namespace smpl
{
    SuffixAutomaton::SuffixAutomaton(size_t s)
    {
        //create init state. No need to init the mapping array yet. Only needed after adding something to it.
        states = new SFA_State[s*2];
        memset(states[0].map, 0, 1024);
        states[0].len = 0;
        states[0].link = -1;
        states[0].isClone = false;
        states[0].invLink.clear();
        
        stateSize = 1;
        charsAdded = 0;
        last = 0;
    }

    SuffixAutomaton::~SuffixAutomaton()
    {
        delete[] states;
        states = nullptr;
        stateSize = 0;
        charsAdded = 0;
        last = -1;
    }

    void SuffixAutomaton::reset()
    {
        memset(states[0].map, 0, 1024);
        states[0].len = 0;
        states[0].link = -1;
        states[0].isClone = false;
        states[0].invLink.clear();
        stateSize = 1;
        last = 0;
        charsAdded = 0;
    }

    void SuffixAutomaton::extend(unsigned char c)
    {
        charsAdded++;
        int curr = stateSize;
        memset(states[curr].map, 0, 1024);
        states[curr].len = states[last].len + 1;
        states[curr].firstPos = states[last].len;
        states[curr].isClone = false;
        states[curr].invLink.clear();
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
                states[cloneIndex].firstPos = states[q].firstPos;
                states[cloneIndex].link = states[q].link;
                memcpy(states[cloneIndex].map, states[q].map, 1024);
                states[cloneIndex].len = states[p].len + 1;
                states[cloneIndex].isClone = true;
                states[cloneIndex].invLink.clear();
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
    
    void SuffixAutomaton::mapAllPositions()
    {
        for(int i=1; i<stateSize; i++)
        {
            states[states[i].link].invLink.push_back(i);
        }
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
    
    std::vector<int> SuffixAutomaton::getAllOccurences(int v, int length)
    {
        std::vector<int> output;
        getAllOccurences(v, length, output);
        return output;
    }
    void SuffixAutomaton::getAllOccurences(int v, int length, std::vector<int>& output)
    {
        if(!states[v].isClone)
            output.push_back( states[v].firstPos - length + 1 );
        
        for(int u : states[v].invLink)
        {
            getAllOccurences(u, length, output);
        }
    }

    int SuffixAutomaton::traverse(int startP, unsigned char character)
    {
        return states[startP].map[character];
    }

    ShortSuffixAutomaton::ShortSuffixAutomaton(size_t s)
    {
        //create init state. No need to init the mapping array yet. Only needed after adding something to it.
        states = new SFA_State_Short[s*2];
        memset(states[0].map, 0, 512);
        states[0].len = 0;
        states[0].link = -1;
        stateSize = 1;
        charsAdded = 0;
        last = 0;
    }

    ShortSuffixAutomaton::~ShortSuffixAutomaton()
    {
        delete[] states;
        states = nullptr;
        stateSize = 0;
        charsAdded = 0;
        last = -1;
    }

    void ShortSuffixAutomaton::reset()
    {
        memset(states[0].map, 0, 512);
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
        memset(states[curr].map, 0, 512);
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
                //states[cloneIndex] = states[q];
                states[cloneIndex].firstPos = states[q].firstPos;
                states[cloneIndex].link = states[q].link;
                memcpy(states[cloneIndex].map, states[q].map, 512);
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

    ChainedSuffixAutomaton::ChainedSuffixAutomaton(int maxSize, int totalBuffers, int maxMatchLength)
    {
        this->maxSize = maxSize;
        this->overlap = maxMatchLength + 1;
        totalAmount = __max(totalBuffers, 2);
        this->individualSize = maxSize / totalAmount;

        //create all possible automatons
        for (int i = 0; i < totalAmount; i++)
        {
            SAs.push_back(new ShortSuffixAutomaton(individualSize + overlap));
            searchStates.push_back(0);
            offsets.push_back(0);
        }

        currIndex = 0;
        lastIndex = totalAmount - 1;
    }

    ChainedSuffixAutomaton::~ChainedSuffixAutomaton()
    {
        for (int i = 0; i < SAs.size(); i++)
            if (SAs[i] != nullptr)
                delete SAs[i];
    }

    int ChainedSuffixAutomaton::extend(unsigned char c)
    {
        int i = currIndex;
        SAs[i]->extend(c);
        if (currOffset < offsets[lastIndex] + individualSize + overlap)
            SAs[lastIndex]->extend(c);

        currOffset++;
        if ((currOffset % individualSize) == 0)
        {
            lastIndex = currIndex;
            currIndex = (currIndex + 1) % totalAmount;

            //replace
            offsets[currIndex] = currOffset;
            SAs[currIndex]->reset();
            searchStates[currIndex] = -1;
            return currIndex;
        }
        return -1;
    }

    bool ChainedSuffixAutomaton::searchNext(unsigned char c)
    {
        //do it for every Suffix Automaton
        int threadMem[24];
        for(int i=0; i<SAs.size(); i++) { threadMem[i] = -1; }

        bool ok = false;
        for (int i = 0; i < SAs.size(); i++)
        {
            int sState = searchStates[i];
            if (sState == -1)
                continue;

            sState = SAs[i]->traverse(sState, c);
            int actualPos = offsets[i] + (SAs[i]->getState(sState)->firstPos - currSearchLen);
            if (sState > 0)
            {
                threadMem[i] = actualPos;
                searchStates[i] = sState;
            }
            else
            {
                threadMem[i] = -1;
                searchStates[i] = -1;
            }
        }

        for (int i = 0; i < SAs.size(); i++)
        {
            //find 1 that is valid
            if (threadMem[i] >= 0)
            {
                currMatch.start = threadMem[i];
                currMatch.length = currSearchLen+1;
                currSearchLen++;
                return true;
            }
        }
        return false;
    }

    void ChainedSuffixAutomaton::resetSearch()
    {
        currMatch = { 0, 0 };
        currSearchLen = 0;
        for (int i = 0; i < SAs.size(); i++)
        {
            searchStates[i] = 0;
        }
    }

    SearchState ChainedSuffixAutomaton::extractSearch()
    {
        return currMatch;
    }

}