#pragma once
#include "../Observer.h"

template <typename T>
class SkipObserver : public Observer<T>
{
    int counter;
    int skipCount;

public:
    explicit SkipObserver(std::function<void(T)> onNext,
                          std::function<void()> onCompleted,
                          int skipCount)
        : Observer<T>(onNext, onCompleted),
          counter(0),
          skipCount(skipCount)
    {
    }

    void OnNext(T v) override
    {
        if (this->isStopped) return;

        if (counter++ < skipCount) return;

        this->_onNext(v);
    }
};
