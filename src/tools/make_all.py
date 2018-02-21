from PIL import Image
from random import choice, randint
import random
import math, os
import to_png
from PIL import Image, ImageDraw
import re, shutil



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
	pat = Image.open(env.lp(pattern))
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

	save(oname, tex)

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
	im = Image.open(env.lp(path))
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
		self.scale = None

	def sp2(self, cat, ind, name):
		""" save path from cat+ind """
		return os.path.join(self.dst, "{}/{:03}-{}.png".format(cat, ind, name))
		
	def sp(self, path):
		""" save path """
		return os.path.join(self.dst, path)
		
	def lp(self, path):
		""" load path """
		return os.path.join(self.src, path)


def save(name, img):	
	path = env.sp(name)
	img.save(path)	
	print(path)

		
def get_tile(img, posx, dim):
	pos = vmul(posx, dim)
	end = add(pos, dim)	
	return img.crop(pos + end)


def rot(img, n):
	return img.rotate(-n * 90)


	

def gen_coast(path):
	img = Image.open(env.lp(path))
	
	dim = env.half_tile
		
	a = get_tile(img, (0,0), dim)
	b = get_tile(img, (1,0), dim)
	c = get_tile(img, (2,0), dim)

	black = Image.new('RGBA', dim)
	black.paste((0,0,0,255), (0,0,dim[0],dim[1]))
	
	save('3109-phys-coast.png', black);
	save('3110-phys-coast.png', black);
	save('3111-phys-coast.png', black);
	save('3112-phys-coast.png', black);


	save('3119-phys-coast.png', rot(a, 1))
	save('3120-phys-coast.png', rot(a, 2))
	save('3117-phys-coast.png', rot(a, 3))
	save('3118-phys-coast.png', rot(a, 0))

	save('3137-phys-coast.png', rot(c, 3))
	save('3138-phys-coast.png', rot(c, 0))
	save('3139-phys-coast.png', rot(c, 1))
	save('3140-phys-coast.png', rot(c, 2))
	
	save('3132-phys-coast.png', rot(c, 2))
	save('3129-phys-coast.png', rot(c, 3))
	save('3130-phys-coast.png', rot(c, 0))
	save('3131-phys-coast.png', rot(c, 1))

	b2 = rot(b, 2)
	save('3116-phys-coast.png', b2)
	save('3135-phys-coast.png', b2)
	save('3124-phys-coast.png', b2)
	save('3127-phys-coast.png', b2)

	b3 = rot(b, 3)
	save('3113-phys-coast.png', b3)
	save('3136-phys-coast.png', b3)
	save('3121-phys-coast.png', b3)
	save('3128-phys-coast.png', b3)

	b1 = rot(b, 1)
	save('3126-phys-coast.png', b1)
	save('3123-phys-coast.png', b1)
	save('3134-phys-coast.png', b1)
	save('3115-phys-coast.png', b1)

	b0 = b
	save('3125-phys-coast.png', b0)
	save('3122-phys-coast.png', b0)
	save('3133-phys-coast.png', b0)
	save('3114-phys-coast.png', b0)
	

	





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
	with open(env.lp(rc_name), 'r') as f:
		for line in f.readlines():
			line = line.strip()
			xs = line.split()

			if len(xs) == 0:
				continue
			
			if xs[0] == 'image-path':
				dire_rc, _ = os.path.split(rc_name)
				img = Image.open(env.lp(os.path.join(dire_rc, xs[1].strip('"')))).convert('RGBA')
				
			if xs[0] == 'sprite':
				if s != None:
					ss.append(s)
					#print(s.pos, s.dim, s.shift)

				#print(xs[1])
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
	
			
	
	
	
	
				

def extract(img_map, name, fname, unit):
	if isinstance(fname, str):
		img = Image.open(env.lp(fname), 'r')
	else:
		img = fname
		
	J = len(img_map)
	I = len(img_map[0])
	for j in range(J):
		for i in range(I):
			ind = img_map[j][i]
			if ind != 0:
				x = i*unit[0]
				y = j*unit[1]				
				new = img.crop((x,y,x+unit[0],y+unit[1]))
				save('{:04}-{}.png'.format(ind, name), new)

				
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
		
	save('3105-phys-terrain-blend.png', rot(im, 0))
	save('3106-phys-terrain-blend.png', rot(im, 1))
	save('3107-phys-terrain-blend.png', rot(im, 2))
	save('3108-phys-terrain-blend.png', rot(im, 3))
	


# coast tile size HALF TILE
coast_map = [
	[3119,3116,3135,3124,3127,3120],
	[3126,3137,  0,  0,3138,3113],
	[3123,  0,3131,3132,  0,3136],
	[3134,  0,3130,3129,  0,3121],
	[3115,3140,  0,  0,3139,3128],
	[3118,3125,3122,3133,3114,3117],
]



# river tile size TILE
river_major_map = [
	[3002, 3004, 3007, 3005, 3141],
	[3015, 3006, 3016, 3012, 3144],
	[3013, 3010, 3014, 3011, 3143],
	[3009, 3008, 3003,    0, 3142],
]

river_minor_map = [
	[  2+3016,  4+3016,  7+3016,  5+3016, 141+3004],
	[ 15+3016,  6+3016, 16+3016, 12+3016, 144+3004],
	[ 13+3016, 10+3016, 14+3016, 11+3016, 143+3004],
	[  9+3016,  8+3016,  3+3016,  0   , 142+3004],
]

road_map = [
	[3082,3083,3084,3085,3081],
	[3086,3087,3088,3089,   0],
]

