//--http://blog.csdn.net/haitunxiaomo/article/details/42042345
#include "IO/FileBaseStream.h"
#include "IO/MemStream.h"
#include "IO/CEFile.h"
#include "IO/BaseStream.h"
template <typename T>
CPtr<T>::CPtr(T *p) {
	m_pU = new unsigned int(1);
	m_p = p;
}

template <typename T>
CPtr<T>::CPtr(const CPtr& src) {
	m_p = src.m_p;
	m_pU = src.m_pU;
	if (m_pU){
		++*m_pU;
	}
}

template <typename T>
CPtr<T>& CPtr<T>::operator= (const CPtr& rhs) {
	if (rhs.m_pU){
		++*rhs.m_pU;
	}

	decrUsePtr();
	m_p = rhs.m_p;
	m_pU = rhs.m_pU;

	return *this;
}

template <typename T> T* CPtr<T>::get() const
{
	return m_p;
}

template <typename T>
void CPtr<T>::reset()
{
	CPtr<T> t_defaultvalue;
	(*this) = t_defaultvalue;
}

template <typename T>
bool CPtr<T>::operator== (T* rhs)
 {
	 return m_p == rhs;
 }

 template <typename T>
 bool CPtr<T>::operator!= (T* rhs)
 {
	 return m_p != rhs;
 }

 template <typename T>
 bool CPtr<T>::operator== (const CPtr<T>& rhs)
 {
	 return m_p == rhs.get();
 }

 template <typename T>
 bool CPtr<T>::operator!= (const CPtr<T>& rhs)
 {
	 return m_p != rhs.get();
 }

 template <typename T>
 T *CPtr<T>::operator->() {
	 if (m_p)
		 return m_p;
    return NULL;
 }

 template <typename T>
 const T *CPtr<T>::operator->() const {
	 if (m_p)
		 return m_p;
     return NULL;
}

template <typename T>
T &CPtr<T>::operator*() {
	if (m_p)
		return *m_p;
    int *icrash = NULL;
	*(icrash + 33) = 23563;
	return *m_p;
}

template <typename T>
const T &CPtr<T>::operator*() const {
	if (m_p)
		return *m_p;
	int *icrash = NULL;
	*(icrash + 33) = 23563;
	return *m_p;
}

template <typename T>
CPtr<T>::~CPtr() {
	decrUsePtr();
}

template <typename T>
void CPtr<T>::decrUsePtr() {
	if (m_pU && --*m_pU == 0) {
		delete m_p;
		delete m_pU;
    }
}


template class CPtr<FileBaseStream>;
template class CPtr<MemStream>;
template class CPtr<CEFile>;
template class CPtr<BaseStream>;

  