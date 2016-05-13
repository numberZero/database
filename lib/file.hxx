#pragma once

class File
{
public:
	File() = default;
	File(File const &) = delete;
	File(File &&) noexcept;
	explicit File(int s) noexcept;
	~File();

	explicit operator bool () const noexcept;
	explicit operator int () const noexcept;

	File &operator = (File const &) = delete;
	File &operator = (File&&) noexcept;

	bool operator ! () const noexcept;

	int get() const noexcept;
	void reset(int s);
	int release() noexcept;

private:
	int fd = -1;
};
