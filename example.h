#include "CallbackNotifications.h"


// Tcl file sourcing start
struct TclSourcingStartTag {};
using TclSourcingStartListeners = notifications::Callbacks< TclSourcingStartTag, const QString &>;
using ScopedTclSourcingStartListener = notifications::TScopedListener<TclSourcingStartListeners>;
