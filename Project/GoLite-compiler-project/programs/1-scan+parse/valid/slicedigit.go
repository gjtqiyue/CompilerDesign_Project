package main

func Slice(n int) []int {
  var slice []int;
  
  for n > 0 {
    slice = append(slice, n % 10);
    n /= 10;
  }
  return slice;
}

func main() {
  num := Slice(54321);
  
  for i:= 0; i < len(num); i++ {
    print(num[i]);
  }
}
