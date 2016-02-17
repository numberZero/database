#include "db.th.hxx"

// ObjectReference

template <typename _Object>
bool ObjectReference<_Object>::isEmpty() const
{
	return !object;
}

template <typename _Object>
ObjectReference<_Object>* ObjectReference<_Object>::getNext()
{
	return object ? nullptr : next;
}

template <typename _Object>
off_t ObjectReference<_Object>::getOffset()
{
	if(!object)
		throw std::logic_error("File offset accesed in empty reference");
	return offset;
}

template <typename _Object>
_Object* ObjectReference<_Object>::get() const
{
	return object;
}

template <typename _Object>
_Object& ObjectReference<_Object>::operator*()
{
	return *object;
}

template <typename _Object>
_Object* ObjectReference<_Object>::operator->()
{
	return object;
}

// IterableTable::Iterator

template <typename _Object>
bool IterableTable<_Object>::Iterator::operator== (Iterator const &b)
{
	return ref == b.ref;
}

template <typename _Object>
bool IterableTable<_Object>::Iterator::operator!= (Iterator const &b)
{
	return ref != b.ref;
}

template <typename _Object>
typename IterableTable<_Object>::Iterator &IterableTable<_Object>::Iterator::operator++ ()
{
	++ref;
	return *this;
}

template <typename _Object>
_Object &IterableTable<_Object>::Iterator::operator* ()
{
}

template <typename _Object>
_Object *IterableTable<_Object>::Iterator::operator-> ()
{
}
