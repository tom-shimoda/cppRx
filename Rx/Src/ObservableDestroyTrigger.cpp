#include "ObservableDestroyTrigger.h"
#include "Disposable.h"

ObservableDestroyTrigger::~ObservableDestroyTrigger()
{
    if (auto p = _disposable.lock()) p->Dispose();
}

void ObservableDestroyTrigger::AddDisposableOnDestroy(std::weak_ptr<Disposable> disposable)
{
    _disposable = disposable;
}
