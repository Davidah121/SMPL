#include <iostream>
#include <chrono>
#include <thread>
#include "SimpleJSON.h"

int main(int argc, char** argv)
{
    if(argc == 3)
    {
        printf("Loading json of valid applications\n");
        system("curl https://discord.com/api/v9/applications/detectable -o detectable.json"); //lazy by the way

        std::string applicationName = argv[1];
        int minutes = std::stoi(argv[2]);
        printf("Starting quest for %s lasting for %d minutes\n", argv[1], minutes);

        //search names for application name. doesn't have to be exact
        smpl::SimpleJSON json;
        json.load("detectable.json");

        std::vector<smpl::JNode*> allNames = json.getNodesPattern({"", "name"});
        

        
        std::this_thread::sleep_for(std::chrono::minutes(minutes));
        printf("Ending quest\n");
    }
    return 0;
}