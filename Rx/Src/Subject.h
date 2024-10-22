#pragma once
#include <list>
#include <memory>
#include <stdexcept>

#include "Observable.h"
#include "Observer.h"

template <typename T>
class Subject
{
    // Disposeで該当Observerの登録解除ができるようペアにしておく
    struct Source
    {
        std::shared_ptr<Observer<T>> observer;
        std::shared_ptr<Disposable> disposer;

        Source(std::shared_ptr<Observer<T>> observer, std::shared_ptr<Disposable> disposer)
            : observer(std::move(observer)),
              disposer(std::move(disposer))
        {
        }
    };

    struct Disposer : Disposable
    {
        std::list<Source>* src;
        std::list<Source*>* willDispose;

        Disposer(std::list<Source>* src, std::list<Source*>* willDispose): src(src), willDispose(willDispose)
        {
        }

        ~Disposer() override = default;

        void Dispose() override
        {
            if (IsDisposed()) return;

            for (auto itr = src->begin(); itr != src->end();)
            {
                if ((*itr).disposer.get() == this)
                {
                    willDispose->emplace_back(&*itr);
                }
                ++itr;
            }

            // 基底を呼ぶのを忘れずに。(忘れると、寿命が来る前に手動Disposeした場合にエラーとなる)
            Disposable::Dispose();
        }
    };

    // 登録物
    std::list<Source> source;
    // 廃棄予定のもの
    std::list<Source*> willDisposeSourceList;

    // 廃棄予定のものを廃棄
    void Dispose()
    {
        for (auto willItr = willDisposeSourceList.begin(); willItr != willDisposeSourceList.end();)
        {
            auto isHit = false;

            for (auto itr = source.begin(); itr != source.end();)
            {
                if (&*itr == *willItr)
                {
                    itr = source.erase(itr);
                    isHit = true;
                    break;
                }
                ++itr;
            }

            if (isHit)
            {
                willItr = willDisposeSourceList.erase(willItr);
                continue;
            }
            ++willItr;
        }
    }

public:
    void OnNext(T v)
    {
        // Dispose単体で呼んだ場合は、予約されただけの状態なのでここで廃棄される
        Dispose();

        for (auto&& e : source)
        {
            e.observer->OnNext(v);
        }

        // OnNext処理内にてDisposeを呼んだ場合はここで廃棄される
        Dispose();
    }

    void OnCompleted()
    {
        for (auto&& e : source)
        {
            e.observer->OnCompleted();
        }
    }

    std::shared_ptr<Observable<T>> GetObservable()
    {
        auto disposer = std::make_shared<Disposer>(&source, &willDisposeSourceList);

        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                source.emplace_back(o, disposer);
                return disposer;
            },
            disposer,
            nullptr
        );
    }
};
