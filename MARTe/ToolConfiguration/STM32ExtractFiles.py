#!/usr/bin/python
import sys
import xml.sax
from shutil import copyfile


class STM32ExtractFilesHandler( xml.sax.ContentHandler ):
    def __init__(self, cubeProjectFolder, destinationFolder):
        self.cubeProjectFolder = cubeProjectFolder
        self.destinationFolder = destinationFolder 

    def startElement(self, tag, attributes):
        self.CurrentData = tag
        if tag == "file":
            filenameSrc = attributes["name"]
            filenameSrc = filenameSrc.replace('\\', '/')
            
            filenameDest = filenameSrc.rsplit('/', 1)[-1]
            print ('Copying ' + filenameSrc + ' to ' + filenameDest)
            if filenameSrc[0] != '/':
                filenameSrc = self.cubeProjectFolder + '/' + filenameSrc
            filenameDest = self.destinationFolder + '/' + filenameDest
            copyfile(filenameSrc, filenameDest)            

if ( __name__ == "__main__"):
    '''
    Usage
    user@hostname>python STM32CubeFiles.py cubeProjectFolder cubeProjectName destinationFolder
    '''
    argc = len(sys.argv)
    if(argc < 4):
        print ('usage:python STM32CubeFiles.py cubeProjectFolder cubeProjectName destinationFolder')
        exit()

    cubeProjectFolder = sys.argv[1]
    cubeProjectName = sys.argv[2]
    destinationFolder = sys.argv[3]
    # create an XMLReader
    parser = xml.sax.make_parser()
    # turn off namepsaces
    parser.setFeature(xml.sax.handler.feature_namespaces, 0)

    # override the default ContextHandler
    Handler = STM32ExtractFilesHandler(cubeProjectFolder, destinationFolder)
    parser.setContentHandler( Handler )

    print (cubeProjectFolder + '/' + cubeProjectName + '.gpdsc')
    parser.parse(cubeProjectFolder + '/' + cubeProjectName + '.gpdsc')

