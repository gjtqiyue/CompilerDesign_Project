package m

type a []int
type b a
type c b

type d struct {b int; x rune; y struct{ x rune; y int;}; }
type e d
type x []e
