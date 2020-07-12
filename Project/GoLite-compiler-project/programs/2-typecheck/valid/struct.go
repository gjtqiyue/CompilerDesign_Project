package m

var m struct {x int; y struct { x struct {x int;};};}

func f() {
	var a = m.y.x.x
}
