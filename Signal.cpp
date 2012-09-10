#include "Signal.hpp"
#include "Link.hpp"

namespace MPO
{
    // Disconnect all links
    AnySignal::~AnySignal()
    {
        while( !m_links.empty() )
            delete m_links.begin()->second;
        unregisterName();
    }

    // Emit the message msg through all link connected to signal
    void AnySignal::emit( Message::Ptr msg )
    {
        Message::Emitted::Entry entry( msg, nullptr );
        for( LinkMap::const_iterator it = m_links.begin();
                it != m_links.end(); ++it )
        {
            entry.link = it->second;
            Message::emitted.add( entry );
        }
    }

    // Global Signal map
    SignalMap AnySignal::m_signalMap;
}
