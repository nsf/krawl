import "std/string"

func SayHello() {
	s := string.FromCStr("Hello, world")
	string.Print(s)
	string.Free(s)
}

