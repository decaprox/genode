#include <base/printf.h>
#include <base/log.h>

#include "VirtualBoxImpl.h"
#include "VBox/com/MultiResult.h"

#include "dummy/macros.h"

static bool debug = false;

HRESULT VirtualBoxBase::setError(HRESULT aResultCode, const char *pcsz, ...)
{
	Genode::printf(ESC_ERR);

	va_list list;
	va_start(list, pcsz);

	Genode::printf("%s : %s", this->getComponentName(),
	               Utf8Str(pcsz, list).c_str());

	va_end(list);

	Genode::printf(ESC_END "\n");

	TRACE(aResultCode);
}

HRESULT VirtualBox::CreateAppliance(IAppliance**)                               DUMMY(E_FAIL)

void    VirtualBoxBase::clearError()                                            TRACE()
HRESULT VirtualBoxBase::setError(HRESULT aResultCode)                           DUMMY(E_FAIL)
HRESULT VirtualBoxBase::setError(const com::ErrorInfo &ei)                      DUMMY(E_FAIL)
HRESULT VirtualBoxBase::handleUnexpectedExceptions(VirtualBoxBase *const,
                                                   RT_SRC_POS_DECL)             TRACE(E_FAIL)
HRESULT VirtualBoxBase::setErrorInternal(HRESULT, GUID const&, char const*,
                                         com::Utf8Str, bool, bool)              DUMMY(E_FAIL)
HRESULT VirtualBoxBase::initializeComForThread(void)                            TRACE(S_OK)
void    VirtualBoxBase::uninitializeComForThread(void)                          TRACE()
