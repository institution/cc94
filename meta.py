
def f(f, *args, **kwargs):
	return f.format(*args, **kwargs)

def printf(f, *args, **kwargs):
	print(f.format(*args, **kwargs))


def endline():
	print('')


def printft(f, *args, **kwargs):
	printf('\t' + f, *args, **kwargs)


def rwstruct(Name, *args):
	nts = []
	name = Name.lower()
	
	printf("struct {} {{", Name)
	for e in args:
		t,n = e.split(' ')
		nts.append((n,t))
		printft("{t} {n};", n=n, t=t)
	endline()	
	# constructors
	printft('{}() {{}}', Name)
	printft('{}({}): {} {{}}', Name,
		', '.join(f('{} {}_',t,n) for (n,t) in nts),
		', '.join(f('{}({}_)',n,n) for (n,t) in nts),
	)
	printf("}}; //{}", Name)
	endline()
		
	printf("{} read_{}(ifstream &f) {{", Name, name)
	printft('{} {};', Name, name)
	for n,t in nts:
		printft('{name}.{n} = read_{t}(f);', name=name, n=n, t=t)	
	printft('return {};', name)
	printf('}}')
	endline()

	printf("void write_{}(ofstream &f, const {} &{}) {{", name, Name, name)
	for n,t in nts:
		printft('write_{t}(f, {name}.{n});', name=name, n=n, t=t)	
	printf('}}')
	endline()
