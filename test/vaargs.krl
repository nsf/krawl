import "stdio.h"

const (
	N_END = iota
	N_ONE
	N_TWO
	N_THREE
	N_FOUR
	N_FIVE
	N_SIX
	N_SEVEN
)

func test_vaarg(msg *byte, ...) {
	stdio.printf("%s: ", msg)

	var vl va_list
	va_start(&vl)
	for {
		i := va_arg(&vl, int)
		if i == N_END {
			break
		}

		stdio.printf("%d ", i)
	}
	va_end(&vl)
	stdio.printf("\n")
}

func main(argc int, argv **byte) int {
	test_vaarg("hello", N_THREE, N_ONE, N_THREE, N_THREE, N_SEVEN, N_END)
	return 0
}
