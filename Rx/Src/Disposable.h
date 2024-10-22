#pragma once
#include <memory>

class ObservableDestroyTrigger;

class Disposable : public std::enable_shared_from_this<Disposable>
{
    bool isDisposed;

public:
    Disposable();
    virtual ~Disposable() = default;

    virtual void Dispose();
    bool IsDisposed() const { return isDisposed; }
    std::shared_ptr<Disposable> AddTo(ObservableDestroyTrigger* obj);
    std::shared_ptr<Disposable> AddTo(std::weak_ptr<ObservableDestroyTrigger> obj);
};
