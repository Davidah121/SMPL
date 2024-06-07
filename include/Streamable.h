#include <vector>
#include <memory>

template<typename T>
class Streamable
{
public:
    virtual void add(T) = 0;
    virtual void seek(size_t) = 0;
    virtual T get() = 0;
    virtual void get(T*, size_t size) = 0;
    virtual void pop_back() = 0;
    virtual size_t size() = 0;
};

template<typename T>
class StreamableList : public Streamable<T>
{
public:
    StreamableList()
    {
        buffer = std::shared_ptr<std::vector<T>>(new std::vector<T>());
    }
    ~StreamableList()
    {

    }

    virtual void add(T data)
    {
        buffer->push_back(data);
    }
    virtual void seek(size_t index)
    {
        offset = index;
    }
    virtual T get()
    {
        return buffer->at(offset++);
    }
    virtual void get(T* input, size_t size)
    {
        for(size_t i=0; i<size; i++)
            input[i] = buffer->at(offset+i);
        offset += size;
    }
    virtual void pop_back()
    {
        buffer->pop_back();
    }
    virtual size_t size()
    {
        return buffer->size();
    }
private:
    std::shared_ptr<std::vector<T>> buffer = nullptr;
    size_t offset = 0;
};