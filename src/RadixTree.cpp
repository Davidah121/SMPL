#include "RadixTree.h"

namespace smpl
{
    RadixTree::RadixTree()
    {
        //add empty root node
        tree.setRootNode(new TreeNode<std::pair<std::string, bool>>());
        tree.getRoot()->children.resize(256); //lazy way
    }

    RadixTree::~RadixTree()
    {
        
    }

    void RadixTree::insert(std::string input)
    {
        //traverse the root node
        //check the first char
        size_t index = 0;
        size_t totalCharProcessed = 0;
        size_t currNodeCharsProcessed = 0;
        int insertType = -1; //no insert if -1
        TreeNode<std::pair<std::string, bool>>* lastNode = nullptr;
        TreeNode<std::pair<std::string, bool>>* currNode = tree.getRoot();
        while(index < input.size())
        {
            if(currNodeCharsProcessed >= currNode->data.first.size())
            {
                int childIndex = input[index];
                currNodeCharsProcessed = 0;

                lastNode = currNode;
                if(currNode->children[childIndex] != nullptr)
                    currNode = currNode->children[childIndex];
                else
                {
                    insertType = 1; //full insert
                    break;
                }
            }
            else
            {
                //check if the characters match at the specified index
                if(input[index] == currNode->data.first[currNodeCharsProcessed])
                {
                    currNodeCharsProcessed++;
                    totalCharProcessed++;
                    index++;
                }
                else
                {
                    //mismatch. Split this node into 2
                    insertType = 3; //split insert
                    break;
                }
            }
        }

        //check if we ran out of characters but the number of characters processed in the current node is not all of them
        if(currNode != nullptr)
        {
            if(index >= input.size() && currNodeCharsProcessed > 0 && currNodeCharsProcessed < currNode->data.first.size())
            {
                //if the string was found but input.size() is smaller than the total stringSize
                //we should insert the smaller sub string by spliting the current node
                insertType = 2;
            }

            if(insertType == 1)
            {
                //insert the entire remaining string into a child node
                int childIndex = input[index];
                TreeNode<std::pair<std::string, bool>>* newNode = new TreeNode<std::pair<std::string, bool>>();
                newNode->children.resize(256);
                newNode->data.first = input.substr(index);
                newNode->data.second = true; //end of a string

                currNode->children[childIndex] = newNode;
            }
            else if(insertType == 2)
            {
                //current node needs to be split into 2. The input string is a substring of the node
                //being split (the substring of the input string I mean).
                int oldChildIndex = currNode->data.first[0];

                TreeNode<std::pair<std::string, bool>>* newParentNode = new TreeNode<std::pair<std::string, bool>>();
                newParentNode->children.resize(256);
                newParentNode->data.first = currNode->data.first.substr(0, currNodeCharsProcessed);
                newParentNode->data.second = true; //end of a string
                currNode->data.first = currNode->data.first.substr(currNodeCharsProcessed);

                int childIndex = currNode->data.first[0];
                newParentNode->children[childIndex] = currNode;
                lastNode->children[oldChildIndex] = newParentNode;
            }
            else if(insertType == 3)
            {
                //current node needs to be split into 2. Found a mismatch.
                int oldChildIndex = currNode->data.first[0];
                TreeNode<std::pair<std::string, bool>>* newParentNode = new TreeNode<std::pair<std::string, bool>>();
                newParentNode->children.resize(256);
                newParentNode->data.first = currNode->data.first.substr(0, currNodeCharsProcessed); //similar parts of input and existing substring
                newParentNode->data.second = false; //Not the end of a string. Just a prefix
                
                TreeNode<std::pair<std::string, bool>>* newSubNode = new TreeNode<std::pair<std::string, bool>>();
                newSubNode->children.resize(256);
                newSubNode->data.first = input.substr(index); //remainder of input string. (the substring to be inserted)
                newSubNode->data.second = true; //end of a string

                currNode->data.first = currNode->data.first.substr(currNodeCharsProcessed); //the mismatch of the found substring
                
                int child1Index = currNode->data.first[0];
                int child2Index = newSubNode->data.first[0];

                newParentNode->children[child1Index] = currNode;
                newParentNode->children[child2Index] = newSubNode;

                lastNode->children[oldChildIndex] = newParentNode;
            }
            else
            {
                //just make sure that the node is considered an end node
                currNode->data.second = true;
            }
        }
    }
    
