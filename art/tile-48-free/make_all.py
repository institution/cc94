from PIL import Image
from random import choice, randint
import random
import math
import to_png



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



def sub(a,b):
	return (a[0]-b[0], a[1]-b[1])

def add(a,b):
	return (a[0]+b[0], a[1]+b[1])
	
	
def fcolor_diff(a,b):
	""" Return color difference
	return -- 1.0 max diff, 0.0 min diff
	"""
	t = abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])
	return t/3.0
	
def make_fcolor(c):
	#print (c)
	#a = (c >> 0) & int('11', 2)
	#b = (c >> 2) & int('11', 2)
	#g = (c >> 4) & int('11', 2)
	#r = (c >> 6) & int('11', 2)
	r,g,b,a = c
	return (r/255, g/255, b/255, a/255)
	
MASK_ARRAY = [[1,1,1],[1,0,1],[1,1,1]]
MASK_OFFSET = (1,1)

class Mask:
	def __init__(self, mask, off):
		self.mask = mask
		self.off = off
		self.dim = (len(mask[0]), len(mask))

	def calc_simm(self, im0, q0, im1, q1):
		""" Calc similarity score beetween points 'q0 on 'im0 and
			q1 on im1 according to 'self.mask		
		"""
		
		# left-top corners
		p0 = sub(q0, self.off)
		p1 = sub(q1, self.off)
		
		# iterate over mask, add similarity
		sim = 0
		for j in range(self.dim[1]):
			for i in range(self.dim[0]):				
				mask_value = self.mask[j][i]
				
				#ff = 
				#import ipdb; ipdb.set_trace()
				c0 = make_fcolor(im0.getpixel( add(p0,(i,j)) ))
				c1 = make_fcolor(im1.getpixel( add(p1,(i,j)) ))
				
				weight = mask_value * c0[3] * c1[3]				
				sim += (1.0 - fcolor_diff(c0, c1)) * weight
		
		return sim




def gen_patch_texture(oname, pattern, dim):
	"""
	pattern -- filename
	oname -- output filename
	dim -- new texture size
	"""
	
	mask = Mask(MASK_ARRAY, MASK_OFFSET)
	
	dx,dy = dim
	
	# source pattern in RGBA
	pat = Image.open(pattern)
	src = Image.new("RGBA", pat.size)
	src.paste(pat)
	
	# destination image x 3x3
	dst = Image.new("RGBA", (dx*3, dy*3))
	
	# wholy transparent	
	dst.paste((0,0,0,0), (0,0,dx*3, dy*3))
	
	# points to fill in random order
	dst_pts = []
	for j in range(dy, dy+dy):
		for i in range(dx, dx+dx):
			dst_pts.append((i,j))
	random.shuffle(dst_pts)
	

	# src points range
	x0 = mask.off[1]
	x1 = src.size[1] - (mask.dim[1] - mask.off[1] - 1)
	y0 = mask.off[0]
	y1 = src.size[0] - (mask.dim[0] - mask.off[0] - 1)

		
	# fill
	progress = 0
	last_proc = -1000
	for d in dst_pts:	
		
		# show progress
		proc = int(progress*100/len(dst_pts))
		if proc - last_proc >= 20:
			print(proc, '%')
			last_proc = proc
		progress += 1
				
		# tournament select	most similar src point
		tournament_size = ((x1-x0) * (y1-y0)) // 5
		best_fit = float("-inf")
		src_pt = (-1,-1)
		for _ in range(tournament_size):
			s = (random.randint(x0,x1-1), random.randint(y0,y1-1))
			fit = mask.calc_simm(src, s, dst, d)
			if fit > best_fit:
				best_fit = fit
				src_pt = s
		
		# paste source pixel to dst
		paste3x3(dst, d, src.getpixel(src_pt))
			
	tex = dst.crop((dx,dy,dx+dx,dy+dy))
	tex.save(oname)
	
	print(oname)

def paste3x3(dst, pos, c):
	x,y = pos
	dx, dy = dst.size[0]//3, dst.size[1]//3
	dst.paste(c, (x-dx,y-dy, x-dx+1,y-dy+1))
	dst.paste(c, (x,y-dy, x+1,y-dy+1))
	dst.paste(c, (x+dx,y-dy, x+dx+1,y-dy+1))
	
	dst.paste(c, (x-dx,y, x-dx+1,y+1))
	dst.paste(c, (x,y, x+1,y+1))
	dst.paste(c, (x+dx,y, x+dx+1,y+1))
	
	dst.paste(c, (x-dx,y+dy, x-dx+1,y+dy+1))
	dst.paste(c, (x,y+dy, x+1,y+dy+1))
	dst.paste(c, (x+dx,y+dy, x+dx+1,y+dy+1))
	

