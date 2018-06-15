#include <Cocoa/Cocoa.h>
#include "ciCEF.h"

namespace coc {

void ciCEF::windowSetup(CefWindowInfo& windowInfo) {
    NSView * view =  (NSView *) cinder::app::getWindow()->getNative();
    NSWindow * cocoaWindow = [ view window ];
    [cocoaWindow setReleasedWhenClosed:NO];
    windowInfo.SetAsWindowless(view);
}

} // namespace coc
