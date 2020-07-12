//cap returns an int, cannot be assigned to string
package main

func main(){
	var s []string
	var k string = cap(s)
}
