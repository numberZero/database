#pragma once
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace packer
{
	struct PackingError: std::runtime_error { using std::runtime_error::runtime_error; };
#define sc(b,s) struct s: b { using b::b; }
	sc(PackingError, ParseError);
	sc(ParseError, ReadError);
	sc(PackingError, SerializeError);
	sc(SerializeError, WriteError);
#undef sc

	namespace detail
	{
		template <typename _TypeClass, typename _Type>
		struct StaticOnly
		{
			static void parse(std::istream& stream, _Type& value);
			static void serialize(std::ostream& stream, _Type const& value);
		};

		template <typename _Class, typename... _Entries>
		struct Struct;

		template <typename _Class>
		struct Struct<_Class>
		{
			typedef _Class Class;
			typedef _Class Type;
			static constexpr std::size_t StaticSize = 0;
			static constexpr std::size_t DynamicHeader = 0;

			static void parse(char const buffer[], Type& value) {}
			static void serialize(char buffer[], Type const& value) {}
			static void parse(std::istream& stream, Type& value) {}
			static void serialize(std::ostream& stream, Type const& value) {}
		};

		template <typename _Class, typename _Entry, typename... _Entries>
		struct Struct<_Class, _Entry, _Entries...>
		{
			typedef _Class Class;
			typedef _Class Type;
			typedef _Entry Entry;
			typedef Struct<_Class, _Entries...> Next;
			static constexpr std::size_t StaticSize = Next::StaticSize + Entry::StaticSize;
			static constexpr std::size_t DynamicHeader = Next::DynamicHeader + Entry::DynamicHeader;

			static void parse(char const buffer[StaticSize], Type& value);
			static void serialize(char buffer[StaticSize], Type const& value);

			static void parse(std::istream& stream, Type& value)
			{
				Entry::PackerType::parse(stream, Entry::get(value));
				Next::parse(stream, value);
			}

			static void serialize(std::ostream& stream, Type const& value)
			{
				Entry::PackerType::serialize(stream, Entry::get(value));
				Next::serialize(stream, value);
			}
		};
	}

	namespace type
	{
		template <typename _Type, std::size_t _Size>
		struct Integer: detail::StaticOnly<Integer<_Type, _Size>, _Type>
		{
			static_assert(std::is_integral<_Type>::value, "packer::type::Integer can work with integral types only");
			typedef _Type Type;
			static constexpr std::size_t StaticSize = _Size;
			static constexpr std::size_t DynamicHeader = 0;
			typedef typename std::make_unsigned<Type>::type Holder;
			typedef detail::StaticOnly<Integer<Type, StaticSize>, Type> SO;

			using SO::parse;
			using SO::serialize;
			static void parse(char const buffer[StaticSize], _Type& value);
			static void serialize(char buffer[StaticSize], _Type const& value);
		};

		template <typename _Type, std::size_t _Size>
		struct Enum: detail::StaticOnly<Enum<_Type, _Size>, _Type>
		{
			static_assert(std::is_enum<_Type>::value, "packer::type::Integer can work with integral types only");
			typedef _Type Type;
			static constexpr std::size_t StaticSize = _Size;
			static constexpr std::size_t DynamicHeader = 0;
			typedef typename std::make_unsigned<Type>::type Holder;
			typedef detail::StaticOnly<Enum<Type, StaticSize>, Type> SO;

			using SO::parse;
			using SO::serialize;
			static void parse(char const buffer[StaticSize], _Type& value);
			static void serialize(char buffer[StaticSize], _Type const& value);
		};

		template <>
		struct Enum<bool, 1>: detail::StaticOnly<Enum<bool, 1>, bool>
		{
			typedef bool Type;
			static constexpr std::size_t StaticSize = 1;
			static constexpr std::size_t DynamicHeader = 0;
			typedef unsigned char Holder;
			typedef detail::StaticOnly<Enum<Type, StaticSize>, Type> SO;

			using SO::parse;
			using SO::serialize;
			static void parse(char const buffer[StaticSize], bool& value);
			static void serialize(char buffer[StaticSize], bool const& value);
		};

		using Boolean = Enum<bool, 1>;

		template <typename _Type, std::size_t _Size>
		struct String;

		template <std::size_t _Size>
		struct String<char[_Size], _Size>: detail::StaticOnly<String<char[_Size], _Size>, char[_Size]>
		{
			typedef char Type[_Size];
			static constexpr std::size_t StaticSize = _Size;
			static constexpr std::size_t DynamicHeader = 0;

			static void parse(char const buffer[StaticSize], Type& value);
			static void serialize(char buffer[StaticSize], Type const& value);
			static void parse(std::istream& stream, Type& value);
			static void serialize(std::ostream& stream, Type const& value);
		};

		template <std::size_t _Size>
		struct String<std::string, _Size>
		{
			typedef std::string Type;
			typedef Integer<std::size_t, 4> Header;
			static constexpr std::size_t StaticSize = _Size;
			static constexpr std::size_t DynamicHeader = Header::StaticSize;

			static void parse(char const buffer[StaticSize], Type& value);
			static void serialize(char buffer[StaticSize], Type const& value);
			static void parse(std::istream& stream, Type& value);
			static void serialize(std::ostream& stream, Type const& value);
		};

		template <>
		struct String<std::string, 0>
		{
			typedef std::string Type;
			typedef Integer<std::size_t, 4> Header;
			static constexpr std::size_t DynamicHeader = Header::StaticSize;

			static void parse(std::istream& stream, Type& value);
			static void serialize(std::ostream& stream, Type const& value);
		};

		template <typename _Type, typename... _Entries>
		struct Struct
		{
			typedef _Type Type;
			typedef detail::Struct<_Type, _Entries...> Contents;
			static constexpr std::size_t StaticSize = Contents::StaticSize;
			static constexpr std::size_t DynamicHeader = Contents::DynamicHeader;

			static void parse(char const buffer[StaticSize], Type& value)
			{
				Contents::parse(buffer, value);
			}

			static void serialize(char buffer[StaticSize], Type const& value)
			{
				Contents::serialize(buffer, value);
			}

			static void parse(std::istream& stream, Type& value)
			{
				Contents::parse(stream, value);
			}

			static void serialize(std::ostream& stream, Type const& value)
			{
				Contents::serialize(stream, value);
			}
		};
	}

	namespace pack
	{
		struct Entry
		{
		};

		template <typename _Class, typename _FieldType, std::size_t _Size, _FieldType _Class:: *_Field>
		struct Integer: Entry
		{
			typedef type::Integer<_FieldType, _Size> PackerType;
			typedef _Class Class;
			typedef typename PackerType::Type FieldType;
			static constexpr _FieldType _Class:: *Field = _Field;
			static FieldType& get(_Class& c) { return c.*Field; }
			static FieldType const& get(_Class const& c) { return c.*Field; }
		};

		template <typename _Class, typename _FieldType, std::size_t _Size, _FieldType _Class:: *_Field>
		struct Enum: Entry
		{
			typedef type::Enum<_FieldType, _Size> PackerType;
			typedef _Class Class;
			typedef typename PackerType::Type FieldType;
			static constexpr _FieldType _Class:: *Field = _Field;
			static FieldType& get(_Class& c) { return c.*Field; }
			static FieldType const& get(_Class const& c) { return c.*Field; }
		};

		template <typename _Class, bool _Class:: *_Field>
		using Boolean = Enum<_Class, bool, 1, _Field>;

		template <typename _Class, typename _FieldType, std::size_t _Size, _FieldType _Class:: *_Field>
		struct String: Entry
		{
			typedef type::String<_FieldType, _Size> PackerType;
			typedef _Class Class;
			typedef typename PackerType::Type FieldType;
			static constexpr _FieldType _Class:: *Field = _Field;
			static FieldType& get(_Class& c) { return c.*Field; }
			static FieldType const& get(_Class const& c) { return c.*Field; }
		};

		template <typename _Class, typename _PackerType, typename _PackerType::Type _Class:: *_Field>
		struct Struct: Entry
		{
			typedef _Class Class;
			typedef _PackerType PackerType;
			typedef typename PackerType::Type FieldType;
			static constexpr typename PackerType::Type _Class:: *Field = _Field;
			static FieldType& get(_Class& c) { return c.*Field; }
			static FieldType const& get(_Class const& c) { return c.*Field; }
		};

		template <typename _Class, typename _PackerType>
		struct Parent: Entry
		{
			typedef _Class Class;
			typedef _PackerType PackerType;
			typedef typename PackerType::Type FieldType;
			static_assert(std::is_base_of<FieldType, Class>::value, "packer::pack::Parent: _PackerType must be a packer for any base of _Class");
			static FieldType& get(_Class& c) { return static_cast<FieldType&>(c); }
			static FieldType const& get(_Class const& c) { return static_cast<FieldType const&>(c); }
		};
	}
}

