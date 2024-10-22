#pragma once
#include "../Observer.h"

template <typename T>
class IntervalObserver : public Observer<T>
{
    int counter;
    int intervalCount;

public:
    explicit IntervalObserver(std::function<void(T)> onNext,
                              std::function<void()> onCompleted,
                              int skipCount)
        : Observer<T>(onNext, onCompleted),
          counter(0),
          intervalCount(skipCount)
    {
    }

    void OnNext(T v) override
    {
        if (this->isStopped) return;

        if (counter++ < intervalCount - 1) return;

        counter = 0;
        this->_onNext(v);
    }
};
