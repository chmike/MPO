#include <iostream>
#include <map>

#include "MPO.hpp"


using namespace std;
using namespace MPO;


// Serie of test to valide the MPO package.


// Define MsgA Message class
class MsgA: public Message
{
public:
    typedef boost::shared_ptr<MsgA> Ptr;
    static const TypeDef& Type() { return MsgA::m_type; }
    virtual const TypeDef& type() const { return MsgA::Type(); }
private:
    static const TypeDef m_type;
};
const TypeDef MsgA::m_type( "MsgA", &Message::Type() );


// Define MsgB Message class
class MsgB : public MsgA
{
public:
    typedef boost::shared_ptr<MsgB> Ptr;
    static const TypeDef& Type() { return MsgB::m_type; }
    virtual const TypeDef& type() const { return MsgB::Type(); }
private:
    static const TypeDef m_type;
};
const TypeDef MsgB::m_type( "MsgB", &MsgA::Type() );


// Defined MyAction Action class
class MyAction : public Action
{
public:
    // Define shared pointer on object
    typedef boost::shared_ptr<MyAction> Ptr;

    /*  Constructor
        Note: slots are initialized because the operator () is used. */
    MyAction( const std::string& name )
        : Action(name), m_slotMsgM(this), m_slotMsgA(this), m_slotMsgB(this)
    {
        // Register slots with their names
        add( "slotMsgM", m_slotMsgM );
        add( "slotMsgA", m_slotMsgA );
        add( "slotMsgB", m_slotMsgB );

        // Register signals with their names
        add( "signalMsgM", m_signalMsgM );
        add( "signalMsgA", m_signalMsgA );
        add( "signalMsgB", m_signalMsgB );
    }

    // Slot method processing messages of type Message
    void processMsgM( Message::Ptr msg, Link* = nullptr)
    {
        m_processedDynamicMsgType = msg->type().name();
        m_processedStaticMsgType = msg->Type().name();
    }

    // Slot method processing messages of type MsgA
    void processMsgA( MsgA::Ptr msg, Link* = nullptr )
    {
        m_processedDynamicMsgType = msg->type().name();
        m_processedStaticMsgType = msg->Type().name();
    }

    // Slot method processing messages of type MsgB
    void processMsgB( MsgB::Ptr msg, Link* = nullptr )
    {
        m_processedDynamicMsgType = msg->type().name();
        m_processedStaticMsgType = msg->Type().name();
    }

    // Return static class type
    static const TypeDef& Type() { return MyAction::m_type; }

    // Return instance class type
    virtual const TypeDef& type() const { return MyAction::Type(); }

    // exit program if dynamic type is not the one expected
    void expectDynamicType( const std::string& op, const std::string& dynamicType )
    {
        if( dynamicType != m_processedDynamicMsgType )
        {
            cout << "Failed! " << endl
                 << op << ": invalid dynamic type!" << endl << "   Expected "
                 << dynamicType <<
                    ", got " << m_processedDynamicMsgType << " instead." <<endl;
            exit(1);
        }
        m_processedDynamicMsgType = "";

    }

    // exit program if static type is not the one expected
    void expectStaticType( const std::string& op, const std::string& staticType )
    {
        if( staticType != m_processedStaticMsgType )
        {
            cout << "Failed! " << endl
                 << op << ": invalid static type!" << endl << "   Expected "
                 << staticType <<
                    ", got " << m_processedStaticMsgType << " instead." << endl;
            exit(1);
        }
        m_processedStaticMsgType = "";
    }

    // Define slots
    Slot<Message, MyAction, &MyAction::processMsgM> m_slotMsgM;
    Slot<MsgA, MyAction, &MyAction::processMsgA> m_slotMsgA;
    Slot<MsgB, MyAction, &MyAction::processMsgB> m_slotMsgB;

