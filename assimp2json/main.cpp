/*
Assimp2Json
Copyright (c) 2011, Alexander C. Gessler

Licensed under a 3-clause BSD license. See the LICENSE file for more information.

*/

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>

#include <assimp/version.h>
#include <assimp/postprocess.h>

#include <iostream>

#include "version.h"

// json_exporter.cpp
extern Assimp::Exporter::ExportFormatEntry Assimp2Json_desc;

int unrecog_exit(int ex = -1)
{
	std::cout << "usage: assimp2json [--log --verbose] input [output]" << std::endl;
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

	int nextarg = 1;
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

	if (argc < nextarg+1) {
		return unrecog_exit(-2);
	}

	const char* in = argv[nextarg], *out = (argc < nextarg+2 ? NULL : argv[nextarg+1]);
	
	Assimp::Importer imp;
	const aiScene* const sc = imp.ReadFile(in,aiProcessPreset_TargetRealtime_MaxQuality);
	if (!sc) {
		std::cerr << "failure reading file: " << in << std::endl;
		return -3;
	}

	Assimp::Exporter exp;
	exp.RegisterExporter(Assimp2Json_desc);

	if(out) {
		if(aiReturn_SUCCESS != exp.Export(sc,"json",out)) {
			std::cerr << "failure exporting file: " << out << ": " << exp.GetErrorString() << std::endl;
			return -4;
		}
	}
	else {
		// write to stdout, but we might do better than using ExportToBlob()
		const aiExportDataBlob* const blob = exp.ExportToBlob(sc,"json");
		if(!blob) {
			std::cerr << "failure exporting to (stdout) " << exp.GetErrorString() << std::endl;
			return -5;
		}

		const std::string s(static_cast<char*>( blob->data), blob->size);
		std::cout << s << std::endl;
	}
	return 0;
}