def get_pal(path):
	im = Image.open(path)
	pal = []
	for y in range(im.size[1]):
		for x in range(im.size[0]):
			pal.append(im.getpixel((x,y)))
	return pal
	



class Env: 
	def __init__(self):
		self.dst = None
		self.src = None
		self.tile = None
		self.half_tile = None

	def sp2(self, cat, ind):
		""" save path from cat+ind """
		return os.path.join(self.dst, "{}/{}.png".format(cat, ind))
		
	def sp(self, path):
		""" save path """
		return os.path.join(self.dst, path)
		
	def lp(self, path):
		""" load path """
		return os.path.join(self.src, path)


def save(cat, ind, img):	
	path = env.sp2(cat, ind)
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
	
	dim = env.half_tile
		
	a = get_tile(img, (0,0), dim)
	b = get_tile(img, (1,0), dim)
	c = get_tile(img, (2,0), dim)

	black = Image.new('RGBA', dim)
	black.paste((0,0,0,255), (0,0,dim[0],dim[1]))
	
	save('PHYS0_SS', 109, black);
	save('PHYS0_SS', 110, black);
	save('PHYS0_SS', 111, black);
	save('PHYS0_SS', 112, black);


	save('PHYS0_SS', 119, rot(a, 1))
	save('PHYS0_SS', 120, rot(a, 2))
	save('PHYS0_SS', 117, rot(a, 3))
	save('PHYS0_SS', 118, rot(a, 0))

	save('PHYS0_SS', 137, rot(c, 3))
	save('PHYS0_SS', 138, rot(c, 0))
	save('PHYS0_SS', 139, rot(c, 1))
	save('PHYS0_SS', 140, rot(c, 2))
	
	save('PHYS0_SS', 132, rot(c, 2))
	save('PHYS0_SS', 129, rot(c, 3))
	save('PHYS0_SS', 130, rot(c, 0))
	save('PHYS0_SS', 131, rot(c, 1))

	b2 = rot(b, 2)
	save('PHYS0_SS', 116, b2)
	save('PHYS0_SS', 135, b2)
	save('PHYS0_SS', 124, b2)
	save('PHYS0_SS', 127, b2)

	b3 = rot(b, 3)
	save('PHYS0_SS', 113, b3)
	save('PHYS0_SS', 136, b3)
	save('PHYS0_SS', 121, b3)
	save('PHYS0_SS', 128, b3)

	b1 = rot(b, 1)
	save('PHYS0_SS', 126, b1)
	save('PHYS0_SS', 123, b1)
	save('PHYS0_SS', 134, b1)
	save('PHYS0_SS', 115, b1)

	b0 = b
	save('PHYS0_SS', 125, b0)
	save('PHYS0_SS', 122, b0)
	save('PHYS0_SS', 133, b0)
	save('PHYS0_SS', 114, b0)
	

	





class Sprite:
	
	def __init__(self, txt='default', img=None, pos=None, dim=None, shift=None):
		self.txt = txt
		self.img = img
		self.pos = pos
		self.dim = dim
		self.shift = shift
	
		

def load_sprites(rc_name):
	"""
	image-path "fonttiny.png"

	sprite "FONTTINY.FF.032"
	pos 0 0
	dim 2 6
	"""
	img = None
	ss = []
	s = None
	with open(rc_name, 'r') as f:
		for line in f.readlines():
			line = line.strip()
			xs = line.split()

			if len(xs) == 0:
				continue
			
			if xs[0] == 'image-path':
				img = Image.open(xs[1]).convert('RGBA')
				
			if xs[0] == 'sprite':
				if s != None:
					ss.append(s)
					print(s.pos, s.dim, s.shift)

				print(xs[1])
				s = Sprite()
				s.img = img
				
				
			if xs[0] == 'pos':
				s.pos = int(xs[1]), int(xs[2])
				
			if xs[0] == 'dim':
				s.dim = int(xs[1]), int(xs[2])
				
			if xs[0] == 'shift':
				s.shift = int(xs[1]), int(xs[2])

		ss.append(s)
		print(s.pos, s.dim, s.shift)
		
	return ss
			
			
