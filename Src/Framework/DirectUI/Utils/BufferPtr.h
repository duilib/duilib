#ifndef BufferPtr_h__
#define BufferPtr_h__
//////////////////////////////////////////////////////////////////////////
// http://www.cnblogs.com/ldcsaa/archive/2012/02/18/2356638.html
// T                 : 数据类型（内置类型或结构体）
// MAX_CACHE_SIZE    : 预申请内存的最大数目，以 sizeof(T) 为单位，如果该值设置合理，对于
//                     需要动态递增缓冲区的 buffer 来说能大大提高效率
template<class T, size_t MAX_CACHE_SIZE = 0>
class CBufferPtrT
{
public:
	explicit CBufferPtrT(size_t size = 0, bool zero = false)    {Reset(); Malloc(size, zero);}
	explicit CBufferPtrT(const T* pch, size_t size)    {Reset(); Copy(pch, size);}
	// 拷贝构造函数要分两种情形
	CBufferPtrT(const CBufferPtrT& other)    {Reset(); Copy(other);}
	template<size_t S> CBufferPtrT(const CBufferPtrT<T, S>& other)    {Reset(); Copy(other);}

	~CBufferPtrT() {Free();}

	T* Malloc(size_t size = 1, bool zero = false)
	{
		Free();
		return Alloc(size, zero, false);
	}

	T* Realloc(size_t size, bool zero = false)
	{
		return Alloc(size, zero, true);
	}

	void Free()
	{
		if(m_pch)
		{
			free(m_pch);
			Reset();
		}
	}

	template<size_t S> CBufferPtrT& Copy(const CBufferPtrT<T, S>& other)
	{
		if((void*)&other != (void*)this)
			Copy(other.Ptr(), other.Size());

		return *this;
	}

	CBufferPtrT& Copy(const T* pch, size_t size)
	{
		Malloc(size);

		if(m_pch)
			memcpy(m_pch, pch, size * sizeof(T));

		return *this;
	}

	// 动态扩大 buffer
	template<size_t S> CBufferPtrT& Cat(const CBufferPtrT<T, S>& other)
	{
		if((void*)&other != (void*)this)
			Cat(other.Ptr(), other.Size());

		return *this;
	}

	// 动态扩大 buffer
	CBufferPtrT& Cat(const T* pch, size_t size = 1)
	{
		size_t pre_size = m_size;
		Realloc(m_size + size);

		if(m_pch)
			memcpy(m_pch + pre_size, pch, size * sizeof(T));

		return *this;
	}

	template<size_t S> bool Equal(const CBufferPtrT<T, S>& other) const
	{
		if((void*)&other == (void*)this)
			return true;
		else if(m_size != other.Size())
			return false;
		else if(m_size == 0)
			return true;
		else
			return (memcmp(m_pch, other.Ptr(), m_size * sizeof(T)) == 0);
	}

	bool Equal(T* pch) const
	{
		if(m_pch == pch)
			return true;
		else if(!m_pch || !pch)
			return false;
		else
			return (memcmp(m_pch, pch, m_size * sizeof(T)) == 0);
	}

	T*    Ptr()    {return m_pch;}
	const T*    Ptr()    const    {return m_pch;}
	T&    Get(int i)    {return *(m_pch + i);}
	const T&    Get(int i)    const    {return *(m_pch + i);}
	size_t    Size()    const    {return m_size;}
	bool    IsValid()    const    {return m_pch != 0;}
	// 啊哈，竟然是类型安全的
	operator    T*    ()    {return Ptr();}
	operator const    T*    ()    const    {return Ptr();}
	// 哇塞，竟然还支持索引访问
	T& operator    []    (int i)    {return Get(i);}
	const T& operator    []    (int i)    const    {return Get(i);}
	bool operator    ==    (T* pv)    const    {return Equal(pv);}
	template<size_t S> bool operator    ==    (const CBufferPtrT<T, S>& other)    {return Equal(other);}
	// 赋值操作符要分两种情形
	CBufferPtrT& operator    =    (const CBufferPtrT& other)    {return Copy(other);}
	template<size_t S> CBufferPtrT& operator    =    (const CBufferPtrT<T, S>& other)    {return Copy(other);}

private:
	void Reset()    {m_pch = 0; m_size = 0; m_capacity = 0;}
	size_t GetAllocSize(size_t size)    {return max(size, min(size * 2, m_size + MAX_CACHE_SIZE));}

	T* Alloc(size_t size, bool zero = false, bool is_realloc = false)
	{
		if(size >= 0 && size != m_size)
		{
			size_t rsize = GetAllocSize(size);
			if(size > m_capacity || rsize < m_size)
			{
				m_pch = is_realloc ? 
					(T*)realloc(m_pch, rsize * sizeof(T))    
					: (T*)malloc(rsize * sizeof(T));

				   if(m_pch || rsize == 0)
				   {
					   m_size        = size;
					   m_capacity    = rsize;
				   }
				   else
					   Reset();
			}
			else
				m_size = size;
		}

		if(zero && m_pch)
			memset(m_pch, 0, m_size * sizeof(T));

		return m_pch;
	}

private:
	T*        m_pch;
	size_t    m_size;
	size_t    m_capacity;
};
#endif // BufferPtr_h__