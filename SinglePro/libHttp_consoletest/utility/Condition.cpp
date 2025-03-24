//#include "stdafx.h"
#include "Condition.h"

namespace pcutil
{

CCondition::CCondition()
        : m_gate( 1 , 0x7FFFFFFF ),
        m_queue( 0 , 0x7FFFFFFF ),
        m_blocked( 0 ),
        m_unblocked( 0 ),
        m_toUnblock( 0 )
{

}

CCondition::~CCondition()
{


}

void CCondition::signal()
{
    _wake( false );
}

void CCondition::broadcast()
{
    _wake( true );
}

void CCondition::_wake( bool broadcast )
{
    WaitForSingleObject( m_gate , INFINITE );
    m_internal.Lock();

    if ( m_unblocked != 0 )
    {
        m_blocked -= m_unblocked;
        m_unblocked = 0;
    }

    if ( m_blocked > 0 )
    {
        m_toUnblock = ( broadcast ) ? m_blocked : 1;
        m_internal.Unlock();
        m_queue.Release( 1 , NULL );

    }
    else
    {
        m_internal.Unlock();
        m_gate.Release( 1 , NULL );
    }
}

void CCondition::_preWait()
{
    WaitForSingleObject( m_gate , INFINITE );
    m_blocked++;
    m_gate.Release( 1 , NULL );

}

void CCondition::_postWait( bool timedout )
{
    m_internal.Lock();
    m_unblocked++;

    if ( m_toUnblock != 0 )
    {
        bool last = --m_toUnblock == 0;
        m_internal.Unlock();

        if ( timedout )
        {
            WaitForSingleObject( m_queue , INFINITE );
        }

        if ( last )
        {
            m_gate.Release( 1 , NULL );
        }
        else
        {
            m_queue.Release( 1 , NULL );
        }
    }
    else
    {
        m_internal.Unlock();
    }
}

void CCondition::_dowait()
{
    try
    {
        WaitForSingleObject( m_queue, INFINITE );
        _postWait( false );
    }
    catch ( ... )
    {
        _postWait( false );
        throw;
    }
}

bool CCondition::_timedDowait( DWORD timeout )
{
    try
    {
        int ret = WaitForSingleObject( m_queue, timeout );
        bool rc = ( ret != WAIT_TIMEOUT );
        _postWait( !rc );
        return rc;
    }
    catch ( ... )
    {
        _postWait( false );
    }
    return false;
}


//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//CCondition::CCondition() :
//        m_blocked( 0 ),
//        m_unblocked( 0 ),
//        _state( CCondition::StateIdle ),
//        m_gate( 1 , 0x7FFFFFFF ),
//        m_queue( 0 , 0x7FFFFFFF )
//{
//}
//
//CCondition::~CCondition()
//{
//}
//
//void CCondition::signal()
//{
//    wake( false );
//}
//
//void CCondition::broadcast()
//{
//    wake( true );
//}
//
//void CCondition::wake( bool broadcast )
//{
//    WaitForSingleObject( m_gate , INFINITE );
//    m_internal.Lock();
//    if ( m_unblocked != 0 )
//    {
//        m_blocked -= m_unblocked;
//        m_unblocked = 0;
//    }
//    if ( m_blocked > 0 )
//    {
//        ATLASSERT( _state == StateIdle );
//        _state = ( broadcast ) ? StateBroadcast : StateSignal;
//        m_internal.Unlock();
//        m_queue.Release( 1 , NULL );
//    }
//    else
//    {
//        m_internal.Unlock();
//        m_gate.Release( 1 , NULL );
//    }
//}
//
//void CCondition::preWait()
//{
//    WaitForSingleObject( m_gate , INFINITE );
//    m_blocked++;
//    m_gate.Release( 1 , NULL );
//}
//
//void CCondition::postWait( bool timedOutOrFailed )
//{
//    m_internal.Lock();
//    m_unblocked++;
//    if ( _state == StateIdle )
//    {
//        ATLASSERT( timedOutOrFailed );
//        return;
//    }
//    if ( timedOutOrFailed )
//    {
//        if ( m_blocked == m_unblocked )
//        {
//            _state = StateIdle;
//            WaitForSingleObject( m_queue , INFINITE );
//            m_internal.Unlock();
//            m_gate.Release( 1 , NULL );
//        }
//    }
//    else
//    {
//        if ( _state == StateSignal || m_blocked == m_unblocked ) 
//        {
//            _state = StateIdle;
//            m_internal.Unlock();
//            m_gate.Release( 1 , NULL );
//        }
//        else 
//        {
//            m_internal.Unlock();
//            m_queue.Release( 1 , NULL );
//
//        }
//    }
//}
//
//void CCondition::dowait()
//{
//    try
//    {
//        WaitForSingleObject( m_queue, INFINITE );
//        postWait( false );
//    }
//    catch ( ... )
//    {
//        postWait( true );
//    }
//}
//
//bool CCondition::timedDowait( DWORD timeout )
//{
//    try
//    {
//        int ret = WaitForSingleObject( m_queue, timeout );
//        bool rc = ( ret != WAIT_TIMEOUT );
//        postWait( !rc );
//        return rc;
//    }
//    catch ( ... )
//    {
//        postWait( true );
//        return false;
//    }
//}









}