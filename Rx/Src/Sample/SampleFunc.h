#pragma once
#include <iostream>
#include <memory>
#include <string>

#include "../Observable.h"
#include "../ObservableDestroyTrigger.h"
#include "../ObservableUtil.h"
#include "../Subject.h"
#include "../Unit.h"

class SampleFunc
{
    // オブザーバー連結パターン
    static void ObserverChainSample(const std::shared_ptr<Subject<std::string>>& subject)
    {
        auto _ = subject->GetObservable()
                        ->Select<std::string>([](const std::string& s)
                        {
                            std::cout << s << " is coming. Repeat twice." << std::endl;
                            return s + s;
                        })
                        ->Where([](const std::string& s)
                        {
                            if (s == "FugaFuga")
                            {
                                std::cout << "FugaFuga don't output" << std::endl;
                                return false;
                            }

                            return true;
                        })
                        ->Subscribe([](const std::string& s)
                        {
                            std::cout << "value: " << s << std::endl;
                        });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext("test");

        std::cout << "Send value. (2)" << std::endl;
        subject->OnNext("Fuga");

        std::cout << "Send value. (3)" << std::endl;
        subject->OnNext("HogeHoge");
    }

    // Selectの戻り値の型が異なる場合の例
    static void SelectObservableSample(const std::shared_ptr<Subject<std::string>>& subject)
    {
        auto _ = subject->GetObservable()
                        ->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                        ->Where([](int i) { return i > 0; })
                        ->Subscribe([](int i)
                        {
                            std::cout << "value: " << i << std::endl;
                        });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext("123");

        std::cout << "Send value. (2)" << std::endl;
        subject->OnNext("0");

        std::cout << "Send value. (3)" << std::endl;
        subject->OnNext("-456");
    }

    // Subscribeを複数回行う例
    static void SubscribeManyTimesSample(const std::shared_ptr<Subject<std::string>>& subject)
    {
        auto d1 = subject->GetObservable()
                         ->Select<std::string>([](const std::string& s) { return s + s; })
                         ->Where([](const std::string& s) { return s != "FugaFuga"; })
                         ->Subscribe([](const std::string& s)
                         {
                             std::cout << "value: " << s << std::endl;
                         });

        auto d2 = subject->GetObservable()
                         ->Subscribe([](const std::string& s)
                         {
                             std::cout << "--------------------------------------" << std::endl;
                         });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext("test");

        std::cout << "Send value. (2)" << std::endl;
        subject->OnNext("Fuga");

        std::cout << "Send value. (3)" << std::endl;
        subject->OnNext("HogeHoge");
    }

    // Dipose例
    static void DisposeSample(const std::shared_ptr<Subject<std::string>>& subject)
    {
        auto d1 = subject->GetObservable()
                         ->Select<std::string>([](const std::string& s) { return s + s; })
                         ->Where([](const std::string& s) { return s != "FugaFuga"; })
                         ->Subscribe([](const std::string& s)
                         {
                             std::cout << "value: " << s << std::endl;
                         });

        static std::shared_ptr<Disposable> d2;
        d2 = subject->GetObservable()
                    ->Subscribe([](const std::string& s)
                    {
                        std::cout << "--------------------------------------" << std::endl;

                        // 登録した処理内でのDispose → この例の場合、初回出力後に廃棄される
                        d2->Dispose();
                    });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext("test");

        std::cout << "Send value. (2)" << std::endl;
        subject->OnNext("Fuga");
        d1->Dispose();

        std::cout << "Send value. (3)" << std::endl;
        subject->OnNext("HogeHoge");
    }

    // AddToによるDispose例
    static void AddToSample(const std::shared_ptr<Subject<std::string>>& subject)
    {
        // 寿命同期テスト用オブジェクト
        struct LifeTimeObject : ObservableDestroyTrigger
        {
        };
        auto lifetimeObj = std::make_shared<LifeTimeObject>();

        auto d = subject->GetObservable()
                        ->Select<std::string>([](const std::string& s) { return s + s; })
                        ->Where([](const std::string& s) { return s != "FugaFuga"; })
                        ->Subscribe([](const std::string& s)
                        {
                            std::cout << "value: " << s << std::endl;
                        })
                        ->AddTo(lifetimeObj);

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext("test");

        // d->Dispose(); // 寿命が来る前に手動でDispose
        lifetimeObj = nullptr; // 寿命同期用オブジェクトを削除

        std::cout << "Send value. (2)" << std::endl;
        subject->OnNext("Fuga");

        std::cout << "Send value. (3)" << std::endl;
        subject->OnNext("HogeHoge");
    }

    // 渡すものがない場合の例
    static void EmptySample(const std::shared_ptr<Subject<Unit>>& subject)
    {
        auto d = subject->GetObservable()
                        ->Subscribe([](Unit _)
                        {
                            std::cout << "Empty." << std::endl;
                        });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext(Unit());
    }

    // 毎フレーム更新処理として登録する例
    static void EveryUpdateSample()
    {
        // 毎フレーム更新したい処理の登録例 (UnirxのObservable.EveryUpdate()的な)
        auto _ =
            ObservableUtil::EveryUpdate()
            ->Take(3) // Disposeを呼ぶためのTake
            ->Subscribe([](Unit _)
            {
                std::cout << "Update." << std::endl;
            });
    }

    // 同一メソッドチェーンSubscribeしない場合の例
    static void ColdObservableSample()
    {
        auto subject = std::make_shared<Subject<Unit>>();
        auto o = subject->GetObservable()
                        ->Where([](Unit _)
                        {
                            std::cout << "1" << std::endl;
                            return true;
                        })
                        ->Select<int>([](Unit _)
                        {
                            std::cout << "2" << std::endl;
                            return 3;
                        })
                        ->Where([](int i)
                        {
                            std::cout << i << std::endl;
                            return true;
                        });

        auto d = o->Subscribe([](int _)
        {
            std::cout << "End." << std::endl;
        });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext(Unit());
    }

public:
    static void DoIt()
    {
        // オブザーバー連結パターン
        // ObserverChainSample(std::make_shared<Subject<std::string>>());

        // Selectの戻り値の型が異なる場合の例
        // SelectObservableSample(std::make_shared<Subject<std::string>>());

        // Subscribeを複数回行う例
        // SubscribeManyTimesSample(std::make_shared<Subject<std::string>>());

        // Dipose例
        // DisposeSample(std::make_shared<Subject<std::string>>());

        // AddToによるDispose例
        // AddToSample(std::make_shared<Subject<std::string>>());

        // 渡すものがない場合の例
        // EmptySample(std::make_shared<Subject<Unit>>());

        // 毎フレーム更新処理として登録する例
        // EveryUpdateSample();

        // 同一メソッドチェーンSubscribeしない場合の例
        ColdObservableSample();
    }
};
