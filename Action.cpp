#include "Action.hpp"
#include "Signal.hpp"
#include "Slot.hpp"

namespace MPO
{
    const TypeDef Action::m_type( "Action" );

    // Register the Slot name
    void Action::add( const std::string& name, AnySignal& signal )
        { signal.setName( m_name + "::" + name ); }

    // Register the Slot name
    void Action::add( const std::string& name, AnySlot& slot )
        { slot.setName( m_name + "::" + name ); }

    Action::ActionMap Action::m_actions;
}

