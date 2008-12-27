#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include "npapi.h"

class CNSStreamImpl : public IStream
{
public:
	CNSStreamImpl(NPStream *p);
	~CNSStreamImpl(void);

private:
	NPStream *innerStream;
public:
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek( 
		/* [in] */ LARGE_INTEGER dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [out] */ ULARGE_INTEGER *plibNewPosition) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetSize( 
		/* [in] */ ULARGE_INTEGER libNewSize) = 0;

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo( 
		/* [unique][in] */ IStream *pstm,
		/* [in] */ ULARGE_INTEGER cb,
		/* [out] */ ULARGE_INTEGER *pcbRead,
		/* [out] */ ULARGE_INTEGER *pcbWritten) = 0;

	virtual HRESULT STDMETHODCALLTYPE Commit( 
		/* [in] */ DWORD grfCommitFlags) = 0;

	virtual HRESULT STDMETHODCALLTYPE Revert( void) = 0;

	virtual HRESULT STDMETHODCALLTYPE LockRegion( 
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType) = 0;

	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType) = 0;

	virtual HRESULT STDMETHODCALLTYPE Stat( 
		/* [out] */ __RPC__out STATSTG *pstatstg,
		/* [in] */ DWORD grfStatFlag) = 0;

	virtual HRESULT STDMETHODCALLTYPE Clone( 
		/* [out] */ __RPC__deref_out_opt IStream **ppstm) = 0;	
};
