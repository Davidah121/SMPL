#include "ext/DirectoryNotifier.h"
#include "SimpleDir.h"
#include <stdio.h>
#include <stdlib.h>


DirectoryNotifier::DirectoryNotifier()
{
    #if defined(__unix__)
        rootFD = inotify_init1(IN_NONBLOCK);
    #elif defined(_WIN32)
    #endif
}

DirectoryNotifier::~DirectoryNotifier()
{
    #if defined(__unix__)
        //remove all watch descriptors
        for(auto& it : directoryToWD)
        {
            inotify_rm_watch(rootFD, it.second);
        }
        close(rootFD);

        rootFD = 0;
        wdToDirectoryInfo.clear();
        directoryToWD.clear();
    #elif defined(_WIN32)
        //remove all notify handles
        for(InternalNotifyHandler* h : notifyHandles)
        {
            if(h != nullptr)
            {
                CloseHandle(h->ovl.hEvent);
                CloseHandle(h->directoryHandle);
                delete h;
            }
        }
        notifyHandles.clear();
    #endif

    eventQueue = std::queue<NotifyTuple>();
}

void DirectoryNotifier::addDirectoryToWatch(std::string dir, bool includeSubDirectories)
{
    if(!dir.empty())
    {
        if(dir.back() != '/' || dir.back() != '\\')
            dir += "/";
    }

    #if defined(__unix__)
        if(rootFD == 0)
            return;
        
        int wd = inotify_add_watch(rootFD, dir.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVE);
        if(wd == 0)
            return;
        
        wdToDirectoryInfo.insert({wd, {dir, includeSubDirectories}});
        directoryToWD.insert({dir, wd});

        if(includeSubDirectories)
        {
            smpl::SimpleDir d = smpl::SimpleDir(dir);
            std::vector<std::string> dirs = d.getFolders();
            for(std::string subDirectory : dirs)
            {
                addDirectoryToWatch(dir+subDirectory, includeSubDirectories);
            }
        }
    #elif defined(_WIN32)
        //check everything
        InternalNotifyHandler* inh = new InternalNotifyHandler();
        inh->subDirsToo = includeSubDirectories;
        inh->directoryStart = dir;

        std::wstring dirAsWide = smpl::StringTools::toWideString(dir);
        inh->directoryHandle = CreateFileW(dirAsWide.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
        inh->ovl = OVERLAPPED{};
        ZeroMemory(inh->buffer.data(), inh->buffer.size());
        inh->ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        
        //init check stuff
        if(inh->ovl.hEvent != NULL && inh->directoryHandle != NULL)
        {
            notifyHandles.push_back(inh);
            int err = ReadDirectoryChangesW(inh->directoryHandle, inh->buffer.data(), inh->buffer.size()*4, true, NOTIFY_CONST, NULL, &inh->ovl, NULL);
        }
    #endif
}

void DirectoryNotifier::removeDirectoryToWatch(std::string dir)
{
    if(!dir.empty())
    {
        if(dir.back() != '/' || dir.back() != '\\')
            dir += "/";
    }
    
    #if defined(__unix__)

    //first, get wd
    int wd = -1;

    auto it = directoryToWD.find(dir);
    if(it != directoryToWD.end())
        wd = it->second;
    
    if(wd != -1)
    {
        inotify_rm_watch(rootFD, wd);
        wdToDirectoryInfo.erase(wd);
        directoryToWD.erase(dir);
    }

    #elif defined(_WIN32)
    //find the directory in the notifyHandles
    int i=0;
    while(i < notifyHandles.size())
    {
        if(smpl::StringTools::equalsIgnoreCase(notifyHandles[i]->directoryStart, dir))
        {
            break;
        }
        i++;
    }
    if(i < notifyHandles.size())
    {
        while(i < notifyHandles.size()-1)
        {
            auto temp = notifyHandles[i];
            notifyHandles[i] = notifyHandles[i+1];
            notifyHandles[i+1] = temp;
            i++;
        }
        notifyHandles.pop_back();
    }
    #endif
}

void DirectoryNotifier::renameDirectoryToWatch(std::string dir, std::string newName)
{
    if(!dir.empty())
    {
        if(dir.back() != '/' || dir.back() != '\\')
            dir += "/";
    }
    else
        return;

    if(!newName.empty())
    {
        if(newName.back() != '/' || newName.back() != '\\')
            newName += "/";
    }
    else
        return;
    #if defined(__unix__)

    //first, get wd
    int wd = -1;

    auto it = directoryToWD.find(dir);
    if(it != directoryToWD.end())
        wd = it->second;
    
    if(wd != -1)
    {
        InternalDirectoryInfo oldInfo = getDirectoryInfoFromWD(wd);
        oldInfo.directory = newName;

        wdToDirectoryInfo[wd] = oldInfo;
        directoryToWD[newName] = wd;
        directoryToWD.erase(dir);
    }

    #elif defined(_WIN32)
    int i=0;
    while(i < notifyHandles.size())
    {
        if(smpl::StringTools::equalsIgnoreCase(notifyHandles[i]->directoryStart, dir))
        {
            notifyHandles[i]->directoryStart = newName;
            break;
        }
        i++;
    }
    #endif
}

void DirectoryNotifier::watchForEvents()
{
    #if defined(__unix__)
        //use blocking read
        const int bufferLen = 0xFFFF;
        const int EVENT_SIZE = sizeof(inotify_event);
        std::vector<char> finalBuffer;
        char buffer[bufferLen];

        while(true)
        {
            int len = read(rootFD, buffer, bufferLen);
            
            if(len <= 0)
                break;

            for(int i=0; i<len; i++)
            {
                finalBuffer.push_back(buffer[i]);
            }
        }
        
        int index=0;

        std::vector<NotifyTuple> tempBufferForRename;
        std::vector<std::string> tempCreateBuffer; //buffer for new directories to add

        std::map<uint32_t, uint32_t> renamePairs;

        while(index < finalBuffer.size())
        {
            inotify_event* event = (inotify_event*)&buffer[index];

            NotifyTuple currEvent;
            if(event->mask & IN_CREATE)
            {
                InternalDirectoryInfo dirInfo = getDirectoryInfoFromWD(event->wd);
                currEvent.operation = OP_TYPE_CREATE;
                currEvent.v1 = dirInfo.directory + event->name;

                if(smpl::SimpleDir::isDirectory(currEvent.v1) && dirInfo.subDirectoriesToo)
                {
                    addDirectoryToWatch(currEvent.v1, true);
                }
            }
            else if(event->mask & IN_DELETE)
            {
                currEvent.operation = OP_TYPE_DELETE;
                currEvent.v1 = getDirectoryInfoFromWD(event->wd).directory + event->name;

                //If its a directory we are watching. remove it
                removeDirectoryToWatch(currEvent.v1);
            }
            else if(event->mask & IN_MODIFY)
            {
                currEvent.operation = OP_TYPE_MODIFY;
                currEvent.v1 = getDirectoryInfoFromWD(event->wd).directory + event->name;
            }
            else if(event->mask & IN_MOVED_FROM || event->mask & IN_MOVED_TO)
            {
                if(event->mask & IN_MOVED_FROM)
                    currEvent.v1 = getDirectoryInfoFromWD(event->wd).directory + event->name; //v1 represents from
                else
                    currEvent.v2 = getDirectoryInfoFromWD(event->wd).directory + event->name; //v2 represents to
                
                currEvent.operation = OP_TYPE_MOVE;
                
                if(event->cookie != 0)
                {
                    if(event->mask & IN_MOVED_FROM)
                    {
                        renamePairs.insert( {event->cookie, tempBufferForRename.size()} );
                    }
                    else
                    {
                        //find the index to modify
                        auto it = renamePairs.find(event->cookie);
                        if(it != renamePairs.end())
                        {
                            tempBufferForRename[it->second].operation = OP_TYPE_RENAME;
                            tempBufferForRename[it->second].v2 = getDirectoryInfoFromWD(event->wd).directory + event->name;

                            index += EVENT_SIZE + event->len;
                            continue;
                        }
                        else
                        {
                            //false flag. Just a move
                        }
                    }
                }
            }
            else
                currEvent.operation = OP_TYPE_UNKNOWN;

            //lazy way
            if(currEvent.operation != OP_TYPE_UNKNOWN)
                tempBufferForRename.push_back(currEvent);

            index += EVENT_SIZE + event->len;
        }

        for(NotifyTuple& ev : tempBufferForRename)
        {
            //check if it is a directory that we are watching.
            //If so, rename it. Assume it is.
            renameDirectoryToWatch(ev.v1, ev.v2);
            eventQueue.push(ev);
        }

    #elif defined(_WIN32)

        for(int i=0; i<notifyHandles.size(); i++)
        {
            //read all changes. Windows does not give all changes at once for some reason. May need to wait for a few milliseconds between changes.
            //actually get information
            int err = WaitForMultipleObjects(1, &notifyHandles[i]->ovl.hEvent, false, 0);
            if(err != WAIT_OBJECT_0)
            {
                continue;
            }

            DWORD bytesRet = 0;
            err = GetOverlappedResult(notifyHandles[i]->directoryHandle, &notifyHandles[i]->ovl, &bytesRet, TRUE);
            if(err != 0)
            {
                notifyHandles[i]->alreadyReading = false;
                DWORD offset = 0;
                NotifyTuple lastEvent;
                while(true)
                {
                    std::wstring filename;
                    FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)&notifyHandles[i]->buffer[offset];
                    
                    if(fni->Action != 0)
                    {
                        for(int c=0; c<fni->FileNameLength/2; c++)
                            filename += fni->FileName[c];

                        NotifyTuple newEvent = {};
                        switch (fni->Action)
                        {
                        case FILE_ACTION_ADDED:
                            newEvent.operation = OP_TYPE_CREATE;
                            break;
                        case FILE_ACTION_REMOVED:
                            newEvent.operation = OP_TYPE_CREATE;
                            break;
                        case FILE_ACTION_MODIFIED:
                            newEvent.operation = OP_TYPE_CREATE;
                            break;
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            newEvent.operation = OP_TYPE_RENAME;
                            break;
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            newEvent.operation = OP_TYPE_RENAME;
                            break;
                        default:
                            break;
                        }

                        newEvent.v1 = notifyHandles[i]->directoryStart + "/" + smpl::StringTools::toUTF8String(filename);
                        if(fni->Action == FILE_ACTION_RENAMED_NEW_NAME)
                        {
                            //second part of the rename operation
                            if(lastEvent.operation == OP_TYPE_RENAME)
                            {
                                lastEvent.v2 = newEvent.v1;
                                newEvent = lastEvent;
                            }
                        }

                        if(fni->Action != FILE_ACTION_RENAMED_OLD_NAME)
                        {
                            eventQueue.push(newEvent);
                        }
                        lastEvent = newEvent;
                    }
                    offset += fni->NextEntryOffset;
                    if(fni->NextEntryOffset == 0)
                        break;
                }
                
                int err = ReadDirectoryChangesW(notifyHandles[i]->directoryHandle, notifyHandles[i]->buffer.data(), notifyHandles[i]->buffer.size()*4, notifyHandles[i]->subDirsToo, NOTIFY_CONST, NULL, &notifyHandles[i]->ovl, NULL);
            }

        }
    #endif
}

NotifyTuple DirectoryNotifier::getNextEvent()
{
    NotifyTuple currEvent = eventQueue.front();
    eventQueue.pop();
    return currEvent;
}

bool DirectoryNotifier::empty()
{
    return eventQueue.empty();
}

#if defined(__unix__)
    InternalDirectoryInfo DirectoryNotifier::getDirectoryInfoFromWD(int wd)
    {
        auto it = wdToDirectoryInfo.find(wd);
        if(it != wdToDirectoryInfo.end())
        {
            return it->second;
        }
        return {};
    }
#endif
