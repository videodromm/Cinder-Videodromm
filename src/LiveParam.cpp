#include "LiveParam.h"
#include "cinder/Filesystem.h"
#include <fstream>

#include "cinder/Quaternion.h"

using namespace ci;
using namespace live;

std::unique_ptr<JsonBag> JsonBag::mInstance = nullptr;
std::once_flag JsonBag::mOnceFlag;

JsonBag::JsonBag()
{
	mJsonFilePath = app::getAssetPath("") / "live_params.json";
	
	// Create json file if it doesn't already exist.
	if( ! fs::exists( mJsonFilePath ) ) {
		std::ofstream oStream( mJsonFilePath.string() );
		oStream.close();
	}
	
	wd::watch( mJsonFilePath, [this]( const fs::path &absolutePath )
	{
		this->load();
	} );
}

void JsonBag::save() const
{
	JsonTree doc;
	JsonTree params = JsonTree::makeArray( "params" );
	
	for( const auto& item : mItems ) {
		item.second->save( item.first, &params );
	}
	
	doc.pushBack( params );
	doc.write( writeFile( mJsonFilePath ), JsonTree::WriteOptions() );
}

void JsonBag::load()
{
	if( ! fs::exists( mJsonFilePath ) ) {
		return;
	}
	
	try {
		JsonTree doc( loadFile( mJsonFilePath ) );
		JsonTree params( doc.getChild( "params" ) );
		for( JsonTree::ConstIter item = params.begin(); item != params.end(); ++item ) {
			const auto& name = item->getKey();
			if( mItems.count( name ) ) {
				mItems.at( name )->load( name, item );
			} else {
				CI_LOG_E( "No item named " + name );
			}
		}
	}
	catch( const JsonTree::ExcJsonParserError& )  {
		CI_LOG_E( "Failed to parse json file." );
	}
}

void JsonBag::removeTarget( void *target )
{
	if( ! target )
		return;
	
	for( auto it = mItems.cbegin(); it != mItems.cend(); ++it ) {
		if( it->second->getTarget() == target ) {
			mItems.erase( it );
			return;
		}
	}
	CI_LOG_E( "Target not found." );
}

JsonBag* live::bag()
{
	std::call_once(JsonBag::mOnceFlag,
				   [] {
					   JsonBag::mInstance.reset( new JsonBag );
				   });
	
	return JsonBag::mInstance.get();
}

template<>
void Param<bool>::save( const std::string& name, ci::JsonTree* tree ) const
{
	tree->addChild( ci::JsonTree( name, ci::toString( mValue ) ) );
}

template<>
void Param<int>::save( const std::string& name, ci::JsonTree* tree ) const
{
	tree->addChild( ci::JsonTree( name, ci::toString( mValue ) ) );
}

template<>
void Param<float>::save( const std::string& name, ci::JsonTree* tree ) const
{
	tree->addChild( ci::JsonTree( name, ci::toString( mValue ) ) );
}

template<>
void Param<glm::vec2>::save( const std::string& name, ci::JsonTree* tree ) const
{
	auto v = ci::JsonTree::makeArray( name );
	v.pushBack( ci::JsonTree( "x", ci::toString( mValue.x ) ) );
	v.pushBack( ci::JsonTree( "y", ci::toString( mValue.y ) ) );
	tree->addChild( v );
}

template<>
void Param<glm::vec3>::save( const std::string& name, ci::JsonTree* tree ) const
{
	auto v = ci::JsonTree::makeArray( name );
	v.pushBack( ci::JsonTree( "x", ci::toString( mValue.x ) ) );
	v.pushBack( ci::JsonTree( "y", ci::toString( mValue.y ) ) );
	v.pushBack( ci::JsonTree( "z", ci::toString( mValue.z ) ) );
	tree->addChild( v );
}

template<>
void Param<glm::vec4>::save( const std::string& name, ci::JsonTree* tree ) const
{
	auto v = ci::JsonTree::makeArray( name );
	v.pushBack( ci::JsonTree( "x", ci::toString( mValue.x ) ) );
	v.pushBack( ci::JsonTree( "y", ci::toString( mValue.y ) ) );
	v.pushBack( ci::JsonTree( "z", ci::toString( mValue.z ) ) );
	v.pushBack( ci::JsonTree( "w", ci::toString( mValue.w ) ) );
	tree->addChild( v );
}

template<>
void Param<glm::quat>::save( const std::string& name, ci::JsonTree* tree ) const
{
	auto v = ci::JsonTree::makeArray( name );
	v.pushBack( ci::JsonTree( "w", ci::toString( mValue.w ) ) );
	v.pushBack( ci::JsonTree( "x", ci::toString( mValue.x ) ) );
	v.pushBack( ci::JsonTree( "y", ci::toString( mValue.y ) ) );
	v.pushBack( ci::JsonTree( "z", ci::toString( mValue.z ) ) );
	tree->addChild( v );

}

template<>
void Param<ci::Color>::save( const std::string& name, ci::JsonTree* tree ) const
{
	auto v = ci::JsonTree::makeArray( name );
	v.pushBack( ci::JsonTree( "r", ci::toString( mValue.r ) ) );
	v.pushBack( ci::JsonTree( "g", ci::toString( mValue.g ) ) );
	v.pushBack( ci::JsonTree( "b", ci::toString( mValue.b ) ) );
	tree->addChild( v );
}

template<>
void Param<bool>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	update( iter->getValue<bool>() );
}

template<>
void Param<int>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	update( iter->getValue<int>() );
}

template<>
void Param<float>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	update( iter->getValue<float>() );
}

template<>
void Param<glm::vec2>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	glm::vec2 v;
	v.x = iter->getChild( "x" ).getValue<float>();
	v.y = iter->getChild( "y" ).getValue<float>();
	update( v );
}

template<>
void Param<glm::vec3>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	glm::vec3 v;
	v.x = iter->getChild( "x" ).getValue<float>();
	v.y = iter->getChild( "y" ).getValue<float>();
	v.z = iter->getChild( "z" ).getValue<float>();
	update( v );
}

template<>
void Param<glm::vec4>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	glm::vec4 v;
	v.x = iter->getChild( "x" ).getValue<float>();
	v.y = iter->getChild( "y" ).getValue<float>();
	v.z = iter->getChild( "z" ).getValue<float>();
	v.w = iter->getChild( "w" ).getValue<float>();
	update( v );
}

template<>
void Param<glm::quat>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	glm::quat q;
	q.w = iter->getChild( "w" ).getValue<float>();
	q.x = iter->getChild( "x" ).getValue<float>();
	q.y = iter->getChild( "y" ).getValue<float>();
	q.z = iter->getChild( "z" ).getValue<float>();
	update( q );
}

template<>
void Param<ci::Color>::load( const std::string& name, ci::JsonTree::ConstIter& iter )
{
	ci::Color c;
	c.r = iter->getChild( "r" ).getValue<float>();
	c.g = iter->getChild( "g" ).getValue<float>();
	c.b = iter->getChild( "b" ).getValue<float>();
	update( c );
}
