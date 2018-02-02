import shutil
import os
import re

def copy_dir(src, dst):
	try:
		os.makedirs(dst)#, exist_ok = True)
	except OSError:
		pass
	
	for fn in os.listdir(src):
		if re.match('\d\d\d\.png', fn):
			fsrc = os.path.join(src, fn)
			fdst = os.path.join(dst, fn)
			print('cp', fsrc, fdst)
			shutil.copyfile(fsrc, fdst)



for dd in ['BUILDING_SS','PARCH_SS','TERRAIN_SS','COLONY_PIK','ICONS_SS','PHYS0_SS','WOODTILE_SS']:
	

	src = dd
	dst = os.path.join('../../res/tile-48-free/', dd)

	if os.path.exists(src):
		copy_dir(src, dst)
