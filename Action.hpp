#ifndef ACTION_HPP
#define ACTION_HPP

/** @file Action.hpp
    @brief Abstract base class for user defined Action objects.
    @author Christophe Meessen <meessen@cppm.in2p3.fr>
    @bug No known bugs.
 */

#include <string>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Message.hpp"

/*!
    @brief Action: Root base class of user defined Action classes.

    Named Action objects and their Signal and Slot communication mechanism allow
    to instantiate data processing network with Message type aware
    interconnection when the program is configured.

    User must extend this class by inheritance to define his own type of
    actions.

    @li
    Action object are referenced by shared_ptr which can be obtained by its
    getPtr() method. A convenient alias for shared_ptr<Action> is Action::Ptr.

    @li
    Action objects are identified by a unique name (string). This object name
    provides a convenient way to access them or to identify objects in a
    logging output. The name must be assigned in the Action constructor.

    @li
    All Action object instances are referenced in the global ActionMap map with
    their name as key. The map may be accessed with the method map().
    An Action object may be retrieved from the map with the method get().
    Clearing the map should delete all Action instances.

    @li
    Action objects have also a unique type that must be defined in each class. A
    type specifies the Action class type as a string and the parent Action
    class. This supports introspection at the class level for Action objects.
    @see Type.hpp.

    @li
    Action objects have any number of Slot and Signal member variables. Signal
    to Slot connections defines the path followed by the Message data through
    the data processing network (oriented graph). The action objects are the
    vertices and the Signal to Slot connections the oriented edges.

    Signals and Slots are identified by a unique Signal or Slot name
    in the scope of the Action object they belong to. These names are used as
    reference (address) to establish the connection link from a Signal to a
    Slot.

    A Signal is used to emit Message objects to all the connected Slot.
    A Slot is statically bound to a slot method that is invoked for each
    received message. When a Message is passed to a Signal, the slot method
    bound to the connected Slot will be called with the Message object as method
    argument.

    A Slot method is a method with the following signature:

    @code
        void aSlotMethod( <i>TMsg</i>::Ptr msg, Link* link ) { ... }
    @endcode

    where <i>TMsg</i> is set to the Message class or subclass accepted by the
    slot.

    The following code illustrate how a slot and its method would be defined and
    its name registered so that a Signal can be connected to it.

    @code
        class MyMessage : public Message {...};

        class MyAction ...
        {
        private:
            ...
            void mySlotMethod( MyMessage::Ptr m, Link * l ) { ... }
            ...
            Slot<MyAction, MyMessage, &MyAction::mySlotMethod> m_mySlot;
            ...
        public:
            void configure(...)
            {
                ...
                // Register m_mySlot with the name "mySlot"
                add( "mySlot", &m_mySlot );
                ...
            }
            ...
        };
    @endcode

    The msg argument will be set to the message emitted by the Signal through
    the connection link. The slot method may then access the data in the
    message. The link argument will point to the connection link followed by
    the Message. It allows the slot method to access the Signal or Action object
    that sent the Message or to close the link if it's appropriate.

    The following code illustrate a user defined Signal and how a Message
    is sent with it.

    @code
        class MyAction ...
        {
        private:
            ...
            Signal<MyMessage> m_mySignal;
            ...
        public:
            void configure(...)
            {
                ...
                // Register m_mySignal with the name "mySignal"
                add( "mySignal", &m_mySignal );
                ...
            }
            ...
            void someMethod(...)
            {
                ...
                // Instantiate a new message
                MyMessage::Ptr myMsg(new MyMessage(...));
                ...
                // Emit myMsg as mySignal
                m_mySignal.emit( myMsg );
                ...
            }
            ...
        };
    @endcode

    A connection between a Signal and Slot of two Action objects is setup
    with the following instruction:

    @code
        Link::connect( "actionOne", "mySignal", "actionTwo", "mySlot");
    @endcode

    This will establish a connection between the signal "mySignal" of the
    Action object "actionOne" to the slot "mySlot" of the Action object
    "actionTwo".
*/


