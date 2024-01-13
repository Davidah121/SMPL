#include <catch2/catch_amalgamated.hpp>
#include "RadixTree.h"

TEST_CASE("Testing RadixTree Class", "[RadixTree]")
{
    SECTION("INSERT TEST 1")
    {
        glib::RadixTree tree = glib::RadixTree();
        tree.insert("test");
        tree.insert("slow");
        tree.insert("water");

        tree.insert("test"); //already exist. Shouldn't do anything

        glib::Tree<std::pair<std::string, bool>>* rawTree = tree.getRawTree();
        REQUIRE(rawTree->getRoot()->children['t']->data.first == "test");
        REQUIRE(rawTree->getRoot()->children['t']->data.second == true);
        REQUIRE(rawTree->getRoot()->children['s']->data.first == "slow");
        REQUIRE(rawTree->getRoot()->children['s']->data.second == true);
        REQUIRE(rawTree->getRoot()->children['w']->data.first == "water");
        REQUIRE(rawTree->getRoot()->children['w']->data.second == true);
        
        tree.insert("slower");
        REQUIRE(rawTree->getRoot()->children['s']->children['e']->data.first == "er");
        REQUIRE(rawTree->getRoot()->children['s']->children['e']->data.second == true);
    }

    SECTION("INSERT TEST 2")
    {
        glib::RadixTree tree = glib::RadixTree();
        tree.insert("tester");
        tree.insert("test");

        glib::Tree<std::pair<std::string, bool>>* rawTree = tree.getRawTree();
        REQUIRE(rawTree->getRoot()->children['t']->data.first == "test");
        REQUIRE(rawTree->getRoot()->children['t']->data.second == true);
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->data.first == "er");
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->data.second == true);
    }

    SECTION("INSERT TEST 3")
    {
        glib::RadixTree tree = glib::RadixTree();
        tree.insert("test");
        tree.insert("team");

        glib::Tree<std::pair<std::string, bool>>* rawTree = tree.getRawTree();
        REQUIRE(rawTree->getRoot()->children['t']->data.first == "te");
        REQUIRE(rawTree->getRoot()->children['t']->data.second == false);
        REQUIRE(rawTree->getRoot()->children['t']->children['s']->data.first == "st");
        REQUIRE(rawTree->getRoot()->children['t']->children['s']->data.second == true);
        REQUIRE(rawTree->getRoot()->children['t']->children['a']->data.first == "am");
        REQUIRE(rawTree->getRoot()->children['t']->children['a']->data.second == true);

        tree.insert("toast");
        
        REQUIRE(rawTree->getRoot()->children['t']->data.first == "t");
        REQUIRE(rawTree->getRoot()->children['t']->data.second == false);
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->data.first == "e");
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->data.second == false);
        REQUIRE(rawTree->getRoot()->children['t']->children['o']->data.first == "oast");
        REQUIRE(rawTree->getRoot()->children['t']->children['o']->data.second == true);
        
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->children['s']->data.first == "st");
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->children['s']->data.second == true);
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->children['a']->data.first == "am");
        REQUIRE(rawTree->getRoot()->children['t']->children['e']->children['a']->data.second == true);
    }
}