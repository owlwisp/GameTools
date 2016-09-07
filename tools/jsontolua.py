import json
import types
import json
import os
import sys
import platform
import re
from os import unlink, popen, getcwd, walk, chdir
from os.path import exists, join 
from collections import OrderedDict

PATH = os.getcwd()


def space_str(layer):
	spaces = ""
	for i in range(0,layer):
		spaces += '\t'
	return spaces

def dic_to_lua_str(data,layer=0):

	d_type = type(data)

	if  d_type is types.StringTypes or d_type is str or d_type is types.UnicodeType:
		yield ("'" + data + "'")

	elif d_type is types.BooleanType:
		if data:
			yield ('true')
		else:
			yield ('false')
	elif d_type is types.IntType or d_type is types.LongType or d_type is types.FloatType:
		yield (str(data))
	elif d_type is types.ListType:
		yield ("{\n")
		yield (space_str(layer+1))

		for i in range(0,len(data)):
			for sub in  dic_to_lua_str(data[i],layer+1):
				yield sub
			if i < len(data)-1:
				yield (',')
		yield ('\n')
		yield (space_str(layer))
		yield ('}')
	elif d_type is types.DictType:
		yield ("\n")
		yield (space_str(layer))
		yield ("{\n")
		data_len = len(data)
		data_count = 0
		for k,v in data.items():
			data_count += 1
			yield (space_str(layer+1))
			if type(k) is types.IntType:
				yield ('[' + str(k) + ']')
			else:
				yield (k) 
			yield (' = ')
			try:
				for sub in  dic_to_lua_str(v,layer +1):
					yield sub
				if data_count < data_len:
					yield (',\n')

			except Exception, e:
				print 'error in ',k,v
				raise
		yield ('\n')
		yield (space_str(layer))
		yield ('}')
	else:
		raise d_type , 'is error'

def str_to_lua_table(jsonStr):
	data_dic = None
	try:
		data_dic = json.loads(jsonStr)
	except Exception, e:
		data_dic =[]
	finally:
		pass

	bytes = ''
	for itr in dic_to_lua_str(data_dic):
		bytes += itr
	return bytes

# ---------------------------------------------------------#
# 
# 		tools
# 
# --------------------------------------------------------#

def optparseInfo():
  from optparse import OptionParser

  usage = """
  python [option] ... [-f | --filePath] [filePath]
  python [option] ... [-p | -j]

  %prog [-f resource_dir(outPut the  file path)] [-j(resolve *.json )]

  Examples:
      %prog
      %prog -f ../Resources
  """

  def _help(option, opt, value, parser):
    print "------------------------------"
    parser.print_help()
    print "------------------------------"
    sys.exit(0)
    pass

  parser = OptionParser(usage)
  parser.add_option("-p" ,"--path", action="store", dest="path", default="LuaDir",  help = "path for save the lua file!")
  parser.add_option("-o" ,"--open", action="store_true", dest="isOpen", default= "false",  help = "selected, will be open the file dir!")
  parser.add_option("-v" ,"--version", action="store_true", dest="version", default= "true",  help = "version = 1.0.0v")

  (options, args) = parser.parse_args(sys.argv)
  # print "filePath is " + options.filePath
  return options
  pass

def isWindowsSystem():
    return 'Windows' in platform.system()
 
def isMacSystem():
    return 'Darwin' in platform.system()

def removeDir(path):
	if isWindowsSystem():
		os.system("rd /s /q  " + path)

	elif isMacSystem():
		os.system("rm -rf " + path)
		
		pass
	pass

def createDir(path):
	start = 0
	index = 0 

	while  True:   

		if isWindowsSystem():
			index = path.find("\\",start) 
		elif isMacSystem():
			index = path.find("/",start) 
			pass
		if index == -1:
			break

		subPath = path[0:index]
		if os.path.exists(subPath) == 0 and subPath:
			os.mkdir(subPath)
			pass

		start = index + 1
		pass

	pass

def getFileSegmentation():
	segmentation = ''

	if isMacSystem():
		segmentation = "/"

	elif isWindowsSystem():
		segmentation = "\\"
	pass

	return segmentation
	pass

def getFileNameWithSuffix(dir_paths,key):
	regular_plist = re.compile('[a-zA-Z0-9_]+\.' + key + "$")
	sources = []
	for root, dirs, files in walk(dir_paths):
		for name in files:
			if regular_plist.match(name):
				tmp = root[len(dir_paths) + 1:]
				if len(tmp) > 0:
					tmp = tmp + getFileSegmentation()
					pass 

				sources.append((name,tmp))
	return sources
	pass

def file_to_lua_file(jsonFile,luaFile):
	with open(luaFile,'w') as luafile:
		with open(jsonFile) as jsonfile:
			luafile.write(str_to_lua_table(jsonfile.read()))


def main():

	options = optparseInfo()
  	luaPath = options.path
  	isOpen = options.isOpen

	outoPath = ""
	if isMacSystem():
		outoPath = PATH + "/" + luaPath + "/"

	elif isWindowsSystem():
		outoPath = PATH + "\\" + luaPath + "\\"
	pass

	removeDir(outoPath)
	createDir(outoPath)



  	ret = getFileNameWithSuffix(PATH,"json")
  	for element in  ret :
  		name = element[0]
  		midPath = element[1]

  		index = name.find(".json")
  		strName = name[0 : index]
  		
  		createDir(outoPath + midPath)

  		file_to_lua_file(midPath + name, outoPath + midPath + strName + ".lua")

	pass
	if isOpen:
		os.system("open " + outoPath)
		pass
pass


if __name__ == '__main__':
	main()



