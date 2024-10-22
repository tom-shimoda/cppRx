#pragma once
#include <functional>

template <typename T>
class Observer
{
protected:
    std::function<void(T)> _onNext;
    std::function<void()> _onCompleted;
    bool isStopped;

public:
    explicit Observer(std::function<void(T)> onNext,
                      std::function<void()> onCompleted)
        : _onNext(std::move(onNext)),
          _onCompleted(std::move(onCompleted)),
          isStopped(false)
    {
    }

    virtual ~Observer() = default;

    virtual void OnNext(T v)
    {
        if (this->isStopped) return;

        _onNext(v);
    }

    virtual void OnCompleted()
    {
        if (this->isStopped) return;
        
        _onCompleted();
        isStopped = true;
    }
};
