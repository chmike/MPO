#ifndef LINK_HPP
#define LINK_HPP

#include "Signal.hpp"
#include "Slot.hpp"

namespace MPO
{


/**
    @brief Link class to connect a Signal with a Slot

    Deleting a Link class instances removes the connection.

    Link connections are established by using the static connect() method
    using the Action, Signal and Slot names as references.

    The intended use is to establish the Link connections based on a
    configuration description using the names to be readable and editable by a
    user.

    The following example shows how to establish a connection Link.
    The Ping Action object has a Signal named "output" and the Pong Action
    object has a slot named "input".

    @code
        class MyMessage : public Message
        {
            ...
        };

        class MyClass
        {
        public:
            MyClass( const std::string& name ) :
                m_name(name), m_input(this)
            {
                m_input.setName( name + "::input" );
                m_output.setName( name + "::output" );
            }

            // input method processing Message received by bound Slot
            void input( MyMessage::Ptr m, Link* l ) { ... }

            // input Slot bound to the input method
            Slot<MyMessage, MyClass, &MyClass::input> m_input;

            // output Signal
            Signal<MyMessage> m_output;
        };
    @endcode

    @code
        // Instantiate an instance named ping
        MyClass* ping = new MyClass("Ping");

        // Instantiate an instance named pong
        MyClass* pong = new MyClass("Pong");

        // Connects the "output" Signal of "Ping" to the "input" Slot of "Pong"
        Link::connect( "Ping::output", "Pong::input");
    @endcode

    Every Message emitted by Ping.m_output will now be processed by the Pong::input method.

    @see Slot
    @see Signal
 */
class Link
{
    friend class Message; // Message instance calls forward()

public:
    /// The destructor disconnects the connection Link
    ~Link()
    {
        Message::emitted.erase(this);
        m_signal->disconnect( *m_slot );
        m_slot->disconnect( *this );
    }

    /**
     * @brief Establish a connection from a Signal to a Slot
     *
     * @param signal Signal to connect from
     * @param slot Slot to connect to
     * @param forceStatic true if a static cast must always be performed
     * @return false if signal or slot is a nullptr
     */
    static bool connect( AnySignal* signal, AnySlot* slot, bool forceStatic = false );

    /**
     * @brief Establish a connection from a named Signal to a named Slot
     *
     * @param signalName Name of Signal to connect from
     * @param slotName Name of Slot to connect to
     * @param forceStatic true if a static cast must always be performed
     * @return false if signal or slot is a nullptr
     */
    static bool connect( const std::string& signalName,
                         const std::string& slotName,
                         bool forceStatic = false )
    {
        return Link::connect( AnySignal::get(signalName),
                              AnySlot::get(slotName),
                              forceStatic );
    }

    /**
     * @brief Disconnect a Signal with a Slot, return true if a link existed
     *
     * @param signal Signal to disconnect from
     * @param slot Slot to disconnect from
     * @return True if signal and slot were connected, False otherwise
     */
    static bool disconnect( AnySignal* signal, AnySlot* slot )
        { return signal && slot && signal->disconnect(*slot); }

    /**
     * @brief Disconnect a named Signal with a named Slot,
     *        return true if a link existed
     *
     * @param signalName Name of Signal to disconnect from
     * @param slotName Name of Slot to disconnect from
     * @return True if signal and slot were connected, False otherwise
     */
    static bool disconnect( const std::string&  signalName,
                            const std::string&  slotName )
    {
        return Link::disconnect( AnySignal::get(signalName),
                              AnySlot::get(slotName) );
    }

    /**
     * @brief Return true if the Signal and Slot are connected
     *
     * @param signal Signal connected from
     * @param slot Slot connected to
     * @return True if signal and slot are connected, False otherwise
     */
    static bool isConnected( AnySignal* signal, AnySlot* slot )
        { return signal && slot && signal->isConnected(*slot); }

    /**
     * @brief Return true if the named Signal and named Slot are connected
     *
     * @param signalName Name of Signal connected from
     * @param slotName Name of Slot connected to
     * @return True if signal and slot are connected, False otherwise
     */
    static bool isConnected( const std::string& signalName,
                             const std::string& slotName )
    {
        return Link::isConnected( AnySignal::get(signalName),
                                  AnySlot::get(slotName) );
    }

    /**
     * @brief Return a pointer on the Signal or nullptr if not connected
     *
     * @return the connected Signal member variable or nullptr if not connected
     */
    AnySignal* signal() const { return m_signal; }

    /**
     * @brief Return a pointer on the Slot or nullptr if not connected
     *
     * @return the connected Slot member variable or nullptr if not connected
     */
    AnySlot* slot() const { return m_slot; }

protected:
    /**
     * @brief Forwards the emitted message to the slot and call its function
     *
     * @param msg The Message to pass as Slot function argument
     */
    void forward( Message::Ptr msg ) { m_slotFunction( msg, this); }

private:
    /**
     * @brief Constructor binding a Signal and a Slot and called by static connect
     *
     * @param signal Signal to connect from
     * @param slot Slot to connect to
     * @param forceStatic true if a static cast must always be performed
     */
    Link( AnySignal& signal, AnySlot& slot, bool forceStatic );

    /// Disconnects the Link
    void disconnect();

protected:
    AnySignal* m_signal;              ///< Signal member variable
    AnySlot* m_slot;                  ///< Slot member variable
    AnySlot::Function m_slotFunction; ///< Slot function to call
};


} // namespace MPO

#endif // LINK_HPP
