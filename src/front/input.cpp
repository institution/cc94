#include "input.hpp"

namespace front {
	namespace input {
	
		char const* get_eventtype_name(EventType x) {
			switch (x) {
				case EventNone: return "EventNone";

				// application
				case EventQuit: return "EventQuit";

				// window
				case EventWindowEvent: return "EventWindowEvent";
				case EventSysWMEvent: return "EventSysWMEvent";

				// keyboard
				case EventKeyDown: return "EventKeyDown";
				case EventKeyUp: return "EventKeyUp";
				case EventTextEditing: return "EventTextEditing";
				case EventTextInput: return "EventTextInput";

				// mouse
				case EventMouseMotion: return "EventMouseMotion";
				case EventMouseButtonDown: return "EventMouseButtonDown";
				case EventMouseButtonUp: return "EventMouseButtonUp";
				case EventMouseWheel: return "EventMouseWheel";

				// joystick
				case EventJoyAxisMotion: return "EventJoyAxisMotion";
				case EventJoyBallMotion: return "EventJoyBallMotion";
				case EventJoyHatMotion: return "EventJoyHatMotion";
				case EventJoyButtonDown: return "EventJoyButtonDown";
				case EventJoyButtonUp: return "EventJoyButtonUp";
				case EventJoyDeviceAdded: return "EventJoyDeviceAdded";
				case EventJoyDeviceRemoved: return "EventJoyDeviceRemoved";

				// game controller
				case EventControllerAxisMotion: return "EventControllerAxisMotion";
				case EventControllerButtonDown: return "EventControllerButtonDown";
				case EventControllerButtonUp: return "EventControllerButtonUp";
				case EventControllerDeviceAdded: return "EventControllerDeviceAdded";
				case EventControllerDeviceRemoved: return "EventControllerDeviceRemoved";
				case EventControllerDeviceMapped: return "EventControllerDeviceMapped";

				// touch 
				case EventFingerDown: return "EventFingerDown";
				case EventFingerUp: return "EventFingerUp";
				case EventFingerMotion: return "EventFingerMotion";

				// gesture
				case EventDollarGesture: return "EventDollarGesture";
				case EventDollarRecord: return "EventDollarRecord";
				case EventMultiGesture: return "EventMultiGesture";

				// clipboard
				case EventClipboardUpdate: return "EventClipboardUpdate";

				// file drop
				case EventDropFile: return "EventDropFile";

				// audio hotplug
				case EventAudioDeviceAdded: return "EventAudioDeviceAdded";
				case EventAudioDeviceRemoved: return "EventAudioDeviceRemoved";

				// render
			//	case EventRenderTargetsReset: return "EventRenderTargetsReset";
				case EventRenderDeviceReset: return "EventRenderDeviceReset";

				// user
				case EventPress: return "EventPress";
				case EventRelease: return "EventRelease";
				case EventMotion: return "EventMotion";

				case EventUser: return "EventUser";

			//	case EventEnd: return "EventEnd";
				
				default: return "EventNoName";
			}

		}




		void print_window_event(SDL_Event const& event)
		{
			if (event.type == SDL_WINDOWEVENT) {
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SHOWN:
					print("Window %|| shown\n", event.window.windowID);
					break;
				case SDL_WINDOWEVENT_HIDDEN:
					print("Window %|| hidden\n", event.window.windowID);
					break;
				case SDL_WINDOWEVENT_EXPOSED:
					print("Window %|| exposed\n", event.window.windowID);
					break;
				case SDL_WINDOWEVENT_MOVED:
					print("Window %|| moved to %||,%||\n",
							event.window.windowID, event.window.data1,
							event.window.data2);
					break;
				case SDL_WINDOWEVENT_RESIZED:
					print("Window %|| resized to %||x%||\n",
							event.window.windowID, event.window.data1,
							event.window.data2);
					break;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					print("Window %|| size changed to %||x%||\n",
							event.window.windowID, event.window.data1,
							event.window.data2);
					break;
				case SDL_WINDOWEVENT_MINIMIZED:
					print("Window %|| minimized\n", event.window.windowID);
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					print("Window %|| maximized\n", event.window.windowID);
					break;
				case SDL_WINDOWEVENT_RESTORED:
					print("Window %|| restored\n", event.window.windowID);
					break;
				case SDL_WINDOWEVENT_ENTER:
					print("Mouse entered window %||\n",
							event.window.windowID);
					break;
				case SDL_WINDOWEVENT_LEAVE:
					print("Mouse left window %||\n", event.window.windowID);
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					print("Window %|| gained keyboard focus\n",
							event.window.windowID);
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					print("Window %|| lost keyboard focus\n",
							event.window.windowID);
					break;
				case SDL_WINDOWEVENT_CLOSE:
					print("Window %|| closed\n", event.window.windowID);
					break;
				default:
					print("Window %|| got unknown event %||\n",
							event.window.windowID, event.window.event);
					break;
				}
			}
		}



