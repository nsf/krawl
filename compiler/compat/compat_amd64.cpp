#include "../krawl.hpp"

size_t pointer_size()
{
	return 64;
}

stype_t *va_list_structure()
{
	return new array_stype_t(builtin_stypes[BUILTIN_VOID], 24);
}
