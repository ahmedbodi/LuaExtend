#pragma once


template <typename T>
class CPtr
{
public:
	CPtr(T *point = 0);
	CPtr(const CPtr& srcptr);
	~CPtr();
	CPtr& operator= (const CPtr& p);
	bool operator== (T*p);
	bool operator== (const CPtr& p);
	bool operator!= (T*p);	
	bool operator!= (const CPtr& p);
	T &operator*();
	T*get() const;
	const T &operator*() const;
	T *operator->();
	const T *operator->() const;	
	void reset();
private:
	T *m_p;	
	unsigned int *m_pU;
	void decrUsePtr();
};
struct ESM
{
	enum MD
	{
		FAM_NONE = 0,
		FAM_READ = 1,
		FAM_WRITE = 2,
	};
};
struct SeekType
{
	enum ST{
		PT_BEGIN = 0,
		PT_CURRENT = 1,
		PT_END = 2,
	};
};
class BaseStream
{
public:
	BaseStream(){ m_mode = ESM::FAM_NONE; }
	BaseStream(int accessMode){ m_mode = accessMode; }
	virtual ~BaseStream(){}
	int getAccessMode() const { return m_mode; }
	virtual short readShort() const;
	virtual void writeShort(short v);
	virtual int	read(void *o, int s) const = 0;
	virtual int write(void *d, int s) = 0;
    virtual bool rOrw() const = 0;
	virtual bool openFS() = 0;
	virtual bool seek(int o, int mode) = 0;
	
	virtual bool isEof() const = 0;
protected:
	void setMode(int newMode){ m_mode = newMode; }
private:
	int m_mode;
};


typedef CPtr<BaseStream> BaseStreamPtr;