//~result of e^+7.000000e-001 = +2.013753e+000

// computes e^x taylor series for x = 0.7 with 1000 iterations

package main

func main (){
	var x = 0.7
	var result float64 = 0.0
	var n float64 = 0.0
	for n < 10000.0 {
		// compute x^n
		var exp, counter float64 = 1.0, 1.0
		if n == 0.0 {
			exp = 1.0
		} else{
			for counter <= n {
				exp *= float64(x)
				counter ++
			}
		}
		
		// compute n!
		var fact, iter float64 = 1.0 ,1.0
		if n == 0.0 {
			fact = 1.0
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
