package main

type cat struct {
 fur bool
 stripes int
}

func foo(x1 bool, x2 int, x3 int, x4, x5 float64, x6 rune, x7 float64, x8, x9 string, x10 bool, x11, x12 []int, x13 [5]float64, x14 []cat, x15 [10]cat) []cat {
 return x14 
}

func main() {
 a := 5
 var b, c []int
 var d [5]float64
 var e []cat
 var f [10]cat
 h := foo(true,
    1, 
    a + 10,
    1. + float64(a),
    1.2 * 1.2,
    'a',
    100.0,
    "s1",
    `s2`,
    1 == 2,
    b,
    c,
    d,
    e,
    f)
}


