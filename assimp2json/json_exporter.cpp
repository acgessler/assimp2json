
#include <assimp.hpp>
#include <export.hpp>

void Assimp2Json(const char* ,IOSystem*,const aiScene*);
Assimp::Exporter::ExportFormatEntry Assimp2Json_desc = Assimp::Exporter::ExportFormatEntry(
	"json",
	"Plain JSON representation of the Assimp scene data structure",
	"json",
	Assimp2Json,
	0u);

void Assimp2Json(const char* file,IOSystem* io,const aiScene* scene) 
{

}


