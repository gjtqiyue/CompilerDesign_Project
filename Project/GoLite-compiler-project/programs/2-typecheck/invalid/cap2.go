//only array or slice can call cap()
package m

func f() {
	var x int
	var y = cap(x)
}
