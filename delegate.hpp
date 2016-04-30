#ifndef FSX_DELEGATE_HPP
#define FSX_DELEGATE_HPP

#include <iostream>
#include <vector>

namespace fsx
{
    template <typename T>
    class delegate;

    template <typename T, typename... Args>
    class delegate<T(Args...)>
    {
    private:
        typedef T(*internal_function)(void*, Args&&...);
        typedef std::pair<void*, internal_function> stub;
        std::vector<stub> vec;

        template <T(*function)(Args...)>
        static inline T function_stub(void*, Args&&... args)
        {
            return (function)(std::forward<Args>(args)...);
        }

        template <typename C>
        static inline T lambda_stub(void* lambda, Args&&... args)
        {
            return (*static_cast<C*>(lambda))(std::forward<Args>(args)...);
        }

        template <typename C, T(C::*function)(Args...)>
        static inline T class_function_stub(void* instance, Args&&... args)
        {
            return (static_cast<C*>(instance)->*function)(std::forward<Args>(args)...);
        }

    public:
        delegate()
        {}

        template <T(*function)(Args...)>
        delegate& bind()
        {
            stub m_stub;
            m_stub.first = nullptr;
            m_stub.second = &function_stub<function>;
            this->vec.push_back(std::move(m_stub));
            return *this;
        }

        template <typename C, T(C::*function)(Args...)>
        delegate& bind(C& instance)
        {
            stub m_stub;
            m_stub.first = &instance;
            m_stub.second = &class_function_stub<C, function>;
            this->vec.push_back(std::move(m_stub));
            return *this;
        }

        template <typename C>
        delegate& bind(const C& lambda)
        {
            stub m_stub;
            m_stub.first = const_cast<void*>(static_cast<const void*>(&lambda));
            m_stub.second = &lambda_stub<C>;
            this->vec.push_back(std::move(m_stub));
            return *this;
        }

        size_t size()
        {
            return this->vec.size();
        }

        void clear()
        {
            this->vec.clear();
        }

        delegate& invoke(Args&&... args)
        {
            for (auto& m_stub : this->vec)
            {
                m_stub.second(m_stub.first, std::forward<Args>(args)...);
            }
            return *this;
        }
    };
}
#endif
