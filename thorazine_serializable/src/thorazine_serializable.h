#include <type_traits>
#include <iostream>
#include <concepts> 
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

namespace thorazine
{
	template< class T >
	class objective_t
	{
	protected:

		using self_t = T;

	public:

		template< class... Args >
		__forceinline static std::shared_ptr< self_t > instantiate( Args&&... args )
		{
			return std::make_shared< self_t >( std::forward< Args >( args )... );
		}

	};

	class interface_t
	{
	public:

		virtual ~interface_t( ) = default;

	};

	class serializer_t
	{
	public:

		__forceinline static constexpr bool is_serializer( ) noexcept
		{
			return true;
		}

		template< class T >
		__forceinline static std::size_t serialize( std::ofstream& o, const T& v ) noexcept
		{
			constexpr auto v_size = sizeof( T );

			try
			{
				o.write( reinterpret_cast< const char* >( &v_size ), sizeof( v_size ) ); // write size
				o.write( reinterpret_cast< const char* >( &v ), v_size ); // write value
			}
			catch ( ... )
			{
				return 0ull;
			}

			return v_size + sizeof( v_size );
		}

		template< class T >
		__forceinline static std::size_t serialize( std::ofstream& o, const T& v, const std::size_t s ) noexcept
		{
			try
			{
				o.write( reinterpret_cast< const char* >( &s ), sizeof( s ) ); // write size
				o.write( reinterpret_cast< const char* >( &v ), s ); // write value
			}
			catch ( ... )
			{
				return 0ull;
			}

			return s + sizeof( s );
		}

	};

	class deserializer_t
	{
	public:

		__forceinline static constexpr bool is_deserializer( ) noexcept
		{
			return true;
		}

		template< class T >
		__forceinline static std::size_t deserialize( std::ifstream& i, T& v ) noexcept
		{
			constexpr auto v_size = sizeof( T );
			constexpr auto size_of_v_size = sizeof( v_size );
			auto v_runtime_size = 0ull;

			try
			{
				i.read( reinterpret_cast< char* >( &v_runtime_size ), size_of_v_size );

				if ( v_runtime_size != v_size )
					throw std::exception{ };

				i.read( reinterpret_cast< char* >( &v ), v_size );
			}
			catch ( ... )
			{
				return 0ull;
			}

			return v_size + size_of_v_size;
		}

		template< class T >
		__forceinline static std::size_t deserialize( std::ifstream& i, T& v, const std::size_t s ) noexcept
		{
			constexpr auto size_of_v_size = sizeof( s );
			auto v_runtime_size = 0ull;

			try
			{
				i.read( reinterpret_cast< char* >( &v_runtime_size ), size_of_v_size );

				if ( v_runtime_size != s )
					throw std::exception{ "failed to deserialize, object size not match!" };

				i.read( reinterpret_cast< char* >( &v ), s );
			}
			catch ( ... )
			{
				return 0ull;
			}

			return s + size_of_v_size;
		}

	};

	template< class S, class D >
	class serializable_t : public interface_t
	{
	protected:

		using local_serializer_t = S;
		using local_deserializer_t = D;

		static_assert( local_serializer_t::is_serializer( ) );
		static_assert( local_deserializer_t::is_deserializer( ) );

	public:

		virtual std::size_t serialize( std::ofstream& o ) const = 0;
		virtual std::size_t deserialize( std::ifstream& o ) = 0;

		virtual std::size_t class_size( bool skip_vftable = true ) const
		{
			return skip_vftable ? 0ull : sizeof( void* );
		}

	};

	using default_serializable_t = serializable_t< serializer_t, deserializer_t >;

	template< class T >
	class default_object_t
		: public default_serializable_t, public objective_t< T >
	{
	public:



	};
}