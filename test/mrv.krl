import "stdio.h"

func getab() (byte, byte) {
	return 'a', 'b'
}

func gettwo() (byte, byte) {
	return getab()
}

func propagate(a, b byte) (byte, byte) {
	return a, b
}

func main(argc int, argv **byte) int {
	a, b := propagate(gettwo())
	stdio.printf("%c %c\n", a, b)
	return 0
}
