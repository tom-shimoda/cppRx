#pragma once

#pragma once
#include <iostream>
#include <memory>
#include <string>

#include "../Observable.h"
#include "../ObservableDestroyTrigger.h"
#include "../Subject.h"
#include "../Unit.h"

namespace Test
{
    using TestResult = std::tuple<bool, std::string>;
    
    static void IsClear(TestResult res)
    {
        if (!std::get<0>(res))
        {
            std::cout << "\033[31m" << "Test failed: " << "\033[m" << std::get<1>(res) << std::endl;
        }
    }

    // Where テスト
    static TestResult WhereTest()
    {
        std::string res;

        const auto subject = std::make_shared<Subject<std::string>>();
        auto _ = subject->GetObservable()
                        ->Where([](const std::string& s)
                        {
                            return s != "Fuga";
                        })
                        ->Subscribe([&](const std::string& s) mutable
                        {
                            res = s;
                        });

        // 実行処理
        subject->OnNext("test");
        bool test1 = res == "test";

        subject->OnNext("Fuga");
        bool test2 = res == "test";

        return {test1 && test2, "WhereTest"};
    }

    // Select テスト
    static TestResult SelectTest()
    {
        int res = -1;

        const auto subject = std::make_shared<Subject<std::string>>();
        auto _ = subject->GetObservable()
                        ->Select<int>([](const std::string& s)
                        {
                            return atoi(s.c_str());
                        })
                        ->Subscribe([&](int i) mutable
                        {
                            res = i;
                        });

        // 実行処理
        subject->OnNext("123");

        return {res == 123, "SelectTest"};
    }

    // Skip テスト
    static TestResult SkipTest()
    {
        std::vector<int> res;

        const auto subject = std::make_shared<Subject<int>>();
        auto _ = subject->GetObservable()
                        ->Skip(3)
                        ->Subscribe([&](int i) mutable
                        {
                            res.emplace_back(i);
                        });

        // 実行処理
        for (int i = 0; i < 5; i++)
        {
            subject->OnNext(i);
        }

        bool test1 = res[0] == 3;
        bool test2 = res[1] == 4;

        return {test1 && test2, "SkipTest"};
    }

    // Take テスト
    static TestResult TakeTest()
    {
        int res = 0;

        const auto subject = std::make_shared<Subject<Unit>>();
        static std::shared_ptr<Disposable> d;
        d = subject->GetObservable()
                   ->Take(3)
                   ->Subscribe([&](Unit u) mutable
                   {
                       ++res;
                   });

        // 実行処理
        for (int i = 0; i < 5; i++)
        {
            subject->OnNext(Unit());
        }

        return {res == 3, "TakeTest"};
    }

    // Interval テスト
    static TestResult IntervalTest()
    {
        std::vector<int> res;

        const auto subject = std::make_shared<Subject<int>>();
        auto _ = subject->GetObservable()
                        ->Interval(3)
                        ->Subscribe([&](int i) mutable
                        {
                            res.emplace_back(i);
                        });

        // 実行処理
        for (int i = 0; i < 9; i++)
        {
            subject->OnNext(i);
        }

        bool test1 = res[0] == 2;
        bool test2 = res[1] == 5;
        bool test3 = res[2] == 8;

        return {test1 && test2 && test3, "IntervalTest"};
    }

    // Where Chain テスト
    static TestResult WhereChainTest()
    {
        int res = -1;

        const auto subject = std::make_shared<Subject<std::string>>();
        auto _ = subject->GetObservable()
                        ->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                        ->Where([](int i) { return i > 0; })
                        ->Subscribe([&](int i) mutable
                        {
                            res = i;
                        });

        // 実行処理
        subject->OnNext("123");
        bool test1 = res == 123;

        subject->OnNext("0");
        bool test2 = res == 123;

        subject->OnNext("-456");
        bool test3 = res == 123;

        return {test1 && test2 && test3, "WhereChainTest"};
    }

    // Select Chain テスト
    static TestResult SelectChainTest()
    {
        int res = -1;

        const auto subject = std::make_shared<Subject<std::string>>();
        auto _ = subject->GetObservable()
                        ->Where([](const std::string& s) { return s == "123"; })
                        ->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                        ->Subscribe([&](int i) mutable
                        {
                            res = i;
                        });

        // 実行処理
        subject->OnNext("123");
        bool test1 = res == 123;

        subject->OnNext("0");
        bool test2 = res == 123;

        subject->OnNext("-456");
        bool test3 = res == 123;

        return {test1 && test2 && test3, "SelectChainTest"};
    }

    // Skip Chain テスト
    static TestResult SkipChainTest()
    {
        int res = -1;

        const auto subject = std::make_shared<Subject<std::string>>();
        auto _ = subject->GetObservable()
                        ->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                        ->Skip(2)
                        ->Where([](int i) { return i > 0; })
                        ->Subscribe([&](int i) mutable
                        {
                            res = i;
                        });

        // 実行処理
        subject->OnNext("123");
        bool test1 = res == -1;

        subject->OnNext("123");
        bool test2 = res == -1;

        subject->OnNext("123");
        bool test3 = res == 123;

        subject->OnNext("456");
        bool test4 = res == 456;

        return {test1 && test2 && test3 && test4, "SkipChainTest"};
    }

