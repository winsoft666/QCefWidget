#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ViewComponent.h"
#include <wrl.h>

class DCompTargetImpl
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
          IDCompositionTarget> {
   public:
    DCompTargetImpl(ViewComponent* owner);
    void RemoveOwnerRef();

    // Inherited via IDCompositionTarget
    virtual HRESULT __stdcall SetRoot(IDCompositionVisual* visual) override;

   private:
    ViewComponent* m_viewComponentOwner;
};