#define MESSAGE_INTEGER(struct,field,size) packer::pack::Integer<struct, decltype(struct::field), size, &struct::field>
#define MESSAGE_INTEGER_NS(struct,field) packer::pack::Integer<struct, decltype(struct::field), sizeof(struct::field), &struct::field>
#define MESSAGE_BOOLEAN_NS(struct,field) packer::pack::Boolean<struct, &struct::field>
#define MESSAGE_ENUM(struct,field,size) packer::pack::Enum<struct, decltype(struct::field), size, &struct::field>
#define MESSAGE_ENUM_NS(struct,field) packer::pack::Enum<struct, decltype(struct::field), sizeof(struct::field), &struct::field>
#define MESSAGE_STRING(struct,field,size) packer::pack::String<struct, decltype(struct::field), size, &struct::field>
#define MESSAGE_STRING_NS(struct,field) packer::pack::String<struct, char[sizeof(struct::field)], sizeof(struct::field), &struct::field>

#define MESSAGE_STRUCT(struct,field,pk) packer::pack::Struct<struct, pk, &struct::field>
#define MESSAGE_STRUCT_A(struct,field,pk) packer::pack::Struct<struct, pk<decltype(struct::field)>, &struct::field>

/***************************************
 * 
 * Implementation
 * 
 **************************************/