    // Take Chain テスト
    static TestResult TakeChainTest()
    {
        int res = -1;

        const auto subject = std::make_shared<Subject<std::string>>();
        auto _ = subject->GetObservable()
                        ->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                        ->Take(2)
                        ->Where([](int i) { return i > 0; })
                        ->Subscribe([&](int i) mutable
                        {
                            res = i;
                        });

        // 実行処理
        subject->OnNext("123");
        bool test1 = res == 123;

        subject->OnNext("456");
        bool test2 = res == 456;

        subject->OnNext("789");
        bool test3 = res == 456;

        return {test1 && test2 && test3, "TakeChainTest"};
    }

    // Interval Chain テスト
    static TestResult IntervalChainTest()
    {
        int res = -1;

        const auto subject = std::make_shared<Subject<std::string>>();
        auto _ = subject->GetObservable()
                        ->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                        ->Interval(2)
                        ->Where([](int i) { return i > 0; })
                        ->Subscribe([&](int i) mutable
                        {
                            res = i;
                        });

        // 実行処理
        subject->OnNext("123");
        bool test1 = res == -1;

        subject->OnNext("123");
        bool test2 = res == 123;

        subject->OnNext("456");
        bool test3 = res == 123;

        subject->OnNext("456");
        bool test4 = res == 456;

        return {test1 && test2 && test3 && test4, "IntervalChainTest"};
    }

    // Subscribeを複数回行うテスト
    static TestResult SubscribeManyTimesTest()
    {
        std::string res1, res2;

        const auto subject = std::make_shared<Subject<Unit>>();
        auto d1 = subject->GetObservable()
                         ->Subscribe([&](Unit _) mutable
                         {
                             res1 = "test1";
                         });

        auto d2 = subject->GetObservable()
                         ->Subscribe([&](Unit _) mutable
                         {
                             res2 = "test2";
                         });

        // 実行処理
        subject->OnNext(Unit());

        return {res1 == "test1" && res2 == "test2", "SubscribeManyTimesTest"};
    }

    // Diposeテスト
    static TestResult DisposeTest()
    {
        int res1 = 0, res2 = 0;

        const auto subject = std::make_shared<Subject<Unit>>();
        auto d1 = subject->GetObservable()
                         ->Subscribe([&](Unit _) mutable
                         {
                             ++res1;
                         });

        static std::shared_ptr<Disposable> d2;
        d2 = subject->GetObservable()
                    ->Subscribe([&](Unit _) mutable
                    {
                        ++res2;

                        // 登録した処理内でのDisposeテスト → この例の場合、初回出力後に廃棄される
                        d2->Dispose();
                    });

        // 実行処理
        subject->OnNext(Unit());
        bool test1 = res1 == 1 && res2 == 1;

        subject->OnNext(Unit());
        bool test2 = res1 == 2 && res2 == 1;
        d1->Dispose();

        subject->OnNext(Unit());
        bool test3 = res1 == 2 && res2 == 1;

        return {test1 && test2 && test3, "DisposeTest"};
    }

    // AddToテスト
    static TestResult AddToTest()
    {
        std::string res;

        // 寿命同期テスト用オブジェクト
        struct LifeTimeObject : ObservableDestroyTrigger
        {
        };
        auto lifetimeObj = std::make_shared<LifeTimeObject>();

        const auto subject = std::make_shared<Subject<std::string>>();
        auto d = subject->GetObservable()
                        ->Subscribe([&](const std::string& s) mutable
                        {
                            res = s;
                        })
                        ->AddTo(lifetimeObj);

        // 実行処理
        subject->OnNext("test");
        bool test1 = res == "test";

        lifetimeObj = nullptr; // 寿命同期用オブジェクトを削除

        subject->OnNext("Fuga");
        bool test2 = res == "test";

        return {test1 && test2, "AddToTest"};
    }

    // 同一メソッドチェーンSubscribeしない場合のテスト
    static TestResult ColdObservableTest()
    {
        auto res = -1;
        
        auto subject = std::make_shared<Subject<std::string>>();
        auto o = subject->GetObservable()
                        ->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                        ->Where([](int i) { return i > 0; });

        auto d = o->Subscribe([&](int i) mutable 
        {
            res = i;
        });

        // 実行処理
        subject->OnNext("123");
        bool test1 = res == 123;

        subject->OnNext("0");
        bool test2 = res == 123;

        subject->OnNext("-456");
        bool test3 = res == 123;

        return {test1 && test2 && test3 , "ColdObservableTest"};
    }
    
    static void DoTest()
    {
        IsClear(WhereTest());
        IsClear(SelectTest());
        IsClear(SkipTest());
        IsClear(TakeTest());
        IsClear(IntervalTest());

        IsClear(WhereChainTest());
        IsClear(SelectChainTest());
        IsClear(SkipChainTest());
        IsClear(TakeChainTest());
        IsClear(IntervalChainTest());

        IsClear(SubscribeManyTimesTest());
        IsClear(DisposeTest());
        IsClear(AddToTest());
        IsClear(ColdObservableTest());
    }
};
