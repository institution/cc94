from PIL import Image
import sys, os, re


def merge_texture(obasename, idir):
	"""
	optput:
		basename.rc
		basename.png
		basename.hpp
	"""
	dst = Image.new('RGBA', (2048, 2048))

	x,y = 0,0
	y1 = 0

	
	dst.paste((63,63,63,255), (0,0,dst.size[0],dst.size[1]))
	rc = []

	#rc.append('texture {}'.format(texture_id))
	#rc.append('image-path "{}"'.format(opath+'.png'))
	#rc.append('')
	
	for path in lst:
		name,_ = os.path.splitext(os.path.basename(path))
		if not re.match("\d\d\d\d.*", name):
			print("skipping", name);
			continue
			
		id = int(name[:4])
		desc = name[5:]
	
		im = Image.open(path)
		dx,dy = im.size

		# if cannot fit in current line than move to the next one
		if not (x + dx < 2048):
			x = 0
			y = y1
			y1 = 0

		dst.paste(im, (x, y))

		rc.append('sprite {}'.format(id))
		rc.append('name "{}"'.format(name))
		rc.append('box {} {} {} {}'.format(x,y,*im.size))
		rc.append('')
		
		y1 = max(y1, y+dy)
		x += dx
			
		
	
	dst.save(opath+'.png')
	print(opath+'.png')
	with open(opath+'.rc', 'w') as f:
		f.write('\n'.join(rc))
	print(opath+'.rc')




if __name__ == '__main__':
	opath = sys.argv[1]
	lst = sys.argv[2:]

	merge_texture(opath, lst)

