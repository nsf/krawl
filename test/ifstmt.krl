import "stdio.h"

func main(argc int, argv **byte) int {
	big, small := 100, 1

	if big > small {
		stdio.printf("big > small\n")
	}

	if small > big {} else {
		stdio.printf("!(small > big)\n")
	}

	if big == small {
		stdio.printf("true\n")
	} else {
		stdio.printf("false\n")
	}

	if big != small {
		stdio.printf("true\n")
	} else {
		stdio.printf("false\n")
	}

	if big != 0 {
		if small == 1 {
			stdio.printf("all ok\n")
		}
	} else {
	}

	if big == -1 { stdio.printf("omg, no!\n") } else {
		if small != 15 {
			stdio.printf("interesting\n")
		}

		if small != 1 {} else {
			stdio.printf("very interesting\n")
		}
	}

	a, b, c := 10, 20, 30
	if big > small {
		a, b, (c) := 1, 2, 3
		stdio.printf("%d %d %d\n", a, b, c)
	}
	stdio.printf("%d %d %d\n", a, b, c)

	return 0
}
