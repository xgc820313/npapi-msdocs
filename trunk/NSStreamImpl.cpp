#include "NSStreamImpl.h"

CNSStreamImpl::CNSStreamImpl(NPStream *p)
{
	innerStream = p;
}

CNSStreamImpl::~CNSStreamImpl(void)
{
}
