
#include <assimp.hpp>
#include <export.hpp>

#include <aiVersion.h>
#include <aiPostProcess.h>

#include <iostream>

#include "Version.h"

// json_exporter.cpp
extern Assimp::Exporter::ExportFormatEntry Assimp2Json_desc;

int unrecog_exit(int ex = -1)
{
	std::cout << "usage: assimp2json [--log --verbose] input output" << std::endl;
	return ex;
}

void printver() 
{
	std::cout << "assimp2json v" << ASSIMP2JSON_VERSION_MAJOR << "." << ASSIMP2JSON_VERSION_MINOR << "\n";
	std::cout << "(c) 2011, Alexander C. Gessler\nLicensed under a 3-clause BSD license\n" <<
		"(assimp version: " 
		<< aiGetVersionMajor() << "." 
		<< aiGetVersionMinor() << "." 
		<< aiGetVersionRevision() << ")"
		<< std::endl;
}

void printhelp()
{
}

int main (int argc, char *argv[])
{
	if (argc == 1) {
		return unrecog_exit(-1);
	}

	unsigned int nextarg = 1;
	while(nextarg < argc && argv[nextarg][0] == '-') {
		if (!strcmp(argv[nextarg],"--help")) {
			printhelp();
			return 0;
		}
		else if (!strcmp(argv[nextarg],"--version")) {
			printver();
			return 0;
		}
		++nextarg;
	}

	if (argc != nextarg+2) {
		return unrecog_exit(-2);
	}

	const char* in = argv[nextarg], *out = argv[nextarg+1];

	Assimp::Importer imp;
	const aiScene* const sc = imp.ReadFile(in,aiProcessPreset_TargetRealtime_MaxQuality);
	if (!sc) {
		std::cout << "failure reading file: " << in << std::endl;
		return -3;
	}

	Assimp::Exporter exp;
	exp.RegisterExporter(Assimp2Json_desc);

	if(aiReturn_SUCCESS != exp.Export(sc,"json",out)) {
		std::cout << "failure writing file: " << out << ": " << exp.GetErrorString() << std::endl;
		return -4;
	}
	return 0;
}