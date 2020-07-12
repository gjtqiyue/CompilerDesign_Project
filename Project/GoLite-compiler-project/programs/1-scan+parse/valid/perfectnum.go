//To test if a input is a perfect number which means it equals to the sum of its proper positive divisors excluding itself
package a

func isPerfectNum(n int) bool {
  var sum int = 0;
  for i := 1; i < n; i++ {
    if (n % i == 0) {
			sum += i
		}
  }
  
  if sum == n {
		return 1
	} else {
		return 0
	}
}
