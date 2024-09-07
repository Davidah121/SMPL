#pragma once
#include "BuildOptions.h"
#include "SimpleFile.h"

namespace smpl
{
    class DLL_OPTION SimplePipe : public SimpleFile
    {
    public:
        SimplePipe(std::string command, char type);
        ~SimplePipe();
        
        int getExitCode();
        void close();
    private:
        int exitCode = 0;
    };
}