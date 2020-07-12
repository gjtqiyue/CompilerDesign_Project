/* Invalid type cast */
package main

type complex_type struct { x string; }

func main(){
	var h complex_type
	var l complex_type = complex_type(h)
}
