#include "SimplePipe.h"

namespace glib
{
    SimplePipe::SimplePipe(std::string command, char type)
    : SimpleFile()
    {
        std::string openType = "";
        switch(type & 0x0F)
        {
        case SimpleFile::READ:
            openType = "rb";
            break;
        case SimpleFile::WRITE:
            openType = "wb";
            break;
        default:
            //error should throw exception
            return;
        }
        FILE* pipe = nullptr;

        #ifdef _WIN32
            pipe = _popen(command.c_str(), openType.c_str());
        #else
            pipe = popen(command.c_str(), openType.c_str());
        #endif

        SimpleFile::init(pipe, command, type);
    }

    SimplePipe::~SimplePipe()
    {
        if(isOpen())
        {
            close();
        }
    }

    int SimplePipe::getExitCode()
    {
        return exitCode;
    }

    void SimplePipe::close()
    {
        #ifdef _WIN32
            exitCode = _pclose(getFilePointer()) >> 8;
        #else
            exitCode = pclose(getFilePointer()) >> 8;
        #endif
        cFile = nullptr;
    }
}