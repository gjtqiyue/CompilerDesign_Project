// Bessie Luo
// computes e^x taylor series for x = 10 with 100 iterations

package main

func main (){
	var x int = 10
	var result float64 = 0.0
	var n float64 = 0
	for n < 100 {
		// compute x^n
		var exp, counter float64 = 1, 1
		if n == 0 {
			exp = 1
		} else{
			for counter <= n {
				exp *= float64(x)
				counter ++
			}
		}
		
		// compute n!
		var fact, iter float64 = 1,1
		if n == 0 {
			fact = 1
		} else {
			for iter <= n {
				fact *= iter;
				iter ++
			}
		}
		
		//compute summation of x^n/n!
		result = result + float64(exp/fact)
		
		//increase loop counter
		n ++
	}
	print("result of e^", x , " = ", result)
}
