a = 1
b = 0.1
c = True
d = [a,c]
print a, c, d
e = [d]
d[0] = e
print d
print a
print b
print a + b
print a + True
print a * False
print a * a
print a * b
print a * True
print a * False
print a * d
print a - a
print None
print a - b
print a, a - c, b, c
print a is b
print d[0]
print True or False
print False or True
print [2] or [1]
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
print a
print b
print c
print d
print a is b
print a is c
print a is d
print a is not b
print a is not c
print a is not d
print a is d
print a not in d
print a in d
print (+a)
print (-a)
print a ** b
print a ** c
print None
a = {1:1.0,2:2.0,3:3.0}
print a[1]
print a[2]
print a[3]
a
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
print a
for i in [None, 0, True, False, 1 == 1, a is a]:
    print i
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
print a
print b
print c

d= {1:True, 10.0:False, 20:[1,1,2,3,5,8,13]}
print d[i]
print d[10.0]
print d[20]

left = [1, 2.0, True, False, 2, 0, 1.0, 0.0]
for i in left:
    for j in left:
        print
        print i
        print j
        print i + j
        print i - j
        print i * j
        if j != 0:
            print i / j
            print i // j
            print i % j
        print i ** j
        print i == j
        print i != j 
        print +i
        print -i
        print i or j
        print i and j
        print not i

print
print 
for i in left:
    for j in left:
        for k in left:
            print i < j < k
            print i < j > k
            print i < j <= k
            print i < j >= k
            print i < j == k
            print i < j != k
            print i > j < k
            print i > j > k
            print i > j <= k
            print i > j >= k
            print i > j == k
            print i > j != k
            print i <= j < k
            print i <= j > k
            print i <= j <= k
            print i <= j >= k
            print i <= j == k
            print i <= j != k
            print i >= j < k
            print i >= j > k
            print i >= j <= k
            print i >= j >= k
            print i >= j == k
            print i >= j != k
            print i == j < k
            print i == j > k
            print i == j <= k
            print i == j >= k
            print i == j == k
            print i == j != k
            print i != j < k
            print i != j > k
            print i != j <= k
            print i != j >= k
            print i != j == k
            print i != j != k

a = [0,1,2]
a[2] = a
print a
print a + a
print a in a
for i in a:
    print a * 2
    print 2 * a
do = {}
do[True] = a
do[False] = a * 2
do[1] = 3
do[None] = 9001
print do[True]
print do[False]
print do[1]
print do[None]
if a:
    pass

a = 10.0
print a // 2.7
a //=2.7
print a
