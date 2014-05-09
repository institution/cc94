import io
import struct
import sys
from ctypes import c_uint8, c_uint16, c_uint32, c_int32
import sys, os
from collections import namedtuple
from PIL import Image, ImageDraw
import numpy as np
from functools import reduce 
import operator

# PIL bugfix
Image.isNumberType = lambda x: isinstance(x, (int,float))

prefix = os.path.split(__file__)[0]

def export_pallete(pal, dname):
	img = Image.new('P', (160,160), 0)
	img.putpalette(reduce(operator.add, pal))
	
	d = ImageDraw.ImageDraw(img)
	d.setfill(1)
	for j in range(16):
		for i in range(16):
			d.setink(j*16 + i)			
			d.rectangle((i*10, j*10, (i+1)*10, (j+1)*10))
		
	img.save('{}/pal.png'.format(dname))



def read_pallete(data, dname):
	pal = []
	for i in range(256):
		r,g,b = data[i*3:i*3+3]
		pal.append((r<<2,g<<2,b<<2))
		
	export_pallete(pal, dname)
		
	return pal


def get_dname(fname):
	""" Get destination file path
	"""
	#import pdb; pdb.set_trace();
	
	ddirname = os.path.basename(fname).replace('.', '_')
	
	dname = os.path.join(prefix, 'COLONIZE', ddirname)
	os.makedirs(dname, exist_ok=True)
	return dname


def get_default_pallete(fname):
	dname = get_dname('VICEROY.PAL')
	dir = os.path.dirname(fname)
	with open(dir+'/VICEROY.PAL', 'rb') as f:
		pal = read_pallete(f.read(256*3), dname)
		
	return pal


			


TileInfo = namedtuple('TileInfo', [
	'start_offset', 'length', 
	'width_padded', 'height_padded', 
	'width', 'height'
])
		
def read_tileinfos(data):	
	n = len(data)//16

	xs = []	
	for i in range(n):
		xs.append(TileInfo(*struct.unpack('IIHHHH', data[i*16:(i*16+16)])))
	return xs
		
	

PikHeader = namedtuple('Header', [
	'height', 'width', 
	'unk1', 'unk2', 	
])

def read_pik_header(data):
	assert len(data) == 8
	return PikHeader(*struct.unpack('HHHH', data))

def read_image(fmt, ti, data, pal):
	if fmt == 'PIK':	
		return read_image_pik(ti, data, pal)
	elif fmt == 'SS':
		return read_image_ss(ti, data, pal)
	elif fmt == 'FF':
		return read_image_ss(ti, data, pal)
	else:
		assert 0, fmt
	
	
def read_image_pik(ti, data, pal):
	img = Image.new('RGBA', (ti.width, ti.height), 'black')
	pix = img.load()
	
	k = ti.start_offset

	for j in range(ti.height):
		for i in range(ti.width):
			pix[i,j] = pal[data[k]]+(255,)
			k += 1
			
	return img
			
		
	
def read_image_ss(ti, data, pal):
	img = Image.new('RGBA', (ti.width, ti.height), 'black')
	pix = img.load()
	
	i = j = 0
	k = ti.start_offset
	bg = 0xFD
	
	def write_ind(ci, l=1):
		if ci == bg:
			c = pal[ci]+(0,)
		else:
			c = pal[ci]+(255,)
			
		nonlocal i
		while l > 0:
			pix[i,j] = c
			i += 1
			l -= 1
	
	def nextline():
		nonlocal i,j
		i = 0
		j += 1
	
	def read():
		nonlocal k
		r = data[k]
		k += 1
		return r
	
	
	lm = read()
	while 1:   #j < ti.height:  #k < ti.length:
		
		# line mode
		if lm == 255:
			write_ind(bg, ti.width - i)
			nextline()
			lm = read()
			
		elif lm == 252:
			break
			
		else:
			x = read()
			if x == 255:
				write_ind(bg, ti.width - i)			
				nextline()
				lm = read()			
		
			else:
				if lm == 254:
					# pix			
					if x == 254:
						l = read()
						ci = read()
						write_ind(ci, l)
					
					else:
						write_ind(x)
					
				elif lm == 253: 
					ci = read()
					write_ind(ci, x)
				
				else:
					print('ERROR: unknown lm:', lm)
					assert 0
				
	return img
	

def empty_img(w,h):
	img = []
	for j in range(h):
		img.append([None]*w)
	return img


def decode_mads_header(part):
	pass
	'''
	offset
	
	0x03 2LE palette size 
	0x26 2LE num of tiles
	0x8e 2LE unfabed image size
	0x94 2LE fabed image part size
	'''
	

def main():
	fnames = sys.argv[1:]
	for fname in fnames:	
		print('loading {}'.format(fname))
		unpack(fname)



