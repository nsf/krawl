import "stdio.h"

func single_return() int { return 5 }
func single_return_named() (i int) { i = 10; return i }
func single_return_named2() (i int) { i = 15 }
func single_return_named3() (i int) { i = 20; return }
func multi_return() (int, double) { return 5, 3.1415 }
func multi_return_named() (a, b bool) { a, b = true, false; return a, b }
func multi_return_named2() (a int, b double) { a, b = 7, 4.525; return }
func multi_return_named3() (a double, b int8) { a, b = 7.333, 127 }

func multi_return_named_defa() (a, b, c int) { a = 1 }
func multi_return_named_defb() (a, b, c int) { b = 2 }
func multi_return_named_defc() (a, b, c int) { c = 3 }

func countA(str *byte) (count int) {
	for *str != 0 {
		if *str == 'A' {
			count++
		}
		str++
	}
}

func test_if(wantone bool) int {
	if wantone {
		return 1
	} else {
		return 0
	}
}

const (
	ONE = iota
	TWO
	THREE
	FOUR
	FIVE
)

func test_switch(what int) int {
	switch what {
	case ONE:
		return 1
	case TWO:
		return 2
	case THREE:
		return 3
	case FOUR:
		return 4
	case FIVE:
		return 5
	default:
		return 0
	}
}

func test_switch2(what int) int {
	switch what {
	case ONE:
		return 1
	case TWO:
		return 2
	case THREE:
		return 3
	case FOUR:
		return 4
	case FIVE:
		return 5
	}
	return 0
}

func main(argc int, argv **byte) int {
	stdio.printf("%d\n", single_return())
	stdio.printf("%d\n", single_return_named())
	stdio.printf("%d\n", single_return_named2())
	stdio.printf("%d\n", single_return_named3())

	{
		a, b := multi_return()
		stdio.printf("%d %f\n", a, b)
	}

	{
		a, b := multi_return_named()
		stdio.printf("%d %d\n", a, b)
	}

	{
		a, b := multi_return_named2()
		stdio.printf("%d %f\n", a, b)
	}
	{
		a, b := multi_return_named3()
		stdio.printf("%f %d\n", a, b)
	}
	{
		a1, b1, c1 := multi_return_named_defa()
		stdio.printf("%d %d %d\n", a1, b1, c1)
		a2, b2, c2 := multi_return_named_defb()
		stdio.printf("%d %d %d\n", a2, b2, c2)
		a3, b3, c3 := multi_return_named_defc()
		stdio.printf("%d %d %d\n", a3, b3, c3)
	}

	stdio.printf("%d\n", countA("AAbbccAdfA"))
	stdio.printf("if %d %d\n", test_if(true), test_if(false))
	stdio.printf("switch %d %d %d\n",
		     test_switch(ONE), test_switch(THREE), test_switch(99))
	stdio.printf("switch2 %d %d %d\n",
		     test_switch2(TWO), test_switch2(THREE), test_switch2(99))
	return 0
}
