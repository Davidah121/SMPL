#pragma once
#include <vector>
#include <memory>
#include "Concurrency.h"
#include "Stack.h"
#include "Queue.h"
#include "SimpleFile.h"

namespace smpl
{
    template<typename T>
    class Streamable
    {
    public:
        virtual size_t write(T*, size_t) = 0;
        virtual size_t read(T*, size_t size) = 0;
        size_t waitRead(T*, size_t size);
        virtual bool pop() = 0;
        virtual size_t size() = 0;
        virtual size_t getLocation() = 0;
        void setEOF();
        bool getEOF();

    protected:
        void lock();
        void unlock();

    private:
        bool isEnd = true;
        HybridSpinLock commonMutex;
    };

    template<typename T>
    class StreamableVector : public Streamable<T>
    {
    public:
        StreamableVector();
        StreamableVector(const std::vector<T>& arr);
        ~StreamableVector();

        virtual size_t write(T* data, size_t size);
        virtual size_t read(T* input, size_t size);
        virtual bool pop();
        virtual size_t size();
        virtual void clear();
        virtual size_t getLocation();

        std::vector<T>& getBuffer();
    private:
        std::vector<T> buffer;
        size_t offset = 0;
    };

    template<typename T>
    class StreamableQueue : public Streamable<T>
    {
    public:
        StreamableQueue();
        StreamableQueue(const Queue<T>& arr);
        ~StreamableQueue();
        virtual size_t write(T* data, size_t size);
        virtual size_t read(T* input, size_t size);
        virtual bool pop();
        virtual size_t size();
        virtual void clear();
        Queue<T>& getBuffer();
        virtual size_t getLocation();
    private:
        Queue<T> buffer;
    };

    template<typename T>
    class StreamableStack : public Streamable<T>
    {
    public:
        StreamableStack();
        StreamableStack(const Stack<T>& arr);
        ~StreamableStack();

        virtual size_t write(T* data, size_t size);
        virtual size_t read(T* input, size_t size);
        virtual bool pop();
        virtual size_t size();
        virtual void clear();
        Stack<T>& getBuffer();
        virtual size_t getLocation();
    private:
        Stack<T> buffer;
    };

    class StreamableFile : Streamable<unsigned char>
    {
    public:
        StreamableFile(std::string filename, char type);
        ~StreamableFile();

        virtual size_t write(unsigned char* data, size_t size);
        virtual size_t read(unsigned char* input, size_t size);
        virtual bool pop();
        virtual size_t size();
        SimpleFile& getBuffer();
        virtual size_t getLocation();
    private:
        SimpleFile buffer;
    };

    template<typename T>
    inline void Streamable<T>::lock()
    {
        commonMutex.lock();
    }
    template<typename T>
    inline void Streamable<T>::unlock()
    {
        commonMutex.unlock();
    }
    template<typename T>
    inline size_t Streamable<T>::waitRead(T* data, size_t size)
    {
        while(!getEOF())
        {
            size_t readSize = read(data, size);
            if(readSize > 0)
                return readSize;
        }
        return 0;
    }
    template<typename T>
    inline void Streamable<T>::setEOF()
    {
        commonMutex.lock();
        isEnd = true;
        commonMutex.unlock();
    }
    template<typename T>
    inline bool Streamable<T>::getEOF()
    {
        commonMutex.lock();
        bool v = isEnd;
        commonMutex.unlock();
        return v;
    }

    template<typename T>
    inline StreamableVector<T>::StreamableVector()
    {
    }

    template<typename T>
    inline StreamableVector<T>::StreamableVector(const std::vector<T>& arr)
    {
        buffer = arr;
        offset = 0;
    }

    template<typename T>
    inline StreamableVector<T>::~StreamableVector()
    {
    }

    template<typename T>
    inline size_t StreamableVector<T>::write(T* data, size_t size)
    {
        this->lock();
        for(size_t index = 0; index < size; index++)
        {
            buffer.push_back(data[index]);
        }
        this->unlock();
        return size;
    }

    template<typename T>
    inline size_t StreamableVector<T>::read(T* input, size_t size)
    {
        this->lock();
        if(size < this->buffer.size())
        {
            for(size_t i=0; i<size; i++)
                input[i] = buffer.at(offset+i);
            offset += size;
        }
        this->unlock();
        return 0;
    }
    
    template<typename T>
    inline bool StreamableVector<T>::pop()
    {
        this->lock();
        bool v = buffer.size() > 0;
        buffer.pop_back();
        this->unlock();
        return v;
    }
    template<typename T>
    inline size_t StreamableVector<T>::size()
    {
        this->lock();
        size_t totalSize = buffer.size();
        this->unlock();
        return totalSize;
    }
    
    template<typename T>
    inline void StreamableVector<T>::clear()
    {
        this->lock();
        buffer.clear();
        this->unlock();
    }

    template<typename T>
    size_t StreamableVector<T>::getLocation()
    {
        this->lock();
        size_t loc = offset;
        this->unlock();
        return loc;
    }

