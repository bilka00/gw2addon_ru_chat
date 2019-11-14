// Copyright (c) 2017-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#ifndef TAO_PEGTL_CONTRIB_PARSE_TREE_HPP
#define TAO_PEGTL_CONTRIB_PARSE_TREE_HPP

#include <cassert>
#include <memory>
#if defined(_MSC_VER) && _MSC_VER == 1900
#include <tuple>
#endif
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include "../config.hpp"
#include "../memory_input.hpp"
#include "../normal.hpp"
#include "../nothing.hpp"
#include "../parse.hpp"

#include "../analysis/counted.hpp"
#include "../analysis/generic.hpp"
#include "../internal/conditional.hpp"
#include "../internal/demangle.hpp"
#include "../internal/iterator.hpp"

namespace tao
{
   namespace TAO_PEGTL_NAMESPACE
   {
      namespace parse_tree
      {
         template< typename T >
         struct basic_node
         {
            using node_t = T;
            using children_t = std::vector< std::unique_ptr< node_t > >;
            children_t children;

            const std::type_info* id = nullptr;
            std::string source;

            TAO_PEGTL_NAMESPACE::internal::iterator m_begin;
            TAO_PEGTL_NAMESPACE::internal::iterator m_end;

            // each node will be default constructed
            basic_node() = default;

            // no copy/move is necessary
            // (nodes are always owned/handled by a std::unique_ptr)
            basic_node( const basic_node& ) = delete;
            basic_node( basic_node&& ) = delete;

            ~basic_node() = default;

            // no assignment either
            basic_node& operator=( const basic_node& ) = delete;
            basic_node& operator=( basic_node&& ) = delete;

            bool is_root() const noexcept
            {
               return id == nullptr;
            }

            template< typename U >
            bool is() const noexcept
            {
               return id == &typeid( U );
            }

            std::string name() const
            {
               assert( !is_root() );
               return TAO_PEGTL_NAMESPACE::internal::demangle( id->name() );
            }

            position begin() const
            {
               return position( m_begin, source );
            }

            position end() const
            {
               return position( m_end, source );
            }

            bool has_content() const noexcept
            {
               return m_end.data != nullptr;
            }

            std::string content() const
            {
               assert( has_content() );
               return std::string( m_begin.data, m_end.data );
            }

            template< tracking_mode P = tracking_mode::IMMEDIATE, typename Eol = eol::lf_crlf >
            memory_input< P, Eol > as_memory_input() const
            {
               assert( has_content() );
               return { m_begin.data, m_end.data, source, m_begin.byte, m_begin.line, m_begin.byte_in_line };
            }

            template< typename... States >
            void remove_content( States&&... /*unused*/ ) noexcept
            {
               m_end.reset();
            }

            // all non-root nodes are initialized by calling this method
            template< typename Rule, typename Input, typename... States >
            void start( const Input& in, States&&... /*unused*/ )
            {
               id = &typeid( Rule );
               source = in.source();
               m_begin = in.iterator();
            }

            // if parsing of the rule succeeded, this method is called
            template< typename Rule, typename Input, typename... States >
            void success( const Input& in, States&&... /*unused*/ ) noexcept
            {
               m_end = in.iterator();
            }

            // if parsing of the rule failed, this method is called
            template< typename Rule, typename Input, typename... States >
            void failure( const Input& /*unused*/, States&&... /*unused*/ ) noexcept
            {
            }

            // if parsing succeeded and the (optional) transform call
            // did not discard the node, it is appended to its parent.
            // note that "child" is the node whose Rule just succeeded
            // and "*this" is the parent where the node should be appended.
            template< typename... States >
            void emplace_back( std::unique_ptr< node_t > child, States&&... /*unused*/ )
            {
               assert( child );
               children.emplace_back( std::move( child ) );
            }
         };

         struct node
            : basic_node< node >
         {
         };

         namespace internal
         {
            template< typename Node >
            struct state
            {
               std::vector< std::unique_ptr< Node > > stack;

