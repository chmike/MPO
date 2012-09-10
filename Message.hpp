#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <list>

#include "Type.hpp"

namespace MPO
{

class Link;

/**
    @brief Message root base class of user defined messages

    The user must extend this base class to define Message classes holding
    user specific informations. The user defined class may carry any
    information as member variable and have user defined method to
    ease access to the information.

    A user defined Message class must have a unique type name that must be
    defined with the class to support portable class type introspection.

    Message objects are transmitted between Action objects through Link
    connections from Signal to Slot member variables.

    The lifetime of Message objects is managed by reference counting using
    shared_ptr<>. When the reference counter reaches 0, the Message object
    is deleted. For convenience it is recommended that the user defines the
    class subtype Ptr as an alias of shared_ptr<>.

    The following example shows how to define the class type and the Ptr type.
    @See TypeDef

    @code
        class MyMessage : public Message
        {
        public:
            /// Define MyMessage shared_ptr alias
            typedef boost::shared_ptr<MyMessage> Ptr;

            ...

            /// Return static class type
            static const TypeDef& Type() { return A::m_type; }

            /// Return instance class type
            virtual const TypeDef& type() const { return A::Type(); }

        private:
            ...
            static const TypeDef m_type; ///< Class type definition
        };

        // Define MyMessage class type as direct subclass of Message
        const TypeDef MyMessage::m_type( "MyMessage", &Message::Type() );
    @endcode

    Note that a newly instantiaded Message \b must be assigned to a shared_ptr
    before it can be emitted by a Signal as shown in the following example.

    @code
        // Assuming we have a Signal start connected to some Slot.
        Signal<MyMessage> start;
        start.setName( "start" );
        Link::connect( "start", "..." );

        // A new Message must be assigned to a shared_ptr (Ptr) to be emitted
        MyMessage::Ptr msg( new MyMessage( ... ) );
        start.emit( msg );
    @endcode

*/
class Message : public boost::enable_shared_from_this<Message>
{
    friend class AnySignal;
    friend class Link;

public:

    /// Virtual Message destructor
    virtual ~Message() {}

    /// Define Message smart pointer type
    typedef boost::shared_ptr<Message> Ptr;

    /// Define the Message notifier call back function type
    typedef boost::function<void ()> MessageNotifier;

    /**
     * @brief Return a copy of the shared_ptr on the instance
     *
     * This requires that the newly instantiated Message was assigned
     * to a shared_ptr and at least one still exists.
     *
     * @return shared_ptr copy on the instance
     */
    Ptr getPtr() { return shared_from_this(); }

    /**
     * @brief Return casted shared_ptr on the Message instance
     *
     * This requires that the newly instantiated Message was assigned
     * to a shared_ptr and at least one still exists. This call make it
     * possible to cast the returned pointer to a user defined Message
     * sub class.
     *
     * @return shared_ptr on typed instance or nullptr if the instance is not
     *         of the same class of a sub class of the class provided as
     *         template argument
     */
    template <class TMessage>
    typename TMessage::Ptr getPtr()
    {
        return boost::dynamic_pointer_cast<TMessage>( shared_from_this() );
    }

    /**
     * @brief Returns the static Message type
     *
     * @return the TypeDef of the Action class
     */
    static const TypeDef& Type() { return Message::m_type; }

    /**
     * @brief Returns the type of the Message instance
     *
     * @return the TypeDef of the Message instance
     */
    virtual const TypeDef& type() const { return Message::Type(); }

    /**
     * @brief Process the next pending Message or return false if empty
     *
     * @return False if the Message queue is empty
     */
    static bool processNext() { return emitted.processNext(); }

    /**
     * @brief set the Message queuing notification call back function
     *
     * If a message notification call back was already set, it will be
     * replaced by the new one. Passing 0 as argument will clear any
     * message notification call back that was previously set.
     *
     * @param messageNotifier a function pointer on the message notifier
     *                        callback function or 0 to clear an existing
     *                        callback.
     */
    void setMessageNotifier( MessageNotifier messageNotifier )
    {
        emitted.setMessageNotifier( messageNotifier );
    }


protected:

    /// Queue of emitted Message pending to be processed
    class Emitted
    {
    public:

        /// Define the Message notifier call back function type
        typedef boost::function<void ()> MessageNotifier;

        /// Pending Message entry
        struct Entry
        {
            /**
             * @brief Constructor of Pending Message entry
             *
             * @param msg Pending Message in queue
             * @param link Link traversed by Message
             */
            Entry( Message::Ptr msg, Link* link ) : msg(msg), link(link) {}

            /// Default constructor
            Entry() : link(0) {}

            Message::Ptr msg; ///< Pending Message
            Link* link;       ///< Link traversed by Message
        };

        /**
         * @brief Return true if the queue is empty
         *
         * @return true if the queue is empty
         */
        bool empty() const { return m_queue.empty(); }

        /**
         * @brief Return the number of entries in the queue
         *
         * @return the number of entries in the queue
         */
        size_t size() const { return m_queue.size(); }

        /**
         * @brief Add entry to the queue for FIFO processing
         *
         * Once a message is queued, the Message notification call back
         * function will be called if one has been provided
         *
         * @param entry Entry to add to the queue
         */
        void add( const Entry& entry )
        {
            m_queue.push_front( entry );
            if( m_notify )
                m_notify();
        }

        /**
         * @brief Extract entry from the queue for FIFO processing
         *
         * @param[out] entry extracted from the queue
         * @throws runtime_error is called on an empty Message queue
         */
        void get( Entry& entry )
        {
            if( empty() )
               throw std::runtime_error( "Message::Emitted::get called on empty Message queue" );
            entry = m_queue.back();
            m_queue.pop_back();
        }

        /**
         * @brief Erase all queue entry with the given link
         *
         * This method is called when a Link is deleted to ensure that all
         * pending Message on the Link are removed.
         *
         * @param link to look for and remove in the Message queue
         */
        void erase( Link* link )
        {
            for( Queue::iterator it = m_queue.begin(); it != m_queue.end();)
                if( it->link == link )
                    it = m_queue.erase(it);
                else
                    ++it;
        }

        /**
         * @brief Process the next Message in the queue or return false if empty
         *
         * @return false if the Message queue is empty after processing the call
         */
        bool processNext();

        /**
         * @brief set the Message queuing notification call back function
         *
         * If a message notification call back was already set, it will be
         * overriden by the new one. Passing 0 as argument will clear any
         * message notification call back that was set previously.
         *
         * @param messageNotifier a function pointer on the message notifier
         *                        callback function or 0 to clear an existing
         *                        callback.
         */
        void setMessageNotifier( MessageNotifier messageNotifier )
        {
            m_notify = messageNotifier;
        }

    private:
        /// Define the Message queue type
        typedef std::list<Entry> Queue;
        Queue m_queue;            ///< The Message entry queue
        MessageNotifier m_notify; ///< Message notifier
    };

    //! Global emit queue
    static Emitted emitted; ///< Emitted Message queue

private:
    /// The Message class type with the name "Message" and no parent class
    static const TypeDef m_type;
};

} // namespace MPO

#endif // MESSAGE_HPP
