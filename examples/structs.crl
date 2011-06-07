import C "stdio.h"

type Data struct {
	name *byte
	age int
}

type Weird struct {
	array [10]int
	inner struct {
		a, b, c int
	}
}

func main(argc int, argv **byte) int {
	var d Data = {"nsf", 23}
	var w Weird = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}, {11, 22, 33}}

	// pointer to an inner anonymous struct
	in := &w.inner
	in.a, in.b, in.c = 111, 222, 333

	// print everything
	C.printf("name: %s, age: %d\n", d.name, d.age)
	for i := 0; i < 10; i++ {
		C.printf("w.array[%d] == %d\n", i, w.array[i])
	}
	C.printf("a: %d, b: %d, c: %d\n", w.inner.a, w.inner.b, w.inner.c)
	return 0
}
