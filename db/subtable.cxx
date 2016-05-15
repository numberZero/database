#include "subtable.hxx"
// #include "dbcommon.hxx"

template class Subtable<Teacher>;
template class Subtable<Subject>;
template class Subtable<Room>;
template class Subtable<Group>;
template class Subtable<Time>;

template<typename _Object>
Subtable< _Object>::Subtable(std::string const &file):
	DataTable(file)
{
}

template<typename _Object>
Subtable< _Object>::Subtable(File && file):
	DataTable(std::move(file))
{
}

template<typename _Object>
auto Subtable< _Object>::table() -> DataTable &
{
	return *static_cast<DataTable*>(this);
}

template<typename _Object>
auto Subtable< _Object>::table() const -> DataTable const &
{
	return *static_cast<DataTable const *>(this);
}

template<typename _Object>
void Subtable< _Object>::add_row(Id row, Id to)
{
}
