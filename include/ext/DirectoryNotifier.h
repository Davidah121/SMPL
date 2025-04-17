#include <iostream>
#include <vector>
#include <map>
#include <queue>

#if defined(__unix__)
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/inotify.h>
    struct InternalDirectoryInfo
    {
        std::string directory = "";
        bool subDirectoriesToo = false;
    };
#elif defined(_WIN32)
    #include "TargetVer.h"
    #include <Windows.h>
    struct InternalNotifyHandler
    {
        std::string directoryStart = "";
        bool subDirsToo = false;
        HANDLE directoryHandle = 0;
        OVERLAPPED ovl = {0};
        DWORD bytesRet = 0;
        bool alreadyReading = false;
        std::vector<int> buffer = std::vector<int>(256);
    };
#endif

struct NotifyTuple
{
    int operation = -1;
    std::string v1 = "";
    std::string v2 = "";
};

class DirectoryNotifier
{
public:
    static const int OP_TYPE_CREATE = 0;
    static const int OP_TYPE_DELETE = 1;
    static const int OP_TYPE_MODIFY = 2;
    static const int OP_TYPE_MOVE = 3;
    static const int OP_TYPE_RENAME = 4;
    static const int OP_TYPE_UNKNOWN = -1;
    
    DirectoryNotifier();
    ~DirectoryNotifier();

    void addDirectoryToWatch(std::string dir, bool includeSubDirectories);
    void removeDirectoryToWatch(std::string dir);
    void renameDirectoryToWatch(std::string dir, std::string newName);
    
    void watchForEvents();
    NotifyTuple getNextEvent();
    bool empty();

private:
    #if defined(__unix__)
        InternalDirectoryInfo getDirectoryInfoFromWD(int wd);

        int rootFD = 0;
        std::map<int, InternalDirectoryInfo> wdToDirectoryInfo;
        std::map<std::string, int> directoryToWD;
    #elif defined(_WIN32)
        static const int NOTIFY_CONST = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                                        FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                                        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
                                        FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY;
        
        std::vector<InternalNotifyHandler*> notifyHandles;
    #endif
    std::queue<NotifyTuple> eventQueue;
};