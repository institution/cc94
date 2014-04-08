import struct
import random
import sys


TUNDRA=1
DESERT=2
PLAINS=3
PRAIRIE=4
GRASSLAND=5
SAVANNAH=6
MARSH=7
SWAMP=8
ARCTIC=10
OCEAN=11
SEALANE=12


HILL = 1;
MOUNTAIN = 2;
ROAD = 4;
MINOR_RIVER = 16;
MAJOR_RIVER = 32;
FOREST = 8;
DOCKS = 64;


pool = [OCEAN] + [TUNDRA]*2 + [PLAINS]*10



def main():
	fname = sys.argv[1]
	save(fname, gen(15,12))
	
def save(fname, xss):
	w = len(xss[0])
	h = len(xss)
	
	with open(fname, 'wb') as f:
		f.write(b'COLMAPB')
		f.write(struct.pack('1B', 1))     # num_sections (1 - terrain data only)
		f.write(struct.pack('2H', w, h))
	
		for j in range(h):
			for i in range(w):
				f.write(struct.pack('2B', xss[j][i].biome, xss[j][i].phys))
				
		# add units
		#unit.type
		#unit.color
		

class Tile: 
	def __init__(self,b,p):
		self.biome = b
		self.phys = p

def gen(w,h):
	xss = [[Tile(0,0) for i in range(w)] for j in range(h)]
	
	for j in range(h):
		for i in range(w):
			xss[j][i].biome = OCEAN

	for j in range(1,h-1):
		for i in range(1,w-1):
			assert j != 0
			assert j != h-1
			xss[j][i].biome = random.choice(pool)
			
	for _ in range(800):
		j = random.randrange(1,h-1)
		i = random.randrange(1,w-1)
		
		xss[j][i].biome = evo(xss, i, j)

	while 1:
		j = random.randrange(1,h-1)
		i = random.randrange(1,w-1)
		if xss[j][i].biome != OCEAN:
			xss[j][i].phys = MOUNTAIN
			break

	for xs in xss:
		print [(x.biome,x.phys) for x in xs]
	return xss
	
	

	
def evo(xss, i, j):
	k = [
		xss[j-1][i].biome,
		xss[j+1][i].biome,
		xss[j][i-1].biome,
		xss[j][i-1].biome,
		xss[j][i].biome,
		xss[j-1][i].biome,
		xss[j+1][i].biome,
		xss[j][i-1].biome,
		xss[j][i-1].biome,
		xss[j][i].biome,
		
		xss[j-1][i+1].biome,
		xss[j+1][i+1].biome,
		xss[j-1][i-1].biome,
		xss[j+1][i-1].biome,
	]
	r = random.choice(list(k))
	
	return r
	
	

if __name__ == '__main__':
	main()

