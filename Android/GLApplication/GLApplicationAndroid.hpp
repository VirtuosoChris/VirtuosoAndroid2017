//
//  GLApplicationAndroid.hpp
//
//
//  Created by Chris Pugh on 12/26/15.
//
//

#ifndef GLApplicationAndroid_hpp
#define GLApplicationAndroid_hpp

#include <functional>

#include <android/sensor.h>
#include <android/log.h>
#include <android/native_window_jni.h>	// for native window JNI
#include <android_native_app_glue.h>

#include "../EGL/EGLAndroid.h"

namespace Virtuoso
{
    namespace Android
    {
        /// Event / input interface for Android GL Application class GLApplicationAndroid
        struct GLApplicationInterface
        {
            std::function <void(void)> onCreate; ///< Called at GLApplicationAndroid ctor begin.  Window and surface not valid at this point
            std::function <void(void)> onStart; ///< Called after onCreate() or after onStop().
            std::function <void(void)> onStop;
            std::function <void(void)> onDestroy; ///< release all native resources here

            /// On lost focus, pause game, stop audio, and consider stopping or throttling rendering
            std::function <void(bool)> onWindowFocusChanged;
            std::function <void(void)> onConfigurationChanged;

            std::function <void(void)> onFrame; ///< update loop / render frame method
            std::function <void(void)> onPause; ///< should stop or reduce rendering here.
            std::function <void(void)> onResume; ///< should prepare to begin rendering again.  Should not start until also have focus.
            std::function <int(AInputEvent *)> onInput; ///\todo we should have an input handler mechanism later on

            /// initialize interface to do nothing functions
            GLApplicationInterface();
        };

        /// A native OpenGL application for Android
        /// Has a GLApplicationInterface as a member.  The use case is to construct, populate the interface callbacks,
        /// Handles the lifecycle of an Android GL native activity
        class GLApplicationAndroid
        {
        protected:


            android_app *androidApp;

            bool p_resumed;
            bool p_initWindow;
            bool p_focused;

        private:

            void constructor_loop();

            static void handle_cmd_init(struct android_app *app, int32_t cmd);

            static void handle_cmd(struct android_app *app, int32_t cmd);

            inline bool ready() { return (initWindow && resumed && focused); }

            static int32_t onInputEventBase(struct android_app* app, AInputEvent* event)
            {
                return (reinterpret_cast<GLApplicationAndroid*>(app->userData))->interface.onInput(event);
            };

        public:

            JNIEnv *env;

            EGLAndroid egl; ///< EGL context and surface

            GLApplicationInterface interface; ///< Event callback interface for the application

            const bool &resumed; ///< Application in resumed state?
            const bool &initWindow; ///< Window pointer is valid?
            const bool &focused; ///< Window has focus

            /// Constructor.  Blocks until valid window, EGL context / surface, and app in resumed state.
            GLApplicationAndroid(struct android_app *appIn,
                                 const GLApplicationInterface &anInterfaceIn,
                                 const WindowFormat &format,
                                int glesVersion);

            /// Application main loop.  Pumps event handler and calls appropriate callbacks in "interface"
            void loop();

            virtual ~GLApplicationAndroid();
        };
    }
}
#endif /* GLApplicationAndroid_hpp */
