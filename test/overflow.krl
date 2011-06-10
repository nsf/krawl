import "stdio.h"

func testadd(i1, i2, i3 int8) int {
	return i1 + i2 + i3
}

func main(argc int, argv **byte) int {
	{
		var a uint8 = 250
		var b uint16 = a + 10
		stdio.printf("%d\n", b)
	}

	{
		var a, b, c int8 = 120, 120, 120
		d := testadd(a, b, c)
		stdio.printf("%d\n", d)
	}
	{
		var a, b uint8 = 250, 200
		avg := (a + b) / 2
		stdio.printf("%d %d\n", avg, sizeof(avg))
	}
	return 0
}
