/* Invalid increment on struct type*/
package main

type i struct { n int; }

func main(){
	var b i 
	b--
}
