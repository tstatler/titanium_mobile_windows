/**
 * Titanium.Map.View for Windows
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "TitaniumWindows/Map/View.hpp"
#include "TitaniumWindows/Utility.hpp"
#include "TitaniumWindows/Map/Annotation.hpp"
#include "TitaniumWindows/UI/WindowsViewLayoutDelegate.hpp"

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
using namespace Windows::Devices::Geolocation;
#endif

namespace TitaniumWindows
{
	namespace Map
	{
		View::View(const JSContext& js_context) TITANIUM_NOEXCEPT
			: Titanium::Map::View(js_context)
		{
			TITANIUM_LOG_DEBUG("View::ctor Initialize");
		}

		View::~View()
		{
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			mapview__ = nullptr;
#endif
		}

		void View::postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments) {
			Titanium::Map::View::postCallAsConstructor(js_context, arguments);

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			mapview__ = ref new MapControl();

			// Set service token
			// TODO : Load this from app properties, windows_map_service_token?
			mapview__->MapServiceToken = "VrSrmXR8B5bgklWrs0CK_w";

			Titanium::Map::View::setLayoutDelegate<TitaniumWindows::UI::WindowsViewLayoutDelegate>();

			layoutDelegate__->set_defaultHeight(Titanium::UI::LAYOUT::FILL);
			layoutDelegate__->set_defaultWidth(Titanium::UI::LAYOUT::FILL);

			getViewLayoutDelegate<TitaniumWindows::UI::WindowsViewLayoutDelegate>()->setComponent(mapview__);

			zoom(12); // use a default zoom of 12 for Windows?
#endif
		}

		void View::JSExportInitialize() 
		{
			JSExport<View>::SetClassVersion(1);
			JSExport<View>::SetParent(JSExport<Titanium::Map::View>::Class());
		}

		uint32_t View::get_maxZoomLevel() const TITANIUM_NOEXCEPT
		{
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			return static_cast<uint32_t>(mapview__->MaxZoomLevel); // should be 20
#else
			return 20;
#endif
		}

		uint32_t View::get_minZoomLevel() const TITANIUM_NOEXCEPT
		{
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			return static_cast<uint32_t>(mapview__->MinZoomLevel); // should be 1
#else
			return 1;
#endif
		}

		void View::set_mapType(const Titanium::Map::MAP_TYPE& mapType) TITANIUM_NOEXCEPT
		{
			Titanium::Map::View::set_mapType(mapType);
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			if (mapType == Titanium::Map::MAP_TYPE::HYBRID) {
				mapview__->Style = MapStyle::AerialWithRoads;
			} else if (mapType == Titanium::Map::MAP_TYPE::SATELLITE) {
				mapview__->Style = MapStyle::Aerial;
			} else if (mapType == Titanium::Map::MAP_TYPE::NORMAL) {
				mapview__->Style = MapStyle::Road;
			}
#endif
		}

		void View::set_region(const Titanium::Map::MapRegionTypev2& region) TITANIUM_NOEXCEPT
		{
			Titanium::Map::View::set_region(region);

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			// set center to supplied lat/long
			BasicGeoposition location = { region.latitude, region.longitude };
			auto gp = ref new Geopoint(location);
			mapview__->Center = gp;

			// If there are deltas, use them to set the view bounds
			if (region.latitudeDelta > 0 && region.longitudeDelta > 0) {
				BasicGeoposition top_left = { region.latitude - region.latitudeDelta, region.longitude - region.longitudeDelta };
				BasicGeoposition bottom_right = { region.latitude + region.latitudeDelta, region.longitude + region.longitudeDelta };

				auto bounds = ref new GeoboundingBox(bottom_right, top_left);
				mapview__->TrySetViewBoundsAsync(bounds, nullptr, get_animate() ? MapAnimationKind::Default : MapAnimationKind::None);
				mapview__->DesiredPitch = region.tilt;
			} else {
				// no deltas, use the long/lat and zoom to set view area/region
				if (region.zoom > 0) {
					mapview__->TrySetViewAsync(gp, static_cast<double>(region.zoom), nullptr, region.tilt, get_animate() ? MapAnimationKind::Default : MapAnimationKind::None);
				} else {
					mapview__->TrySetViewAsync(gp, nullptr, nullptr, region.tilt, get_animate() ? MapAnimationKind::Default : MapAnimationKind::None);
				}
			}
#endif
		}

		void View::set_showsBuildings(const bool& buildings) TITANIUM_NOEXCEPT
		{
			Titanium::Map::View::set_showsBuildings(buildings);
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			mapview__->LandmarksVisible = buildings;
#endif
		}

		void View::set_traffic(const bool& traffic) TITANIUM_NOEXCEPT
		{
			Titanium::Map::View::set_traffic(traffic);
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			mapview__->TrafficFlowVisible = traffic;
#endif
		}

		void View::addAnnotation(const Annotation_shared_ptr_t& annotation) TITANIUM_NOEXCEPT
		{
			Titanium::Map::View::addAnnotation(annotation);
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			auto native_annotation_ptr = annotation->get_object().GetPrivate<TitaniumWindows::Map::Annotation>();
			if (native_annotation_ptr != nullptr) {
				mapview__->Children->Append(native_annotation_ptr->GetMapIcon());
			}
#endif
		}

		void View::removeAnnotation(const Annotation_shared_ptr_t& annotation) TITANIUM_NOEXCEPT
		{
			Titanium::Map::View::removeAnnotation(annotation);

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			for (size_t i = 0; i < mapview__->Children->Size; i++) {
				auto element = mapview__->Children->GetAt(i);
				Windows::UI::Xaml::Controls::Grid^ icon = dynamic_cast<Windows::UI::Xaml::Controls::Grid^>(element);
				if (icon == nullptr) {
					continue; // not an annotation
				}
				// find annotation identified by title or a reference.
				auto annotation_ptr = annotation->get_object().GetPrivate<TitaniumWindows::Map::Annotation>()->GetMapIcon();
				if (icon == annotation_ptr) {
					//mapview__->MapElements->RemoveAt(i);
					mapview__->Children->RemoveAt(i);
					break;
				}
			}
#endif
		}

		void View::zoom(const uint32_t& zoom) TITANIUM_NOEXCEPT
		{
			Titanium::Map::View::zoom(zoom);
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			if (zoom != 0) // no zoom specified in the region object?
			{
				// TODO Enforce min/max zoom levels?
				mapview__->ZoomLevel = zoom;
			}
#endif
		}

	} // namespace Map
}  // namespace Titanium
