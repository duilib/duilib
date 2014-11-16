#include "StdAfx.h"

namespace DuiLib {

CDelegateBase::CDelegateBase(void* pObject, void* pFn) 
{
    m_pObject = pObject;
    m_pFn = pFn; 
}

CDelegateBase::CDelegateBase(const CDelegateBase& rhs) 
{
    m_pObject = rhs.m_pObject;
    m_pFn = rhs.m_pFn; 
}

CDelegateBase::~CDelegateBase()
{

}

bool CDelegateBase::Equals(const CDelegateBase& rhs) const 
{
    return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; 
}

bool CDelegateBase::operator() (void* param) 
{
    return Invoke(param); 
}

void* CDelegateBase::GetFn() 
{
    return m_pFn; 
}

void* CDelegateBase::GetObject() 
{
    return m_pObject; 
}

CEventSource::~CEventSource()
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject) delete pObject;
    }
}

CEventSource::operator bool()
{
    return m_aDelegates.GetSize() > 0;
}

void CEventSource::operator+= (const CDelegateBase& d)
{ 
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) return;
    }

    m_aDelegates.Add(d.Copy());
}

void CEventSource::operator+= (FnType pFn)
{ 
    (*this) += MakeDelegate(pFn);
}

void CEventSource::operator-= (const CDelegateBase& d) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) {
            delete pObject;
            m_aDelegates.Remove(i);
            return;
        }
    }
}
void CEventSource::operator-= (FnType pFn)
{ 
    (*this) -= MakeDelegate(pFn);
}

bool CEventSource::operator() (void* param) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && !(*pObject)(param) ) return false;
    }
    return true;
}

} // namespace DuiLib