    // Define signals
    Signal<Message> m_signalMsgM;
    Signal<MsgA> m_signalMsgA;
    Signal<MsgB> m_signalMsgB;

private:
    // Define the MyAction class type
    static const TypeDef m_type;
    // Place holder for processed message type
    std::string m_processedDynamicMsgType;
    std::string m_processedStaticMsgType;
};
// Ininitialize MyAction::m_type
const TypeDef MyAction::m_type( "MyAction", &Action::Type() );


// -------------------------------

// Other set of Message and Action classes to test connections

// Define MsgB Message class
class Ball : public Message
{
public:
    Ball() : pingCnt(0), pongCnt(0), totCount(0), maxCount(0) {}
    typedef boost::shared_ptr<Ball> Ptr;
    static const TypeDef& Type() { return Ball::m_type; }
    virtual const TypeDef& type() const { return Ball::Type(); }
    int pingCnt, pongCnt, totCount, maxCount;
private:
    static const TypeDef m_type;
};
const TypeDef Ball::m_type( "Ball", &Message::Type() );

class Ping : public Action
{
public:
    // Define shared pointer on object
    typedef boost::shared_ptr<Ping> Ptr;

    //  Constructor
    Ping( const std::string& name )
        : Action(name), m_input(this)
    {
        add( "input", m_input );
        add( "output", m_output );
    }

    void start( Ball::Ptr ball, int maxCount )
    {
        ball->maxCount = maxCount;
        ball->pingCnt = ball->pongCnt = ball->totCount = 0;
        receive( ball );
    }

    // Receive ball
    void receive( Ball::Ptr ball, Link* = nullptr )
    {
        if( ball == nullptr ) // Should not be required
            throw std::runtime_error( "Ping::receive: null Ball" );
        ++ball->pingCnt;
        if( ball->totCount++ < ball->maxCount )
            m_output.emit( ball );
    }

    Slot<Ball, Ping, &Ping::receive> m_input;
    Signal<Ball> m_output;
    static const TypeDef& Type() { return Ping::m_type; }
    virtual const TypeDef& type() const { return Ping::Type(); }
    // Define the MyAction class type
    static const TypeDef m_type;
};
const TypeDef Ping::m_type( "Ping", &Action::Type() );


class Pong : public Action
{
public:
    // Define shared pointer on object
    typedef boost::shared_ptr<Pong> Ptr;

    //  Constructor
    Pong( const std::string& name ) : Action(name), m_input(this)
    {
        add( "input", m_input );
        add( "output", m_output );
    }

    // Receive ball simply send it forward
    void receive( Ball::Ptr ball, Link* = nullptr )
    {
        if( !ball )
            throw std::runtime_error( "Pong::receive: null Ball" );
        ++ball->pongCnt;
        if( ball->totCount < ball->maxCount )
            m_output.emit( ball );
    }

    Slot<Ball, Pong, &Pong::receive> m_input;
    Signal<Ball> m_output;
    static const TypeDef& Type() { return Pong::m_type; }
    virtual const TypeDef& type() const { return Pong::Type(); }
    // Define the MyAction class type
    static const TypeDef m_type;
};
const TypeDef Pong::m_type( "Pong", &Action::Type() );

