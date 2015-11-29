#include <boost/operators.hpp>
#include <cstddef>
#include "details.hpp"
#include <functional>
#include <iosfwd>
#include <tuple>
#include <type_traits>

namespace llvm
{
  class Module;
  class Type;
  class Value;
};

namespace boost { namespace llvm
{
  class value;
  class ref;

  // ================================================================================
  // Types.
  // ================================================================================
  class type
  {
  public:
    // Fundamental type creators.
    static type void_();
    static type char_();
    static type wchar_();
    static type int_(size_t bits);
    static type float_(size_t bits);

    // Aggregate type creators.
    static type struct_( // anonymous
        array_ref<type> members
      , string_refs const & member_names = string_refs()
      );
    static type struct_(string_ref name); // opaque
    static type struct_( // complete
        string_ref name
      , array_ref<type> members
      , string_refs const & member_names = string_refs()
      );

    // Compound type creators.
    type operator[](size_t) const; // array
    type operator*() const; // pointer
    friend type operator*(type, size_t); // vector
    friend type operator*(size_t, type); // vector
    // function
    template<typename... Types, BOOST_LLVM_ENABLE_IF_TYPE_SEQUENCE(Types)>
      type operator()(Types &&...) const;

    // From-host type creator.
    template<typename T> static type get()
    {
      using U = typename std::remove_reference<T>::type;
      using V = typename std::remove_cv<U>::type;
      return get_impl<V>();
    }

  private:
    template<typename T> struct get_impl // scalar
      { explicit operator type() const; };
    template<typename T> struct get_impl<T*> // pointer
      { explicit operator type() const; };
    template<typename...Ts> struct get_impl<std::tuple<Ts...>> // aggregate
    {
      get_impl(string_refs const & member_names = string_refs()); // anonymous
      get_impl( // complete
          string_ref name, string_refs const & member_names = string_refs()
        );
      explicit operator type() const;
    };
    template<typename T, size_t N> struct get_impl<T[N]> // array
      { explicit operator type() const; };
    template<typename T> struct get_impl<T[]> // array
      { explicit operator type() const; };

  public:
    // Constant creators.
    value operator()(std::nullptr_t);
    template<typename T
      , typename std::enable_if<
            std::is_pointer<T>::value
                && !std::is_convertible<T, string_ref>::value
          >::type* = nullptr
      >
      value operator()(T);
    template<typename T, BOOST_LLVM_ENABLE_IF(std::is_integral<T>)>
      value operator()(T);
    template<typename T, BOOST_LLVM_ENABLE_IF(std::is_floating_point<T>)>
      value operator()(T);
    value operator()(string_ref);
    template<typename...Ts> value operator()(std::tuple<Ts...> const &);
    value operator()(array_ref<value> const &);
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_1D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_2D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_3D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_4D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_5D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_6D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_7D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_8D(T));
    template<typename T> value operator()(BOOST_LLVM_INIT_LIST_9D(T));

    // C-style cast; e.g., (i32)(x);
    value operator()(value);

    // Access to LLVM.
    ::llvm::Type * ptr() const;
    explicit operator ::llvm::Type *() const;
  };


  // ================================================================================
  // Values.
  // ================================================================================
  class value
    : boost::ordered_euclidian_ring_operators<value>
    , boost::bitwise<value>
    , boost::shiftable<value>
  {
  public:
    // Non-copyable to help avoid "instruction does not dominate all uses"
    // errors.
    value() = delete;
    value(value const &) = delete;
    value & operator=(value const &) = delete;

    // Constant constructors.
    value(std::nullptr_t);
    template<typename T, BOOST_LLVM_ENABLE_IF(std::is_integral<T>)>
      value(T);
    template<typename T, BOOST_LLVM_ENABLE_IF(std::is_floating_point<T>)>
      value(T);
    value(string_ref);
    template<typename T> value(BOOST_LLVM_INIT_LIST_1D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_2D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_3D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_4D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_5D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_6D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_7D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_8D(T));
    template<typename T> value(BOOST_LLVM_INIT_LIST_9D(T));

    // Accessors.
    value operator[](value) const;
    ref operator*() const;
    value dot(size_t) const;
    value dot(string_ref) const;
    value arrow(size_t) const;
    value arrow(string_ref) const;

    // Function call.
    template<typename... Args, BOOST_LLVM_ENABLE_IF_VALUE_SEQUENCE(Args)>
      value operator()(Args &&...) const;

    // Conversion to Boolean.
    friend value test(value);

  private:
    value operator+=(value);
    value operator-=(value);
    value operator*=(value);
    value operator/=(value);
    value operator%=(value);
    value operator&=(value);
    value operator|=(value);
    value operator^=(value);
    value operator<<=(value);
    value operator>>=(value);

    // Access to LLVM.
    ::llvm::Value * ptr() const;
    explicit operator ::llvm::Value *() const;
  };

