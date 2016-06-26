#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <tuple>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <assert.h>

namespace RoSpace
{
template< typename WorkerType, typename EventType>
class CThreadPool {
public:
    CThreadPool()
    {
        m_bStop = false;
    }

    ~CThreadPool()
    {
        if ( !m_bStop )
        {
            JoinAll ();
        }
    }

    void JoinAll()
    {
        m_bStop = true;
        m_cvNotifyWorker.notify_all ();

        for(auto & t : m_vecWorkers)
        {
            std::get<1>(t).join();
        }
    }

    void QueEvent(EventType pEvent)
    {
        std::lock_guard<std::mutex> lk(m_mtxSyncEvents);
        m_queEvents.push (pEvent);
        m_cvNotifyWorker.notify_one ();
    }
protected:
    bool Init(unsigned uThreadCount)
    {
        assert( m_vecWorkers.empty () && uThreadCount );

        std::vector<pWorkerType> vecWorkerObjectes;

        for(unsigned u=0; u<uThreadCount; ++u)
        {
            pWorkerType pWorker = std::make_shared<WorkerType>();

            if ( !pWorker->Init(u) )
            {
                return false;
            }
            else
            {
                vecWorkerObjectes.push_back ( pWorker );
            }
        }

        for(unsigned u=0; u<uThreadCount; ++u)
        {
            m_vecWorkers.push_back (
                        std::make_tuple(u, std::thread(&CThreadPool::ThreadRun,
                                                       this, u),
                                        vecWorkerObjectes[u]) );
        }

        return true;
    }
private:
    void ThreadRun(unsigned uThreadIndex)
    {
        while ( true )
        {
            std::unique_lock<std::mutex> lk(m_mtxSyncEvents);
            m_cvNotifyWorker.wait (lk, [&]() -> bool {
                                       return !m_queEvents.empty () || m_bStop;
                                   });
            if ( !m_queEvents.empty () )
            {
                EventType xEvent = m_queEvents.front ();
                m_queEvents.pop ();
                lk.unlock ();
                std::get<2>(m_vecWorkers[uThreadIndex])->ProcessEvent( xEvent );
            }
            else
            {
                break;
            }
        }
    }

private:
    typedef std::shared_ptr<WorkerType> pWorkerType;
    std::vector< std::tuple<unsigned, std::thread, pWorkerType> > m_vecWorkers; // 工作者
    std::queue< EventType > m_queEvents; // 事件队列
    std::mutex m_mtxSyncEvents; // 同步事件队列的互斥量
    std::condition_variable m_cvNotifyWorker; // 通知工作者的条件变量
    std::atomic_bool m_bStop; // 是否已经停止
};

}

#endif // THREADPOOL_H
