#pragma once
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <array>

namespace glib
{
    struct SFA_State
    {
        int len = 0;
        int firstPos = 0;
        int link = -1;
        std::array<unsigned int, 256> map = std::array<unsigned int, 256>();
    };

    struct SFA_State_Short
    {
        int len = 0;
        int firstPos = 0;
        int link = -1;
        std::array<unsigned short, 256> map = std::array<unsigned short, 256>();
    };

    class SuffixAutomaton
    {
    public:
        /**
         * @brief Construct a new Suffix Automaton object
         *      Can find suffixes in O(N) time after being built.
         *      Takes O(NK) to store where K is the characterSetSize which is constant.
         *          K is equal to 256 which is a byte in size.
         * 
         * @param s
         *      The expected size of the string.
         *      Used to initialize the state array.
         *      Can be larger than the expected string size but can never be smaller otherwise, an error occurs.
         */
        SuffixAutomaton(size_t s);

        /**
         * @brief Destroy the Suffix Automaton object
         *      Deletes the state array
         */
        ~SuffixAutomaton();
        
        /**
         * @brief Resets the Suffix Automaton to its initial state. Useful if recreating the
         *      suffix automaton is necessary.
         * 
         */
        void reset();

        /**
         * @brief Extends the Suffix Automaton with a new character c.
         *      This is equivalent to concatenation on the end of a string.
         *      Can not be undone.
         * 
         * @param c 
         */
        void extend(unsigned char c);

        /**
         * @brief Get the Root State of the Suffix Automaton.
         *      This is equivalent to getState(0)
         * 
         * @return SFA_State* 
         */
        SFA_State* getRootState();

        /**
         * @brief Gets a State from the list of states.
         * 
         * @param index 
         * @return SFA_State* 
         */
        SFA_State* getState(int index);

        /**
         * @brief Returns a state by traversing from the specified state with desired character.
         *      If it returns 0, the transition does not exist and that particular substring does not exist.
         * 
         * @param startP 
         *      Must be a valid state index. Will not check if it isn't.
         *      
         * @param character 
         *      The desired character that is next in search substring.
         *      Must be in the valid range of the characterSetSize
         *      returns 0 if it is not.
         * 
         * @return int 
         */
        int traverse(int startP, unsigned char character);

        /**
         * @brief Returns the total number of values added to the Suffix Automaton.
         * 
         * @return size_t 
         */
        size_t size();

        /**
         * @brief Returns the total number of states stored.
         *      Not equivalent to how many characters were added / the length of the largest suffix.
         *      Size bounded to O(2N-1)
         * 
         * @return size_t 
         */
        size_t getTotalStatesCreated();
        
    private:
        int last = 0;
        int root = 0;
        size_t charsAdded = 0;
        size_t stateSize = 0;
        SFA_State* states = nullptr;
    };

    class ShortSuffixAutomaton
    {
    public:
        /**
         * @brief Construct a new Suffix Automaton object
         *      Can find suffixes in O(N) time after being built.
         *      Takes O(NK) to store where K is the characterSetSize which is constant.
         *          K is equal to 256 which is a byte in size.
         *      
         *      Unlike with the normal suffix Automaton, this uses unsigned shorts instead of unsigned ints
         *          to store the map for each node. This results in faster processing in exchange for maximum
         *          size of the Suffix Automaton before problems arise.
         * 
         * @param s
         *      The expected size of the string.
         *      Used to initialize the state array.
         *      Can be larger than the expected string size but can never be smaller otherwise, an error occurs.
         */
        ShortSuffixAutomaton(size_t s);

        /**
         * @brief Destroy the Suffix Automaton object
         *      Deletes the state array
         */
        ~ShortSuffixAutomaton();
        
        /**
         * @brief Resets the Suffix Automaton to its initial state. Useful if recreating the
         *      suffix automaton is necessary.
         * 
         */
        void reset();

        /**
         * @brief Extends the Suffix Automaton with a new character c.
         *      This is equivalent to concatenation on the end of a string.
         *      Can not be undone.
         * 
         * @param c 
         */
        void extend(unsigned char c);

        /**
         * @brief Get the Root State of the Suffix Automaton.
         *      This is equivalent to getState(0)
         * 
         * @return SFA_State_Short* 
         */
        SFA_State_Short* getRootState();

        /**
         * @brief Gets a State from the list of states.
         * 
         * @param index 
         * @return SFA_State_Short* 
         */
        SFA_State_Short* getState(int index);

        /**
         * @brief Returns a state by traversing from the specified state with desired character.
         *      If it returns 0, the transition does not exist and that particular substring does not exist.
         * 
         * @param startP 
         *      Must be a valid state index. Will not check if it isn't.
         *      
         * @param character 
         *      The desired character that is next in search substring.
         *      Must be in the valid range of the characterSetSize
         *      returns 0 if it is not.
         * 
         * @return int 
         */
        int traverse(int startP, unsigned char character);

        /**
         * @brief Returns the total number of values added to the Suffix Automaton.
         * 
         * @return size_t 
         */
        size_t size();

        /**
         * @brief Returns the total number of states stored.
         *      Not equivalent to how many characters were added / the length of the largest suffix.
         *      Size bounded to O(2N-1)
         * 
         * @return size_t 
         */
        size_t getTotalStatesCreated();
        
    private:
        int last = 0;
        int root = 0;
        size_t charsAdded = 0;
        size_t stateSize = 0;
        SFA_State_Short* states = nullptr;
    };

    class ChainedSuffixAutomaton
    {
    public:
        ChainedSuffixAutomaton(int maxSize, int individualSize, int overlap);
        ~ChainedSuffixAutomaton();

        void extend(unsigned char  c);
        bool searchNext(unsigned char  c);
        void resetSearch();
        std::pair<int, int> extractSearch();
        void printStuff();

    private:
        int overlap = 258;
        int maxSize = 32768;
        int individualSize = 4096;
        int totalAmount = 0;
        int currOffset = 0;
        int currIndex = 0;
        int lastIndex = -1;
        int currSearchLen = 0;
        std::vector<int> offsets;
        std::vector<ShortSuffixAutomaton*> SAs;
        std::vector<int> searchStates;
        int lastGoodState = 0;
        std::pair<int, int> currMatch;
    };

}
