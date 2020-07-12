/* Invalid field selection */
package main

var a struct { n int; }

func main() {

	a.k = 3
}
