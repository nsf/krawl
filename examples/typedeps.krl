import C "stdio.h"

func test() ([2]int, bool) {
	return {5, 10}, true
}

func main(argc int, argv **byte) int {
	a, b := test()
	C.printf("%d %d\n", a[0], a[1])
	return 0
}
