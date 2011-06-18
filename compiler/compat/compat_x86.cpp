#include "../krawl.hpp"

size_t pointer_size()
{
	return 32;
}

stype_t *va_list_structure()
{
	return new pointer_stype_t(builtin_stypes[BUILTIN_VOID]);
}