template <typename _TypeClass, typename _Type>
void packer::detail::StaticOnly<_TypeClass, _Type>::parse(std::istream& stream, _Type& value)
{
	char buffer[_TypeClass::StaticSize];
	stream.read(buffer, _TypeClass::StaticSize);
	if(!stream.good())
		throw ReadError("Can't parse: can't read from stream");
	_TypeClass::parse(buffer, value);
}

template <typename _TypeClass, typename _Type>
void packer::detail::StaticOnly<_TypeClass, _Type>::serialize(std::ostream& stream, _Type const& value)
{
	char buffer[_TypeClass::StaticSize];
	_TypeClass::serialize(buffer, value);
	stream.write(buffer, _TypeClass::StaticSize);
	if(!stream.good())
		throw WriteError("Can't serialize: can't write to stream");
}

template <typename _Type, std::size_t _Size>
void packer::type::Integer<_Type, _Size>::parse(char const buffer[StaticSize], _Type& value)
{
	Holder temp;
	bool sign;
	for(std::size_t k = 0; k < StaticSize; ++k)
	{
		std::size_t shift = (StaticSize - k - 1) * 8;
		unsigned char byte = buffer[k];
		if(std::is_signed<_Type>::value && (k == 0))
		{
			sign = byte & 0x80 != 0;
			byte &= 0x7F;
		}
		temp |= static_cast<Holder>(byte) << shift;
	}
	if(sign)
		value = -temp;
	else
		value = temp;
}

template <typename _Type, std::size_t _Size>
void packer::type::Integer<_Type, _Size>::serialize(char buffer[StaticSize], _Type const& value)
{
	Holder temp;
	bool sign = value < 0;
	if(sign)
		temp = -value;
	else
		temp = value;
	for(std::size_t k = 0; k < StaticSize; ++k)
	{
		std::size_t shift = (StaticSize - k - 1) * 8;
		unsigned char byte = (temp >> shift) & 0xFF;
		buffer[k] = byte;
	}
	if(sign)
		buffer[0] |= 0x80; // sign bit
	else
		buffer[0] &= 0x7F; // make sure we stored unsigned value (necessary if sizeof(_Type) < StaticSize)
}

template <typename _Type, std::size_t _Size>
void packer::type::Enum<_Type, _Size>::parse(char const buffer[StaticSize], _Type& value)
{
	Integer<Holder, StaticSize>::parse(buffer, reinterpret_cast<Holder&>(value));
}

template <typename _Type, std::size_t _Size>
void packer::type::Enum<_Type, _Size>::serialize(char buffer[StaticSize], _Type const& value)
{
	Integer<Holder, StaticSize>::serialize(buffer, reinterpret_cast<Holder const&>(value));
}
