#include "Message.hpp"
#include "Link.hpp"

namespace MPO
{
    // Instantiate the Message TypeDef variable
    const TypeDef Message::m_type("Message");

    // Instantiate the emitted Message queue
    Message::Emitted Message::emitted;

    // Process the next queued entry if and return false if no more entries
    bool Message::Emitted::processNext()
    {
        if( empty() )
            return false;
        Entry entry;
        get( entry );
        if( entry.link != nullptr )
            entry.link->forward( entry.msg );

        return !empty();
    }



}
