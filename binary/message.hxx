#pragma once
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

/**
 * 
 * \defgroup packer::type::*
 * 	Each class in \c packer::type should have the following methods:
 * @{
 * 
 * * \fn static std::size_t dynamic_size(_Type const& value)
 * 		\returns buffer size needed to store the \p value serialized
 * 
 * * \fn static std::size_t dynamic_parse(char const *buffer, std::size_t length, _Type& value)
 * 		\brief parses first bytes in \p buffer and stores result in \p value.
 * 		\returns count of bytes consumed
 * 
 * * \fn static std::size_t dynamic_serialize(char *buffer, std::size_t length, _Type const& value)
 * 		\brief serializes \p value into first bytes in \p buffer
 * 		\returns number of bytes used
 * 
 * * \fn static void static_parse(char const buffer[], Type& value)
 * 		\brief parses WHOLE \p buffer into \p value
 * 
 * * \fn static void static_serialize(char buffer[], Type const& value) 
 * 		\brief serializes \p value into WHOLE \p buffer
 * 
 * * \fn static void dynamic_parse(std::istream& stream, _Type& value)
 * 		\brief reads from \p stream, stores parsed value in \p value
 * 		\note advances stream pointer accordingly, instead of returning byte count
 * 
 * * \fn static void dynamic_serialize(std::ostream& stream, _Type const& value)
 * 		\brief serializes \p value to \p stream
 * 		\note advances stream pointer accordingly, instead of returning byte count
 * 
 * @}
 */