# forest size TILE
forest_map = [
	[  3070,  3072,  3071,  3069],
	[  3078,  3080,  3079,  3077],
	[  3074,  3076,  3075,  3073],
	[  3066,  3068,  3067,  3065],
]

# 
hill_map = [	
	[3054, 3056, 3055, 3053],
	[3062, 3064, 3063, 3061],
	[3058, 3060, 3059, 3057],
	[3050, 3052, 3051, 3049],
]

# [N,E,S,W]
forest_neigh = [
	(3065, (0,0,0,0)),
	(3066, (0,1,0,0)),
	(3067, (0,0,0,1)),
	(3068, (0,1,0,1)),	
	(3069, (0,0,1,0)),
	(3077, (1,0,1,0)),
	(3073, (1,0,0,0)),
	(3070, (0,1,1,0)),
	(3072, (0,1,1,1)),
	(3071, (0,0,1,1)),	
	(3076, (1,1,0,1)),
	(3078, (1,1,1,0)),
	(3074, (1,1,0,0)),
	(3075, (1,0,0,1)),
	(3079, (1,0,1,1)),
	(3080, (1,1,1,1)),
]



def gen_random_texture(path_dst, path_src, dim):
	img = random_texture(get_pal(path_src), dim)
	save(path_dst, img)



def gen_tile_lines():
	# 8 lines used to draw units paths

	img = Image.new('RGBA', env.tile)
	draw = ImageDraw.Draw(img)

	h = env.half_tile[0]
	H = env.tile[0]
	
	ds = [(h,0),(H,0),(H,h),(H,H),(h,H),(0,H),(0,h),(0,0)]

	circle = sub((h,h), (env.scale, env.scale)) + add((h,h), (env.scale, env.scale))
	
	for i,(x,y) in enumerate(ds, 1):
		img.paste((0,0,0,0), (0,0) + img.size)		
		draw.line(env.half_tile + (x,y), fill = (255,255,255,255), width = env.scale*1)
		draw.ellipse(circle, fill=(255,255,255,255), outline=None)
		save('{:04}-{}.png'.format(i, 'zz-interface-path'), img)


def copy_pngs(dst, src):
	try:
		os.makedirs(env.sp(dst))#, exist_ok = True)
	except OSError:
		pass
	
	for fn in os.listdir(env.lp(src)):
		if re.match('\d\d\d\d\-.*\.png', fn):
			fsrc = env.lp(os.path.join(src, fn))
			fdst = env.sp(os.path.join(dst, fn))
			print(fdst)
			shutil.copyfile(fsrc, fdst)



def assert_dirs(*xs):
	for x in xs:
		try:
			os.makedirs(env.sp(x))
		except os.error:
			pass


env = Env()

def main():

	env.tile = (48,48)
	env.half_tile = (24,24)
	env.dst = '../../art/tile-48-free/build'
	env.src = '../../art/tile-48-free'
	env.scale = 3

	
	#assert_dirs('ICONS_SS','PHYS0_SS','TERRAIN_SS','ZZ','BUILDING_SS')
		
	extract(river_major_map, 'phys-river-major', 'phys/tile-river-major.png', env.tile)
	extract(river_minor_map, 'phys-river-minor', 'phys/tile-river-major.png', env.tile)

	gen_terrain_blend_mask()

	gen_coast('phys/tile-coast.png')

	gen_random_texture('4001-terrain-tundra.png', 'terrain/tundra-pal.png', env.tile)
	gen_random_texture('4002-terrain-dessert.png', 'terrain/desert-pal.png', env.tile)
	gen_random_texture('4003-terrain-plains.png', 'terrain/plains-pal.png', env.tile)
	gen_random_texture('4004-terrain-prairie.png', 'terrain/prairie-pal.png', env.tile)
	gen_patch_texture("4005-terrain-grassland.png", "terrain/grassland-pat.png", env.tile)
	gen_patch_texture("4006-terrain-savannah.png", "terrain/savannah-pat.png", env.tile)
	gen_random_texture('4007-terrain-march.png', 'terrain/march-pal.png', env.tile)
	gen_random_texture('4008-terrain-swamp.png', 'terrain/swamp-pal.png', env.tile)
	gen_random_texture('4010-terrain-arctic.png', 'terrain/arctic-pal.png', env.tile)
	gen_random_texture('4011-terrain-water.png', 'terrain/water-pal.png', env.tile)

	bgdim = vmul((32,24), (env.scale, env.scale))
	gen_random_texture('0020-zz-woodtile-woodtile.png', 'zz/woodtile-pal.png', bgdim)
	gen_random_texture('0021-zz-parch-sandtile.png', 'zz/sandtile-pal.png', bgdim)

	gen_tile_lines()

	test_plant_sprites()

	img = plant_sprites_forest(load_sprites("phys/forest/trees.rc"), gen_separated_points(20))
	extract(forest_map, 'phys-forest' , img, env.tile)
	img.save(env.lp('build/phys-forest.png'))

	#img = plant_sprites_forest(load_sprites("PHYS0_SS/hill/hills.rc"), gen_separated_points(10))
	#extract('PHYS0_SS', hill_map, img, env.tile)
	#img.save(env.lp('tmp/phys-hill.png'))


	to_png.convert_dir(env.sp(""), env.lp("building"))
	to_png.convert_dir(env.sp(""), env.lp("icon"))
	to_png.convert_dir(env.sp(""), env.lp("phys"))

	extract(road_map, 'phys-road', 'phys/tile-road.png', add(env.tile, (8,8)))

	copy_pngs('', 'icon')
	copy_pngs('', 'building')
	copy_pngs('', 'phys')
	copy_pngs('', 'terrain')
	copy_pngs('', 'zz')



if __name__ == '__main__':
	main()
