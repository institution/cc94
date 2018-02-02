import os


'BUILDING_SS'
gen svg

'PARCH_SS'
gen texture

'TERRAIN_SS'
gen texture

'COLONY_PIK'
copy

'ICONS_SS'
gen blend
gen svg
copy

'PHYS0_SS'
gen terrain

'WOODTILE_SS'
gen texture

'ZZ'
copy


for dd in ['BUILDING_SS','PARCH_SS','TERRAIN_SS','COLONY_PIK','ICONS_SS','PHYS0_SS','WOODTILE_SS', 'ZZ']:
	

	src = dd
	dst = os.path.join('../../res/tile-48-free/', dd)

	if os.path.exists(src):
		
		for x in os.listdir(src):
			print(x)
