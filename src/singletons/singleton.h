#ifndef SINGLETON_H
#define SINGLETON_H

#include <sstream>

template <typename T>
class Singleton
{
  public:
    static T& Instance()
    {
      static T instance;
      return instance;
    }

    virtual void Init() = 0;

  protected:
    Singleton() = default;
    virtual ~Singleton() = default;

    bool _initialized = false;
};

#endif
