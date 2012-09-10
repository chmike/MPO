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
    /**
     * @brief Constructor
     *
     * @param name Name of the type
     * @param ptr Pointer on the parent class TypeDef or nullptr if none
     */
    TypeDef( const std::string& name, const TypeDef* ptr = nullptr )
        : m_name(name), m_parent(ptr) {}

    /**
     * @brief Return true if this type is same or subtype of type
     *
     * @param type TypeDef of the class to compare with
     * @return true if this type is same or subtype of type
     */
    bool isSameOrSubtypeOf( const TypeDef* type ) const
    {
        while( type )
            if( type->name() == name() )
                return true;
            else
                type = type->parent();
        return false;
    }

    /**
     * @brief Return name of type
     *
     * @return name of type
     */
    const std::string& name() const { return m_name; }

    /**
     * @brief Return parent TypeDef or nullptr if has no parent
     *
     * @return const parent TypeDef or nullptr if has no parent
     */
    const TypeDef* parent() const { return m_parent; }

private:
    const std::string m_name; ///< Name of defined type
    const TypeDef* m_parent;  ///< Pointer to parent type or nullptr if none
};

} // namespace MPO

#endif // TYPE_HPP
