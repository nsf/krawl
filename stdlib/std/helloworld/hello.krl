import "std/string"

func HelloWorld() (s *string.String) {
	s = string.FromCStr("Hello, world")
}

func SayHello() {
	s := HelloWorld()
	string.Print(s)
	string.Free(s)
}
