
def f(f, *args, **kwargs):
	return f.format(*args, **kwargs)

def printf(f, *args, **kwargs):
	print(f.format(*args, **kwargs))


def endline():
	print('')


def printft(f, *args, **kwargs):
	printf('\t' + f, *args, **kwargs)


def make_rwtype(Name, *args, **kwargs):
	nts = []
	name = Name.lower()
	
	sid = kwargs.pop('sid', -1)
	
	printf("struct {} {{", Name)
	
	# members
	for e in args:
		t,n = e.split(' ')
		nts.append((n,t))
		printft("{t} {n};", n=n, t=t)
	endline()
	
	# null constructor
	printft('{}() {{}}', Name)
	endline()
	
	# init constructor
	printft('{}({}): {} {{}}', Name,
		', '.join(f('{} {}_',t,n) for (n,t) in nts),
		', '.join(f('{}({}_)',n,n) for (n,t) in nts),
	)
	endline()
	
	# sid method
	if sid != -1:
		printft("static constexpr uint16 sid = {};", sid)
		endline()
	
	printf("}}; //{}", Name)
	endline()
		
	make_read(Name, name, nts)
	make_write(Name, name, nts)
	make_str(Name, nts)


def make_str(type_, nts):
	s = f((
		'inline ostream& operator<<(ostream &out, const {type} &obj)	{{'
		'	cout << "{type}(" << {lst} << ")";'
		'}}'
		),
		type = type_,
		lst = "<<','<<".join(f('"{mem}"<<"="<<obj.{mem}', mem=mem) for mem,_ in nts),
	)
	print(s)
	endline()

def make_write(type, name, nts, h=False):
	if h: 
		printf("inline void write_{}(ostream &f, const {} &{});", name, type, name)
	else:
		printf("inline void write_{}(ostream &f, const {} &{}) {{", name, type, name)
		for n,t in nts:
			printft('write_{t}(f, {name}.{n});', name=name, n=n, t=t)	
		printf('}}')
	

	
def make_read(type, name, nts, h=False):
	if h:
		printf("inline {} read_{}(istream &f);", type, name)
			
	else:
		# read
		printf("inline {} read_{}(istream &f) {{", type, name)
		printft('{} {};', type, name)
		for n,t in nts:
			printft('{name}.{n} = read_{t}(f);', name=name, n=n, t=t)	
		printft('return {};', name)
		printf('}}')
		endline()


