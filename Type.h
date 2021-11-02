#pragma once
#include<functional>
#include<mutex>
#include"nocopyable.h"
typedef std::function<void()> TimerCallback;
//typedef std::lock_guard<std::mutex> MutexLockGuard;
typedef std::unique_lock<std::mutex> UniqueLockGuard;
