#include <assert.h>
#include <sys/eventfd.h>
#include<signal.h>
#include "EventLoop.h"
#include "Epoller.h"

__thread EventLoop *t_loopInThisThread = 0;

int createEventFd()
{
    int fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (fd < 0)
    {
        LOG_SYSFATAL << "fail in create event fd!!!!!" << ENDL;
        abort();
    }
    return fd;
}

class IgnoreSigPipe
{
public:
    IgnoreSigPipe(){
        ::signal(SIGPIPE,SIG_IGN);
    }
};
IgnoreSigPipe initSigPie;


EventLoop::EventLoop()
    : looping_(false),
      poller_(new Epoller(this)),
      quit_(false),
      pollReturnTime(),
      callPendingFunctors_(false),
      wakeupFd_(createEventFd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      timerQueue_(new TimerQueue(this)),
      threadId_(std::this_thread::get_id())
{

    LOG << "EventLoop " << this << " created in thread " << threadId_ << ENDL;
    if (t_loopInThisThread)
    {
        LOG_SYSFATAL << "Another EventLoop " << t_loopInThisThread << "extis in this thread " << threadId_
                     << ENDL;
    }
    else
    {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallback_(std::bind(&EventLoop::handelRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = NULL;
}

EventLoop *EventLoop::getEventLoopInCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    while (!quit_)
    {
        activeChannels_.clear();
        pollReturnTime = poller_->poll(&activeChannels_, kPollTimeMs);
        for (auto &channel : activeChannels_)
        {
            channel->handleEvent(pollReturnTime);
        }
        doPendingFunctors();
    }
    LOG << "EventLoop " << this << " stop looping " << ENDL;
    looping_ = false;
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callPendingFunctors_ = true;
    {
        UniqueLockGuard lg(mutex_);
        pendingFunctors_.swap(functors);
    }
    for (auto &functor : functors)
        functor();

    callPendingFunctors_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
        wakeup();
}

void EventLoop::updateChannel(Channel *c)
{
    assert(c->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(c);
}

void EventLoop::removeChannel(Channel *c)
{
    assert(c->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(c);
}

//?????????????????????IO??????????????????????????? ????????????????????????
//?????????????????????????????????runInLoop()???cb????????????????????? IO?????????????????????????????????Functor.
void EventLoop::runInLoop(const Functor &cb)
{
    //1. ????????????  2.????????????
    if (isInLoopThread())
        cb();
    else
        queueInLoop(cb);
}
//????????????wakeup()??????poll()????????????????????????
void EventLoop::wakeup()
{
    uint64_t one = 1;
    int n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_SYSFATAL << "EventLoop::wakeup() error" << ENDL;
    }
}

void EventLoop::handelRead()
{
    uint64_t one = 1;
    int n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_SYSFATAL << "EventLoop::handelRead() error" << ENDL;
    }
}

void EventLoop::queueInLoop(const Functor &cb)
{
    UniqueLockGuard lg(mutex_);
    pendingFunctors_.push_back(cb);

    if (!isInLoopThread() || callPendingFunctors_)
    {
        wakeup();
    }
}