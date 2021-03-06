//~done
//store all prime numbers within 100000 using Sieve of Eratosthenes

package main

func floor(s float64) int {
	var i int = 0
	for float64(i) < s {
		i = i + 1
	}
	//s is not a whole number
	if !(float64(i) == s) {
		return(i-1)
	} else{
		return(i)
	}
	
}
func sqrt(x int) float64{
	var iter = 10
	var guess, quot float64 = 1.0, 0.0
	
	for iter >= 0 {
		quot = float64(x) / guess;
		guess = 0.5 * (guess + quot);
		iter = iter - 1;
	}
	return guess
}

func IsPrimeSqrt(x int) bool {
    	for i := 2; i <= floor(sqrt(x)); i++ {
        	if x%i == 0 {
           	 	return false
        	}
    	}
    return x > 1
}

func main() {
  var number int = 10000
  var list []int

  for i := 0; i <= number; i++ {
	if(IsPrimeSqrt(i)){
		list = append(list,i)
	}
  }
  print("done")
}
