#include "Disposable.h"
#include "ObservableDestroyTrigger.h"

Disposable::Disposable(): isDisposed(false)
{
}

void Disposable::Dispose()
{
    isDisposed = true;
}

std::shared_ptr<Disposable> Disposable::AddTo(ObservableDestroyTrigger* obj)
{
    auto res = shared_from_this();
    obj->AddDisposableOnDestroy(res);
    return res;
}

std::shared_ptr<Disposable> Disposable::AddTo(std::weak_ptr<ObservableDestroyTrigger> obj)
{
    auto res = shared_from_this();
    obj.lock()->AddDisposableOnDestroy(res);
    return res;
}
