#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
import json
import types
import json
import os
import sys
import platform
import re
import copy
from os import unlink, popen, getcwd, walk, chdir
from os.path import exists, join 
from collections import OrderedDict
 
reload(sys)
sys.setdefaultencoding('utf-8')

PATH = os.getcwd()





def optparseInfo():
  from optparse import OptionParser

  usage = """
  python [option] ... [-p | --filePath] [filePath]
  python [option] ... [-o | -v]

  %prog [-o] [-p Dir]

  Examples:
      %prog
      %prog -p Dir
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


def readModel():
	return readFile("model.ss") 
	pass

def readFile(filaName):
	fp = open(filaName,'r')
	data = fp.read()
	fp.close()
	return data 
	pass


def getTmplIdValue(tileKeyArr):
	return tileKeyArr["tmplId"]
	pass


def getJsonKeysArray(jsonStr):
	data_dic = None
	try:
		data_dic = json.loads(jsonStr)
	except Exception, e:
		data_dic = []
	finally:
		pass

	array = []
	for element in  data_dic :
		tmpArr = []
		for key in element.keys():
			tmpArr.append((key,type(element[key])))
			pass
		array = list(set(array).union(set(tmpArr)))

		pass
	return array 

	pass


def getJsonValue(data):
	d_type = type(data)

	if  d_type is types.StringTypes or d_type is str or d_type is types.UnicodeType:
		return "'" + data + "'"

	elif d_type is types.BooleanType:
		if data:
			return 'true'
		else:
			return 'false'
	elif d_type is types.IntType or d_type is types.LongType or d_type is types.FloatType:
		return  str(data)
	elif d_type is types.ListType:
		string = "{"
		for i in range(0,len(data)):
			string += getJsonValue(data[i])
			if i < len(data) -1:
				string += ','

		string +='}'
		return string 
	elif d_type is types.DictType:
		string = "{"
		data_len = len(data)
		data_count = 0
		for k,v in data.items():
			data_count += 1
			if type(k) is types.IntType:
				string += '[' + str(k) + ']'
			else:
				string += k 
			string += ' = '
			for sub in  getJsonValue(v):
				string +=  sub
			if data_count < data_len:
				string += ','

		string += '}'
		return string 
		pass

	pass



def getDefaultValue(d_type):
	if  d_type is types.StringTypes or d_type is str or d_type is types.UnicodeType:
		return "''"

	elif d_type is types.BooleanType:
			return 'false'
	elif d_type is types.IntType or d_type is types.LongType or d_type is types.FloatType:
		return str(0)
	elif d_type is types.ListType:
		return "{}"
	elif d_type is types.DictType:
		return "{}"
	pass

# k1 = { 1,"旅程的开始","1个英雄达到3级",5,1,1,1,3,{1,1,3},28,"achievement_004"}
# 

def modifyValue(res,data ,tileKeyArr):

	value = ""

	data_dic = json.loads(data)
	count = 0
	for i in  range(len(data_dic)):

		value += "k" + str(data_dic[i]["tmplId"]) + " = { "
		tmpCount = 0
		for j in range(len(tileKeyArr)):
			if data_dic[i].has_key(tileKeyArr[j][0]):
				value += getJsonValue(data_dic[i][tileKeyArr[j][0]])
			else:
				value += getDefaultValue(tileKeyArr[j][1])
			pass

			tmpCount += 1
			if tmpCount < len(tileKeyArr):
				value += ' ,'
			pass
			
			pass

		value += "}"
		count += 1
		if count < len(data_dic):
				value += ' ,\n\t'
		pass
		pass

	strinfo = re.compile("VALUEKEY")

  	return  strinfo.sub(value,res)
	pass

def modifyTitle(data ,array):

	titleStr = ""
	count = 0
	for title in  array :
		titleStr += title[0] + ' = ' + str(count + 1) 
		
		count += 1
		if count < len(array):
			titleStr += ' ,\n\t'
		pass
		pass
	strinfo = re.compile("TITLEKEY")

  	return  strinfo.sub(titleStr,data)
 
	pass

def modifyMetatable(data ,fileName):

	strinfo = re.compile("FILENAME")

	return strinfo.sub(fileName,data)
	pass


def writeFiles(path, data):
  fp = open(path,'w')
  fp.write(data)
  fp.close()
  pass


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

	baseData = readModel()

	ret = getFileNameWithSuffix(PATH,"json")
	for element in  ret :
  		name = element[0]
  		midPath = element[1]

  		index = name.find(".json")
  		strName = name[0 : index]
  		
  		print "handle the file -> ",strName
  		tmpDir = outoPath + midPath

  		if os.path.exists(tmpDir) == 0 and tmpDir:
			os.mkdir(tmpDir)
			pass

		data = copy.deepcopy(baseData)

		tmpData = readFile(midPath + name)
		

		# 替换基础配置
		data = modifyMetatable(data,strName)
		# 替换title值
		tileKeyArr = getJsonKeysArray(tmpData)

		data = modifyTitle(data,tileKeyArr)
		# 替换value值
		data = modifyValue(data,tmpData,tileKeyArr)
		
		writeFiles(outoPath + midPath + strName + ".lua", data)
	pass

	if isOpen == True:
		os.system("open " + outoPath)
		pass
	pass



if __name__ == '__main__':
	main()