# collide if point strictly inside box, dont collide when on box border
def collide_point(ab, p):
	"""
	ab -- (a,b) a is left-top iside pixel, b is after bottom-right [+(1,1) pixel]
	p -- (x,y) point is located in left-top corner of this pixel
	p must be strictly inside ab to collide
	"""
	A,B = ab
	if (A[0] < p[0] < B[0]) and (A[1] < p[1] < B[1]):
		return True
	else:
		return False

# collide if line strictly inside box, dont collide when on box border
def collide_line_cross(ab, p):	
	"""
	ab -- (a,b) a is left-top iside pixel, b is after bottom-right [+(1,1) pixel]
	p -- (x,y) point is located in left-top corner of this pixel
	line passing throu p must be strictly inside ab to collide
	"""
	A,B = ab
	if (A[0] < p[0] < B[0]) or (A[1] < p[1] < B[1]):
		return True
	else:
		return False
	


def range2d(xx,yy):
	for x in range(xx):
		for y in range(yy):
			yield (x,y)


def test_plant_sprites():
	im = Image.new('RGBA', env.tile)
	im.paste((0,255,0,255), (0,0,48,48))
	
	s = Sprite()
	s.img = im
	s.pos=(0,0)
	s.dim=(48,48)
	s.shift=(24,48)
	
	r = plant_sprites_forest(sprites=[s], points=[(24,48)])
	for xy in range2d(*r.size):
		assert r.getpixel(xy) == (0,255,0,255)

		


def gen_random_points(N):
	points = []
	for _ in range(N):
		points.append((
			random.randint(0, env.tile[0]),
			random.randint(0, env.tile[1])			
		))
	return points



def dist2(a,b):
	dx = a[0]-b[0]
	dy = a[1]-b[1]
	return dx*dx + dy*dy

def min_dist2(xs, y):
	md = 1000000
	for x in xs:
		d = dist2(y,x)
		if d and d < md:
			md = d
	return md

def gen_separated_points(N):
	points = gen_random_points(N*2*2)

	dists = {}
	for p in points:
		dists[p] = min_dist2(points, p)

	points = sorted(points, key = lambda p: dists[p])

	points = points[N*2:]

	dists = {}
	for p in points:
		dists[p] = min_dist2(points, p)

	points = sorted(points, key = lambda p: dists[p])

	points = points[N:]

	return points
	
	
def plant_sprites_forest(sprites, points):
	
	# create 4x4
	img = Image.new('RGBA', vmul(env.tile, (4, 4)))
	
	# generate sps (sprite, point) for one tile
	sps = [(random.choice(sprites), points[i]) for i in range(len(points))]
		

	#sps[0] = (sprites[0],(48-12,48))
	#pos 12 11
	#dim 24 28
	#shift 12 28

	# replicate sps 4x4 times
	sps2 = []
	for t in range2d(4,4):
		for s, p in sps:
			sps2.append((s, add(vmul(t, env.tile), p)))
			
	# remove from sps2 offending entries
	sps3 = []
	for spr, ptr in sps2:
		A = sub(ptr, spr.shift)
		B = add(A, spr.dim)
		ab = A,B
		
		collide = any([
			collide_line_cross(ab, vmul(env.tile, (0,0))),	
			collide_line_cross(ab, vmul(env.tile, (3,3))),
			collide_line_cross(ab, vmul(env.tile, (4,4))),
			collide_point(ab, vmul(env.tile, (1,1))),
			collide_point(ab, vmul(env.tile, (1,2))),
			collide_point(ab, vmul(env.tile, (2,1))),
			collide_point(ab, vmul(env.tile, (2,2)))
		])
		
		if not collide:
			sps3.append((spr, ptr))

	# sort
	sps3 = sorted(sps3, key = lambda sp: sp[1][1] * 10000 + sp[1][0])
		
	# render
	for spr, ptr in sps3:
		A = sub(ptr, spr.shift)
		box = spr.pos + add(spr.pos, spr.dim)
		
		tmp = Image.new('RGBA', img.size)
		tmp.paste(spr.img.crop(box), A)
		img.alpha_composite(tmp)
		
		#img.paste(su, A + add(A,spr.dim))

	return img
	
			
	
	
	
	
				

