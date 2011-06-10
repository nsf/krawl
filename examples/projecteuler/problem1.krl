import C "stdio.h"

const below = 1000

func main(argc int, argv **byte) int {
	sum := 0
	for i := 0; i < below; i++ {
		if i % 3 == 0 || i % 5 == 0 {
			sum += i
		}
	}
	C.printf("sum: %d\n", sum)
	return 0
}
