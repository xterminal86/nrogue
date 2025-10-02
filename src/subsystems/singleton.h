#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton
{
  public:
    virtual ~Singleton() = default;

    static T& Instance()
    {
      static T instance;
      return instance;
    }

    void Init()
    {
      if (_initialized)
      {
        return;
      }

      InitSpecific();

      _initialized = true;
    }

  protected:
    Singleton() = default;

    virtual void InitSpecific() = 0;

    bool _initialized = false;
};

#endif