int main()
{
    Message::Ptr mm( new Message() );
    MsgA::Ptr ma( new MsgA() );
    MsgB::Ptr mb( new MsgB() );

    MyAction::Ptr action(new MyAction("myAction"));
    std::string op;

    cout << "Test Slot dynamic cast : ";

    op = "   Invoke action m_slotMsgM with mm";
    action->m_slotMsgM( mm );
    action->expectDynamicType( op, "Message" );
    action->expectStaticType( op, "Message" );

    op = "   Invoke action m_slotMsgM with ma";
    action->m_slotMsgM( ma );
    action->expectDynamicType( op, "MsgA" );
    action->expectStaticType( op, "Message" );

    op = "   Invoke action m_slotMsgM with mb";
    action->m_slotMsgM( mb );
    action->expectDynamicType( op, "MsgB" );
    action->expectStaticType( op, "Message" );

    op = "   Invoke action m_slotMsgA with mm";
    action->m_slotMsgA( mm );
    action->expectDynamicType( op, "" );
    action->expectStaticType( op, "" );

    op = "   Invoke action m_slotMsgA with ma";
    action->m_slotMsgA( ma );
    action->expectDynamicType( op, "MsgA" );
    action->expectStaticType( op, "MsgA" );

    op = "   Invoke action m_slotMsgA with mb";
    action->m_slotMsgA( mb );
    action->expectDynamicType( op, "MsgB" );
    action->expectStaticType( op, "MsgA" );

    op = "   Invoke action m_slotMsgB with mm";
    action->m_slotMsgB( mm );
    action->expectDynamicType( op, "" );
    action->expectStaticType( op, "" );

    op = "   Invoke action m_slotMsgB with ma";
    action->m_slotMsgB( ma );
    action->expectDynamicType( op, "" );
    action->expectStaticType( op, "" );

    op = "   Invoke action m_slotMsgB with mb";
    action->m_slotMsgB( mb );
    action->expectDynamicType( op, "MsgB" );
    action->expectStaticType( op, "MsgB" );
    cout << "Ok" << endl;


    cout << "Test Slot static cast  : ";

    op = "   Invoke action m_slotMsgM with mm";
    action->m_slotMsgM.getStaticCastFunction()( mm, nullptr );
    action->expectDynamicType( op, "Message" );
    action->expectStaticType( op, "Message" );

    op = "   Invoke action m_slotMsgM with ma";
    action->m_slotMsgM.getStaticCastFunction()( ma, nullptr );
    action->expectDynamicType( op, "MsgA" );
    action->expectStaticType( op, "Message" );

    op = "   Invoke action m_slotMsgM with mb";
    action->m_slotMsgM.getStaticCastFunction()( mb, nullptr );
    action->expectDynamicType( op, "MsgB" );
    action->expectStaticType( op, "Message" );

    //op = "   Invoke action m_slotMsgA with mm";
    //action->m_slotMsgA( mm ); is not a valid static cast

    op = "   Invoke action m_slotMsgA with ma";
    action->m_slotMsgA.getStaticCastFunction()( ma, nullptr );
    action->expectDynamicType( op, "MsgA" );
    action->expectStaticType( op, "MsgA" );

    op = "   Invoke action m_slotMsgA with mb";
    action->m_slotMsgA.getStaticCastFunction()( mb, nullptr );
    action->expectDynamicType( op, "MsgB" );
    action->expectStaticType( op, "MsgA" );

    //op = "   Invoke action m_slotMsgB with mm";
    //action->m_slotMsgB( mm ); is not a valid static cast

    //op = "   Invoke action m_slotMsgB with ma";
    //action->m_slotMsgB( ma ); is not a valid static cast

    op = "   Invoke action m_slotMsgB with mb";
    action->m_slotMsgB.getStaticCastFunction()( mb, nullptr );
    action->expectDynamicType( op, "MsgB" );
    action->expectStaticType( op, "MsgB" );
    cout << "Ok" << endl;


    // Note: Without links emission has no effect
    cout << "Test Signal type check : ";

    action->m_signalMsgM.emit( mm );
    action->m_signalMsgM.emit( ma );
    action->m_signalMsgM.emit( mb );

    // action->m_signalMsgA.emit( mm ); // Ok: static Error
    action->m_signalMsgA.emit( ma );
    action->m_signalMsgA.emit( mb );

    // action->m_signalMsgB.emit( mm ); // Ok: static Error
    // action->m_signalMsgB.emit( ma ); // Ok: static Error
    action->m_signalMsgB.emit( mb );

    cout << "Ok" << endl;

    cout << "Test connecting links  : ";

    try
    {
        if( Link::isConnected( "myAction::signalMsgM", "myAction::slotMsgM") )
        {
            cout << "Failed!" << endl;
            cout << "   Link myAction::signalMsgM -> myAction::slotMsgM exist." << endl;
            exit(1);
        }

        Link::connect( "myAction::signalMsgM", "myAction::slotMsgM");

        if( !Link::isConnected( "myAction::signalMsgM", "myAction::slotMsgM") )
        {
            cout << "Failed!" << endl;
            cout << "   Link myAction::signalMsgM -> myAction::slotMsgM not created." << endl;
            exit(1);
        }

        if( !Link::disconnect( "myAction::signalMsgM", "myAction::slotMsgM") )
        {
            cout << "Failed!" << endl;
            cout << "   Link myAction::signalMsgM -> myAction::slotMsgM not disconnected." << endl;
            exit(1);
        }

        if( Link::isConnected( "myAction::signalMsgM", "myAction::slotMsgM") )
        {
            cout << "Failed!" << endl;
            cout << "   Link myAction::signalMsgM -> myAction::slotMsgM exist." << endl;
            exit(1);
        }
        cout << "Ok" << endl;


        cout << "Test emit message      : ";

        Link::connect( "myAction::signalMsgM", "myAction::slotMsgM");
        op = "   Invoke action m_signalMsgM with mb";
        action->m_signalMsgM.emit( mb );
        while( Message::processNext() );
        action->expectDynamicType( op, "MsgB" );
        action->expectStaticType( op, "Message" );
        Link::disconnect( "myAction::signalMsgM", "myAction::slotMsgM");
        cout << "Ok" << endl;

        cout << "Test Action network    : ";

        Ping* ping = new Ping("Ping");
        Pong* pong = new Pong("Pong");
        Link::connect( "Ping::output", "Pong::input");
        Link::connect( "Pong::output", "Ping::input");

        if( !Link::isConnected("Ping::output", "Pong::input") )
        {
            cout << "Failed!" << endl;
            cout << "   Link Ping::output -> Pong::input not connected." << endl;
            exit(1);
        }
        if( !Link::isConnected("Pong::output", "Ping::input") )
        {
            cout << "Failed!" << endl;
            cout << "   Link Pong::output -> Ping::input not connected." << endl;
            exit(1);
        }

        Ball::Ptr ball( new Ball() );

        ping->start( ball, 15 );

        while( Message::processNext() );

        if( ball->pingCnt != ball->maxCount )
        {
            cout << "Failed!" << endl;
            cout << "   Ball ping counter is not " <<  ball->maxCount
                 << ". Found " << ball->pingCnt << endl;
            exit(1);
        }

        if( ball->pongCnt != ball->maxCount )
        {
            cout << "Failed!" << endl;
            cout << "   Ball pong counter is not " <<  ball->maxCount
                 <<". Found " << ball->pongCnt << endl;
            exit(1);
        }

        // Add a pong object instance duplicating the message ping transactions
        // Will double the ping pong transactions
        Pong* pong2 = new Pong("Pong2");
        Link::connect( "Ping::output", "Pong2::input");
        Link::connect( "Pong2::output", "Ping::input");

        ping->start( ball, 15 );
        while( Message::processNext() );

        if( ball->pingCnt != ball->maxCount )
        {
            cout << "Failed!" << endl;
            cout << "   Ball ping counter is not " <<  ball->maxCount
                 << ". Found " << ball->pingCnt << endl;
            exit(1);
        }

        if( ball->pongCnt != 2*ball->maxCount )
        {
            cout << "Failed!" << endl;
            cout << "   Ball pong counter is not " <<  2*ball->maxCount
                 <<". Found " << ball->pongCnt << endl;
            exit(1);
        }
        cout << "Ok" << endl;

        // destroy all objects
        Action::clearActions();
    }
    catch( std::exception& e )
    {
        cout << "Failed!" << endl;
        cout << "   Exception: " << e.what() << endl;
        exit(1);
    }
    catch( ... )
    {
        cout << "Failed!" << endl;
        cout << "   Unknown exception" << endl;
        exit(1);
    }



    return 0;
}



