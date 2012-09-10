#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <set>
#include <map>

#include "Message.hpp"



namespace MPO
{
class Link;
class AnySignal;
class AnySlot;

/// Link map definition
typedef std::map<AnySlot*,Link*> LinkMap;

/// Signal directory definition
typedef std::map<std::string, AnySignal*> SignalMap;

/// Name set definition
typedef std::set<std::string> NameSet;

/*! The class AnySignal is the base class of all Signal classes

    The class AnySignal may not be instantiated. The purpose of this class is
    to make it possible to define a generic pointer type to any Signal.
*/
class AnySignal
{
    friend class Link;
public:
    /// Disconnects all Link connections
    virtual ~AnySignal();

    /**
     * @brief Returns the class of Message this Signal may emit
     *
     * @return the TypeDef of Message instances this Signal may emit
     */
    const TypeDef& messageType() const { return m_msgType; }

    /**
     * @brief Returns a reference on the set of Link connected to this Signal
     *
     * @return a reference on the set of Link connected to this Signal
     */
    const LinkMap& links() const { return m_links; }

    /**
     * @brief Assign a new name to the Signal or unregister if name is ""
     *
     * @param name Name given to the Signal
     */
    void setName( const std::string &name )
    {
        unregisterName();
        if( name != "" )
            m_signalMap[m_name = name] = this;
    }

    /// Remove the Signal name from the global Signal map and clears name
    void unregisterName()
    {
        m_signalMap.erase( m_name );
        m_name = "";
    }

    /**
     * @brief Return the name of the Signal or "" if none
     *
     * @return the name of the Signal or "" if none
     */
    const std::string& name() const { return m_name; }

    /**
     * @brief Returns the Signal associated to a name or nullptr if not found
     *
     * @param name Name of the Signal to search for
     * @return Pointer to the Signal or nullptr if not found
     */
    static AnySignal* get( const std::string& name )
    {
        SignalMap::const_iterator it = m_signalMap.find( name );
        return it == m_signalMap.end() ? nullptr : it->second;
    }

protected:
    /**
     * @brief Constructor
     *
     * The Constructor is protected to constrain instantiation to sub classes
     * of AnySignal.
     *
     * @param type the Message type the Signal may emit
     */
    AnySignal(const TypeDef& type ) : m_msgType(type) {}

    /**
     * @brief Send the given Message through all Link connections
     *
     * @param msg the Message to sent through all Link connections
     */
    void emit( Message::Ptr msg );

    /**
     * @brief Insert Link in LinkMap (called by link himself)
     *
     * @param link to add to the LinkSet
     */
    void connect( AnySlot& slot, Link& link )
    {
        m_links[&slot] = &link;
    }

    /**
     * @brief Remove connection to slot from map (called by link himself)
     *
     * @param slot Connection to remove from the LinkMap
     * @return true if a connection was disconnected
     */
    bool disconnect( AnySlot &slot) { return m_links.erase( &slot ) == 1; }

    /**
     * @brief Return true if connected to the given Slot
     *
     * @param slot Connection to test if in the LinkMap
     * @return true if connected to the given Slot
     */
    bool isConnected( AnySlot &slot )
        { return m_links.find( &slot ) != m_links.end(); }

    LinkMap m_links;              ///< Map of connected links
    const TypeDef& m_msgType;     ///< Class of Message emitted by the Signal
    std::string m_name;           ///< Name assigned to the Signal
    static SignalMap m_signalMap; ///< Global Signal map
};



/*! @class Signal Action class Signals.

    A Signal is a point of emission of Message objects. When the Signal is
    connected with a Link to a Slot, the emitted Message is sent to the Slot
    and the method associated to the Slot is executed with the Message as
    argument.

    The type of messages a Signal may emit is specified as template argument.

    @code
        Signal<MyMessage> mySignal;
    @endcode

    The class AnySignal may not be instantiated. There purpose of this class is
    to be used as generic pointer type to a Signal.

    A message is emitted by the signal with the following instruction:

    @code
        // Message to emit must be referenced by a shared_ptr
        MyMessage::Ptr myMessage( new MyMessage(...) );

        // Emit the Message
        mySignal.emit( myMessage );
    @endcode

    All connected Slots will receive the message and their slot method will be
    executed by repeated calls to Message::processNext() until the static method
    returns false.

    If there is no connections to the Signal the emit instruction has no effect.
*/
template <class TMsg>
class Signal : public AnySignal
{
public:

    /**
     * @brief Constructor
     *
     * The type of Message is specified as template argument
     */
    Signal() : AnySignal(TMsg::Type()) {}

    /**
     * @brief Emit the message msg through all links
     *
     * @param msg is shared_ptr on Message to emit
     */
    void emit( typename TMsg::Ptr msg ) { AnySignal::emit( msg ); }
};

} // namespace MPO

#endif // SIGNAL_HPP
