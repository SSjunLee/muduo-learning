// excerpts from http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)


#pragma once
#include <map>
#include <vector>
#include "nocopyable.h"
#include "Timestamp.h"
#include "EventLoop.h"
class EventLoop;
struct pollfd;


class Channel;

///
/// IO Multiplexing with poll(2).
///
/// This class doesn't own the Channel objects.
class Epoller : nocopyable
{
 public:
  typedef std::vector<Channel*> ChannelList;

  Epoller(EventLoop* loop);
  ~Epoller();

  /// Polls the I/O events.
  /// Must be called in the loop thread.
  Timestamp poll(ChannelList* activeChannels,int timeoutMs);

  /// Changes the interested I/O events.
  /// Must be called in the loop thread.
  void updateChannel(Channel* channel);
  /// Remove the channel, when it destructs.
  /// Must be called in the loop thread.
  void removeChannel(Channel* channel);

  void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

 private:
  void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;

  typedef std::vector<struct pollfd> PollFdList;
  typedef std::map<int, Channel*> ChannelMap;

  EventLoop* ownerLoop_;
  PollFdList pollfds_;
  ChannelMap channels_;
};



