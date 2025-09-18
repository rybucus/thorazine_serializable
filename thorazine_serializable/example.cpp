#include "src/thorazine_serializable.h"

namespace thorazine
{
	class test_object_t final
		: public default_object_t< test_object_t >
	{
	private:

		using super_t = default_object_t< test_object_t >;

	public:

		constexpr test_object_t( )
			: m_hHandle{ static_cast< std::uint64_t >( -1ll ) }
		{ }

		explicit test_object_t( const std::uint64_t h )
			: m_hHandle{ h }
		{ }

		std::size_t class_size( bool skip_vftable = true ) const override
		{
			return skip_vftable ? sizeof( *this ) - sizeof( void* ) : sizeof( *this );
		}

		std::size_t serialize( std::ofstream& o ) const override
		{
			return super_t::local_serializer_t::serialize
			(
				o,
				*reinterpret_cast< const test_object_t* >( reinterpret_cast< const char* >( this ) + sizeof( void* ) ),
				class_size( )
			);
		}

		std::size_t deserialize( std::ifstream& i ) override
		{
			return super_t::local_deserializer_t::deserialize
			(
				i,
				*reinterpret_cast< test_object_t* >( reinterpret_cast< char* >( this ) + sizeof( void* ) ),
				class_size( )
			);
		}

		std::uint64_t m_hHandle{ };

	};
}


int test_1( )
{
	auto o
		= std::make_unique< std::ofstream >( "out.bin", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary );

	if ( !o->is_open( ) )
		return 1;

	auto object
		= ::thorazine::test_object_t::instantiate( 89 );

	object->serialize( *o );

	o->close( );

	object->m_hHandle = 16;

	std::cout << object->m_hHandle << "\n";

	auto i
		= std::make_unique< std::ifstream >( "out.bin", std::ofstream::binary );

	if ( !i->is_open( ) )
		return 2;
	
	object->deserialize( *i );

	std::cout << object->m_hHandle << "\n";

	return 0;
}

int main( )
{
	const auto result = test_1( );
	return !result ? 0 : 1;
}