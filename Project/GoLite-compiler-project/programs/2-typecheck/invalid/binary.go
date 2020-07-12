//answer is not compatible with bool type in the assignment
package main

type answer bool

func main(){
	var x answer
	var y answer
	var magic bool
	magic = x && y
}
