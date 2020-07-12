//find all x, y satisfying ax + by = n

package main

func findSolutions(a, b, n int) int {
	var x int = 0
	var count int = 0
	for (x * a) < n {
		if (n - x * a) % b == 0 {
			count++
		}
		x++
	}
	
	if (count == 0) {
		println("No solution")
		return 0
	}
	
	return count
}

func main() {
	var result = findSolutions(2, 5, 100)
	print(result)
}