/***************************************
 * 
 * Interface
 * 
 **************************************/

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
			static std::size_t dynamic_size(_Type const& value);
			static std::size_t dynamic_parse(char const *buffer, std::size_t length, _Type& value);
			static std::size_t dynamic_serialize(char *buffer, std::size_t length, _Type const& value);
			static void dynamic_parse(std::istream& stream, _Type& value);
			static void dynamic_serialize(std::ostream& stream, _Type const& value);
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

			static std::size_t dynamic_size(Type const& value) { return 0; }
			static std::size_t dynamic_parse(char const *buffer, std::size_t length, Type& value) { return 0; }
			static std::size_t dynamic_serialize(char *buffer, std::size_t length, Type const& value) { return 0; }
			static void static_parse(char const buffer[], Type& value) {}
			static void static_serialize(char buffer[], Type const& value) {}
			static void dynamic_parse(std::istream& stream, Type& value) {}
			static void dynamic_serialize(std::ostream& stream, Type const& value) {}
		};

		template <typename _Class, typename _Entry, typename... _Entries>
		struct Struct<_Class, _Entry, _Entries...>
		{
			typedef _Class Class;
			typedef _Class Type;
			typedef _Entry Entry;
			typedef Struct<_Class, _Entries...> Next;
			static constexpr std::size_t StaticSize = Next::StaticSize + Entry::PackerType::StaticSize;
			static constexpr std::size_t DynamicHeader = Next::DynamicHeader + Entry::PackerType::DynamicHeader;

			static std::size_t dynamic_size(Type const& value)
			{
				return Entry::PackerType::dynamic_size(Entry::get(value)) + Next::dynamic_size(value);
			}

			static std::size_t dynamic_parse(char const *buffer, std::size_t length, Type& value)
			{
				std::size_t position = Entry::PackerType::dynamic_parse(buffer, length, Entry::get(value));
				position += Next::dynamic_parse(&buffer[position], length - position, value);
				return position;
			}

			static std::size_t dynamic_serialize(char *buffer, std::size_t length, Type const& value)
			{
				std::size_t position = Entry::PackerType::dynamic_serialize(buffer, length, Entry::get(value));
				position += Next::dynamic_serialize(&buffer[position], length - position, value);
				return position;
			}

			static void static_parse(char const buffer[StaticSize], Type& value)
			{
				Entry::PackerType::dynamic_parse(buffer, Entry::get(value));
				Next::dynamic_parse(&buffer[Entry::PackerType::StaticSize], value);
			}

			static void static_serialize(char buffer[StaticSize], Type const& value)
			{
				Entry::PackerType::dynamic_serialize(buffer, Entry::get(value));
				Next::dynamic_serialize(&buffer[Entry::PackerType::StaticSize], value);
			}

			static void dynamic_parse(std::istream& stream, Type& value)
			{
				Entry::PackerType::dynamic_parse(stream, Entry::get(value));
				Next::dynamic_parse(stream, value);
			}

			static void dynamic_serialize(std::ostream& stream, Type const& value)
			{
				Entry::PackerType::dynamic_serialize(stream, Entry::get(value));
				Next::dynamic_serialize(stream, value);
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

			using SO::dynamic_size;
			using SO::dynamic_parse;
			using SO::dynamic_serialize;
			static void static_parse(char const buffer[StaticSize], _Type& value);
			static void static_serialize(char buffer[StaticSize], _Type const& value);
		};

		template <typename _Type, std::size_t _Size>
		struct Enum: detail::StaticOnly<Enum<_Type, _Size>, _Type>
		{
			static_assert(std::is_enum<_Type>::value, "packer::type::Enum can work with enumeration and boolean types only");
			typedef _Type Type;
			static constexpr std::size_t StaticSize = _Size;
			static constexpr std::size_t DynamicHeader = 0;
			typedef typename std::make_unsigned<Type>::type Holder;
			typedef detail::StaticOnly<Enum<Type, StaticSize>, Type> SO;

			using SO::dynamic_size;
			using SO::dynamic_parse;
			using SO::dynamic_serialize;

			static void static_parse(char const buffer[StaticSize], _Type& value)
			{
				Integer<Holder, StaticSize>::dynamic_parse(buffer, reinterpret_cast<Holder&>(value));
			}

			static void static_serialize(char buffer[StaticSize], _Type const& value)
			{
				Integer<Holder, StaticSize>::dynamic_serialize(buffer, reinterpret_cast<Holder const&>(value));
			}
		};

		template <>
		struct Enum<bool, 1>: detail::StaticOnly<Enum<bool, 1>, bool>
		{
			typedef bool Type;
			static constexpr std::size_t StaticSize = 1;
			static constexpr std::size_t DynamicHeader = 0;
			typedef unsigned char Holder;
			typedef detail::StaticOnly<Enum<Type, StaticSize>, Type> SO;

			using SO::dynamic_size;
			using SO::dynamic_parse;
			using SO::dynamic_serialize;

			static void static_parse(char const buffer[StaticSize], bool& value)
			{
				Integer<Holder, StaticSize>::static_parse(buffer, reinterpret_cast<Holder&>(value));
			}

			static void static_serialize(char buffer[StaticSize], bool const& value)
			{
				Integer<Holder, StaticSize>::static_serialize(buffer, reinterpret_cast<Holder const&>(value));
			}
		};

		using Boolean = Enum<bool, 1>;

		template <typename _Type, std::size_t _Size>
		struct String;

		template <std::size_t _Size>
		struct String<char[_Size], _Size>
		{
			typedef char Type[_Size];
			static constexpr std::size_t StaticSize = _Size;
			static constexpr std::size_t DynamicHeader = 0;

			static std::size_t dynamic_size(Type const& value);
			static std::size_t dynamic_parse(char const *buffer, std::size_t length, Type& value);
			static std::size_t dynamic_serialize(char *buffer, std::size_t length, Type const& value);
			static void static_parse(char const buffer[StaticSize], Type& value);
			static void static_serialize(char buffer[StaticSize], Type const& value);
			static void dynamic_parse(std::istream& stream, Type& value);
			static void dynamic_serialize(std::ostream& stream, Type const& value);
		};

		template <std::size_t _Size>
		struct String<std::string, _Size>
		{
			typedef std::string Type;
			typedef Integer<std::size_t, 4> Header;
			static constexpr std::size_t StaticSize = _Size;
			static constexpr std::size_t DynamicHeader = Header::StaticSize;

			static std::size_t dynamic_size(Type const& value);
			static std::size_t dynamic_parse(char const *buffer, std::size_t length, Type& value);
			static std::size_t dynamic_serialize(char *buffer, std::size_t length, Type const& value);
			static void static_parse(char const buffer[StaticSize], Type& value);
			static void static_serialize(char buffer[StaticSize], Type const& value);
			static void dynamic_parse(std::istream& stream, Type& value);
			static void dynamic_serialize(std::ostream& stream, Type const& value);
		};

		template <typename _Type, typename... _Entries>
		struct Struct
		{
			typedef _Type Type;
			typedef detail::Struct<_Type, _Entries...> Contents;
			static constexpr std::size_t StaticSize = Contents::StaticSize;
			static constexpr std::size_t DynamicHeader = Contents::DynamicHeader;

			static std::size_t dynamic_size(_Type const& value)	{ return Contents::dynamic_size(value); }
			static std::size_t dynamic_parse(char const *buffer, std::size_t length, _Type& value)	{ return Contents::dynamic_parse(buffer, length, value); }
			static std::size_t dynamic_serialize(char *buffer, std::size_t length, _Type const& value)	{ return Contents::dynamic_serialize(buffer, length, value); }
			static void static_parse(char const buffer[StaticSize], Type& value)		{ Contents::dynamic_parse(buffer, value); }
			static void static_serialize(char buffer[StaticSize], Type const& value)	{ Contents::dynamic_serialize(buffer, value); }
			static void dynamic_parse(std::istream& stream, Type& value)			{ Contents::dynamic_parse(stream, value); }
			static void dynamic_serialize(std::ostream& stream, Type const& value)	{ Contents::dynamic_serialize(stream, value); }
		};
	}

	namespace pack
	{
		template <typename _Class, typename _PackerType, typename _PackerType::Type _Class:: *_Field>
		struct Member
		{
			typedef _Class Class;
			typedef _PackerType PackerType;
			typedef typename PackerType::Type FieldType;
			static constexpr typename PackerType::Type _Class:: *Field = _Field;

			static FieldType& get(_Class& c) { return c.*Field; }
			static FieldType const& get(_Class const& c) { return c.*Field; }
		};

		template <typename _Class, typename _PackerType>
		struct Parent
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

#define MESSAGE_INTEGER(struct,field,size)	packer::pack::Member<struct, packer::type::Integer<decltype(struct::field), size>, &struct::field>
#define MESSAGE_INTEGER_NS(struct,field)	packer::pack::Member<struct, packer::type::Integer<decltype(struct::field), sizeof(struct::field)>, &struct::field>
#define MESSAGE_BOOLEAN_NS(struct,field)	packer::pack::Member<struct, packer::type::Boolean, &struct::field>
#define MESSAGE_ENUM(struct,field,size)		packer::pack::Member<struct, packer::type::Enum<decltype(struct::field), size>, &struct::field>
#define MESSAGE_ENUM_NS(struct,field)		packer::pack::Member<struct, packer::type::Enum<decltype(struct::field), sizeof(struct::field)>, &struct::field>
#define MESSAGE_STRING(struct,field,size)		packer::pack::Member<struct, packer::type::String<decltype(struct::field), size>, &struct::field>
#define MESSAGE_STRING_NS(struct,field)		packer::pack::Member<struct, packer::type::String<char[sizeof(struct::field)], sizeof(struct::field)>, &struct::field>

#define MESSAGE_STRUCT(struct,field,pckr)	packer::pack::Member<struct, pckr, &struct::field>
#define MESSAGE_STRUCT_A(struct,field,proto)	packer::pack::Member<struct, proto<decltype(struct::field)>, &struct::field>
#define MESSAGE_PARENT(struct,pckr)		packer::pack::Parent<struct, pckr>
#define MESSAGE_PARENT_A(struct,par,proto)	packer::pack::Parent<struct, proto<par>>

/***************************************
 * 
 * Implementation
 * 
 **************************************/

/*** packer::detail::StaticOnly ***/

template <typename _TypeClass, typename _Type>
std::size_t packer::detail::StaticOnly<_TypeClass, _Type>::dynamic_size(_Type const& value)
{
	return _TypeClass::StaticSize;
}

template <typename _TypeClass, typename _Type>
std::size_t packer::detail::StaticOnly<_TypeClass, _Type>::dynamic_parse(char const *buffer, std::size_t length, _Type& value)
{
	if(length != _TypeClass::StaticSize)
		throw packer::ParseError("StaticOnly: buffer has incorrect length");
	_TypeClass::dynamic_parse(buffer, value);
	return _TypeClass::StaticSize;
}

template <typename _TypeClass, typename _Type>
std::size_t packer::detail::StaticOnly<_TypeClass, _Type>::dynamic_serialize(char *buffer, std::size_t length, _Type const& value)
{
	if(length != _TypeClass::StaticSize)
		throw packer::SerializeError("StaticOnly: buffer has incorrect length");
	_TypeClass::dynamic_serialize(buffer, value);
	return _TypeClass::StaticSize;
}

template <typename _TypeClass, typename _Type>
void packer::detail::StaticOnly<_TypeClass, _Type>::dynamic_parse(std::istream& stream, _Type& value)
{
	char buffer[_TypeClass::StaticSize];
	stream.read(buffer, _TypeClass::StaticSize);
	if(!stream.good())
		throw packer::ReadError("Can't dynamic_parse: can't read from stream");
	_TypeClass::dynamic_parse(buffer, value);
}

template <typename _TypeClass, typename _Type>
void packer::detail::StaticOnly<_TypeClass, _Type>::dynamic_serialize(std::ostream& stream, _Type const& value)
{
	char buffer[_TypeClass::StaticSize];
	_TypeClass::dynamic_serialize(buffer, value);
	stream.write(buffer, _TypeClass::StaticSize);
	if(!stream.good())
		throw packer::WriteError("Can't dynamic_serialize: can't write to stream");
}


/*** packer::type::Integer ***/

template <typename _Type, std::size_t _Size>
void packer::type::Integer<_Type, _Size>::static_parse(char const buffer[StaticSize], _Type& value)
{
	Holder temp;
	bool sign = false;
	for(std::size_t k = 0; k < StaticSize; ++k)
	{
		std::size_t shift = (StaticSize - k - 1) * 8;
		unsigned char byte = buffer[k];
		if(std::is_signed<_Type>::value && (k == 0))
		{
			sign = (byte & 0x80) != 0;
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
void packer::type::Integer<_Type, _Size>::static_serialize(char buffer[StaticSize], _Type const& value)
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


/*** packer::type::String ***/

template <std::size_t _Size>
std::size_t packer::type::String<char[_Size], _Size>::dynamic_size(Type const& value)
{
	return StaticSize;
}

template <std::size_t _Size>
std::size_t packer::type::String<char[_Size], _Size>::dynamic_parse(char const *buffer, std::size_t length, Type& value)
{
	if(length < StaticSize)
		throw ParseError("String (array)::dynamic_parse(): buffer is too small");
	std::memcpy(value, buffer, StaticSize);
	return StaticSize;
}

template <std::size_t _Size>
std::size_t packer::type::String<char[_Size], _Size>::dynamic_serialize(char *buffer, std::size_t length, Type const& value)
{
	if(length < StaticSize)
		throw SerializeError("String (C): buffer is too small");
	std::memcpy(buffer, value, StaticSize);
	return StaticSize;
}

template <std::size_t _Size>
void packer::type::String<char[_Size], _Size>::static_parse(char const buffer[StaticSize], Type& value)
{
	std::memcpy(value, buffer, StaticSize);
}

template <std::size_t _Size>
void packer::type::String<char[_Size], _Size>::static_serialize(char buffer[StaticSize], Type const& value)
{
	std::memcpy(buffer, value, StaticSize);
}

template <std::size_t _Size>
void packer::type::String<char[_Size], _Size>::dynamic_parse(std::istream& stream, Type& value)
{
	stream.read(value, StaticSize);
}

template <std::size_t _Size>
void packer::type::String<char[_Size], _Size>::dynamic_serialize(std::ostream& stream, Type const& value)
{
	stream.write(value, StaticSize);
}


template <std::size_t _Size>
std::size_t packer::type::String<std::string, _Size>::dynamic_size(Type const& value)
{
	return Header::dynamic_size(value.size()) + value.size();
}

template <std::size_t _Size>
std::size_t packer::type::String<std::string, _Size>::dynamic_parse(char const *buffer, std::size_t length, Type& value)
{
	std::size_t len;
	std::size_t pos = Header::dynamic_parse(buffer, length, len);
	if(pos + len > length)
		throw ParseError("String (C++): input buffer is too small");
	value.assign(buffer + pos, len);
	return pos + len;
}

template <std::size_t _Size>
std::size_t packer::type::String<std::string, _Size>::dynamic_serialize(char *buffer, std::size_t length, Type const& value)
{
	std::size_t len = value.size();
	std::size_t pos = Header::dynamic_serialize(buffer, length, len);
	if(pos + len > length)
		throw SerializeError("String (C++): output buffer is too small");
	std::memcpy(buffer + pos, value.data(), len);
	return pos + len;
}

template <std::size_t _Size>
void packer::type::String<std::string, _Size>::static_parse(char const buffer[StaticSize], Type& value)
{
	value.assign(buffer, strnlen(buffer, StaticSize));
}

template <std::size_t _Size>
void packer::type::String<std::string, _Size>::static_serialize(char buffer[StaticSize], Type const& value)
{
	std::strncpy(buffer, value.data(), StaticSize);
}

template <std::size_t _Size>
void packer::type::String<std::string, _Size>::dynamic_parse(std::istream& stream, Type& value)
{
	std::size_t len;
	Header::dynamic_parse(stream, len);
	std::unique_ptr<char[]> b(new char[len]);
	stream.read(b.get(), len);
	value.assign(b.get(), len);
}

template <std::size_t _Size>
void packer::type::String<std::string, _Size>::dynamic_serialize(std::ostream& stream, Type const& value)
{
	Header::dynamic_serialize(stream, value.size());
	stream.write(value.data(), value.size());
}
