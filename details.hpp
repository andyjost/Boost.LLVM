#pragma once
#include <type_traits>
#include <initializer_list>
#include <string>
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include "array_ref.hpp"

namespace boost { namespace llvm
{
  using string_ref = ::llvm::StringRef;

  // Accepts a sequence of strings while trying to avoid copying.
  struct string_refs : array_ref<string_ref>
  {
    using base_t = array_ref<string_ref>;
    using base_t::base_t;

    string_refs() : string_refs({})
    {}

    string_refs(base_t const & names)
      : base_t(names), m_storage()
    {}

    template<typename StringSequence
      , typename std::enable_if<
            std::is_constructible<
                string_ref, typename StringSequence::value_type
              >::value
          >::type* =nullptr
      >
    string_refs(StringSequence const & names)
      : base_t({})
      , m_storage(names.begin(), names.end())
    {
      static_cast<base_t&>(*this) = base_t(m_storage);
    }

  private:
    ::llvm::SmallVector<string_ref, 4> m_storage;
  };
}}

namespace boost { namespace llvm { namespace detail
{
  template<typename Condition>
  using enabler = typename std::enable_if<Condition::value>::type*;

  template<typename Condition>
  using disabler = typename std::enable_if<!Condition::value>::type*;

  template<typename Target, typename... Types>
  class is_typed_sequence
  {
    template<typename... Ts>
    static auto test(Ts &&... ts)
        -> decltype(std::initializer_list<Target>{ts...})
    {}

    static void test(...) {}

  public:

    static bool constexpr value = !std::is_same<
        decltype(test(*static_cast<Types *>(nullptr)...)), void
      >::value;
  };

  template<typename Target, typename... Types>
  using enable_for_typed_sequence = enabler<
      is_typed_sequence<Target, Types...>
    >;

  template<typename Target, typename... Types>
  using disable_for_typed_sequence = disabler<
      is_typed_sequence<Target, Types...>
    >;
}}}


// Works for conditions without commas.
#define BOOST_LLVM_ENABLE_IF(Condition) \
    boost::llvm::detail::enabler<Condition> \
  /**/

// Conditions containing commas.
#define BOOST_LLVM_CONVERTIBLE_TO_STRING_REF(T) \
    std::is_convertible<T, string_ref>          \
  /**/

#define BOOST_LLVM_ENABLE_IF_TYPE_SEQUENCE(Types)                           \
    boost::llvm::detail::enabler<                                           \
        boost::llvm::detail::is_typed_sequence<boost::llvm::type, Types...> \
      >                                                                     \
  /**/

#define BOOST_LLVM_ENABLE_IF_VALUE_SEQUENCE(Types)                           \
    boost::llvm::detail::enabler<                                            \
        boost::llvm::detail::is_typed_sequence<boost::llvm::value, Types...> \
      >                                                                      \
  /**/

#define BOOST_LLVM_ENABLE_IF_VALUE_INITIALIZER(T)    \
    boost::llvm::detail::enabler<                    \
        std::is_constructible<boost::llvm::value, T> \
      >                                              \
  /**/

// Multi-dimensional initializer lists.
#define BOOST_LLVM_INIT_LIST_1D(T) std::initializer_list<T>
#define BOOST_LLVM_INIT_LIST_2D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_1D(T)>
#define BOOST_LLVM_INIT_LIST_3D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_2D(T)>
#define BOOST_LLVM_INIT_LIST_4D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_3D(T)>
#define BOOST_LLVM_INIT_LIST_5D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_4D(T)>
#define BOOST_LLVM_INIT_LIST_6D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_5D(T)>
#define BOOST_LLVM_INIT_LIST_7D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_6D(T)>
#define BOOST_LLVM_INIT_LIST_8D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_7D(T)>
#define BOOST_LLVM_INIT_LIST_9D(T) std::initializer_list<BOOST_LLVM_INIT_LIST_8D(T)>

