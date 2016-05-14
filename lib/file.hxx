#pragma once
#include <sys/types.h>

namespace std
{
	struct nothrow_t;
}

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

size_t Read(File &file, void *buffer, size_t bytes);
ssize_t Read(File &file, void *buffer, size_t bytes, std::nothrow_t);

size_t Write(File &file, void const *buffer, size_t bytes);
ssize_t Write(File &file, void const *buffer, size_t bytes, std::nothrow_t);

void Sync(File &file);
bool Sync(File &file, std::nothrow_t);
