import C "stdio.h"

const (
	STRING = iota
	INT
	FLOAT
	UNKNOWN
)

func print_type(t int) {
	switch t {
	case STRING:
		C.printf("This is a string\n")
	case INT:
		C.printf("This is an int\n")
	case FLOAT:
		C.printf("This is a float\n")
	default:
		C.printf("Unknown type\n")
	}
}

func print_generic_type(t int) {
	switch t {
	case STRING:
		C.printf("This is a string\n")
	case INT, FLOAT:
		C.printf("This is a number\n")
	default:
		C.printf("Unknown type\n")
	}
}

func print_less_than_3(t int) {
	switch t {
	case 0:
		C.printf("0")
		fallthrough
	case 1:
		C.printf("1")
		fallthrough
	case 2:
		C.printf("2\n")
	}
}

func main(argc int, argv **byte) int {
	for i := 0; i < 5; i++ {
		print_type(i)
	}
	for i := 0; i < 5; i++ {
		print_generic_type(i)
	}
	for i := 0; i < 5; i++ {
		print_less_than_3(i)
	}
	return 0
}