               state()
               {
                  emplace_back();
               }

               void emplace_back()
               {
                  stack.emplace_back( std::unique_ptr< Node >( new Node ) );  // NOLINT: std::make_unique requires C++14
               }

               std::unique_ptr< Node >& back() noexcept
               {
                  return stack.back();
               }

               void pop_back() noexcept
               {
                  return stack.pop_back();
               }
            };

            template< typename Selector, typename... States >
            void transform( States&&... /*unused*/ ) noexcept
            {
            }

            template< typename Selector, typename Node, typename... States >
            auto transform( std::unique_ptr< Node >& n, States&&... st ) noexcept( noexcept( Selector::transform( n, st... ) ) )
               -> decltype( Selector::transform( n, st... ), void() )
            {
               Selector::transform( n, st... );
            }

            template< unsigned Level, typename Analyse, template< typename... > class Selector >
            struct is_leaf
               : std::false_type
            {
            };

            template< analysis::rule_type Type, template< typename... > class Selector >
            struct is_leaf< 0, analysis::generic< Type >, Selector >
               : std::true_type
            {
            };

            template< analysis::rule_type Type, unsigned Count, template< typename... > class Selector >
            struct is_leaf< 0, analysis::counted< Type, Count >, Selector >
               : std::true_type
            {
            };

            template< analysis::rule_type Type, typename... Rules, template< typename... > class Selector >
            struct is_leaf< 0, analysis::generic< Type, Rules... >, Selector >
               : std::false_type
            {
            };

            template< analysis::rule_type Type, unsigned Count, typename... Rules, template< typename... > class Selector >
            struct is_leaf< 0, analysis::counted< Type, Count, Rules... >, Selector >
               : std::false_type
            {
            };

            template< bool... >
            struct bool_sequence;

            template< bool... Bs >
            using is_all = std::is_same< bool_sequence< Bs..., true >, bool_sequence< true, Bs... > >;

            template< unsigned Level, typename Rule, template< typename... > class Selector >
            using is_unselected_leaf = std::integral_constant< bool, !Selector< Rule >::value && is_leaf< Level, typename Rule::analyze_t, Selector >::value >;

            template< unsigned Level, analysis::rule_type Type, typename... Rules, template< typename... > class Selector >
            struct is_leaf< Level, analysis::generic< Type, Rules... >, Selector >
               : is_all< is_unselected_leaf< Level - 1, Rules, Selector >::value... >
            {
            };

            template< unsigned Level, analysis::rule_type Type, unsigned Count, typename... Rules, template< typename... > class Selector >
            struct is_leaf< Level, analysis::counted< Type, Count, Rules... >, Selector >
               : is_all< is_unselected_leaf< Level - 1, Rules, Selector >::value... >
            {
            };

            template< typename Node, template< typename... > class Selector, template< typename... > class Control >
            struct make_control
            {
               template< typename Rule, bool, bool >
               struct control;

               template< typename Rule >
               using type = control< Rule, Selector< Rule >::value, is_leaf< 8, typename Rule::analyze_t, Selector >::value >;
            };

