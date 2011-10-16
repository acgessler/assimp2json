/*
Assimp2Json
Copyright (c) 2011, Alexander C. Gessler

Licensed under a 3-clause BSD license. See the LICENSE file for more information.

*/

#include <assimp.hpp>
#include <export.hpp>
#include <IOStream.h>
#include <IOSystem.h>

#include <aiScene.h>

#include <sstream>
#include <boost/scoped_ptr.hpp>

namespace {
void Assimp2Json(const char* ,Assimp::IOSystem*,const aiScene*);
}

Assimp::Exporter::ExportFormatEntry Assimp2Json_desc = Assimp::Exporter::ExportFormatEntry(
	"json",
	"Plain JSON representation of the Assimp scene data structure",
	"json",
	Assimp2Json,
	0u);

namespace {

	std::ostream& operator << (std::ostream& ss,const aiMatrix4x4& s) {
		std::stringstream buff;
		buff.imbue( std::locale("C") );

		for(unsigned int x = 0; x < 4; ++x) {
			for(unsigned int y = 0; y < 4; ++y) {
				if(x || y) {
					buff << ',';
				}
				buff << s[x][y];
			}
		}
		return ss << '[' <<  buff.str() << ']';
	}

	// small utility class to simplify serializing the aiScene to Json
class JSONWriter
{

public:

	enum {
		Flag_DoNotIndent = 0x1,
	};

public:

	JSONWriter(Assimp::IOStream& out, unsigned int flags = 0u)
		: out(out)
		, first()
		, flags(flags)
	{
		// make sure that all formatting happens using the standard, C locale and not the user's current locale
		buff.imbue( std::locale("C") );
	}

	~JSONWriter()
	{
		Flush();
	}

public:

	void Flush()	{
		const std::string s = buff.str();
		out.Write(s.c_str(),s.length(),1);
		buff.clear();
	}

	void PushIndent() {
		indent += '\t';
	}

	void PopIndent() {
		indent.erase(indent.end()-1);
	}

	void Key(const std::string& name) {
		AddIndentation();
		Delimit();
		buff << '\"'+name+"\": ";
	}

	template<typename Literal>
	void Element(const Literal& name) {
		AddIndentation();
		Delimit();
		buff << name << '\n';
	}

	template<typename Literal>
	void SimpleValue(const Literal& s) {
		buff << s << '\n';
	}

	void SimpleValue(const aiString& s) {
		buff << '\"' << s.data << "\"\n";
	}

	void StartObj(bool is_element = false) {
		if(is_element && !first) {
			buff << ',';
		}
		first = true;
		AddIndentation();
		buff << "{\n";
		PushIndent();
	}

	void EndObj() {
		PopIndent();
		AddIndentation();
		buff << "{\n";
	}

	void StartArray(bool is_element = false) {
		if(is_element && !first) {
			buff << ',';
		}
		first = true;
		AddIndentation();
		buff << "[\n";
		PushIndent();
	}

	void EndArray() {
		PopIndent();
		AddIndentation();
		buff << "]\n";
	}

	void AddIndentation() {
		if(!(flags & Flag_DoNotIndent)) {
			buff << indent;
		}
	}

	void Delimit() {
		if(!first) {
			buff << ',';
		}
		else {
			buff << ' ';		
			first = false;
		}
	}


private: 
	Assimp::IOStream& out;
	std::string indent, newline;
	std::stringstream buff;
	bool first;

