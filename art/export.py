from PIL import Image
from random import choice, randint

TILE = (64,64)
HALF_TILE = (32,32)

def add(p,q):
	return p[0] + q[0], p[1] + q[1]

def vmul(p,q):
	return p[0] * q[0], p[1] * q[1]

def random_texture(cs, dim):
	im = Image.new("RGBA", dim)
	for y in range(im.size[1]):
		for x in range(im.size[0]):
			im.putpixel((x,y), choice(cs))
	return im

def get_pal(path):
	im = Image.open(path)
	pal = []
	for y in range(im.size[1]):
		for x in range(im.size[0]):
			pal.append(im.getpixel((x,y)))
	return pal
	


def gen_terrain():
	saveas(1, random_texture(get_pal('tundra-pal.png'), TILE), 'TERRAIN_SS')
	saveas(3, random_texture(get_pal('plains-pal.png'), TILE), 'TERRAIN_SS')
	saveas(4, random_texture(get_pal('prairie-pal.png'), TILE), 'TERRAIN_SS')
	saveas(7, random_texture(get_pal('march-pal.png'), TILE), 'TERRAIN_SS')
	
	saveas(10, random_texture(get_pal('arctic-pal.png'), TILE), 'TERRAIN_SS')
	saveas(8, random_texture(get_pal('swamp-pal.png'), TILE), 'TERRAIN_SS')
	saveas(11, random_texture(get_pal('water-pal.png'), TILE), 'TERRAIN_SS')
	
	saveas(2, random_texture(get_pal('desert-pal.png'), TILE), 'TERRAIN_SS')
	
	saveas(33, Image.open('mountain.png'))
	
	saveas(1, random_texture(get_pal('woodtile-pal.png'), (96,72)), 'WOODTILE_SS')
	saveas(1, random_texture(get_pal('sandtile-pal.png'), (96,72)), 'PARCH_SS')
	

def saveas(ind, img, cat='PHYS0_SS'):
	path = '../res/tile64/{}/{:03}.png'.format(cat, ind)
	img.save(path)	
	print(path)
	
		
def get_tile(img, posx, dim):
	pos = vmul(posx, dim)
	end = add(pos, dim)	
	return img.crop(pos + end)


def rot(img, n):
	return img.rotate(-n * 90)


def gen_coast(path):
	img = Image.open(path)
	
	dim = HALF_TILE
		
	a = get_tile(img, (0,0), dim)
	b = get_tile(img, (1,0), dim)
	c = get_tile(img, (2,0), dim)

	saveas(119, rot(a, 1))
	saveas(120, rot(a, 2))
	saveas(117, rot(a, 3))
	saveas(118, rot(a, 0))

	saveas(137, rot(c, 3))
	saveas(138, rot(c, 0))
	saveas(139, rot(c, 1))
	saveas(140, rot(c, 2))
	
	saveas(132, rot(c, 2))
	saveas(129, rot(c, 3))
	saveas(130, rot(c, 0))
	saveas(131, rot(c, 1))

	b2 = rot(b, 2)
	saveas(116, b2)
	saveas(135, b2)
	saveas(124, b2)
	saveas(127, b2)

	b3 = rot(b, 3)
	saveas(113, b3)
	saveas(136, b3)
	saveas(121, b3)
	saveas(128, b3)

	b1 = rot(b, 1)
	saveas(126, b1)
	saveas(123, b1)
	saveas(134, b1)
	saveas(115, b1)

	b0 = b
	saveas(125, b0)
	saveas(122, b0)
	saveas(133, b0)
	saveas(114, b0)
	

	
	



