#pragma once

// http://stackoverflow.com/questions/5956759/c11-stdthread-inside-a-class-executing-a-function-member-with-thread-initia

#include <atomic>
#include <thread>

class Runnable
{
public:
    Runnable() : m_stop(), m_thread(), m_started(false) {}
    virtual ~Runnable() { try { Stop(); } catch (...) { /*??*/ } }

    Runnable(Runnable const&) = delete;
    Runnable& operator =(Runnable const&) = delete;

    void Stop() { m_stop = true; if (m_started) { m_thread.join(); } }
    void Start() { m_thread = std::thread(&Runnable::Run, this); m_started = true; }

protected:
    virtual void Run() = 0;
    std::atomic<bool> m_stop;
    bool m_started;

private:
    std::thread m_thread;
};

