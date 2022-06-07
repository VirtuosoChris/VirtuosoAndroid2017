//
//  GLApplicationAndroid.cpp
//
//
//  Created by Chris Pugh on 12/26/15.
//
//

#include "GLApplicationAndroid.hpp"

#define ALOGE(...) __android_log_print( ANDROID_LOG_ERROR, "GLApplicationAndroid.cpp", __VA_ARGS__ )

namespace Virtuoso
{
    namespace Android
    {
        GLApplicationInterface::GLApplicationInterface() :
                onCreate([]() {}),
                onStart([]() {}),
                onStop([]() {}),
                onDestroy([]() { }),
                onWindowFocusChanged([](bool) {}),
                onConfigurationChanged([]() {}),
                onFrame([]() {}),
                onPause([]() {}),
                onResume([]() {}),
                onInput([](AInputEvent *) -> int {return 0;})
        {
        }


        void GLApplicationAndroid::constructor_loop()
        {
            int32_t lResult = 0;
            int32_t lEvents = 0;

            android_poll_source *lSource = NULL;

            std::clog << "blocking in GLApplicationAndroid constructor for window init & application resumed & focused" << std::endl;

            while (!ready())
            {
                // first arg is delay wait : -1 blocks until event appears, 0 returns instantly
                if ((lResult = ALooper_pollAll(-1, NULL, &lEvents, (void **) &lSource)) < 0)
                {
                    continue;
                }

                if (lSource != NULL)
                {
                    lSource->process(androidApp, lSource);
                }
            }

            std::clog << "blocking ended" << std::endl;
        }


        GLApplicationAndroid::~GLApplicationAndroid()
        {
            std::clog << "GLApplicationAndroid destructor" << std::endl;
            androidApp->activity->vm->DetachCurrentThread();
        }


        void GLApplicationAndroid::loop()
        {
            std::clog << "beginning frame loop" << std::endl;

            int32_t lResult = 0;
            int32_t lEvents = 0;

            android_poll_source *lSource = NULL;

            while (!androidApp->destroyRequested)
            {
                for (; ;)
                {
                    // first arg is delay wait : -1 blocks until event appears, 0 returns instantly
                    if (true == (lResult = ALooper_pollAll(0, NULL, &lEvents, (void **) &lSource) < 0))
                    {
                        break;
                    }

                    if (lSource != NULL)
                    {
                        lSource->process(androidApp, lSource);
                    }
                }

                if (ready()) // during main loop, only call render function when surface is valid, window is resumed, and focused
                {
                    EGLint oldWidth = egl.windowWidth;
                    EGLint oldHeight = egl.windowHeight;

                    // we call eglQuerySurface for width and height each frame.  This is because of a bug in android
                    // when we come back from the lock screen the surface will be created sometimes with portrait dimensions
                    // the egl surface will correct itself several frames later.
                    // We cannot simply add a poll because the ANativeWindow_getWidth method may also be incorrect when we receive the configurationChanged() method
                    // so we cannot simply detect the disparity and poll.  The simplest thing here is to update every frame and inject a resize event if the dims don't match
                    egl.updateDims();

                    if ((oldWidth != egl.windowWidth) || (oldHeight != egl.windowHeight)) {
                        std::clog << "Dimension mismatch : firing onConfigurationChanged()" <<
                        std::endl;
                        interface.onConfigurationChanged();
                    }

                    interface.onFrame();
                }
            }

            std::clog << "exited frame loop" << std::endl;
        }


        void GLApplicationAndroid::handle_cmd(struct android_app *app, int32_t cmd)
        {
            GLApplicationAndroid &application = *reinterpret_cast<GLApplicationAndroid *>(app->userData);

            try
            {
                switch (cmd)
                {
                    case APP_CMD_CONFIG_CHANGED:
                    {
                        std::clog << "onConfigurationChanged()" << std::endl;
                        application.interface.onConfigurationChanged();
                        break;
                    }
                    case APP_CMD_GAINED_FOCUS:
                    {
                        std::clog << "onWindowFocusChanged(true)" << std::endl;
                        application.interface.onWindowFocusChanged(true);
                        application.p_focused = true;
                        break;
                    }
                    case APP_CMD_LOST_FOCUS:
                    {
                        std::clog << "onWindowFocusChanged(false)" << std::endl;
                        application.interface.onWindowFocusChanged(false);
                        application.p_focused = false;
                        break;
                    }
                    case APP_CMD_DESTROY:
                    {
                        std::clog << "onDestroy()" << std::endl;
                        application.interface.onDestroy();
                        break;
                    }
                    case APP_CMD_START:
                    {
                        std::clog << "onStart()" << std::endl;
                        application.interface.onStart();
                        break;
                    }
                    case APP_CMD_STOP:
                    {
                        std::clog << "onStop()" << std::endl;
                        application.interface.onStop();
                        break;
                    }
                    case APP_CMD_WINDOW_RESIZED:
                    {
                        std::clog << "surfaceChanged()" << std::endl;
                        break;
                    }
                    case APP_CMD_TERM_WINDOW:
                    {
                        std::clog << "surfaceDestroyed()" << std::endl;
                        application.egl.unbindSurface();
                        application.egl.destroySurface();
                        application.p_initWindow = false;
                        break;
                    }
                    case APP_CMD_RESUME:
                    {
                        std::clog << "onResume()" << std::endl;
                        application.p_resumed = true;

                        if (application.p_initWindow)
                        {
                            application.interface.onResume();
                        }

                        std::clog<<"end resume callback"<<std::endl;

                        break;
                    }
                    case APP_CMD_INIT_WINDOW:
                    {
                        std::clog << "createSurface()" << std::endl;


                        application.egl.createSurface(app->window);
                        application.egl.makeCurrent();
                        application.p_initWindow = true;

                        if (application.p_resumed)
                        {
                            application.interface.onResume();
                        }

                        break;
                    }
                    case APP_CMD_PAUSE:
                    {
                        std::clog << "onPause()" << std::endl;
                        application.p_resumed = false;
                        application.interface.onPause();
                        break;
                    }
                }
            }
            catch (const std::runtime_error &ex)
            {
                std::clog << "EXCEPTION " << ex.what() << std::endl;
                ALOGE("EXCEPTION : %s\n", ex.what());
                std::terminate();
            }
            catch (const std::exception &ex)
            {
                std::clog << "GL EXCEPTION " << ex.what() << std::endl;
                ALOGE("std::exception : %s\n", ex.what());
                std::terminate();
            }
            catch (...)
            {
                std::clog<<"Unhandled catch"<<std::endl;
                ALOGE("Unhandled catch");
                std::terminate();
            }
        }