def unpack(fname):
	# ------------------------------
	print('unpacking {}'.format(fname))
	ext = os.path.splitext(fname)[1][1:]
	dname = get_dname(fname)
	parts = unpack_mads(fname, dname)
		
	# ------------------------------
	if ext == 'PIK':	
		head = read_pik_header(parts[0])
		
		if len(parts) > 2:
			pal = read_pallete(parts[2], dname)
		else:
			pal = get_default_pallete(fname)
		
		image_part = parts[1]
	
		ti = TileInfo(
			start_offset=0, length=len(parts[1]), 
			width=head.width, height=head.height,
			width_padded=0, height_padded=0,
		)
	
		tis = [ti]
	
	elif ext == 'SS':	
		# header?
		tis = read_tileinfos(parts[1])
		pal = read_pallete(parts[2], dname)
		image_part = parts[3]
		print('{} image(s) found'.format(len(tis)))
	
	else:
		assert 0, ext
	
	# ------------------------------
	for i,ti in enumerate(tis):

		ofile = '{}/{:0>3}.png'.format(dname, i+1)
		
		print('* tile {}; width={}; height={}; path={};'.format(i+1, ti.width, ti.height, ofile))
		img = read_image(ext, ti, image_part, pal)
		
		img.save(ofile)
		



	
  

def unpack_mads(fname, dname):
	with open(fname, 'rb') as f:
		parts = list(iter_madspack(f))
	
	print('parts found in {}:'.format(fname))
	for i,p in enumerate(parts):
		print("* part {}: size={}".format(i, len(p)))
		with open('{}/part{}.b'.format(dname, i), 'wb') as f:
			f.write(p)
			
	return parts
	






def read_uint8(data):
	return data
	# return struct.unpack('<B', data)[0]

def read_uint16(data):
	return struct.unpack('<H', data)[0]

def read_uint32(data):
	return struct.unpack('<I', data)[0]

def read_int32(data):
	return struct.unpack('<i', data)[0]
	



def iter_madspack(f):
	"""
	f -- input stream
	"""
	assert f.read(8).decode('ascii') == 'MADSPACK'
	
	f.seek(14)
	_count = read_uint16(f.read(2))
	_items = []
	
	header = io.BytesIO(f.read(0xA0))
	header.seek(0)
	
	for i in range(_count):
		hash_ = read_uint16(header.read(2))
		size = read_uint32(header.read(4))
		compressed_size = read_uint32(header.read(4))
		
		if size == compressed_size:
			## no compression on this entry
			data = f.read(size)
			
		else:
			## compressed
			compressed_data = f.read(compressed_size)
			data = [None] * size			
			dsize = fab_decompress(compressed_data, data)
			if dsize != size:
				raise Exception('expecting {} bytes of data, got {}'.format(dsize, size))
			
	
		yield bytes(data)
	
	## f.pos
	

	
def fab_decompress(src, dest):
	"""	Decompress fab section
	src -- array
	dest -- array
	return -- length of decoded data
	"""
	
	if not (src[0:3].decode('ascii') == 'FAB'):
		raise Exception('fab_decode: header not found')
	
	shift_val = read_uint8(src[3])
	if not (10 <= shift_val < 14):
		raise Exception('fab_decode: invalid shift_val: {}'.format(shift_val))
	
	# uint8s
	copy_len = 0
	copy_ofs_shift = 16 - shift_val
	copy_ofs_mask = 0xFF << (shift_val - 8)
	copy_len_mask = (1 << copy_ofs_shift) - 1

	# uint32
	copy_ofs = 0xFFFF0000
	

	_bits_left = 16
	_bit_buffer = read_uint16(src[4:6])
		
	i = 6  # src pos
	j = 0  # dest pos
	
	def get_bit():
		nonlocal _bits_left, _bit_buffer, i
		
		_bits_left -= 1
		if _bits_left == 0:
			_bit_buffer = (read_uint16(src[i:i+2]) << 1) | (_bit_buffer & 1)
			i += 2
			_bits_left = 16

		bit = _bit_buffer & 1
		_bit_buffer >>= 1
		return bit
	
	
	while 1:
		if get_bit() == 0:
			if get_bit() == 0:
				copy_len = ((get_bit() << 1) | get_bit()) + 2
				copy_ofs = read_uint8(src[i]) | 0xFFFFFF00;
				i += 1
			else:
				copy_ofs = (((read_uint8(src[i+1]) >> copy_ofs_shift) | copy_ofs_mask) << 8) | read_uint8(src[i])
				copy_len = read_uint8(src[i+1]) & copy_len_mask
				i += 2
				if copy_len == 0:
					copy_len = read_uint8(src[i])
					i += 1
					if copy_len == 0:
						break
					elif copy_len == 1:
						continue	
					else:
						copy_len += 1						
				else:
					copy_len += 2
				
				copy_ofs |= 0xFFFF0000
			
			while copy_len > 0:
				
				dest[j] = dest[j + c_int32(copy_ofs).value]
				j += 1
				copy_len -= 1
			
		else:
			dest[j] = src[i]
			j += 1
			i += 1
		
	return j

		
if __name__ == '__main__':
	main()







