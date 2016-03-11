a = 1
b = 1.0
c = True
d = [a,b,c]
print a, b, c, d
print a + a
print a + b
print a + True
print a * False
print a * a
print a * b
print a * True
print a * False
print a * d
print a - a
print a - b
print a, a - c, b, c
print a is b
print d[0]
print True or False
print False and True
print a > b
print a > c
print a >= b
print a >= c
print a < b
print a < c
print a <= b
print a <= c
a += True
print a
a += b
print a
a -= c
print a
a -= b
print a
print a == b
print a == c
print a != b
print a != True
print a is b
print a is c
print a is d
print a is not b
print a is not c
print a is not d
print a not in d
print a in d
print (+a)
print (-a)
print a ** b
print a ** c
print None
a = 0
while a < 3:
    if a == True:
        a += 1
        print d
    elif a == False:
        a += 1
        print c
    else:
        print a
        break
for i in [1,2,3]:
    print i
a = 3.0 if True else False
b = 0.0
a = 1.0
c = []
while a < 100:
    a += 1
    if a // 2 == a / 2:
        pass
    b = 2.0
    prime = True
    while b ** 2 <= a:
        if a / b == a // b:
            prime = False
            break
        b += 1
    if prime:
        c += [a]
    else:
        continue

print c

a = b = c
print a, b, c

a = 10
b = [0,1,2]
c = 2
b[c] = a