namespace MPO
{

class AnySlot;
class AnySignal;

/**
    @brief Action root base class of user defined actions

    The user must extend this base class to define his own Action class with
    its specific Signal and Slot members as well as action methods.
    The base Action class constructor must be called by the derived class to
    provide a program wide unique name that can be later used as key to access
    the object.

    Named Action objects and their Signal and Slot communication mechanism allow
    to instantiate data processing network with Message type aware
    interconnection when the program is configured.

    @li Action objects are identified by a unique name (string). This object name
    provides a convenient way to access them or to identify objects in a
    logging output. The name must be assigned in the Action constructor.

    @li A shared pointer to the Action object may be retrieved with the getPtr() method
    to ensure the object is not deleted with a call to clearActions() method.

    @li All Action object instances are referenced in the global ActionMap map with
    their name as key. The map may be accessed with the method map().
    An Action object may be retrieved from the map with the method get().
    Clearing the map should delete all Action instances.

    @li Action objects have also a unique type that must be defined in each class. A
    type specifies the Action class type as a string and the parent Action
    class. This supports introspection at the class level for Action objects.
    @see Type.hpp.

    @li Action objects may have any number of Slot and Signal member variables.
    Signal to Slot connections defines the path followed by the Message data
    through the data processing network (oriented graph). The action objects are
    the vertices and the Signal to Slot connections the oriented edges.

    Signals and Slots are identified by a unique Signal or Slot name
    in the scope of the Action object they belong to. These names are used as
    reference (address) to establish the connection link from a Signal to a
    Slot.

    A Signal is used to emit Message objects to all the connected Slot.
    A Slot is statically bound to a slot method that is invoked for each
    received message. When a Message is passed to a Signal, the slot method
    bound to the connected Slot will be called with the Message object as method
    argument.

    A Slot method is a method with the following signature:

    @code
        void aSlotMethod( <i>TMsg</i>::Ptr msg, Link* link ) { ... }
    @endcode

    where <i>TMsg</i> is set to the Message class or subclass accepted by the
    slot.

    The following code illustrate how a slot and its method would be defined and
    its name registered so that a Signal can be connected to it.

    @code
        class MyMessage : public Message {...};

        class MyAction ...
        {
        private:
            ...
            void mySlotMethod( MyMessage::Ptr m, Link * l ) { ... }
            ...
            Slot<MyAction, MyMessage, &MyAction::mySlotMethod> m_mySlot;
            ...
        public:
            void configure(...)
            {
                ...
                // Register m_mySlot with the name "mySlot"
                add( "mySlot", &m_mySlot );
                ...
            }
            ...
        };
    @endcode

    The msg argument will be set to the message emitted by the Signal through
    the connection link. The slot method may then access the data in the
    message. The link argument will point to the connection link followed by
    the Message. It allows the slot method to access the Signal or Action object
    that sent the Message or to close the link if it's appropriate.

    The following code illustrate a user defined Signal and how a Message
    is sent with it.

    @code
        class MyAction ...
        {
        private:
            ...
            Signal<MyMessage> m_mySignal;
            ...
        public:
            void configure(...)
            {
                ...
                // Register m_mySignal with the name "mySignal"
                add( "mySignal", &m_mySignal );
                ...
            }
            ...
            void someMethod(...)
            {
                ...
                // Instantiate a new message
                MyMessage::Ptr myMsg(new MyMessage(...));
                ...
                // Emit myMsg as mySignal
                m_mySignal.emit( myMsg );
                ...
            }
            ...
        };
    @endcode

    A connection between a Signal and Slot of two Action objects is setup
    with the following instruction:

    @code
        Link::connect( "actionOne", "mySignal", "actionTwo", "mySlot");
    @endcode

    This will establish a connection between the signal "mySignal" of the
    Action object "actionOne" to the slot "mySlot" of the Action object
    "actionTwo".
 */
class Action : public boost::enable_shared_from_this<Action>
{
    friend class Link;

public:

    /// Virtual Action destructor
    virtual ~Action() {}

