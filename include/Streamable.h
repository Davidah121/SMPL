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
        virtual void add(T) = 0;
        virtual void add(T*, size_t) = 0;
        virtual void seek(size_t) = 0;
        virtual T get() = 0;
        virtual void get(T*, size_t size) = 0;
        virtual void pop() = 0;
        virtual size_t size() = 0;

    protected:
        void lock();
        void unlock();

    private:
        HybridSpinLock commonMutex;
    };

    template<typename T>
    class StreamableVector : public Streamable<T>
    {
    public:
        StreamableVector();
        ~StreamableVector();

        virtual void add(T data);
        virtual void add(T* data, size_t size);
        virtual void seek(size_t index);
        virtual T get();
        virtual void get(T* input, size_t size);
        virtual void pop();
        virtual size_t size();
    private:
        std::vector<T> buffer;
        size_t offset = 0;
    };

    template<typename T>
    class StreamableQueue : public Streamable<T>
    {
    public:
        StreamableQueue();
        ~StreamableQueue();

        virtual void add(T data);
        virtual void add(T* data, size_t size);

        virtual void seek(size_t index);
        
        virtual T get();
        virtual void get(T* input, size_t size);
        virtual void pop();
        virtual size_t size();
    private:
        Queue<T> buffer;
    };

    template<typename T>
    class StreamableStack : public Streamable<T>
    {
    public:
        StreamableStack();
        ~StreamableStack();

        virtual void add(T data);
        virtual void add(T* data, size_t size);

        virtual void seek(size_t index);

        virtual T get();
        virtual void get(T* input, size_t size);
        virtual void pop_back();
        virtual size_t size();
    private:
        Stack<T> buffer;
    };

    class StreamableFile : Streamable<unsigned char>
    {
    public:
        StreamableFile(std::string filename, char type);
        ~StreamableFile();

        virtual void add(unsigned char data);
        virtual void add(unsigned char* data, size_t size);
        virtual void seek(size_t index);
        virtual unsigned char get();
        virtual void get(unsigned char* input, size_t size);
        virtual void pop();
        virtual size_t size();
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
    inline StreamableVector<T>::StreamableVector()
    {
    }

    template<typename T>
    inline StreamableVector<T>::~StreamableVector()
    {
    }

    template<typename T>
    inline void StreamableVector<T>::add(T data)
    {
        this->lock();
        buffer.push_back(data);
        this->unlock();
    }

    template<typename T>
    inline void StreamableVector<T>::add(T* data, size_t size)
    {
        this->lock();
        for(size_t index = 0; index < size; index++)
        {
            buffer.push_back(data[index]);
        }
        this->unlock();
    }

    template<typename T>
    inline void StreamableVector<T>::seek(size_t index)
    {
        this->lock();
        offset = index;
        this->unlock();
    }

    template<typename T>
    inline T StreamableVector<T>::get()
    {
        this->lock();
        T temp = buffer.at(offset++);
        this->unlock();
        return temp;
    }

    template<typename T>
    inline void StreamableVector<T>::get(T* input, size_t size)
    {
        this->lock();
        for(size_t i=0; i<size; i++)
            input[i] = buffer.at(offset+i);
        offset += size;
        this->unlock();
    }
    
    template<typename T>
    inline void StreamableVector<T>::pop()
    {
        this->lock();
        buffer.pop_back();
        this->unlock();
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
    inline StreamableQueue<T>::StreamableQueue()
    {
    }
    template<typename T>
    inline StreamableQueue<T>::~StreamableQueue()
    {
    }

    template<typename T>
    inline void StreamableQueue<T>::add(T data)
    {
        this->lock();
        buffer.add(data);
        this->unlock();
    }
    template<typename T>
    inline void StreamableQueue<T>::add(T* data, size_t size)
    {
        this->lock();
        for(size_t index = 0; index < size; index++)
        {
            buffer.add(data[index]);
        }
        this->unlock();
    }

    template<typename T>
    inline void StreamableQueue<T>::seek(size_t index)
    {
    }
    
    template<typename T>
    inline T StreamableQueue<T>::get()
    {
        this->lock();
        T temp = buffer.get();
        buffer.pop();
        this->unlock();
        return temp;
    }
    template<typename T>
    inline void StreamableQueue<T>::get(T* input, size_t size)
    {
        this->lock();
        for(size_t i=0; i<size; i++)
        {
            input[i] = buffer.get();
            buffer.pop();
        }
        this->unlock();
    }
    template<typename T>
    inline void StreamableQueue<T>::pop()
    {
        this->lock();
        buffer.pop();
        this->unlock();
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
    inline StreamableStack<T>::StreamableStack()
    {
    }
    template<typename T>
    inline StreamableStack<T>::~StreamableStack()
    {
    }

    template<typename T>
    inline void StreamableStack<T>::add(T data)
    {
        this->lock();
        buffer.add(data);
        this->unlock();
    }
    template<typename T>
    inline void StreamableStack<T>::add(T* data, size_t size)
    {
        this->lock();
        for(size_t index = 0; index < size; index++)
        {
            buffer.add(data[index]);
        }
        this->unlock();
    }

    template<typename T>
    inline void StreamableStack<T>::seek(size_t index)
    {
    }

    template<typename T>
    inline T StreamableStack<T>::get()
    {
        this->lock();
        T temp = buffer.get();
        buffer.pop();
        this->unlock();
        return temp;
    }
    template<typename T>
    inline void StreamableStack<T>::get(T* input, size_t size)
    {
        this->lock();
        for(size_t i=0; i<size; i++)
        {
            input[i] = buffer.get();
            buffer.pop();
        }
        this->unlock();
    }
    template<typename T>
    inline void StreamableStack<T>::pop_back()
    {
        this->lock();
        buffer.pop();
        this->unlock();
    }
    template<typename T>
    inline size_t StreamableStack<T>::size()
    {
        this->lock();
        size_t totalSize = buffer.size();
        this->unlock();
        return totalSize;
    }


    inline StreamableFile::StreamableFile(std::string filename, char type)
    {
        buffer = SimpleFile(filename, type);
    }
    inline StreamableFile::~StreamableFile()
    {
    }

    inline void StreamableFile::add(unsigned char data)
    {
        this->lock();
        buffer.writeByte(data);
        this->unlock();
    }
    inline void StreamableFile::add(unsigned char* data, size_t size)
    {
        this->lock();
        buffer.writeBytes(data, size);
        this->unlock();
    }
    inline void StreamableFile::seek(size_t index)
    {
        this->lock();
        buffer.seek(index);
        this->unlock();
    }
    inline unsigned char StreamableFile::get()
    {
        this->lock();
        unsigned char temp = buffer.readByte();
        this->unlock();
        return temp;
    }
    inline void StreamableFile::get(unsigned char* input, size_t size)
    {
        this->lock();
        buffer.readBytes((char*)input, size);
        this->unlock();
    }
    inline void StreamableFile::pop()
    {
        this->lock();
        buffer.readByte();
        this->unlock();
    }
    inline size_t StreamableFile::size()
    {
        this->lock();
        size_t totalSize = buffer.getSize();
        this->unlock();
        return totalSize;
    }
}