	unsigned int flags;
};


void Write(JSONWriter& out, const aiBone& ai)
{
	out.StartObj();

	out.Key("name");
	out.SimpleValue(ai.mName);

	out.Key("offsetmatrix");
	out.SimpleValue(ai.mOffsetMatrix);

	out.Key("weights");
	out.StartArray();
	for(unsigned int i = 0; i < ai.mNumWeights; ++i) {
		out.StartArray(true);
		out.Element(ai.mWeights[i].mVertexId);
		out.Element(ai.mWeights[i].mWeight);
		out.EndArray();
	}
	out.EndArray();
	out.EndObj();
}


void Write(JSONWriter& out, const aiFace& ai)
{
	out.StartArray();
	for(unsigned int i = 0; i < ai.mNumIndices; ++i) {
		out.Element(ai.mIndices[i]);
	}
	out.EndArray();
}


void Write(JSONWriter& out, const aiMesh& ai)
{
	out.StartObj(); 

	out.Key("name");
	out.SimpleValue(ai.mName);

	out.Key("material");
	out.SimpleValue(ai.mMaterialIndex);

	out.Key("primitivetypes");
	out.SimpleValue(ai.mPrimitiveTypes);

	out.Key("vertices");
	out.StartArray();
	for(unsigned int i = 0; i < ai.mNumVertices; ++i) {
		out.Element(ai.mVertices[i].x);
		out.Element(ai.mVertices[i].y);
		out.Element(ai.mVertices[i].z);
	}
	out.EndArray();

	if(ai.HasNormals()) {
		out.Key("normals");
		out.StartArray();
		for(unsigned int i = 0; i < ai.mNumVertices; ++i) {
			out.Element(ai.mNormals[i].x);
			out.Element(ai.mNormals[i].y);
			out.Element(ai.mNormals[i].z);
		}
		out.EndArray();
	}

	if(ai.HasTangentsAndBitangents()) {
		out.Key("tangents");
		out.StartArray();
		for(unsigned int i = 0; i < ai.mNumVertices; ++i) {
			out.Element(ai.mTangents[i].x);
			out.Element(ai.mTangents[i].y);
			out.Element(ai.mTangents[i].z);
		}
		out.EndArray();

		out.Key("bitangents");
		out.StartArray();
		for(unsigned int i = 0; i < ai.mNumVertices; ++i) {
			out.Element(ai.mBitangents[i].x);
			out.Element(ai.mBitangents[i].y);
			out.Element(ai.mBitangents[i].z);
		}
		out.EndArray();
	}

	if(ai.GetNumUVChannels()) {
		out.Key("numuvcomponents");
		out.StartArray();
		for(unsigned int n = 0; n < ai.GetNumUVChannels(); ++n) {
			out.Element(ai.mNumUVComponents[n]);
		}
		out.EndArray();

		out.Key("texturecoords");
		out.StartArray();
		for(unsigned int n = 0; n < ai.GetNumUVChannels(); ++n) {
			
			out.StartArray(true);
			for(unsigned int i = 0; i < ai.mNumVertices; ++i) {
				out.Element(ai.mTextureCoords[n][i].x);
				out.Element(ai.mTextureCoords[n][i].y);
				out.Element(ai.mTextureCoords[n][i].z);
			}
			out.EndArray();
		}
		out.EndArray();
	}

	if(ai.GetNumColorChannels()) {
		out.Key("colors");
		out.StartArray();
		for(unsigned int n = 0; n < ai.GetNumColorChannels(); ++n) {

			out.StartArray(true);
			for(unsigned int i = 0; i < ai.mNumVertices; ++i) {
				out.Element(ai.mColors[n][i].r);
				out.Element(ai.mColors[n][i].g);
				out.Element(ai.mColors[n][i].b);
				out.Element(ai.mColors[n][i].a);
			}
			out.EndArray();
		}
		out.EndArray();
	}

	if(ai.mNumBones) {
		out.Key("bones");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumBones; ++n) {
			out.Delimit();
			Write(out, *ai.mBones[n]);
		}
		out.EndArray();
	}


	out.Key("faces");
	out.StartArray();
	for(unsigned int n = 0; n < ai.mNumFaces; ++n) {
		out.Delimit();
		Write(out, ai.mFaces[n]);
	}
	out.EndArray();

	out.EndObj();
}


void Write(JSONWriter& out, const aiNode& ai)
{
	out.StartObj();

	out.Key("name");
	out.SimpleValue(ai.mName);

	out.Key("transform");
	out.SimpleValue(ai.mTransformation);

	if(ai.mNumMeshes) {
		out.Key("meshes");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumMeshes; ++n) {
			out.Element(ai.mMeshes[n]);
		}
		out.EndArray();
	}

	if(ai.mNumChildren) {
		out.Key("children");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumChildren; ++n) {
			out.Delimit();
			Write(out,*ai.mChildren[n]);
		}
		out.EndArray();
	}

	out.EndObj();
}

void Write(JSONWriter& out, const aiMaterial& ai)
{
	out.StartObj();

	out.EndObj();
}

void Write(JSONWriter& out, const aiTexture& ai)
{
	out.StartObj();

	out.EndObj();
}

void Write(JSONWriter& out, const aiLight& ai)
{
	out.StartObj();

	out.EndObj();
}

void Write(JSONWriter& out, const aiCamera& ai)
{
	out.StartObj();

	out.EndObj();
}

void Write(JSONWriter& out, const aiScene& ai)
{
	out.StartObj();

	out.Key("rootnode");
	Write(out,*ai.mRootNode);

	out.Key("flags");
	out.SimpleValue(ai.mFlags);

	if(ai.HasMeshes()) {
		out.Key("meshes");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumMeshes; ++n) {
			out.Delimit();
			Write(out,*ai.mMeshes[n]);
		}
		out.EndArray();
	}

	if(ai.HasMaterials()) {
		out.Key("materials");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumMaterials; ++n) {
			out.Delimit();
			Write(out,*ai.mMaterials[n]);
		}
		out.EndArray();
	}

	if(ai.HasLights()) {
		out.Key("lights");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumLights; ++n) {
			out.Delimit();
			Write(out,*ai.mLights[n]);
		}
		out.EndArray();
	}

	if(ai.HasCameras()) {
		out.Key("cameras");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumCameras; ++n) {
			out.Delimit();
			Write(out,*ai.mCameras[n]);
		}
		out.EndArray();
	}

	if(ai.HasTextures()) {
		out.Key("textures");
		out.StartArray();
		for(unsigned int n = 0; n < ai.mNumTextures; ++n) {
			out.Delimit();
			Write(out,*ai.mTextures[n]);
		}
		out.EndArray();
	}

	out.EndObj();
}

void Assimp2Json(const char* file,Assimp::IOSystem* io,const aiScene* scene) 
{
	boost::scoped_ptr<Assimp::IOStream> str(io->Open(file,"wt"));
	if(!str) {
		//throw Assimp::DeadlyExportError("could not open output file");
	}

	Write(JSONWriter(*str),*scene);
}


}