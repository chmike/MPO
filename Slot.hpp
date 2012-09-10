#ifndef SLOT_HPP
#define SLOT_HPP

#include <set>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <stdexcept>

#include "Message.hpp"


namespace MPO
{
class Link;
class AnySlot;

/// Link set definition
typedef std::set<Link*> LinkSet;

/// Slot directory definition
typedef std::map<std::string, AnySlot*> SlotMap;


/*! The class AnySlot is the base class of all Slot classes

    The class AnySlot may not be instantiated. The purpose of this class is
    to make it possible to define a generic pointer type to any type of Slot.
*/
class AnySlot
{
    friend class Link;

public:

    /// Pointer to a generic Message processing Function
    typedef boost::function<void ( Message::Ptr, Link* )> Function;

    /// Destructor disconnecting all Link
    ~AnySlot();

    /**
     * @brief Returns the class of Message this Slot may accept
     *
     * @return the TypeDef of Message instances this Slot may accept
     */
    const TypeDef& messageType() const { return m_msgType; }

    /**
     * @brief Return the Slot function applying a dynamic cast on the Message
     *
     * @return Function performing a dynamic cast on the Message argument
     */
    Function getDynamicCastFunction() const { return m_dynamicCastFunction; }

    /**
     * @brief Return the Slot function applying a static cast on the Message
     *
     * @return Function performing a static cast on the Message argument
     */
    Function getStaticCastFunction() const { return m_staticCastFunction; }

    /**
     * @brief Operator () calling the Slot method directly
     *
     * @param msg The Message to pass as argument to the Slot method
     */
    void operator()(Message::Ptr msg )
        { m_dynamicCastFunction( msg, nullptr ); }

    /**
     * @brief Return a const reference on the link set connected to the slot
     *
     * @return a const reference on the link set connected to the slot
     */
    const LinkSet& links() const { return m_links; }

    /**
     * @brief Assign a new name to the Slot or unregister if name is ""
     *
     * @param name Name given to the Slot
     */
    void setName( const std::string &name )
    {
        unregisterName();
        if( name != "" )
            m_slotMap[m_name = name] = this;
    }

    /// Remove the Slot name from the global Slot map and clears name
    void unregisterName()
    {
        m_slotMap.erase( m_name );
        m_name = "";
    }

    /**
     * @brief Return the name of the Slot or "" if none
     *
     * @return the name of the Slot or "" if none
     */
    const std::string& name() const { return m_name; }

    /**
     * @brief Returns the Slot associated to a name or nullptr if not found
     *
     * @param name Name of the Slot to search for
     * @return Pointer to the Slot or nullptr if not found
     */
    static AnySlot* get( const std::string& name )
    {
        SlotMap::const_iterator it = m_slotMap.find( name );
        return it == m_slotMap.end() ? nullptr : it->second;
    }

protected:

    /**
     * @brief Constructor
     *
     * The Constructor is protected to constrain instantiation to sub classes
     * of AnySlot.
     *
     * @param type the Message type the Slot may accept
     */
    AnySlot( const TypeDef& type ) : m_msgType(type) {}

    /**
     * @brief Insert Link in LinkSet (called by link himself)
     *
     * @param link to add to the LinkSet
     */
    void connect( Link& link ) { m_links.insert( &link ); }

    /**
     * @brief Remove link from set (called by link himself)
     *
     * @param link to remove from the LinkSet
     */
    void disconnect( Link& link ) { m_links.erase( &link ); }

