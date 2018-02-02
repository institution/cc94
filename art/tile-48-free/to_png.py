# render svg and blender files to png
import subprocess
import os
import xml.etree.ElementTree as ElementTree
import sys
import re

verbose = 0


def render_blend(filename_blend, filename_png):
	with open(os.devnull, "w") as devnull:
		filename_tmp = '_F837HG874W6Y54_'	
		cmd = ['blender', 
			'-b', filename_blend, 
			'-o', filename_tmp, 
			'-F', 'PNG', 
			'-x', '1', 
			'-f', '1'
		]
		subprocess.run(cmd,
			stdout=devnull,
			stderr=devnull,
		)
		os.rename(filename_tmp+'0001.png', filename_png)
		print(filename_png)
		if verbose:
			print()

def render_svg(filename_svg, filename_png, scale=None):
		
	if scale != None:
		tree = ElementTree.parse(filename_svg)
		svg = tree.getroot()
		#import ipdb; ipdb.set_trace()
		
		w = int(svg.attrib['width'])
		h = int(svg.attrib['height'])
		svg.attrib['width'] = str(w * 3)
		svg.attrib['height'] = str(h * 3)
		
		#with open(filename_tmp, 'w') as f:
		tree.write(filename_tmp)
		src_file = filename_tmp
		
	else:
		src_file = filename_svg		
	
	subprocess.run(['inkscape', '-z', src_file, '-e', filename_png])
	print(filename_png)
	if verbose:
		print()
	

def convert_dir(directory):
	for fn in os.listdir(directory):
		if verbose:
			print (fn)
		name, ext = os.path.splitext(fn)
		
		if re.match('\d\d\d', name[0:3]) != None:
			src = os.path.join(directory,fn)
			dst = os.path.join(directory,name[0:3]+'.png')
			if ext == '.svg':
				render_svg(src, dst)
			if ext == '.blend':
				render_blend(src, dst)


def main():	
	directory = sys.argv[1]
	convert_dir(directory)

if __name__ == '__main__':
	main()
