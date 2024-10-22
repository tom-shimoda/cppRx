#pragma once
#include <memory>

#include "Observable.h"
#include "Unit.h"
#include "Subject.h"

namespace ObservableUtil
{
    extern std::shared_ptr<Subject<Unit>> everyUpdateSubject;
    
    inline std::shared_ptr<Observable<Unit>> EveryUpdate()
    {
        return everyUpdateSubject->GetObservable();
    }

    inline void DoEveryUpdate()
    {
        everyUpdateSubject->OnNext(Unit());
    }
}