    /// Define Action smart pointer type
    typedef boost::shared_ptr<Action> Ptr;

    /**
     * @brief Return shared pointer on instance
     *
     * After calling this method, the instance won't be deleted by a call to
     * Action::clearActions() unless the returned shared pointer is deleted.
     *
     * @return Shared pointer on instance
     */
    Ptr getPtr() { return shared_from_this(); }

    /**
     * @brief Return user specified typed shared pointer on instance
     *
     * After calling this method, the instance won't be deleted by a call to
     * Action::clearActions() unless the returned shared pointer is deleted.
     *
     * @return Shared pointer on typed instance or nullptr
     *         if the given template type doesn't match the instance type
     */
    template <class TAction>
    typename TAction::Ptr getPtr()
    {
        return boost::dynamic_pointer_cast<TAction>( shared_from_this() );
    }

protected:
    /**
     * @brief Constructor of the Action instance (protected)
     *
     * A derived class must call this constructor by providing the program wide
     * unique instance name that can be later user as key to access the
     * instance.
     *
     * The constructor is protected to forbid instantiation of Action
     * objects. The user MUST use a derived class which should have its
     * constructor public if it may be instantiated.
     *
     * @param name Name of the Action object instance
     */
    Action( const std::string& name ) : m_name(name)
    {
        Action::add( name, *this );
    }

    /// Definition of a map of Signal member variables with their name as key
    typedef std::map<std::string, AnySignal*> SignalMap;

    /// Map of registered Signal member variables
    SignalMap m_signals;

    /**
     * @brief Register the Slot name
     *
     * @param name Unique Signal name in the Action object instance
     * @param signal Signal member variable to register
     * @throws runtime_error if a signal is already registered with that name
     */
    void add( const std::string& name, AnySignal& signal );

    /**
     * @brief Register the Slot name
     *
     * @param name Unique Slot name in the Action object instance
     * @param slot Slot member variable to register
     */
    void add( const std::string& name, AnySlot& slot );

    /// Map of string name to Action object shared pointers
    typedef std::map<std::string, Ptr > ActionMap;

    /// Global map of registered Action instances
    static ActionMap m_actions;

    /**
     * @brief Register an Action instance in the global Action map
     *
     * @param name Unique program wide Action name
     * @param action Pointer on the Action instance
     * @throws runtime_error if an Action is already registered with that name
     */
    static void add( const std::string& name, Action& action )
    {
        if( m_actions.find( name ) != m_actions.end() )
            throw std::runtime_error( "Duplicate Action name '" + name + "'");
        m_actions[name] = Ptr(&action);
    }

public:

    /// Configure Action instance using configuration data
    virtual void configure() {}

    /**
     * @brief Returns the Action associated to a name or nullptr if none
     *
     * Returns a pointer to the Action associated to the given name or
     * nullptr if there is no action associate with it.
     *
     * @param name Name of the Action to retrieve
     * @return Pointer to the Action or nullptr if not found
     */
    static Action* getAction( const std::string& name )
    {
        ActionMap::const_iterator it = m_actions.find( name );
        return it == m_actions.end() ? nullptr : it->second.get();
    }

    /**
     * @brief Clears the map of actions
     *
     * Clearing the map of actions removes all name to action associations
     * and deletes the action instances if there is no other shared pointer
     * reference to the instances.
     */
    static void clearActions()
    {
        m_actions.clear();
    }

    /**
     * @brief Returns the name of this Action instance
     *
     * @return the Action instance name
     */
    const std::string& name() const { return m_name; }

    /**
     * @brief Returns the static Action instance type
     *
     * @return the TypeDef of the Action class
     */
    static const TypeDef& Type() { return Action::m_type; }

    /**
     * @brief Returns the type of the Action instance
     *
     * @return the TypeDef of the Action instance
     */
    virtual const TypeDef& type() const { return Action::Type(); }

private:

    /// Action instance's name
    std::string m_name;

    /// The Action class type with the name "Action" and no parent class
    static const TypeDef m_type;
};

} // namespace MPO

#endif // ACTION_HPP