def extract(cat, img_map, name, unit):
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
				save(cat, ind, new)

				
def gen_terrain_blend_mask():
	dim = env.tile
	im = Image.new('RGBA', dim)
	
	h = dim[1] * 3.0/16.0
	n = dim[0]/2	
	a = math.pow(n, 1/h)
		
	for y in range(int(h)):
		xs = [x for x in range(dim[0])]
		for _ in range(int(n)):
			x = random.choice(xs)
			im.paste((0,0,0,255), (x,y,x+1,y+1))
			xs.remove(x)
		
		n = n/a
		#print(n)
		
	save('PHYS0_SS', 105, rot(im, 0))
	save('PHYS0_SS', 106, rot(im, 1))
	save('PHYS0_SS', 107, rot(im, 2))
	save('PHYS0_SS', 108, rot(im, 3))
	


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
	[  70,  72,  71,  69],
	[  78,  80,  79,  77],
	[  74,  76,  75,  73],
	[  66,  68,  67,  65],
]

# [N,E,S,W]
forest_neigh = [
	(65, (0,0,0,0)),
	(66, (0,1,0,0)),
	(67, (0,0,0,1)),
	(68, (0,1,0,1)),	
	(69, (0,0,1,0)),
	(77, (1,0,1,0)),
	(73, (1,0,0,0)),
	(70, (0,1,1,0)),
	(72, (0,1,1,1)),
	(71, (0,0,1,1)),	
	(76, (1,1,0,1)),
	(78, (1,1,1,0)),
	(74, (1,1,0,0)),
	(75, (1,0,0,1)),
	(79, (1,0,1,1)),
	(80, (1,1,1,1)),
]



def gen_random_texture(path_dst, path_src, dim):
	img = random_texture(get_pal(path_src), dim)
	img.save(path_dst)
	print(path_dst)
	

env = Env()

def main():

	env.tile = (48,48)
	env.half_tile = (24,24)
	env.dst = '.'
	env.src = '../../res/tile-48-free'
		
	extract('PHYS0_SS', river_major_map, 'PHYS0_SS/tile-river-major.png', env.tile)
	extract('PHYS0_SS', river_minor_map, 'PHYS0_SS/tile-river-major.png', env.tile)

	gen_terrain_blend_mask()

	gen_coast('PHYS0_SS/coast-gen.png')

	gen_random_texture('TERRAIN_SS/001.png', 'TERRAIN_SS/tundra-pal.png', env.tile)
	gen_random_texture('TERRAIN_SS/002.png', 'TERRAIN_SS/desert-pal.png', env.tile)
	gen_random_texture('TERRAIN_SS/003.png', 'TERRAIN_SS/plains-pal.png', env.tile)
	gen_random_texture('TERRAIN_SS/004.png', 'TERRAIN_SS/prairie-pal.png', env.tile)
	gen_patch_texture("TERRAIN_SS/005.png", "TERRAIN_SS/grassland-pat.png", env.tile)
	gen_patch_texture("TERRAIN_SS/006.png", "TERRAIN_SS/savannah-pat.png", env.tile)
	gen_random_texture('TERRAIN_SS/007.png', 'TERRAIN_SS/march-pal.png', env.tile)
	gen_random_texture('TERRAIN_SS/008.png', 'TERRAIN_SS/swamp-pal.png', env.tile)
	gen_random_texture('TERRAIN_SS/010.png', 'TERRAIN_SS/arctic-pal.png', env.tile)
	gen_random_texture('TERRAIN_SS/011.png', 'TERRAIN_SS/water-pal.png', env.tile)
	
	gen_random_texture('WOODTILE_SS/001.png', 'WOODTILE_SS/woodtile-pal.png', (96,72))
	gen_random_texture('PARCH_SS/001.png', 'PARCH_SS/sandtile-pal.png'), (96,72))


	test_plant_sprites()
	img = plant_sprites_forest(load_sprites("lib/map-tree.rc"), gen_separated_points(50))
	img.save('phys-forest.png')
	extract('PHYS0_SS', forest_map, 'phys-forest.png', env.tile)

	to_png.convert_dir("BUILDING_SS")
	to_png.convert_dir("ICONS_SS")





if __name__ == '__main__':
	main()