  // Stand-alone operators.
  value operator!(value);
  value operator+(value);
  value operator-(value);
  value operator~(value);
  value operator<(value, value);
  value operator==(value, value);
  value operator||(value, value);
  value operator&&(value, value);


  // ================================================================================
  // References.
  // ================================================================================
  // note: most operators are acquired via conversion to value.
  struct initializer { explicit initializer(value); };

  class const_ref
  {
    const_ref() {}
  public:
    const_ref(const_ref const &) = default;
    const_ref & operator=(const_ref const &) = delete; // see ref::operator= below!
    const_ref(const_ref &&) = default;

    // Load.
    explicit operator value() const; // emits a load instruction.

    // Address of.
    value operator&() const;

    // Accessors.
    const_ref operator[](value) const;
    const_ref operator*() const;
    const_ref dot(size_t) const;
    const_ref dot(string_ref) const;
    const_ref arrow(size_t) const;
    const_ref arrow(string_ref) const;

    // Initialize.
    const_ref & operator=(initializer) const;
  };

  class ref : boost::incrementable<ref, const_ref>
  {
    using const_ref::const_ref;
  public:

    // Store.
    ref & operator=(value) const; // emits a store instruction.

    // Accessors.
    ref operator[](value) const;
    ref operator*() const;
    ref dot(size_t) const;
    ref dot(string_ref) const;
    ref arrow(size_t) const;
    ref arrow(string_ref) const;

    // Operators.
    ref & operator+=(value) const;
    ref & operator-=(value) const;
    ref & operator*=(value) const;
    ref & operator/=(value) const;
    ref & operator%=(value) const;
    ref & operator&=(value) const;
    ref & operator|=(value) const;
    ref & operator^=(value) const;
    ref & operator<<=(value) const;
    ref & operator>>=(value) const;
    ref & operator++() const;
    ref & operator--() const;

    // Initialize.
    ref & operator=(initializer) const;
  };


  // ================================================================================
  // Intrinsics.
  //
  // The values are computed as constexprs during code generation.  Hence, the
  // return value must be value, not size_t.
  // ================================================================================
  type typeof_(value);
  value sizeof_(type);
  value sizeof_(value);
  value alignof_(type);
  value alignof_(value);
  value offsetof_(type, size_t);
  value offsetof_(type, string_ref);


  // ================================================================================
  // Casts.
  //
  // See C-style casts declared in class type.
  // ================================================================================
  value reinterpret_cast_(type, value);
  value static_cast_(type, value);


  // ================================================================================
  // Modules.
  // ================================================================================
  class module
  {
  public:
    module(string_ref name);
    module(::llvm::Module *);

    // I/O
    static module && read(std::string const & filename);
    static module && read(std::istream & stream);
    void write(std::string const & filename, bool binary=true);
    void write(std::ostream & stream, bool binary=true);

    // Build.
    module & link(module &);
    module & optimize(int level); // 0, 1, 2, 3, 's', 'z'.
    template<typename F> std::function<F> compile(string_ref);

    // Access to LLVM.
    ::llvm::Module * ptr() const;
    explicit operator ::llvm::Module *() const;
  };


  // ================================================================================
  // Labels.
  //
  // The target of a branch instruction.
  // ================================================================================
  class label
  {
  public:
    label();
    label(std::function<void()>);
  };


  // ================================================================================
  // Scopes.
  //
  // Defines where to insert symbols and instructions.  Manages a symbol table
  // containing local variable names.
  // ================================================================================
  class scope
  {
  public:
    scope(module const &);
    scope(value const &); // the value must be a function
    scope(label const &);
    static module const * current_module();
    static value const * current_function();
    static label const * current_label();
  };


  // ================================================================================
  // Control flow.
  // Each of these returns the inserted branch instruction, which is a value.
  // ================================================================================

  // When called, generates code that produces the loop test value.
  using loop_test = std::function<value()>;

  // An argument that can be either a label or thunk.
  struct code
  {
    // When called, generates code into the current context.
    using thunk = std::function<void()>;
    code(label);
    code(thunk=thunk());
  };

  value if_(value, code, code=code());
  value do_(code, loop_test);
  value while_(loop_test, code);
  value for_(code init, loop_test test, code step, code body);
  value goto_(label);
  value goto_(value, array_ref<label>); // computed goto
  struct switch_
  {
    switch_(value);
    switch_ & case_(int64_t, label);
    switch_ && default_(label);
    explicit operator value() const;
  };
  value break_();
  value continue_();
  value return_();
  value return_(value);


  // ================================================================================
  // Symbol creation - global variables and functions.
  // ================================================================================
  struct symbol_name
  {
    enum Kind { RIGID, FLEXIBLE };
    // Creates a rigid symbol name.  The specified name will be the symbol name.
    symbol_name(string_ref);
    // Creates a flexible symbol name.  It may be renamed to avoid collisions.
    friend symbol_name flexible(string_ref);
  };

  // Create functions.
  ref extern_(type, symbol_name, string_refs, code=code());
  ref static_(type, symbol_name, string_refs, code=code());
  ref inline_(type, symbol_name, string_refs, code=code());

