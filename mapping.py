import sys
class mapping:
	def __init__(From,To):
		self.input = From
		self.output = To
		self.map = dict()
	def read_map():
		input_map = open(self.input,encoding = "Big5",'r')
		line = input_map.readlines()
		input_map.close()
		for ln in line:
			lst = ln.split()
			lstzuin = lst[1].split("/")
			for zuin in lstzuin:
				self.map[zuin[0]].append(lst[0])
			self.map[lst[0]] = lst[0]
	def dump_map():
		keylist = self.map.keys()
		outlist = []
		for key in keylist:
			outlist.append(key+" "+" ".join(self.map[key])+"\n")
		output_dump = open(self.output,encoding = "Big5",'w')
		output_dump.writelines(outlist)
		output_dump.close()
if __name__ == '__main__':
	printf("working\n")
	mpp = mapping(sys.argv[1],sys.argv[2])
	mpp.read_map()
	mpp.dump_map()


			

		
