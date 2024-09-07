#pragma once
#include <vector>

namespace smpl
{
    template<typename T>
    class Stack
    {
    public:
        Stack() {}
        ~Stack() {}

        void add(T data)
        {
            buffer.push_back(data);
        }
        void pop()
        {
            buffer.pop_back();
        }
        T get()
        {
            return buffer.back();
        }
        size_t size()
        {
            return buffer.size();
        }
    private:
        std::vector<T> buffer;
    };
}
