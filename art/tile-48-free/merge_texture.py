from PIL import Image
import sys, os

def marge_texture(opath, lst):
	dst = Image.new('RGBA', (2048, 2048))

	x,y = 0,0
	y1 = 0

	
	dst.paste((63,63,63,255), (0,0,dst.size[0],dst.size[1]))
	rc = []
	
	rc.append('image-path "{}"'.format(opath+'.png'))
	rc.append('')
	rc.append('sprites ""')
	rc.append('pos 0 0')
	rc.append('')
	
	for path in lst:
		name,_ = os.path.splitext(os.path.basename(path))
		
		im = Image.open(path)
		dx,dy = im.size
		
		if x + dx < 2048:
			
			dst.paste(im, (x, y))

			rc.append('sprite "{}"'.format(name))
			rc.append('pos {} {}'.format(x,y))
			rc.append('dim {} {}'.format(*im.size))
			rc.append('')
			
			y1 = max(y1, y+dy)
			x += dx
		else:
			x = 0
			y += y1
			y1 = 0

	
	dst.save(opath+'.png')
	print(opath+'.png')
	with open(opath+'.rc', 'w') as f:
		f.write('\n'.join(rc))
	print(opath+'.rc')




if __name__ == '__main__':
	opath = sys.argv[1]
	lst = sys.argv[2:]

	marge_texture(opath, lst)