def combine_forest(name, img_map, unit):
	
	# read trees sprites
	img = Image.open(name, 'r')	
	J = len(img_map)
	I = len(img_map[0])
	trees = []
	for j in range(J):
		for i in range(I):
			ind = img_map[j][i]
			if ind != 0:
				x = i*unit[0]
				y = j*unit[1]				
				kw = img.crop((x,y,x+unit[0],y+unit[1]))
				
				trees.append(kw.crop(kw.getbbox()))
				#path = 'test{}{}.png'.format(i,j)
				#trees[-1].save(path)
								
	
	# num of tree positions on tile
	ntree = 12
	
	# pos,tree	
	pts = zip(
		sorted(
			[(randint(0,unit[0] - 1), randint(0,unit[1] - 1)) for _ in range(ntree)],
			cmp = lambda x,y: cmp(x[1],y[1])
		),
		[choice(trees) for _ in range(ntree)],
	)
	
	def grow_forest(tile, base_pos, neighs):
		base_end = add(base_pos, unit)
		#print(base_pos, base_end)
		
		N,E,S,W = neighs
		for pos,tree in pts:
			
			tree_dim = tree.size
			
			tree_pos = add(base_pos, add(pos, (-tree_dim[0]/2, -tree_dim[1]/2)))
			tree_end = add(tree_pos, tree_dim)
			
			# sprawdz czy nie wystaje na niezarosnietego sasiada
			if not N and tree_pos[1]+2 < base_pos[1]:
				continue
			
			if not S and tree_end[1]-2 >= base_end[1]:
				continue
							
			if not W and tree_pos[0]+2 < base_pos[0]:
				continue
				
			if not E and tree_end[0]-2 >= base_end[0]:
				continue
								
			tile.paste(tree, tree_pos, tree)
			
	
	for ind,neighs in forest_neigh:
		tile = Image.new('RGBA', (unit[0]*3,unit[1]*3), (0,0,0,0))
		
		N,E,S,W = neighs
		
		# rosnij na 5 polach w takiej kolejnosci
		#  0
		# 123
		#  4
		
		# 0
		if N:
			base = (unit[0] * 1, unit[1] * 0)
			grow_forest(tile, base, (1,E,1,W))
		
		# 1
		if W:
			base = (unit[0] * 0, unit[1] * 1)
			grow_forest(tile, base, (N,1,S,1))
			
		# 2
		base = (unit[0] * 1, unit[1] * 1)
		grow_forest(tile, base, (N,E,S,W))
		
		# 3
		if E:
			base = (unit[0] * 2, unit[1] * 1)
			grow_forest(tile, base, (N,1,S,1))
		
		# 4
		if S:
			base = (unit[0] * 1, unit[1] * 2)
			grow_forest(tile, base, (1,E,1,W))
			
		out = tile.crop((unit[0],unit[1],unit[0]*2,unit[1]*2))
		saveas(ind, out)
		
	
"""	
def extract_buildings():
	
	# build_id column-wise
	cols = [
		[1,2,3,17],    # col1
		[4,13,44,16],  # col2...
		[5,14,36,48],
		[6,15,37,47],
		[19,22,25,28,33,40,45],
		[20,23,26,29,34,41],
		[21,24,27,30,35,42],
		[7,8,9,46]
		[10,38,39,43]
	]
"""			
				

def extract(name, img_map, unit):
	img = Image.open(name, 'r')	
	J = len(img_map)
	I = len(img_map[0])
	for j in range(J):
		for i in range(I):
			ind = img_map[j][i]
			if ind != 0:
				x = i*unit[0]
				y = j*unit[1]				
				new = img.crop((x,y,x+unit[0],y+unit[1]))
				saveas(ind, new)
				
def export_blend():
	im = Image.open('blend.png')
	
	saveas(105, rot(im, 0))
	saveas(106, rot(im, 1))
	saveas(107, rot(im, 2))
	saveas(108, rot(im, 3))
	


# coast tile size HALF TILE
coast_map = [
	[119,116,135,124,127,120],
	[126,137,  0,  0,138,113],
	[123,  0,131,132,  0,136],
	[134,  0,130,129,  0,121],
	[115,140,  0,  0,139,128],
	[118,125,122,133,114,117],
]



# river tile size TILE
river_major_map = [
	[  2,  4,  7,  5, 141],
	[ 15,  6, 16, 12, 144],
	[ 13, 10, 14, 11, 143],
	[  9,  8,  3,  0, 142],
]

river_minor_map = [
	[  2+16,  4+16,  7+16,  5+16, 141+4],
	[ 15+16,  6+16, 16+16, 12+16, 144+4],
	[ 13+16, 10+16, 14+16, 11+16, 143+4],
	[  9+16,  8+16,  3+16,  0   , 142+4],
]


# forest size TILE
forest_map = [
	[  1,  1,  1,  1],
	[  1,  0,  0,  0],
	[  0,  0,  0,  0],
	[  0,  0,  0,  0],
]

# [N,E,S,W]
forest_neigh = [
	(65, (0,0,0,0)),
	(66, (0,1,0,0)),
	(67, (0,0,0,1)),
	(68, (0,1,0,1)),
	(69, (0,0,1,0)),
	(70, (0,1,1,0)),
	(71, (0,0,1,1)),
	(72, (0,1,1,1)),
	(73, (1,0,0,0)),
	(74, (1,1,0,0)),
	(75, (1,0,0,1)),
	(76, (1,1,0,1)),
	(77, (1,0,1,0)),
	(78, (1,1,1,0)),
	(79, (1,0,1,1)),
	(80, (1,1,1,1)),
]


# extract('coast.png', coast_map, HALF_TILE)

extract('river-major.png', river_major_map, TILE)
extract('river-major.png', river_minor_map, TILE)

export_blend()

combine_forest('forest.png', forest_map, TILE)

gen_coast('coast-gen.png')
gen_terrain()