    template<typename T>
    inline std::vector<T>& StreamableVector<T>::getBuffer()
    {
        return buffer;
    }

    template<typename T>
    inline StreamableQueue<T>::StreamableQueue()
    {
    }
    template<typename T>
    inline StreamableQueue<T>::StreamableQueue(const Queue<T>& arr)
    {
        buffer = arr;
    }
    template<typename T>
    inline StreamableQueue<T>::~StreamableQueue()
    {
    }

    template<typename T>
    inline size_t StreamableQueue<T>::write(T* data, size_t size)
    {
        this->lock();
        for(size_t index = 0; index < size; index++)
        {
            buffer.write(data[index]);
        }
        this->unlock();
        return size;
    }
    
    template<typename T>
    inline size_t StreamableQueue<T>::read(T* input, size_t size)
    {
        size_t amountRead = 0;
        this->lock();
        if(size < buffer.size())
        {
            amountRead = size;
            for(size_t i=0; i<size; i++)
            {
                input[i] = buffer.read();
                buffer.pop();
            }
        }
        this->unlock();
        return amountRead;
    }
    template<typename T>
    inline bool StreamableQueue<T>::pop()
    {
        this->lock();
        bool v = buffer.size() > 0;
        buffer.pop();
        this->unlock();
        return v;
    }
    template<typename T>
    inline size_t StreamableQueue<T>::size()
    {
        this->lock();
        size_t totalSize = buffer.size();
        this->unlock();
        return totalSize;
    }
    
    template<typename T>
    inline void StreamableQueue<T>::clear()
    {
        this->lock();
        buffer.clear();
        this->unlock();
    }

    template<typename T>
    size_t StreamableQueue<T>::getLocation()
    {
        return 0; //always 0. Can't seek. destructive datastructure
    }
    
    template<typename T>
    inline Queue<T>& StreamableQueue<T>::getBuffer()
    {
        return buffer;
    }

    template<typename T>
    inline StreamableStack<T>::StreamableStack()
    {
    }
    template<typename T>
    inline StreamableStack<T>::StreamableStack(const Stack<T>& arr)
    {
        buffer = arr;
    }
    template<typename T>
    inline StreamableStack<T>::~StreamableStack()
    {
    }

    template<typename T>
    inline size_t StreamableStack<T>::write(T* data, size_t size)
    {
        this->lock();
        for(size_t index = 0; index < size; index++)
        {
            buffer.write(data[index]);
        }
        this->unlock();
        return size;
    }

    template<typename T>
    inline size_t StreamableStack<T>::read(T* input, size_t size)
    {
        size_t amountRead = 0;
        this->lock();
        if(size < buffer.size())
        {
            amountRead = size;
            for(size_t i=0; i<size; i++)
            {
                input[i] = buffer.read();
                buffer.pop();
            }
        }
        this->unlock();
        return amountRead;
    }
    template<typename T>
    inline bool StreamableStack<T>::pop()
    {
        this->lock();
        bool canPop = buffer.size() > 0;
        buffer.pop();
        this->unlock();
        return canPop;
    }
    template<typename T>
    inline size_t StreamableStack<T>::size()
    {
        this->lock();
        size_t totalSize = buffer.size();
        this->unlock();
        return totalSize;
    }
    
    template<typename T>
    inline void StreamableStack<T>::clear()
    {
        this->lock();
        buffer.clear();
        this->unlock();
    }
    
    template<typename T>
    size_t StreamableStack<T>::getLocation()
    {
        return 0; //always 0. Can't seek. destructive datastructure
    }

    template<typename T>
    inline Stack<T>& StreamableStack<T>::getBuffer()
    {
        return buffer;
    }

    inline StreamableFile::StreamableFile(std::string filename, char type)
    {
        buffer = SimpleFile(filename, type);
    }
    inline StreamableFile::~StreamableFile()
    {
    }

    inline size_t StreamableFile::write(unsigned char* data, size_t size)
    {
        this->lock();
        buffer.writeBytes(data, size);
        this->unlock();
        return size;
    }
    inline size_t StreamableFile::read(unsigned char* input, size_t size)
    {
        this->lock();
        size_t bytesRead = buffer.readBytes((char*)input, size);
        this->unlock();
        return bytesRead;
    }
    inline bool StreamableFile::pop()
    {
        this->lock();
        bool canPop = buffer.getBytesLeft() > 0;
        buffer.readByte();
        this->unlock();
        return canPop;
    }
    inline size_t StreamableFile::size()
    {
        this->lock();
        size_t totalSize = buffer.getSize();
        this->unlock();
        return totalSize;
    }

    size_t StreamableFile::getLocation()
    {
        this->lock();
        size_t loc = buffer.currentLocation();
        this->unlock();
        return loc;
    }

    inline SimpleFile& StreamableFile::getBuffer()
    {
        return buffer;
    }
}