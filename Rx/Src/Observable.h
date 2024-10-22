#pragma once
#include <functional>

#include "Disposable.h"
#include "Observer.h"
#include "Observer/SkipObserver.h"
#include "Observer/TakeObserver.h"
#include "Observer/IntervalObserver.h"

// 同一メソッドチェーンSubscribeしなかった場合に、チェーンしたObservableのshared_ptrが解放されてしまうのを回避するためのクラス
class ObservableRef
{
};

template <typename T>
class Observable : public ObservableRef, public std::enable_shared_from_this<Observable<T>>
{
    std::function<std::shared_ptr<Disposable>(std::shared_ptr<Observer<T>>)> subscribe;
    std::shared_ptr<Disposable> disposable;
    std::shared_ptr<ObservableRef> methodChainParent; // 解放されないようメソッドチェーンの親の参照を握っておく

public:
    Observable(std::function<std::shared_ptr<Disposable>(std::shared_ptr<Observer<T>>)> subscribe,
               std::shared_ptr<Disposable> disposable,
               std::shared_ptr<ObservableRef> methodChainParent)
        : subscribe(std::move(subscribe)),
          disposable(std::move(disposable)),
          methodChainParent(methodChainParent)
    {
    }

    std::shared_ptr<Disposable> Subscribe(std::shared_ptr<Observer<T>> observer) const
    {
        return subscribe(observer);
    }

    std::shared_ptr<Disposable> Subscribe(std::function<void(T)> onNext,
                                          std::function<void()> onCompleted = nullptr) const
    {
        return Subscribe(std::make_shared<Observer<T>>(
            [=](const T& v)
            {
                onNext(v);
            },
            [=]
            {
                if (onCompleted != nullptr) onCompleted();
            }
        ));
    }

    template <typename Ret>
    std::shared_ptr<Observable<Ret>> Select(std::function<Ret(T)> select)
    {
        return std::make_shared<Observable<Ret>>(
            [=](std::shared_ptr<Observer<Ret>> o)
            {
                return Subscribe(
                    [=](const T& v)
                    {
                        o->OnNext(select(v));
                    },
                    [=]
                    {
                        o->OnCompleted();
                    }
                );
            },
            disposable,
            std::static_pointer_cast<ObservableRef>(this->shared_from_this())
        );
    }

    std::shared_ptr<Observable<T>> Where(std::function<bool(T)> where)
    {
        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                return Subscribe(
                    [=](const T& v)
                    {
                        if (where(v))
                        {
                            o->OnNext(v);
                        }
                    },
                    [=]
                    {
                        o->OnCompleted();
                    }
                );
            },
            disposable,
            std::static_pointer_cast<ObservableRef>(this->shared_from_this())
        );
    }

    std::shared_ptr<Observable<T>> Skip(int num)
    {
        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                return Subscribe(
                    std::make_shared<SkipObserver<T>>(
                        [=](const T& v)
                        {
                            o->OnNext(v);
                        },
                        [=]
                        {
                            o->OnCompleted();
                        },
                        num
                    )
                );
            },
            disposable,
            std::static_pointer_cast<ObservableRef>(this->shared_from_this())
        );
    }

    std::shared_ptr<Observable<T>> Take(int num)
    {
        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                return Subscribe(
                    std::make_shared<TakeObserver<T>>(
                        [=](const T& v)
                        {
                            o->OnNext(v);
                        },
                        [=]
                        {
                            o->OnCompleted();
                        },
                        num,
                        disposable
                    )
                );
            },
            disposable,
            std::static_pointer_cast<ObservableRef>(this->shared_from_this())
        );
    }

    std::shared_ptr<Observable<T>> Interval(int num)
    {
        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                return Subscribe(
                    std::make_shared<IntervalObserver<T>>(
                        [=](const T& v)
                        {
                            o->OnNext(v);
                        },
                        [=]
                        {
                            o->OnCompleted();
                        },
                        num
                    )
                );
            },
            disposable,
            std::static_pointer_cast<ObservableRef>(this->shared_from_this())
        );
    }
};
