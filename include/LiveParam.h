// from https://github.com/num3ric/Cinder-LiveJsonParam
#pragma once

#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"

#include <boost/noncopyable.hpp>
#include <unordered_map>

#include "Watchdog.h"

// Eric Renaud-Houde - Jan 2015
// Credit to Rich's live DartBag work.

namespace live {
	
	class ParamOwner {
		friend class ParamBase;
	private:
		virtual void removeTarget( void *target ) = 0;
	};
	
	class ParamBase {
	public:
		ParamBase() : mOwner( nullptr ) { }
		ParamBase( void* target ) : mVoidPtr( target ), mOwner( nullptr ) { }
		
		virtual ~ParamBase()
		{
			if( mOwner )
				mOwner->removeTarget( mVoidPtr );
		};
		
		void setOwner( ParamOwner *owner ) { mOwner = owner; }
		
		void * getTarget() const { return mVoidPtr; }
			#if defined( CINDER_MSW )
		virtual void save( const std::string& name, ci::JsonTree* tree ) const = 0;
		virtual void load( const std::string& name, ci::JsonTree::ConstIter& iter ) = 0;
		#endif
	private:
		ParamOwner*	mOwner;
		void*		mVoidPtr;
	};
	
	template<typename T>
	class Parameter : public boost::noncopyable, public ParamBase {
	public:
		Parameter()
		: ParamBase(), mValue()
		{ }
		
		Parameter( T value )
		: ParamBase( &mValue ), mValue( value )
		{ }
		
		void setUpdateFn( const std::function<void()> &updateFn ) {
			mUpdateFn = updateFn;
		};
		
		operator const T&() const { return mValue; }
		
		Parameter<T>& operator=( T value ) { mValue = value; return *this; }
		
		const T&	value() const { return mValue; }
		T&			value() { return mValue; }
		
		//! Short-hand for value()
		const T&	operator()() const { return mValue; }
		//! Short-hand for value()
		T&			operator()() { return mValue; }
		
		const T*	ptr() const { return &mValue; }
		T*			ptr() { return &mValue; }
		
	protected:
		void update( const T& newValue ) {
			if( mValue != newValue ) {
				mValue = newValue;
				mUpdateFn();
			}
		}
			#if defined( CINDER_MSW )
		void save( const std::string& name, ci::JsonTree* tree ) const override;
		void load( const std::string& name, ci::JsonTree::ConstIter& iter ) override;
	#endif
		T						mValue;
		std::function<void()>	mUpdateFn;
		friend class JsonBag;
	};
	
	class JsonBag;
	
	JsonBag* bag();
	
	class JsonBag : public boost::noncopyable, public ParamOwner {
	public:		
		template<typename T>
		static void add( Parameter<T> *param,
						 const std::string &key,
						 const std::function<void()> &updateFn = [](){} )
		{
			auto* bag = live::bag();
			param->setOwner( bag );
			param->setUpdateFn( updateFn );
			bag->mItems.emplace( key, param );
		}
			#if defined( CINDER_MSW )
		void save(ci::fs::path aJsonFilePath) ;//const
		void load(ci::fs::path aJsonFilePath);
		#endif
	private:	
		JsonBag();
		void removeTarget( void *target ) override;

		static std::unique_ptr<JsonBag>	mInstance;
		static std::once_flag			mOnceFlag;
		
		std::unordered_map<std::string, ParamBase*>	mItems;
		
		friend JsonBag* bag();
		
		ci::fs::path	mJsonFilePath;

	};

} //namespace live

