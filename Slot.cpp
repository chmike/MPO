#include "Slot.hpp"
#include "Link.hpp"

namespace MPO
{

    // Disconnect all links to the slot
    AnySlot::~AnySlot()
    {
        while( !m_links.empty() )
            delete *m_links.begin();
    }

    // Global Slot map
    SlotMap AnySlot::m_slotMap;

}
