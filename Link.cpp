#include <iostream>
#include <algorithm>

#include "Link.hpp"
#include "Action.hpp"

namespace MPO
{

// Establish a connection from a Signal to a Slot
bool Link::connect( AnySignal* signal, AnySlot* slot, bool forceStatic )
{
    // if a signal or a slot are not specified, connecting fails
    if( !signal || !slot )
        return false;

    // if not already connected, instantiate the link and connect it
    if( !signal->isConnected( *slot ) )
    {
        Link* link = new Link( *signal, *slot, forceStatic );
        signal->connect( *slot, *link );
        slot->connect( *link );
    }
    return true;
}

// Constructor binding signal and slot: called by static connect
Link::Link( AnySignal& signal, AnySlot& slot, bool forceStatic ) :
    m_signal(&signal), m_slot(&slot)
{
    // Pick the slot function performing a static or dynamic cast on the message
    if( forceStatic ||
            m_signal->messageType().isSameOrSubtypeOf(&m_slot->messageType()) )
        m_slotFunction = m_slot->getStaticCastFunction();
    else
        m_slotFunction = m_slot->getDynamicCastFunction();
}

}
