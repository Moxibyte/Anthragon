#pragma once

#include <anthragon/ILibContext.h>

#include <memory>
#include <utility>

namespace Anthragon
{
    template<typename T>
    class UniquePtrDeleter
    {
        public:
            UniquePtrDeleter() = delete;
            UniquePtrDeleter(ILibContext& ctx) : m_ctx(&ctx) {}
            UniquePtrDeleter(const UniquePtrDeleter&) = default;

            template<typename XT>
            UniquePtrDeleter(const UniquePtrDeleter<XT>& other) :
                m_ctx(other.m_ctx)
            {}
            
            UniquePtrDeleter& operator=(const UniquePtrDeleter&) = default;

            template<typename XT>
            UniquePtrDeleter& operator=(const UniquePtrDeleter<XT>& other)
            {
                m_ctx = other.m_ctx;
                return *this;
            }

            void operator()(T const* obj)
            {
                obj->~T();
                m_ctx->LibFree(obj);
            }

        private:
            ILibContext* m_ctx = nullptr;
    };

    template<typename T>
    class StdAllocator
    {
        public:
            using value_type = T;
            using size_type = size_t;

        public:
            StdAllocator() = delete;
            StdAllocator(ILibContext& ctx) : m_ctx(&ctx) {}
            StdAllocator(const StdAllocator&) = default;

            template<typename XT>
            StdAllocator(const StdAllocator<XT>& other) :
                m_ctx(get_ctx())
            {}

            StdAllocator& operator=(const StdAllocator&) = default;

            template<typename XT>
            StdAllocator& operator=(const StdAllocator<XT>& other)
            {
                m_ctx = get_ctx();
                return *this;
            }

            T* allocate(size_type n)
            {
                return (T*)m_ctx->LibAllocate(sizeof(T) * n);
            }

            void deallocate(T* ptr, size_type n)
            {
                m_ctx->LibFree(ptr);
            }

            ILibContext* get_ctx() { return m_ctx; }

        private:
            ILibContext* m_ctx = nullptr;
    };

    template<typename T>
    using UniquePtr = std::unique_ptr<T, UniquePtrDeleter<T>>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T, typename... Args>
    UniquePtr<T> MakeUnique(ILibContext& ctx, Args... args)
    {
        T* obj = (T*)ctx.LibAllocate(sizeof(T));
        new(obj)T(std::forward<Args>(args)...);
        return UniquePtr<T>(obj, UniquePtrDeleter<T>(ctx));
    }

    template<typename T, typename... Args>
    SharedPtr<T> MakeShared(ILibContext& ctx, Args... args)
    {
        StdAllocator<T> allocator(ctx);
        return std::allocate_shared<T>(allocator, std::forward<Args>(args)...);
    }
}
