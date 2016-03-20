#pragma once
#include "message.hxx"
#include "db/dbhelper.hxx"
#include "db/select.hxx"

static constexpr std::uint_least16_t const zolden_port = 13377;

enum class QueryType {
	Select = 1,
	Insert = 2,
	Remove = 3,
};

template <typename T>
struct NetworkType;

template <typename T, typename... E>
struct _NetworkType_S: packer::type::Struct<T, E...> {};

template <typename T, typename P, typename... E>
struct _NetworkType_P: packer::type::Struct<T, packer::pack::Parent<T, NetworkType<P>>, E...> {};

template <>
struct NetworkType<QueryType>: packer::type::Enum<QueryType, 1> {};

template <>
struct NetworkType<RowData>: _NetworkType_S<RowData
	, MESSAGE_STRING(RowData, teacher, 0)
	, MESSAGE_STRING(RowData, subject, 0)
	, MESSAGE_INTEGER(RowData, room, 2)
	, MESSAGE_INTEGER(RowData, group, 2)
	, MESSAGE_INTEGER(RowData, day, 1)
	, MESSAGE_INTEGER(RowData, lesson, 1)
> {};

template <>
struct NetworkType<Param>: _NetworkType_S<Param
	, MESSAGE_BOOLEAN_NS(Param, do_check)
	, MESSAGE_BOOLEAN_NS(Param, do_return)
	, MESSAGE_BOOLEAN_NS(Param, is_valid)
> {};

template <>
struct NetworkType<StringParam>: _NetworkType_P<StringParam, Param
	, MESSAGE_STRING(StringParam, value, 0)
> {};

template <>
struct NetworkType<IntegerParam>: _NetworkType_P<IntegerParam, Param
	, MESSAGE_INTEGER(IntegerParam, min, 4)
	, MESSAGE_INTEGER(IntegerParam, max, 4)
> {};

template <>
struct NetworkType<SelectionParams>: _NetworkType_S<SelectionParams
	, MESSAGE_STRUCT_A(SelectionParams, teacher, NetworkType)
	, MESSAGE_STRUCT_A(SelectionParams, subject, NetworkType)
	, MESSAGE_STRUCT_A(SelectionParams, room, NetworkType)
	, MESSAGE_STRUCT_A(SelectionParams, group, NetworkType)
	, MESSAGE_STRUCT_A(SelectionParams, day, NetworkType)
	, MESSAGE_STRUCT_A(SelectionParams, lesson, NetworkType)
> {};
