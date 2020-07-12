// multiple default case in switch
package m
func f(){
	switch {
		case x < 7: f1()
		default: f2()
		default: f3()
	}
}
