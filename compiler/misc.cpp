#include "crawlc.hpp"
#include <sys/stat.h>
#include <string.h>

const char *pretty_print_FILE(const char *file)
{
	const char *base = strstr(file, PRETTY_PRINT_FILE_BASE);
	return ((base) ? base + strlen(PRETTY_PRINT_FILE_BASE) + 1 : file);
}

std::string extract_extension(const char *path)
{
	const char *dot = strrchr(path, '.');
	if (!dot)
		return "";

	return dot+1;
}

std::string extract_stem(const char *path)
{
	const char *end = strrchr(path, '.');
	if (!end)
		end = path + strlen(path);

	const char *beg = strrchr(path, '/');
	if (!beg)
		beg = path;
	else
		beg++;

	return std::string(beg, end-beg);
}

std::string extract_dir(const char *path)
{
	const char *sep = strrchr(path, '/');
	if (sep)
		return std::string(path, sep+1 - path);

	return "";
}

std::string replace_extension(const char *path, const char *newext)
{
	const char *dot = strrchr(path, '.');
	if (!dot)
		return path;

	std::string out(path, dot+1-path);
	out.append(newext);
	return out;
}

static uint64_t u64pow(uint64_t base, uint64_t exp)
{
	uint64_t result = 1;
	while (exp) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}

static const char base62_alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz"
                                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void base62_encode64(uint64_t n, char *out)
{
	for (size_t i = 0; i < 11; ++i) {
		uint64_t rem = n % 62;
		n = (n - rem) / 62;
		out[10-i] = base62_alphabet[rem];
	}
}

uint64_t base62_decode64(const char *in)
{
	uint64_t val = 0;
	for (size_t i = 0; i < 11; ++i) {
		uint64_t num = 0;
		char c = in[i];
		if (c >= '0' && c <= '9')
			num = c - '0';
		else if (c >= 'a' && c <= 'z')
			num = c - 'a' + 10;
		else if (c >= 'A' && c <= 'Z')
			num = c - 'A' + 36;

		val += num * u64pow(62, 10-i);
	}
	return val;
}

bool read_stdin(std::vector<char> *out)
{
	size_t read_n = 0;

	while (1) {
		if (feof(stdin))
			break;
		if (ferror(stdin))
			return false;

		out->resize(read_n + 1024);
		size_t n = fread(&(*out)[read_n], 1, 1024, stdin);
		read_n += n;
		// shrink vector in case if we've read less than 1024
		if (n != 1024)
			out->resize(read_n);
	}
	return true;
}

bool read_file(std::vector<char> *out, const char *filename)
{
	struct stat st;
	FILE *f = fopen(filename, "r");
	if (!f)
		return false;

	if (-1 == fstat(fileno(f), &st)) {
		fclose(f);
		return false;
	}

	out->resize(st.st_size);
	if ((int)st.st_size != fread(&(*out)[0], 1, st.st_size, f)) {
		fclose(f);
		return false;
	}

	return true;
}

bool read_FILE(std::vector<char> *out, FILE *f)
{
	size_t read_n = 0;

	while (1) {
		if (feof(f))
			break;
		if (ferror(f))
			return false;

		out->resize(read_n + 1024);
		size_t n = fread(&(*out)[read_n], 1, 1024, f);
		read_n += n;
		// shrink vector in case if we've read less than 1024
		if (n != 1024)
			out->resize(read_n);
	}
	return true;
}
