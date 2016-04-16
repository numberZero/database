#include "misc.hxx"
#include "struct.hxx"

/*** StringParam ***/

void StringParam::refine(StringParam const &b)
{
	do_return |= b.do_return;
	if(!b.do_check)
		return;
	if(do_check)
	{
		if(value != b.value)
			is_valid = false;
	}
	else
	{
		do_check = true;
		value = b.value;
	}
}

bool StringParam::check(char const *data) const
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return value == data;
}

void StringParam::set(std::string const &_value)
{
	do_check = true;
	is_valid = true;
	value = _value;
}

/*** IntegerParam ***/

void IntegerParam::refine(IntegerParam const &b)
{
	do_return |= b.do_return;
	if(!b.do_check)
		return;
	if(do_check)
	{
		min = std::max(min, b.min);
		max = std::min(max, b.max);
	}
	else
	{
		do_check = true;
		min = b.min;
		max = b.max;
	}
	is_valid = min <= max;
}

bool IntegerParam::check(long int data) const
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return (min <= data) && (data <= max);
}

void IntegerParam::set(long _value)
{
	set(_value, _value);
}

void IntegerParam::set(long _min, long _max)
{
	do_check = true;
	is_valid = true;
	min = _min;
	max = _max;
}

void IntegerParam::set(std::string const &_value)
{
	std::size_t pos = _value.find('-');
	if(pos == std::string::npos)
		set(std::stol(_value));
	else
		set(std::stol(_value.substr(0, pos)), std::stol(_value.substr(pos + 1)));
}

/*** BooleanParam ***/

void BooleanParam::refine(BooleanParam const &b)
{
	do_return |= b.do_return;
	if(!b.do_check)
		return;
	if(do_check)
		is_valid = value == b.value;
	do_check = true;
	value = b.value;
}

bool BooleanParam::check(bool data) const
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return value == data;
}

void BooleanParam::set(bool _value)
{
	do_check = true;
	is_valid = true;
	value = _value;
}

void BooleanParam::set(std::string const &_value)
{
	set(parseBoolean(_value));
}

/*** SelectionParams ***/

void SelectionParams::refine(SelectionParams const &b)
{
	teacher.refine(b.teacher);
	subject.refine(b.subject);
	room.refine(b.room);
	group.refine(b.group);
	day.refine(b.day);
	lesson.refine(b.lesson);
}

void SelectionParams::clearReturn()
{
	teacher.do_return = false;
	subject.do_return = false;
	room.do_return = false;
	group.do_return = false;
	day.do_return = false;
	lesson.do_return = false;
}

bool SelectionParams::isValid() const
{
	return
		teacher.is_valid &&
		subject.is_valid &&
		room.is_valid &&
		group.is_valid &&
		day.is_valid &&
		lesson.is_valid;
}