		char const* get_key_name(Key x) {
			switch (x) {
				case KeyNone: return "KeyNone";
				case KeyLMB: return "KeyLMB";
				case KeyMMB: return "KeyMMB";
				case KeyRMB: return "KeyRMB";
				case KeyNum0: return "KeyNum0";
				case KeyNum1: return "KeyNum1";
				case KeyNum2: return "KeyNum2";
				case KeyNum3: return "KeyNum3";
				case KeyNum4: return "KeyNum4";
				case KeyNum5: return "KeyNum5";
				case KeyNum6: return "KeyNum6";
				case KeyNum7: return "KeyNum7";
				case KeyNum8: return "KeyNum8";
				case KeyNum9: return "KeyNum9";
				case KeyA: return "KeyA";
				case KeyQuote: return "KeyQuote";
				case KeyB: return "KeyB";
				case KeyBackslash: return "KeyBackslash";
				case KeyBackspace: return "KeyBackspace";
				case KeyC: return "KeyC";
				case KeyCapsLock: return "KeyCapsLock";
				case KeyComma: return "KeyComma";
				case KeyD: return "KeyD";
				case KeyDelete: return "KeyDelete";
				case KeyDown: return "KeyDown";
				case KeyE: return "KeyE";
				case KeyEnd: return "KeyEnd";
				case KeyEquals: return "KeyEquals";
				case KeyEsc: return "KeyEsc";
				case KeyF: return "KeyF";
				case KeyF1: return "KeyF1";
				case KeyF10: return "KeyF10";
				case KeyF11: return "KeyF11";
				case KeyF12: return "KeyF12";
				case KeyF13: return "KeyF13";
				case KeyF14: return "KeyF14";
				case KeyF15: return "KeyF15";
				case KeyF16: return "KeyF16";
				case KeyF17: return "KeyF17";
				case KeyF18: return "KeyF18";
				case KeyF19: return "KeyF19";
				case KeyF2: return "KeyF2";
				case KeyF20: return "KeyF20";
				case KeyF21: return "KeyF21";
				case KeyF22: return "KeyF22";
				case KeyF23: return "KeyF23";
				case KeyF24: return "KeyF24";
				case KeyF3: return "KeyF3";
				case KeyF4: return "KeyF4";
				case KeyF5: return "KeyF5";
				case KeyF6: return "KeyF6";
				case KeyF7: return "KeyF7";
				case KeyF8: return "KeyF8";
				case KeyF9: return "KeyF9";
				case KeyFIND: return "KeyFIND";
				case KeyG: return "KeyG";
				case KeyBackquote: return "KeyBackquote";
				case KeyH: return "KeyH";
				case KeyHome: return "KeyHome";
				case KeyI: return "KeyI";
				case KeyInsert: return "KeyInsert";
				case KeyJ: return "KeyJ";
				case KeyK: return "KeyK";
				case KeyNumpad0: return "KeyNumpad0";
				case KeyNumpad00: return "KeyNumpad00";
				case KeyNumpad000: return "KeyNumpad000";
				case KeyNumpad1: return "KeyNumpad1";
				case KeyNumpad2: return "KeyNumpad2";
				case KeyNumpad3: return "KeyNumpad3";
				case KeyNumpad4: return "KeyNumpad4";
				case KeyNumpad5: return "KeyNumpad5";
				case KeyNumpad6: return "KeyNumpad6";
				case KeyNumpad7: return "KeyNumpad7";
				case KeyNumpad8: return "KeyNumpad8";
				case KeyNumpad9: return "KeyNumpad9";
				case KeyNumpadA: return "KeyNumpadA";
				case KeyNumpadAmpersand: return "KeyNumpadAmpersand";
				case KeyNumpadAt: return "KeyNumpadAt";
				case KeyNumpadBackspace: return "KeyNumpadBackspace";
				case KeyNumpadBinary: return "KeyNumpadBinary";
				case KeyNumpadColon: return "KeyNumpadColon";
				case KeyNumpadComma: return "KeyNumpadComma";
				case KeyNumpadDecimal: return "KeyNumpadDecimal";
				case KeyNumpadDivide: return "KeyNumpadDivide";
				case KeyNumpadEnter: return "KeyNumpadEnter";
				case KeyNumpadMinus: return "KeyNumpadMinus";
				case KeyNumpadMultiply: return "KeyNumpadMultiply";
				case KeyNumpadPeriod: return "KeyNumpadPeriod";
				case KeyNumpadPlus: return "KeyNumpadPlus";
				case KeyNumpadVerticalBar: return "KeyNumpadVerticalBar";
				case KeyNumpadXor: return "KeyNumpadXor";
				case KeyL: return "KeyL";
				case KeyLAlt: return "KeyLAlt";
				case KeyLCtrl: return "KeyLCtrl";
				case KeyLeft: return "KeyLeft";
				case KeyLEFTBRACKET: return "KeyLEFTBRACKET";
				case KeyLGUI: return "KeyLGUI";
				case KeyLShift: return "KeyLShift";
				case KeyM: return "KeyM";
				case KeyMenu: return "KeyMenu";
				case KeyMinus: return "KeyMinus";
				case KeyMode: return "KeyMode";
				case KeyMute: return "KeyMute";
				case KeyN: return "KeyN";
				case KeyNumLockClear: return "KeyNumLockClear";
				case KeyO: return "KeyO";
				case KeyOper: return "KeyOper";
				case KeyOut: return "KeyOut";
				case KeyP: return "KeyP";
				case KeyPageDown: return "KeyPageDown";
				case KeyPageUp: return "KeyPageUp";
				case KeyPaste: return "KeyPaste";
				case KeyPause: return "KeyPause";
				case KeyPeriod: return "KeyPeriod";
				case KeyPower: return "KeyPower";
				case KeyPrintScreen: return "KeyPrintScreen";
				case KeyPrior: return "KeyPrior";
				case KeyQ: return "KeyQ";
				case KeyR: return "KeyR";
				case KeyRAlt: return "KeyRAlt";
				case KeyRCtrl: return "KeyRCtrl";
				case KeyEnter: return "KeyEnter";
				case KeyRETURN2: return "KeyRETURN2";
				case KeyRGui: return "KeyRGui";
				case KeyRight: return "KeyRight";
				case KeyRightBracket: return "KeyRightBracket";
				case KeyRShift: return "KeyRShift";
				case KeyS: return "KeyS";
				case KeyScrollLock: return "KeyScrollLock";
				case KeySelect: return "KeySelect";
				case KeySemiColon: return "KeySemiColon";
				case KeySeparator: return "KeySeparator";
				case KeySlash: return "KeySlash";
				case KeySleep: return "KeySleep";
				case KeySpace: return "KeySpace";
				case KeyStop: return "KeyStop";
				case KeySysReq: return "KeySysReq";
				case KeyT: return "KeyT";
				case KeyTab: return "KeyTab";
				case KeyU: return "KeyU";
				case KeyUndo: return "KeyUndo";
				case KeyUp: return "KeyUp";
				case KeyV: return "KeyV";
				case KeyVolumeDown: return "KeyVolumeDown";
				case KeyVolumeUp: return "KeyVolumeUp";
				case KeyW: return "KeyW";
				case KeyWWW: return "KeyWWW";
				case KeyX: return "KeyX";
				case KeyY: return "KeyY";
				case KeyZ: return "KeyZ";
				default: return "<KeyNoName>";
			}
		}
		
		
		char const* get_mod_name(Mod x) 
		{
			switch (x) {
				case ModNone: return "ModNone";
				case ModLShift: return "ModLShift";
				case ModRShift: return "ModRShift";
				case ModLCtrl: return "ModLCtrl";
				case ModRCtrl: return "ModRCtrl";
				case ModLAlt: return "ModLAlt";
				case ModRAlt: return "ModRAlt";
				case ModLGui: return "ModLGui";
				case ModRGui: return "ModRGui";
				case ModNum: return "ModNum";
				case ModCaps: return "ModCaps";
				case ModMode: return "ModMode";
				case ModGui: return "ModGui";
				case ModLMB: return "ModLMB";
				case ModMMB: return "ModMMB";
				case ModRMB: return "ModRMB";
				default: return "<ModNoName>";
			}
		}
		
		void print_mod_table() 
		{
		
			print("MT n\tname\n");
			for (uint32_t i=0; i<32; ++i) {
				auto f = uint32_t(1 << i);
				print("MT %||\t%||\n", i, get_mod_name(f));				
			}						
			
		}
		
	}//input
}//front
