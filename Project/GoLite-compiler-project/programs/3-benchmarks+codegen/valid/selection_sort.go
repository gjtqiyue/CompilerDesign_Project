//~Selection sort is finished!
//selection sort
//given an array that is populated with decreasing sequence from 123456 to 0,
//sorts the numbers in increasing order

package m

func sort(a []int, l int) {
    for i := 0; i < l; i++ {
        minValue := a[i]
        ptr := i

        for j := i+1; j < l; j++ {
            if a[j] < minValue {
                minValue = a[j]
                ptr = j
            }
        }

        if ptr != i {
            a[ptr] = a[i]
            a[i] = minValue
        }
    }
}

func main() {
    var a []int

    l := 123456

    for i := 0; i < l; i++ {
        a = append(a, l-i);
    }

    sort(a, l)

    println("Selection sort is finished!");
}