    void RadixTree::remove(std::string input)
    {
        //do stuff
        //do stuff
        //traverse the root node
        //check the first char
        size_t index = 0;
        size_t currNodeCharsProcessed = 0;
        TreeNode<std::pair<std::string, bool>>* lastNode = nullptr;
        TreeNode<std::pair<std::string, bool>>* currNode = tree.getRoot();
        while(index < input.size())
        {
            if(currNodeCharsProcessed >= currNode->data.first.size())
            {
                int childIndex = input[index];
                currNodeCharsProcessed = 0;

                lastNode = currNode;
                if(currNode->children[childIndex] != nullptr)
                    currNode = currNode->children[childIndex];
                else
                {
                    //No next node that is along the correct path so does not exist.
                    return;
                }
            }
            else
            {
                //check if the characters match at the specified index
                if(input[index] == currNode->data.first[currNodeCharsProcessed])
                {
                    currNodeCharsProcessed++;
                    index++;
                }
                else
                {
                    //mismatch. Does not exist
                    return;
                }
            }
        }

        if(index >= input.size())
        {
            //potentially found it.
            if(currNode != nullptr && currNodeCharsProcessed >= currNode->data.first.size())
            {
                //the node we are on may or may not be the correct node. It must also be an end node
                if(currNode->data.second == true)
                {
                    //remove this node.
                    currNode->data.second = false;

                    //check if it has children. If so, keep the node. If not, completly remove it.
                    int childrenCount = 0;
                    int firstChildIndex = -1;
                    for(int i=0; i<256; i++)
                    {
                        if(currNode->children[i] != nullptr)
                        {
                            firstChildIndex = (firstChildIndex == -1) ? i : firstChildIndex;
                            childrenCount++;
                        }
                    }

                    if(childrenCount == 0)
                    {
                        //delete it and remove from parent. If parent only has one child, merge its children into it.
                        int childIndex = currNode->data.first[0];
                        lastNode->children[childIndex] = nullptr;
                        delete currNode;
                    }
                    else if(childrenCount == 1)
                    {
                        //merge the child node into this node.
                        int childIndex = currNode->data.first[0];
                        lastNode->children[childIndex] = currNode->children[firstChildIndex];
                        currNode->children[firstChildIndex]->data.first = currNode->data.first + currNode->children[firstChildIndex]->data.first;
                        delete currNode;
                    }
                }
            }
        }
    }

    bool RadixTree::search(std::string input)
    {
        //do stuff
        //traverse the root node
        //check the first char
        size_t index = 0;
        size_t currNodeCharsProcessed = 0;
        TreeNode<std::pair<std::string, bool>>* currNode = tree.getRoot();
        while(index < input.size())
        {
            if(currNodeCharsProcessed >= currNode->data.first.size())
            {
                int childIndex = input[index];
                currNodeCharsProcessed = 0;

                if(currNode->children[childIndex] != nullptr)
                    currNode = currNode->children[childIndex];
                else
                {
                    //No next node that is along the correct path so does not exist.
                    return false;
                }
            }
            else
            {
                //check if the characters match at the specified index
                if(input[index] == currNode->data.first[currNodeCharsProcessed])
                {
                    currNodeCharsProcessed++;
                    index++;
                }
                else
                {
                    //mismatch. Does not exist
                    return false;
                }
            }
        }

        if(index >= input.size())
        {
            //potentially found it.
            if(currNode != nullptr && currNodeCharsProcessed >= currNode->data.first.size())
            {
                //the node we are on may or may not be the correct node. It must also be an end node
                return currNode->data.second;
            }
        }
        return false;
    }

    Tree<std::pair<std::string, bool>>* RadixTree::getRawTree()
    {
        return &tree;
    }
}