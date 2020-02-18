import random
import string
value = random.choice(string.ascii_lowercase)
for j in range(3):
    value = ""
    for i in range(10):
        value = value + random.choice(string.ascii_lowercase)
    print(value)
    myfile = open("pyfile%d" % (j+1), "w+")
    myfile.write("%s\n" % value)
op1 = random.randrange(1,42,1)
op2 = random.randrange(1,42,1)
print(op1)
print(op2)
print("%d" % (op1*op2))
