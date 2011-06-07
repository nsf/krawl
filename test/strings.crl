import "stdio.h"
import "stdlib.h"
import "string.h"

type String struct {
	data *byte
	len, cap uint
}

func StringNew(cap uint) (s *String) {
	s = stdlib.malloc(sizeof(type String))
	s.len = 0
	s.cap = cap
	s.data = stdlib.malloc(cap + 1)
	s.data[0] = 0
}

func StringFree(s *String) {
	stdlib.free(s.data)
	stdlib.free(s)
}

func StringFromCStr(cstr *byte) (s *String) {
	s = StringFromCStrLen(cstr, string.strlen(cstr).(uint))
}

func StringFromCStrLen(cstr *byte, len uint) (s *String) {
	s = stdlib.malloc(sizeof(type String))
	s.len = len
	s.cap = len
	s.data = stdlib.malloc(s.cap + 1)
	if len != 0 {
		string.memcpy(s.data, cstr, len)
	}
	s.data[len] = 0
}

func StringPrintf(fmt *byte, ...) (s *String) {
	var va va_list
	va_start(&va)
	len := stdio.vsnprintf(nil, 0, fmt, va).(uint)
	va_end(&va)

	s = stdlib.malloc(sizeof(type String))
	s.len = len
	s.cap = len
	s.data = stdlib.malloc(s.cap + 1)
	va_start(&va)
	stdio.vsnprintf(s.data, len+1, fmt, va)
	va_end(&va)
}

func main(argc int, argv **byte) int {
	s := StringNew(0)
	stdio.printf("len: %d, cap: %d, data: '%s'\n",
		     s.len, s.cap, s.data)
	StringFree(s)

	s = StringFromCStr("hello, nsf")
	stdio.printf("len: %d, cap: %d, data: '%s'\n",
		     s.len, s.cap, s.data)
	StringFree(s)

	s = StringPrintf("Hello: %d, %f", 333, 3.1415.(double))
	stdio.printf("len: %d, cap: %d, data: '%s'\n",
		     s.len, s.cap, s.data)
	StringFree(s)
	return 0
}
