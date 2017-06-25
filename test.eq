"g = a + b + avg(c, d, e) - f"
"h = g + 13.2"
"i = - avg(avg(a, b, h), avg(a, c, h), (13.2. - i))"
"j = 1 - i"

acc[0] <- a, b +
acc[0] <- acc[0], c, d, e avg() +
acc[0] <- acc[0], f -

acc[0] <- g, 13.2 +

acc[0] <- b, h avg()
acc[1] <- a, h avg()
acc[0] <- acc[0], acc[1], avg() neg()

1, i -

a = 1
b = 2
c = 3
d = 4
e = 5
f = 6
a, b, +, c, d, e, avg(), +, f, -               
g, 13.2, +
a, b, h, avg(), a, c, h, avg(), 13.2, i, -, avg(), neg()
1, i, -

push a -> stack[0]
push b -> stack[1]
add  stack[0], stack[1] -> stack[2]
push c -> stack[3]
push d -> stack[4]
push e -> stack[5]
avg  stack[2], stack[4], stack[5] -> stack[6]
add  stack[2], stack[7] -> stack[7]
push f -> stack[8]
sub  stack[7], stack[8] -> stack[9]

push b -> stack[0]
push h -> stack[1]
avg  stack[1], stack[0] -> stack[3]
push a -> stack[4]
push h -> stack[5]
avg  stack[4], stack[5] -> stack[6]
avg  stack[4], stack[6] -> stack[7]
neg  stack[8] -> stack[9]

push a -> stack[0]
push b -> stack[1]
add  stack[0], stack[1] -> stack[0]
push c -> stack[1]
push d -> stack[2]
push e -> stack[3]
avg  stack[1], stack[2], stack[3] -> stack[1]
add  stack[0], stack[1] -> stack[0]
push f -> stack[1]
sub  stack[0], stack[1] -> stack[0]

push a -> stack[0]
push b -> stack[1]
push h -> stack[2]
avg  stack[0], stack[1], stack[2] -> stack[0]  //nArgs = 3
push a -> stack[1]
push c -> stack[2]
push h -> stack[3]
avg  stack[1], stack[2], stack[3] -> stack[1]  //nArgs = 3
push 13.2 -> stack[2]
push i -> stack[3]
sub  stack[2], stack[3] -> stack[2]            //nArgs = 2
avg  stack[0], stack[1], stack[2] -> stack[0]
neg  stack[0] -> stack[0]

