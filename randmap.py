import struct
import random
import sys

terr_base = [10, 1, 4]
terr = [10, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4]

def main():
	fname = sys.argv[1]
	save(fname, gen(19,10))
	
def save(fname, xss):
	w = len(xss[0])
	h = len(xss)
	
	with open(fname, 'wb') as f:
		f.write(b'COLMAPB')
		f.write(struct.pack('1B', 1))
		f.write(struct.pack('2H', w, h))
	
		for j in range(h):
			for i in range(w):
				f.write(struct.pack('1B', xss[j][i]))
				
		# add units
		unit.type
		unit.color
		
				

def gen(w,h):
	xss = [[None for i in range(w)] for j in range(h)]
	
	for j in range(h):
		for i in range(w):
			xss[j][i] = 10

	for j in range(1,h-1):
		for i in range(1,w-1):
			assert j != 0
			assert j != h-1
			xss[j][i] = random.choice(terr)
			
	for _ in range(800):
		j = random.randrange(1,h-1)
		i = random.randrange(1,w-1)
		
		xss[j][i] = evo(xss, i, j)
		
	return xss
	
	

	
def evo(xss, i, j):
	k = [
		xss[j-1][i],
		xss[j+1][i],
		xss[j][i-1],
		xss[j][i-1],
		xss[j][i],
		xss[j-1][i],
		xss[j+1][i],
		xss[j][i-1],
		xss[j][i-1],
		xss[j][i],
		
		xss[j-1][i+1],
		xss[j+1][i+1],
		xss[j-1][i-1],
		xss[j+1][i-1],
	]
	print(k)
	return random.choice(list(k))
	
	

if __name__ == '__main__':
	main()
