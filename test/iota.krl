import "stdio.h"

const (
	A = 0
	B
	C
)

const (
	RA = LAST - (iota + 1)
	RB
	RC
	RD
	RE

	LAST = iota
)

const (
	BIT1 = 1 << iota
	BIT2
	BIT3
	BIT4
	BIT5
)

const (
	ALL_BUT_BIT1 = ^(1 << iota).(uint8)
	ALL_BUT_BIT2
	ALL_BUT_BIT3
	ALL_BUT_BIT4
	ALL_BUT_BIT5
)

const (
	a = 1
	b = iota << a
	c = iota << b
	d
)

const (
	ca = iota + 'A'
	cb
	cc
	cd
	ce
)

const (
	f1 = iota.(double) * 4.5
	f2
	f3
	u1 = iota.(uint32) * 2
	u2
	u3
)

const (
	c1 = iota * iota * iota
	c2
	c3
	c4
	c5
)

func main(argc int, argv **byte) int {
	stdio.printf("%d %d %d\n", A, B, C)
	stdio.printf("%d %d %d %d %d\n", RA, RB, RC, RD, RE)
	stdio.printf("%d %d %d %d %d\n",
		     ALL_BUT_BIT1,
		     ALL_BUT_BIT2,
		     ALL_BUT_BIT3,
		     ALL_BUT_BIT4,
		     ALL_BUT_BIT5)
	stdio.printf("%d %d %d %d %d\n", BIT1, BIT2, BIT3, BIT4, BIT5)
	stdio.printf("%d %d %d %d\n", a, b, c, d)
	stdio.printf("%c %c %c %c %c\n", ca, cb, cc, cd, ce)
	stdio.printf("%f %f %f %u %u %u\n", f1, f2, f3, u1, u2, u3)
	stdio.printf("%d %d %d %d %d\n", c1, c2, c3, c4, c5)
	return 0
}
