#include "ObservableUtil.h"

namespace ObservableUtil
{
    std::shared_ptr<Subject<Unit>> everyUpdateSubject = std::make_shared<Subject<Unit>>();
}