  // Create global variables.
  ref extern_(type, symbol_name);
  ref static_(type, symbol_name);


  // ================================================================================
  // Variable access.
  //
  // If a name is provided, then "var" can be used to locate a variable.
  // ================================================================================
  // Allocate a stack variable with optional name.
  ref local(type, string_ref = string_ref());
  template<typename T>
    ref local(string_ref = string_ref());
  // Create a local variable with automatic type deduction.
  ref auto_(value, string_ref = string_ref());


  // ================================================================================
  // Reflection.
  // ================================================================================
  struct string_iterator { /* unspecified */ };
  struct type_iterator { /* unspecified */ };
  struct value_iterator { /* unspecified */ };

  // Sequential type access.
  struct member_types
  {
    member_types(type);
    type_iterator begin() const;
    type_iterator end() const;
    size_t size() const;
    type operator[](size_t) const;
  };

  // Aggregate member access.
  struct member_names
  {
    member_names(type);
    string_iterator begin() const;
    string_iterator end() const;
    size_t size() const;
    std::string const & operator[](size_t) const;
  };

  // Function.
  struct parameter_types
  {
    parameter_types(type);
    type_iterator begin() const;
    type_iterator end() const;
    size_t size() const;
    type operator[](size_t) const;
  };

  struct parameter_names
  {
    parameter_names(type);
    string_iterator begin() const;
    string_iterator end() const;
    size_t size() const;
    std::string const & operator[](size_t) const;
  };

  // Symbol table.
  struct local_names
  {
    local_names();
    string_iterator begin() const;
    string_iterator end() const;
    size_t size() const;
    std::string const & operator[](size_t) const;
  };
  // Lookup the named local variable or function parameter.
  ref var(string_ref);


  // ================================================================================
  // Type traits.
  // ================================================================================
  // Non-standard.
  type element_type(type); // return T from T* or T[].
  type return_type(type); // return T from T(Us...).

  // Primary type categories.
  bool is_array(type);
  bool is_class(type);
  // NA: is_enum
  bool is_floating_point(type);
  bool is_function(type);
  bool is_integral(type);
  // NA: is_lvalue_reference
  // NA: is_member_function_pointer
  // NA: is_member_object_pointer
  bool is_pointer(type);
  // NA: is_rvalue_reference
  // NA: is_union
  bool is_void(type);

  // Composite type categories.
  bool is_arithmetic(type);
  bool is_compound(type);
  bool is_fundamental(type);
  // NA: is_member_pointer
  bool is_object(type);
  // NA: is_reference
  bool is_scalar(type);

  // Type properties.
  // NA: is_abstract
  // NA: is_const
  bool is_empty(type);
  // NA: is_literal_type
  // NA: is_pod
  // NA: is_polymorphic
  bool is_signed(type);
  // NA: is_standard_layout
  // NA: is_trivial
  // NA: is_trivially_copyable
  bool is_unsigned(type);
  // NA: is_volatile

  // Type features
  // NA: has_virtual_destructor
  // NA: is_assignable
  // NA: is_constructible
  // NA: is_copy_assignable
  // NA: is_copy_constructible
  // NA: is_destructible
  // NA: is_default_constructible
  // NA: is_move_assignable
  // NA: is_move_constructible
  // NA: is_trivially_assignable
  // NA: is_trivially_constructible
  // NA: is_trivially_copy_assignable
  // NA: is_trivially_copy_constructible
  // NA: is_trivially_destructible
  // NA: is_trivially_default_constructible
  // NA: is_trivially_move_assignable
  // NA: is_trivially_move_constructible
  // NA: is_nothrow_assignable
  // NA: is_nothrow_constructible
  // NA: is_nothrow_copy_assignable
  // NA: is_nothrow_copy_constructible
  // NA: is_nothrow_destructible
  // NA: is_nothrow_default_constructible
  // NA: is_nothrow_move_assignable
  // NA: is_nothrow_move_constructible

  // Type relationships.
  // NA: is_base_of
  // NA: is_convertible
  bool is_same(type);

  // Property queries.
  size_t alignment_of(type);
  size_t extent(type);
  size_t rank(type);

  // Const-volatile qualifications.
  // NA: add_const
  // NA: add_cv
  // NA: add_volatile
  // NA: remove_const
  // NA: remove_cv
  // NA: remove_volatile

  // Compound type alterations.
  type add_pointer(type); // same as type::operator*
  // NA: add_lvalue_reference
  // NA: add_rvalue_reference
  type decay(type);
  type make_signed(type);
  type make_unsigned(type);
  type remove_all_extents(type);
  type remove_extent(type);
  type remove_pointer(type);
  type remove_reference(type);
  // NA: underlying_type

  // Other type generators.
  // NA: aligned_storage
  // NA: aligned_union
  type common_type(type);
  // NA: conditional
  // NA: enable_if
  // NA: result_of // no overloading; use return_type
}}


int main()
{
  return 0;
}
