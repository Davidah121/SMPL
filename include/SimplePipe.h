#include "SimpleFile.h"

namespace smpl
{
    class SimplePipe : public SimpleFile
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