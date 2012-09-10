#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>

#define nullptr NULL

namespace MPO {

/*! @brief Support for hierarchical type definition accessible as string

  Some classes require to define types hierarchies identified by
  a string name and instance reference. With the Type class, a user may
  specify a class type and identifiy an instance's type and explore its
  parent types.

  Definition example:

  @code

  // Define base class A in hpp file
  class A
  {
  public:
      ...
      //! Return static class type
      static const TypeDef& Type() { return A::m_type; }

      //! Return instance class type
      virtual const TypeDef& type() const { return A::Type(); }

  private:
      static const TypeDef m_type;
  };

  // Define class B with parent class A in hpp file
  class B : public A
  {
  public:
      ...
      //! Return static class type
      static const TypeDef& Type() { return B::m_type; }

      //! Return instance class type
      const TypeDef& type() const { return B::Type(); }
  private:
      static const TypeDef m_type;
  };

  // Define static type definition constants in cpp file
  const TypeDef A::m_type( "A" );
  const TypeDef B::m_type( "B", &A::Type() );

  // --------------------------------
  // Usage examples :

  // Accsess static information
  A::Type().name() : "A"
  A::Type().parent() : nullptr
  B::Type() : "B"
  B::Type().parent()->name() : "A"

  // Accsess instance information
  A* a1 = new A();
  a1->Type() : "A"
  a1->type() : "A"

  A* a2 = new B();
  a2->Type() : "A"
  a2->type() : "B"

  B* b = new B();
  b->Type() : "B"
  b->type() : "B"

  @endcode

*/
class TypeDef
{
public:
    //! Constructor
/**
 * @brief
 *
 * @param n
 * @param p
 */
    TypeDef( std::string n, const TypeDef* p = nullptr )
        : m_name(n), m_parent(p) {}

    //! Return true if this type is same or subtype of type t
    /**
     * @brief
     *
     * @param type
     * @return bool
     */
    bool isSameOrSubtypeOf( const TypeDef& type ) const
    {
        const TypeDef* t = &type;
        do
            if( t->name() == name() )
                return true;
            else
                t = t->parent();
        while( t );
        return false;
    }


    // Return Type name string
    /**
     * @brief
     *
     * @return const std::string
     */
    const std::string& name() const { return m_name; }

    // Return parent Type or nullptr if has no parent
    /**
     * @brief
     *
     * @return const TypeDef
     */
    const TypeDef* parent() const { return m_parent; }

private:
    const std::string m_name; /**< TODO */
    const TypeDef* m_parent; /**< TODO */
};

} // namespace MPO

#endif // TYPE_HPP
