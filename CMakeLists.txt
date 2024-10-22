cmake_minimum_required(VERSION 3.26)
project(Rx)

set(CMAKE_CXX_STANDARD 14)

include_directories(Rx/Src)
include_directories(Rx/Src/Observer)
include_directories(Rx/Src/Sample)
include_directories(Rx/Src/Test)

add_executable(Rx
        Rx/Src/Observer/IntervalObserver.h
        Rx/Src/Observer/SkipObserver.h
        Rx/Src/Observer/TakeObserver.h
        Rx/Src/Sample/EnemySample.h
        Rx/Src/Sample/SampleFunc.h
        Rx/Src/Test/Test.h
        Rx/Src/Disposable.cpp
        Rx/Src/Disposable.h
        Rx/Src/main.cpp
        Rx/Src/Observable.h
        Rx/Src/ObservableDestroyTrigger.cpp
        Rx/Src/ObservableDestroyTrigger.h
        Rx/Src/ObservableUtil.cpp
        Rx/Src/ObservableUtil.h
        Rx/Src/Observer.h
        Rx/Src/Subject.h
        Rx/Src/Unit.h)
