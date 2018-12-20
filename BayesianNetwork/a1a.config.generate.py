f = open("a1a.config.txt","w")
f.write("1605 124 ")
for i in range(124):
	f.write(str(i)+" ")
for i in range(124):
	f.write("1 ")
for i in range(124):
	f.write("2 ")
f.close()

f = open("a1a.config.test.txt","w")
f.write("30956 124 ")
for i in range(124):
	f.write(str(i)+" ")
for i in range(124):
	f.write("1 ")
for i in range(124):
	f.write("2 ")
f.close()