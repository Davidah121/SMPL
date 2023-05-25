#pragma once
#include <iostream>
#include <vector>

struct TrieNode
{
    std::vector<TrieNode> children;
};

class SuffixTrie
{
public:
    /**
     * @brief Construct a new Suffix Trie object.
     *      The alphabet size can be no larger than 255 (1 byte in size).
     *          If necessary, convert your data type into bytes.
     * 
     * @param alphabetSize 
     */
    SuffixTrie(uint8_t alphabetSize);

    /**
     * @brief Destroy the Suffix Trie object
     * 
     */
    ~SuffixTrie();

    /**
     * @brief Adds data to the suffix trie starting from the root.
     *      O(M) where M is the input data size.
     * 
     * @param data 
     * @param size 
     */
    void add(uint8_t* data, uint64_t size);

    /**
     * @brief Searches for the data in the suffix trie starting from the root.
     *      O(M) where M is the input data size.
     * 
     * @param data 
     * @param size 
     * @return true 
     * @return false 
     */
    bool search(uint8_t* data, uint64_t size);

    /**
     * @brief Removes the data from the suffix trie if it exists.
     *      O(M) where M is the input data size.
     * 
     * @param data 
     * @param size 
     */
    void remove(uint8_t* data, uint64_t size);

    /**
     * @brief Removes all data from the suffix trie.
     * 
     */
    void clear();

    /**
     * @brief Gets the root of the trie.
     *          Useful for manual parsing (parsing a stream of data).
     * 
     * @return TrieNode* 
     */
    TrieNode* getRoot();
private:
    TrieNode* root;
    int alphabetSize;
};