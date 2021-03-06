/**
 * Global for Windows
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUMWINDOWS_GLOBALOBJECT_HPP_
#define _TITANIUMWINDOWS_GLOBALOBJECT_HPP_

#include "TitaniumWindows/detail/GlobalBase.hpp"
#include "TitaniumWindows/NativeModuleLoader.hpp"

namespace TitaniumWindows
{
	using namespace HAL;

	/*!
	  @class

	  @discussion This is the Titanium GlobalObject implementation for
	  Windows.
	*/
	class TITANIUMWINDOWS_GLOBAL_EXPORT GlobalObject final : public Titanium::GlobalObject, public JSExport<GlobalObject>
	{
	public:

		GlobalObject(const JSContext&) TITANIUM_NOEXCEPT;

		virtual ~GlobalObject() = default;
		GlobalObject(const GlobalObject&) = default;
		GlobalObject& operator=(const GlobalObject&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
		GlobalObject(GlobalObject&&) = default;
		GlobalObject& operator=(GlobalObject&&) = default;
#endif

		static void JSExportInitialize();

		virtual void registerNativeModuleLoader(NativeModuleLoader* module_loader);

	protected:
		virtual std::string requestResolveModule(const JSObject& parent, const std::string& moduleId, const std::string& dirname = COMMONJS_SEPARATOR__) TITANIUM_NOEXCEPT override final;
		virtual std::string readRequiredModule(const JSObject& parent, const std::string& path) const override final;
		virtual bool requiredModuleExists(const std::string& path) const TITANIUM_NOEXCEPT override final;
		virtual std::shared_ptr<Titanium::GlobalObject::Timer> CreateTimer(Callback_t callback, const std::chrono::milliseconds& interval) const TITANIUM_NOEXCEPT override final;

	private:
		NativeModuleLoader* module_loader__ { nullptr };
	};

}  // namespace TitaniumWindows {

#endif  // _TITANIUMWINDOWS_GLOBALOBJECT_HPP_