    const TypeDef& m_msgType;       ///< Class of Message accepted by Slot
    Function m_dynamicCastFunction; ///< Slot method with dynamic cast of Message
    Function m_staticCastFunction;  ///< Slot method with static cast of Message
    LinkSet m_links;                ///< Set of connected links
    std::string m_name;             ///< Name assigned to the Slot
    static SlotMap m_slotMap;       ///< Global Slot map
};

template <class TMsg, class TObj, void (TObj::*TMethod)(typename TMsg::Ptr, Link*)>
/*! @class Slot  Slots bound to a class method.

    A Slot is a (member) variable bound to a method. It requires to specify as
    template argument the class of Message it may accept and the method it is
    bound to.
    The slot variable must be initialized with a pointer to the object
    owning the method to call.

    @code
        class MyClass ...
        {
        public:
            ...
            // The constructors must initialize the slots by providing this
            MyClass( ... ) : ..., m_slot(this), ... { ... }
            ...
            // The slot to which Signals may connect too
            Slot<MyMessage, MyClass, &MyClass::mySlotMethod> m_slot;
            ...
        protected:
            ...
            // The slot method that will be called for each message received
            // by the slot.
            void mySlotMethod( MyMessage::Ptr m, Link * l ) { ... }
            ...
        };
    @endcode

    A connected Slot will have their bound slot method invoked for each
    received Message that matches the accepted type.

    When a Signal emits a Message, its is queued in the Message::emitted queue.
    It is the call of the Message::processNext() static method that consume
    the next Message in the queue and calls the corresponding Slot method.
    It returns false when the queue is empty.

    A static cast will be performed on the Message if the statically defined
    Message type emitted by the connected Signal matches the polymorphic rule.
    Otherwise a dynamic cast is performed. The slot method will not be called
    if the dynamic cast on the Message returns a nullptr.

    It is possible to establish a connection Link that will always perform a
    static cast. This should be only done if the user can ensure that the
    Message types will always match the expected type of the slot method. This
    avoids the dynamic cast and resulting performance penalty.

    Normally a slot doesn't need to be initialized. The pointer on the owning
    Action object it needs to call the slot method is set when the first
    connection to it is established. But if a call to the slot operator () is to
    be performed before a connection to it is established, then the slot
    requires to be initialized as in the following example:

    @code
        class MyMessage ... ;
        class MyClass
        {
            MyClass(...) : ... m_slot(this),... {...}
            ...
            void mySlotMethod( MyMessage::Ptr m, Link * l ) { ... }
            ...
            Slot<MyMessage, MyAction, &MyAction::mySlotMethod> m_slot;
            ...
        };
    @endcode
*/
class Slot : public AnySlot
{
public:

    /// Type of the current class
    typedef Slot<TMsg,TObj,TMethod> MyType;

    /**
     * @brief Constructor initializing the Slot
     *
     * @param obj Pointer on the object owning the Slot (this)
     */
    Slot( TObj* obj ) : AnySlot( TMsg::Type() )
    {
        m_dynamicCastFunction =
                boost::bind( &MyType::dynamicCastFunction, obj, ::_1, ::_2 );
        m_staticCastFunction =
                boost::bind( &MyType::staticCastFunction, obj, ::_1, ::_2 );
    }

private:

    /**
     * @brief Wrapper of Slot method call with dynamic cast of Message argument
     *
     * @param obj Pointer on the object owning the Slot
     * @param msg shared_ptr on the Message to pass as Slot method argument
     * @param link pointer on Link through which the Message was sent or
     *             nullptr if unspecified or none
     */
    static void dynamicCastFunction( TObj* obj, typename Message::Ptr msg, Link* link )
    {
        typename TMsg::Ptr m = boost::dynamic_pointer_cast<TMsg>(msg);
        if( m && obj )
            (obj->*TMethod)(m, link);
    }

    /**
     * @brief Wrapper of Slot method call with static cast of Message argument
     *
     * @param obj Pointer on the object owning the Slot
     * @param msg shared_ptr on the Message to pass as Slot method argument
     * @param link pointer on Link through which the Message was sent or
     *             nullptr if unspecified or none
     */
    static void staticCastFunction( TObj* obj, typename Message::Ptr msg, Link* link )
    {
        if( msg && obj )
            (obj->*TMethod)(boost::static_pointer_cast<TMsg>(msg), link);
    }
};

} // namespace MPO

#endif // SLOT_HPP
