import "stdlib.h"
import "string.h"
import "stdio.h"

type String struct {
	data *byte
	len, cap uint
}

func New(cap uint) (s *String) {
	s = stdlib.malloc(sizeof(type String))
	s.data = stdlib.malloc(cap + 1)
	s.cap = cap
	s.len = 0
}

func Free(s *String) {
	stdlib.free(s)
	stdlib.free(s.data)
}

func FromCStr(cstr *byte) (s *String) {
	s = stdlib.malloc(sizeof(type String))
	s.data = string.strdup(cstr)
	s.len = string.strlen(cstr)
	s.cap = s.len
}

func Print(s *String) {
	stdio.printf("%.*s\n", s.len, s.data);
}
