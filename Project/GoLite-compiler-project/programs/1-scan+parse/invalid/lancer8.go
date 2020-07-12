// cannot have function definition inside a function, must be top level declaration
package m
func f() {
	func b() {}
}