            template< typename Node, template< typename... > class Selector, template< typename... > class Control >
            template< typename Rule >
            struct make_control< Node, Selector, Control >::control< Rule, false, true >
               : Control< Rule >
            {
#if defined(_MSC_VER) && _MSC_VER == 1900
               template< typename Input, std::size_t... Is, typename... States >
               static void start_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::start( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  Control< Rule >::start( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< typename Input, typename... States >
               static void start( const Input& in, States&&... st ) noexcept( noexcept( start_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  start_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void success_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::success( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  Control< Rule >::success( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< typename Input, typename... States >
               static void success( const Input& in, States&&... st ) noexcept( noexcept( success_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  success_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void failure_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::failure( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  Control< Rule >::failure( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< typename Input, typename... States >
               static void failure( const Input& in, States&&... st ) noexcept( noexcept( failure_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  failure_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void raise_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::raise( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  Control< Rule >::raise( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< typename Input, typename... States >
               static void raise( const Input& in, States&&... st ) noexcept( noexcept( raise_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  raise_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< template< typename... > class Action, typename Input, std::size_t... Is, typename... States >
               static auto apply0_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::template apply0< Action >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
                  -> decltype( auto )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  return Control< Rule >::template apply0< Action >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< template< typename... > class Action, typename Input, typename... States >
               static auto apply0( const Input& in, States&&... st ) noexcept( noexcept( apply0_impl< Action >( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
                  -> decltype( auto )
               {
                  return apply0_impl< Action >( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, std::size_t... Is, typename... States >
               static auto apply_impl( const Iterator& begin, const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::template apply< Action >( begin, in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
                  -> decltype( auto )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  return Control< Rule >::template apply< Action >( begin, in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, typename... States >
               static auto apply( const Iterator& begin, const Input& in, States&&... st ) noexcept( noexcept( apply_impl< Action >( begin, in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
                  -> decltype( auto )
               {
                  return apply_impl< Action >( begin, in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }
#else
               template< typename Input, typename... States >
               static void start( const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::start( in, st... ) ) )
               {
                  Control< Rule >::start( in, st... );
               }

               template< typename Input, typename... States >
               static void success( const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::success( in, st... ) ) )
               {
                  Control< Rule >::success( in, st... );
               }

               template< typename Input, typename... States >
               static void failure( const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::failure( in, st... ) ) )
               {
                  Control< Rule >::failure( in, st... );
               }

               template< typename Input, typename... States >
               static void raise( const Input& in, States&&... st, state< Node >& /*unused*/ )
               {
                  Control< Rule >::raise( in, st... );
               }

               template< template< typename... > class Action, typename Input, typename... States >
               static auto apply0( const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::template apply0< Action >( in, st... ) ) )
                  -> decltype( Control< Rule >::template apply0< Action >( in, st... ) )
               {
                  return Control< Rule >::template apply0< Action >( in, st... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, typename... States >
               static auto apply( const Iterator& begin, const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::template apply< Action >( begin, in, st... ) ) )
                  -> decltype( Control< Rule >::template apply< Action >( begin, in, st... ) )
               {
                  return Control< Rule >::template apply< Action >( begin, in, st... );
               }
#endif
            };

            template< typename Node, template< typename... > class Selector, template< typename... > class Control >
            template< typename Rule >
            struct make_control< Node, Selector, Control >::control< Rule, false, false >
               : Control< Rule >
            {
#if defined(_MSC_VER) && _MSC_VER == 1900
               template< typename Input, std::size_t... Is, typename... States >
               static void start_impl( const Input& in, std::index_sequence< Is... >, States&&... st )
               {
                  Control< Rule >::start( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  state< Node >& state = std::get< sizeof...( st ) - 1 >( std::forward_as_tuple( st... ) );
                  state.emplace_back();
               }

               template< typename Input, typename... States >
               static void start( const Input& in, States&&... st ) noexcept( noexcept( start_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  start_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void success_impl( const Input& in, std::index_sequence< Is... >, States&&... st )
               {
                  Control< Rule >::success( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  state< Node >& state = std::get< sizeof...( st ) - 1 >( std::forward_as_tuple( st... ) );
                  auto n = std::move( state.back() );
                  state.pop_back();
                  for( auto& c : n->children ) {
                     state.back()->children.emplace_back( std::move( c ) );
                  }
               }

               template< typename Input, typename... States >
               static void success( const Input& in, States&&... st ) noexcept( noexcept( success_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  success_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void failure_impl( const Input& in, std::index_sequence< Is... >, States&&... st )
               {
                  Control< Rule >::failure( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  state< Node >& state = std::get< sizeof...( st ) - 1 >( std::forward_as_tuple( st... ) );
                  state.pop_back();
               }

               template< typename Input, typename... States >
               static void failure( const Input& in, States&&... st ) noexcept( noexcept( failure_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  failure_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void raise_impl( const Input& in, std::index_sequence< Is... >, States&&... st )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  Control< Rule >::raise( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< typename Input, typename... States >
               static void raise( const Input& in, States&&... st )
               {
                  raise_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< template< typename... > class Action, typename Input, std::size_t... Is, typename... States >
               static auto apply0_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::template apply0< Action >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
                  -> decltype( auto )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  return Control< Rule >::template apply0< Action >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< template< typename... > class Action, typename Input, typename... States >
               static auto apply0( const Input& in, States&&... st ) noexcept( noexcept( apply0_impl< Action >( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
                  -> decltype( auto )
               {
                  return apply0_impl< Action >( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, std::size_t... Is, typename... States >
               static auto apply_impl( const Iterator& begin, const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::template apply< Action >( begin, in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
                  -> decltype( auto )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  return Control< Rule >::template apply< Action >( begin, in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, typename... States >
               static auto apply( const Iterator& begin, const Input& in, States&&... st ) noexcept( noexcept( apply_impl< Action >( begin, in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
                  -> decltype( auto )
               {
                  return apply_impl< Action >( begin, in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }
#else
               template< typename Input, typename... States >
               static void start( const Input& in, States&&... st, state< Node >& state )
               {
                  Control< Rule >::start( in, st... );
                  state.emplace_back();
               }

               template< typename Input, typename... States >
               static void success( const Input& in, States&&... st, state< Node >& state )
               {
                  Control< Rule >::success( in, st... );
                  auto n = std::move( state.back() );
                  state.pop_back();
                  for( auto& c : n->children ) {
                     state.back()->children.emplace_back( std::move( c ) );
                  }
               }

               template< typename Input, typename... States >
               static void failure( const Input& in, States&&... st, state< Node >& state ) noexcept( noexcept( Control< Rule >::failure( in, st... ) ) )
               {
                  Control< Rule >::failure( in, st... );
                  state.pop_back();
               }

               template< typename Input, typename... States >
               static void raise( const Input& in, States&&... st, state< Node >& /*unused*/ )
               {
                  Control< Rule >::raise( in, st... );
               }

               template< template< typename... > class Action, typename Input, typename... States >
               static auto apply0( const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::template apply0< Action >( in, st... ) ) )
                  -> decltype( Control< Rule >::template apply0< Action >( in, st... ) )
               {
                  return Control< Rule >::template apply0< Action >( in, st... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, typename... States >
               static auto apply( const Iterator& begin, const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::template apply< Action >( begin, in, st... ) ) )
                  -> decltype( Control< Rule >::template apply< Action >( begin, in, st... ) )
               {
                  return Control< Rule >::template apply< Action >( begin, in, st... );
               }
#endif
            };

            template< typename Node, template< typename... > class Selector, template< typename... > class Control >
            template< typename Rule, bool B >
            struct make_control< Node, Selector, Control >::control< Rule, true, B >
               : Control< Rule >
            {
#if defined(_MSC_VER) && _MSC_VER == 1900
               template< typename Input, std::size_t... Is, typename... States >
               static void start_impl( const Input& in, std::index_sequence< Is... >, States&&... st )
               {
                  Control< Rule >::start( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  state< Node >& state = std::get< sizeof...( st ) - 1 >( std::forward_as_tuple( st... ) );
                  state.emplace_back();
                  state.back()->template start< Rule >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< typename Input, typename... States >
               static void start( const Input& in, States&&... st ) noexcept( noexcept( start_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  start_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void success_impl( const Input& in, std::index_sequence< Is... >, States&&... st )
               {
                  Control< Rule >::success( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  state< Node >& state = std::get< sizeof...( st ) - 1 >( std::forward_as_tuple( st... ) );
                  auto n = std::move( state.back() );
                  state.pop_back();
                  n->template success< Rule >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  transform< Selector< Rule > >( n, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  if( n ) {
                     state.back()->emplace_back( std::move( n ), std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  }
               }

               template< typename Input, typename... States >
               static void success( const Input& in, States&&... st ) noexcept( noexcept( success_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  success_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void failure_impl( const Input& in, std::index_sequence< Is... >, States&&... st )
               {
                  Control< Rule >::failure( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  state< Node >& state = std::get< sizeof...( st ) - 1 >( std::forward_as_tuple( st... ) );
                  state.back()->template failure< Rule >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
                  state.pop_back();
               }

               template< typename Input, typename... States >
               static void failure( const Input& in, States&&... st ) noexcept( noexcept( failure_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  failure_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< typename Input, std::size_t... Is, typename... States >
               static void raise_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::raise( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  Control< Rule >::raise( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< typename Input, typename... States >
               static void raise( const Input& in, States&&... st ) noexcept( noexcept( raise_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
               {
                  raise_impl( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< template< typename... > class Action, typename Input, std::size_t... Is, typename... States >
               static auto apply0_impl( const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::template apply0< Action >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
                  -> decltype( auto )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  return Control< Rule >::template apply0< Action >( in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< template< typename... > class Action, typename Input, typename... States >
               static auto apply0( const Input& in, States&&... st ) noexcept( noexcept( apply0_impl< Action >( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
                  -> decltype( auto )
               {
                  return apply0_impl< Action >( in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, std::size_t... Is, typename... States >
               static auto apply_impl( const Iterator& begin, const Input& in, std::index_sequence< Is... >, States&&... st ) noexcept( noexcept( Control< Rule >::template apply< Action >( begin, in, std::get< Is >( std::forward_as_tuple ( st... ) )... ) ) )
                  -> decltype( auto )
               {
                  std::get< sizeof...(st) - 1 >( std::forward_as_tuple( st... ) ); // silence C4100
                  return Control< Rule >::template apply< Action >( begin, in, std::get< Is >( std::forward_as_tuple ( st... ) )... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, typename... States >
               static auto apply( const Iterator& begin, const Input& in, States&&... st ) noexcept( noexcept( apply_impl< Action >( begin, in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... ) ) )
                  -> decltype( auto )
               {
                  return apply_impl< Action >( begin, in, std::make_index_sequence< sizeof...( st ) - 1 >{}, std::forward< States >( st )... );
               }
#else
               template< typename Input, typename... States >
               static void start( const Input& in, States&&... st, state< Node >& state )
               {
                  Control< Rule >::start( in, st... );
                  state.emplace_back();
                  state.back()->template start< Rule >( in, st... );
               }

               template< typename Input, typename... States >
               static void success( const Input& in, States&&... st, state< Node >& state )
               {
                  Control< Rule >::success( in, st... );
                  auto n = std::move( state.back() );
                  state.pop_back();
                  n->template success< Rule >( in, st... );
                  transform< Selector< Rule > >( n, st... );
                  if( n ) {
                     state.back()->emplace_back( std::move( n ), st... );
                  }
               }

               template< typename Input, typename... States >
               static void failure( const Input& in, States&&... st, state< Node >& state ) noexcept( noexcept( Control< Rule >::failure( in, st... ) ) && noexcept( std::declval< node& >().template failure< Rule >( in, st... ) ) )
               {
                  Control< Rule >::failure( in, st... );
                  state.back()->template failure< Rule >( in, st... );
                  state.pop_back();
               }

               template< typename Input, typename... States >
               static void raise( const Input& in, States&&... st, state< Node >& /*unused*/ )
               {
                  Control< Rule >::raise( in, st... );
               }

               template< template< typename... > class Action, typename Input, typename... States >
               static auto apply0( const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::template apply0< Action >( in, st... ) ) )
                  -> decltype( Control< Rule >::template apply0< Action >( in, st... ) )
               {
                  return Control< Rule >::template apply0< Action >( in, st... );
               }

               template< template< typename... > class Action, typename Iterator, typename Input, typename... States >
               static auto apply( const Iterator& begin, const Input& in, States&&... st, state< Node >& /*unused*/ ) noexcept( noexcept( Control< Rule >::template apply< Action >( begin, in, st... ) ) )
                  -> decltype( Control< Rule >::template apply< Action >( begin, in, st... ) )
               {
                  return Control< Rule >::template apply< Action >( begin, in, st... );
               }
#endif
            };

            template< typename >
            struct element
            {
            };

            template< typename >
            struct store_all : std::true_type
            {
            };

         }  // namespace internal

         using store_content = std::true_type;

         // some nodes don't need to store their content
         struct remove_content : std::true_type
         {
            template< typename Node, typename... States >
            static void transform( std::unique_ptr< Node >& n, States&&... st ) noexcept( noexcept( n->Node::remove_content( st... ) ) )
            {
               n->remove_content( st... );
            }
         };

         // if a node has only one child, replace the node with its child, otherwise apply B
         template< typename Base >
         struct fold_one_or : Base
         {
            template< typename Node, typename... States >
            static void transform( std::unique_ptr< Node >& n, States&&... st ) noexcept( noexcept( n->children.size(), Base::transform( n, st... ) ) )
            {
               if( n->children.size() == 1 ) {
                  n = std::move( n->children.front() );
               }
               else {
                  Base::transform( n, st... );
               }
            }
         };

         // if a node has no children, discard the node, otherwise apply B
         template< typename Base >
         struct discard_empty_or : Base
         {
            template< typename Node, typename... States >
            static void transform( std::unique_ptr< Node >& n, States&&... st ) noexcept( noexcept( n->children.empty(), Base::transform( n, st... ) ) )
            {
               if( n->children.empty() ) {
                  n.reset();
               }
               else {
                  Base::transform( n, st... );
               }
            }
         };

         using fold_one = fold_one_or< remove_content >;
         using discard_empty = discard_empty_or< remove_content >;

         template< typename Rule, typename... Collections >
         struct selector : std::false_type
         {
         };

         // TODO: Implement in a non-recursive way
         // TODO: Check for multiple matches (currently: first match wins)
         template< typename Rule, typename Collection, typename... Collections >
         struct selector< Rule, Collection, Collections... >
            : TAO_PEGTL_NAMESPACE::internal::conditional< Collection::template contains< Rule >::value >::template type< typename Collection::type, selector< Rule, Collections... > >
         {
         };

         template< typename Base >
         struct apply
         {
            template< typename... Rules >
            struct to
               : internal::element< Rules >...
            {
               using type = Base;

               template< typename Rule >
               using contains = std::is_base_of< internal::element< Rule >, to >;
            };
         };

         using apply_store_content = apply< store_content >;
         using apply_remove_content = apply< remove_content >;
         using apply_fold_one = apply< fold_one >;
         using apply_discard_empty = apply< discard_empty >;

         template< typename Rule,
                   typename Node,
                   template< typename... > class Selector = internal::store_all,
                   template< typename... > class Action = nothing,
                   template< typename... > class Control = normal,
                   typename Input,
                   typename... States >
         std::unique_ptr< Node > parse( Input&& in, States&&... st )
         {
            internal::state< Node > state;
            if( !TAO_PEGTL_NAMESPACE::parse< Rule, Action, internal::make_control< Node, Selector, Control >::template type >( in, st..., state ) ) {
               return nullptr;
            }
            assert( state.stack.size() == 1 );
            return std::move( state.back() );
         }

         template< typename Rule,
                   template< typename... > class Selector = internal::store_all,
                   template< typename... > class Action = nothing,
                   template< typename... > class Control = normal,
                   typename Input,
                   typename... States >
         std::unique_ptr< node > parse( Input&& in, States&&... st )
         {
            return parse< Rule, node, Selector, Action, Control >( in, st... );
         }

      }  // namespace parse_tree

   }  // namespace TAO_PEGTL_NAMESPACE

}  // namespace tao

#endif
