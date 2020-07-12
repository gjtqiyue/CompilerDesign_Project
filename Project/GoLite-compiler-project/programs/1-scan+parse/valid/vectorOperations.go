//Vector operations (normalize, length and sqrt)

package main

type vector struct {
	x, y, z float64
}

type line struct {
	start, end vector
}

func sqrt(num float64) float64 {
	var temp float64

	var sr = num / 2
	
	temp = sr
	sr = (temp + (num / temp)) / 2
	for (temp - sr) != 0 {
		temp = sr
		sr = (temp + (num / temp)) / 2
	}
	
	return sr
}

func dot(v1, v2 vector) float64 {
	var res float64 = 0
	res = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z
	return res
}

func length(v1 vector) float64 {
	return sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z)
}

func distance(li line) float64 {
	var temp vector
	temp.x = li.end.x - li.start.x
	temp.y = li.end.y - li.start.y
	temp.z = li.end.z - li.start.z
	return length(temp)
}

func normalize(v1 vector) vector {
	var l float64 = 0
	l = length(v1)
	var normalized vector
	normalized.x = v1.x / l
	normalized.y = v1.y / l
	normalized.z = v1.z / l
	return normalized
}

func main() {
	var v1, v2, v3 vector
	var li line
	v1.x = 5
	v1.y = 10
	v1.z = 10
	v2.x = 1
	v2.y = 1
	v2.z = 1
	v3.x = 0
	v3.y = 0
	v3.z = 0
	
	li.start = v2
	li.end = v3
	
	println(dot(v1, v2))
	println(sqrt(2))
	println(distance(li))
}
