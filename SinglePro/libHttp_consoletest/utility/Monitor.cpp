#include "Monitor.h"

namespace pcutil
{
CMonitor::CMonitor()
        : m_nnotify( 0 )
{

}

CMonitor::~CMonitor()
{


}

HRESULT CMonitor::Lock()
{
    m_mutex.Lock();
    m_nnotify = 0;
	return S_OK;
}

HRESULT CMonitor::Unlock()
{
    _NotifyImpl( m_nnotify );
    m_mutex.Unlock();
	return S_OK;
}

//void CMonitor::TryLock()
//{
//
//}

void CMonitor::Wait()
{
    _NotifyImpl( m_nnotify );
    try
    {
        m_cond.waitImpl( m_mutex );
    }
    catch ( ... )
    {
        m_nnotify = 0;
    }
    m_nnotify = 0;
}

bool CMonitor::TimedWait( DWORD dwTimeout )
{
    _NotifyImpl( m_nnotify );
    bool rc = false;
    try
    {
        rc = m_cond.timedWaitImpl( m_mutex, dwTimeout );
    }
    catch ( ... )
    {
        m_nnotify = 0;
    }

    m_nnotify = 0;
    return rc;
}

void CMonitor::Notify()
{
    if ( m_nnotify != -1 )
    {
        ++m_nnotify;
    }

}

void CMonitor::NotifyAll()
{
    m_nnotify = -1;

}

void CMonitor::_NotifyImpl( int nnotify )
{
    if ( nnotify != 0 )
    {
        //
        // -1 means notifyAll.
        //
        if ( nnotify == -1 )
        {
            m_cond.broadcast();
            return;
        }
        else
        {
            //
            // Otherwise notify n times.
            //
            while ( nnotify > 0 )
            {
                m_cond.signal();
                --nnotify;
            }
        }
    }

}















}