#pragma once
#include<functional>
#include<mutex>
#include <memory>
#include "Timestamp.h"
#include"nocopyable.h"
class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback; 
typedef std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)> MessageCallback; 
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback; 

typedef std::function<void()> TimerCallback;
//typedef std::lock_guard<std::mutex> MutexLockGuard;
typedef std::unique_lock<std::mutex> UniqueLockGuard;
