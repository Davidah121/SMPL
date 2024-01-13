#include "SimpleFile.h"

namespace glib
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