#pragma once
#include <memory>

class Disposable;

class ObservableDestroyTrigger
{
    std::weak_ptr<Disposable> _disposable;

public:
    virtual ~ObservableDestroyTrigger();

    void AddDisposableOnDestroy(std::weak_ptr<Disposable> disposable);
};