        /// Event handler that gets used until the window constructor completes successfully for the first time.
        /// We fire a reduced set of callbacks until the application is valid for rendering.
        /// This gets called in the ctor, where a loop blocks until the conditions of ready() are true
        void GLApplicationAndroid::handle_cmd_init(struct android_app *app, int32_t cmd)
        {
            GLApplicationAndroid &application = *reinterpret_cast<GLApplicationAndroid *>(app->userData);

            try
            {
                switch (cmd)
                {
                    case APP_CMD_CONFIG_CHANGED:
                    {
                        std::clog << "onConfigurationChanged()" << std::endl;
                        application.interface.onConfigurationChanged();
                        break;
                    }
                    case APP_CMD_GAINED_FOCUS:
                    {
                        std::clog << "onWindowFocusChanged(true)" << std::endl;
                        application.p_focused = true;
                        break;
                    }
                    case APP_CMD_LOST_FOCUS:
                    {
                        std::clog << "onWindowFocusChanged(false)" << std::endl;
                        application.p_focused = false;
                        break;
                    }
                    case APP_CMD_DESTROY:
                    {
                        std::clog << "onDestroy()" << std::endl;
                        application.interface.onDestroy();
                        break;
                    }
                    case APP_CMD_START:
                    {
                        std::clog << "onStart()" << std::endl;
                        application.interface.onStart();
                        break;
                    }
                    case APP_CMD_STOP:
                    {
                        std::clog << "onStop()" << std::endl;
                        application.interface.onStop();
                        break;
                    }
                    case APP_CMD_TERM_WINDOW:
                    {
                        std::clog << "APP_CMD_TERM_WINDOW init" << std::endl;
                        application.p_initWindow = false;
                        break;
                    }
                    case APP_CMD_RESUME:
                    {
                        std::clog << "APP_CMD_RESUME init" << std::endl;
                        application.p_resumed = true;
                        break;
                    }
                    case APP_CMD_INIT_WINDOW:
                    {
                        std::clog << "APP_CMD_INIT_WINDOW init" << std::endl;
                        application.p_initWindow = true;

                        break;
                    }
                    case APP_CMD_PAUSE:
                    {
                        std::clog << "APP_CMD_PAUSE init" << std::endl;
                        application.p_resumed = false;
                        break;
                    }
                    case APP_CMD_WINDOW_RESIZED:
                    {
                        std::clog << "surfaceChanged()" << std::endl;
                        break;
                    }
                }
            }
            catch (const std::runtime_error &ex)
            {
                std::clog << "EXCEPTION " << ex.what() << std::endl;
                ALOGE("std::runtime_error : %s\n", ex.what());
                std::terminate();
            }
            catch (const std::exception &ex)
            {
                std::clog << "GL EXCEPTION " << ex.what() << std::endl;
                ALOGE("std::exception : %s\n", ex.what());
                std::terminate();
            }
            catch (...)
            {
                std::clog<<"Unhandled catch"<<std::endl;
                ALOGE("Unhandled catch");
                std::terminate();
            }
        }

        GLApplicationAndroid::GLApplicationAndroid(struct android_app *appIn,
                                                   const GLApplicationInterface &anInterfaceIn,
                                                   const WindowFormat &formatIn,
                                                   int glesVersion) :
                androidApp(appIn),
                p_resumed(false),
                p_initWindow(false),
                p_focused(false),
                interface(anInterfaceIn),
                resumed(p_resumed),
                initWindow(p_initWindow),
                focused(p_focused)
        {
            app_dummy();

            // allow native activity thread to communicate with Java
            androidApp->activity->vm->AttachCurrentThread(&env, NULL);
            androidApp->userData = this;
            androidApp->onAppCmd = GLApplicationAndroid::handle_cmd_init;

            std::clog << "onCreate()" << std::endl;
            interface.onCreate();

            constructor_loop();

            std::clog << "Creating EGL context, initializing surface, and making context and surface current" << std::endl;

            egl = EGLAndroid(androidApp->window, formatIn, glesVersion);
            egl.createSurface(androidApp->window);
            egl.makeCurrent();

            androidApp->onAppCmd = GLApplicationAndroid::handle_cmd;
            androidApp->onInputEvent = GLApplicationAndroid::onInputEventBase;
        }
    }
}
