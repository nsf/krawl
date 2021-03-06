import C "stdio.h"

func fib(n int) int {
	a, b := 1, 2
	res := 0

	for b <= n {
	    if b % 2 == 0 {
	       res += b
	    }
	    a, b = b, a + b
	}

	return res
}

func main(argc int, argv **byte) int {
	lim := 4000000
	C.printf("%d\n", fib(lim))
	return 0
}
