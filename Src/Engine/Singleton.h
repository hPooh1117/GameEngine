#pragma once
#include <cassert>
#include <mutex>

class SingletonFinalizer
{
public:
    using FinalizerFunc = void(*)();
    static void addFinalizer(FinalizerFunc func);
    static void Finalize();
};

template<typename T>
class Singleton final
{
public:
    static T& Get()
    {
        std::call_once(initFlag, Create);
        assert(instance);
        return *instance;
    };

private:
    static void Create()
    {
        instance = new T;
        SingletonFinalizer::addFinalizer(&Singleton<T>::Destroy);
    }

    static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }

    

private:
    static std::once_flag initFlag;
    static T* instance;
};

template<typename T> std::once_flag Singleton<T>::initFlag;
template<typename T> T* Singleton<T>::instance = nullptr;