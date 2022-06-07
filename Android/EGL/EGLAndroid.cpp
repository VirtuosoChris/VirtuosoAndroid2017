//
// Created by Chris Pugh on 9/8/15.
//

#include "EGLAndroid.h"
#include <sstream>
#include <android/native_window_jni.h>

#include <android/log.h>
#define LOG_TAG "Virtuoso"
#define ALOGV(...) __android_log_print( ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__ )
#define ALOGE(...) __android_log_print( ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )


namespace Virtuoso
{
    namespace Android
    {
        void EGLAndroid::createSurface(ANativeWindow * window)
        {

            ALOGE("EGL CREATE SURFACE");

            EGLint lFormat = 0;

            if (!eglGetConfigAttrib(display, lConfig, EGL_NATIVE_VISUAL_ID, &lFormat))
                throw std::runtime_error("eglGetConfigAttrib() error : failed to return EGL_NATIVE_VISUAL_ID");

            ANativeWindow_setBuffersGeometry(window, 0, 0, lFormat);

            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);


            surface = eglCreateWindowSurface(display, lConfig, window, NULL);

            if (surface == EGL_NO_SURFACE)
                throw std::runtime_error("eglCreateWindowSurface() : unable to make surface");

            if (!eglQuerySurface(display, surface, EGL_WIDTH, &p_windowWidth)
                || !eglQuerySurface(display, surface, EGL_HEIGHT, &p_windowHeight)
                || (windowWidth <= 0) || (windowHeight <= 0))
                throw std::runtime_error("eglQuerySurface() : created surface has zero area");

            std::clog << "\nCreated EGL surface of size " << windowWidth << " " << windowHeight << "\n" << std::endl;
        }


        EGLAndroid &EGLAndroid::operator=(EGLAndroid &&in)
        {
            display = in.display;
            in.display = EGL_NO_DISPLAY;
            surface = in.surface;
            in.surface = EGL_NO_SURFACE;
            context = in.context;
            in.context = EGL_NO_CONTEXT;
            p_windowHeight = in.p_windowHeight;
            p_windowWidth = in.p_windowWidth;
            lConfig = in.lConfig;
            p_glesVersion = in.p_glesVersion;
            return *this;
        }


        EGLAndroid::EGLAndroid()
                : display(EGL_NO_DISPLAY),
                  surface(EGL_NO_SURFACE),
                  context(EGL_NO_CONTEXT),
                  lConfig(0),
                  p_windowWidth(0),
                  p_windowHeight(0),
                  p_glesVersion(0),
                  windowWidth(p_windowWidth),
                  windowHeight(p_windowHeight),
                  glesVersion(p_glesVersion)
        {
        }


        std::string EGLAndroid::availableConfigs() const
        {
            EGLint numConfigs;
            eglGetConfigs(display, NULL, 0xffff, &numConfigs);
            EGLConfig *configs = new EGLConfig[numConfigs];
            eglGetConfigs(display, configs, numConfigs, &numConfigs);

            std::stringstream sstr;
            for (unsigned int i = 0; i < numConfigs; i++)
            {
                EGLint value = 0;

                if (eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &value))
                {
                    sstr << "EGL_BUFFER_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_COLOR_BUFFER_TYPE, &value))
                {
                    sstr << "EGL_COLOR_BUFFER_TYPE : " <<
                    ((value == EGL_RGB_BUFFER) ? "EGL_RGB_BUFFER" : "EGL_LUMINANCE_BUFFER") << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &value))
                {
                    sstr << "EGL_RED_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &value))
                {
                    sstr << "EGL_GREEN_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &value))
                {
                    sstr << "EGL_BLUE_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &value))
                {
                    sstr << "EGL_ALPHA_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_SAMPLE_BUFFERS, &value))
                {
                    sstr << "EGL_SAMPLE_BUFFERS : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &value))
                {
                    sstr << "EGL_SAMPLES : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE, &value))
                {
                    sstr << "EGL_STENCIL_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_MAX_SWAP_INTERVAL, &value))
                {
                    sstr << "EGL_MAX_SWAP_INTERVAL : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_MIN_SWAP_INTERVAL, &value))
                {
                    sstr << "EGL_MIN_SWAP_INTERVAL : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_NATIVE_RENDERABLE, &value))
                {
                    sstr << "EGL_NATIVE_RENDERABLE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &value))
                {
                    sstr << "EGL_DEPTH_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_LEVEL, &value))
                {
                    sstr << "EGL_LEVEL : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_LUMINANCE_SIZE, &value))
                {
                    sstr << "EGL_LUMINANCE_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_ALPHA_MASK_SIZE, &value))
                {
                    sstr << "EGL_ALPHA_MASK_SIZE : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_BIND_TO_TEXTURE_RGB, &value))
                {
                    sstr << "EGL_BIND_TO_TEXTURE_RGB : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_BIND_TO_TEXTURE_RGBA, &value))
                {
                    sstr << "EGL_BIND_TO_TEXTURE_RGBA : " << value << '\n';
                }

                if (eglGetConfigAttrib(display, configs[i], EGL_CONFIG_CAVEAT, &value))
                {
                    switch (value)
                    {
                        case EGL_NONE:
                            break;
                        case EGL_SLOW_CONFIG:
                            sstr << "EGL_CONFIG_CAVEAT : EGL_SLOW_CONFIG" << '\n';
                            break;
#ifdef EGL_NON_CONFORMANT
                        case EGL_NON_CONFORMANT:
                            sstr<<"EGL_CONFIG_CAVEAT : EGL_NON_CONFORMANT"<<'\n';
                            break;
#endif
                    }
                }

                sstr << "\n\n--------------------------\n\n";
            }

            delete[]configs;

            return sstr.str();
        }


        void EGLAndroid::init(ANativeWindow *window, const EGLint *attributes, EGLint glesVersion)
        {
            EGLint lErrorResult;

            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

            if (display == EGL_NO_DISPLAY) throw std::runtime_error("eglGetDisplay() : failed to get display");

            if (!eglInitialize(display, NULL, NULL))
                throw std::runtime_error("eglInitialize() failed");

            EGLint numConfigs = 0;

            eglGetConfigs(display, NULL, 0xffff, &numConfigs);

            EGLConfig* configs = new EGLConfig[numConfigs];

            if (eglGetConfigs(display, configs, 0xffff, &numConfigs) == EGL_FALSE)
            {
                throw std::runtime_error("eglGetConfigs() : failed to query MAX_CONFIGS");
            }

            if (numConfigs < 1)
                throw std::runtime_error("eglGetConfigs(): returned 0 for numConfigs");

            // we are doing a manual matching for the EGL configuraion properties here;
            // this prevents the OS from overriding some of our settings
            // for example, GearVR front buffer should not be mutisampled because it's wasteful but the OS settings could
            // override this if we used eglChooseConfig()
            for (int i = 0; i < numConfigs; i++)
            {
                EGLint value = 0;

                int j = 0;
                for (; attributes[j] != EGL_NONE; j += 2)
                {
                    eglGetConfigAttrib(display, configs[i], attributes[j], &value);

                    bool matchBitfield = false;

                    // we match the positive bits of the bitfield attributes, per the spec for eglChooseConfig
                    switch (attributes[j])
                    {
                        case EGL_SURFACE_TYPE:
                        case EGL_CONFORMANT:
                        case EGL_RENDERABLE_TYPE:
                            matchBitfield = true;
                        default:
                            break;
                    }

                    if (matchBitfield)
                    {
                        if ((value & attributes[j]) != attributes[j])
                        {
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (value != attributes[j + 1])
                    {
                        break;
                    }
                }
                if (attributes[j] == EGL_NONE)
                {
                    lConfig = configs[i];
                    break;
                }
            }

            if (!lConfig)
            {
                std::clog << "Egl init : exact egl config not found.  using eglChooseConfig" << std::endl;

                if (!eglChooseConfig(display, attributes, &lConfig, 1, &numConfigs))
                    throw std::runtime_error("choose config failed");

                if (numConfigs < 1)
                    throw std::runtime_error("eglChooseConfig(): matching config not found");
            }

            EGLint contextAttrs[] = {
                    EGL_CONTEXT_CLIENT_VERSION, glesVersion,
                    EGL_NONE
            };

            context = eglCreateContext(display, lConfig, EGL_NO_CONTEXT, contextAttrs);

            if (context == EGL_NO_CONTEXT) throw std::runtime_error("unable to create egl context");

            delete []configs;
        }


        EGLAndroid::EGLAndroid(ANativeWindow *window, WindowFormat format, EGLint glesVersionIn) :
                display(EGL_NO_DISPLAY),
                surface(EGL_NO_SURFACE),
                context(EGL_NO_CONTEXT),
                lConfig(0),
                p_windowWidth(0),
                p_windowHeight(0),
                p_glesVersion(glesVersionIn),
                windowWidth(p_windowWidth),
                windowHeight(p_windowHeight),
                glesVersion(p_glesVersion)
        {
            init(window, format);
        }


        EGLAndroid::EGLAndroid(ANativeWindow *window, const EGLint *attribs, EGLint glesVersionIn) :
                display(EGL_NO_DISPLAY),
                surface(EGL_NO_SURFACE),
                context(EGL_NO_CONTEXT),
                lConfig(0),
                p_windowWidth(0),
                p_windowHeight(0),
                p_glesVersion(glesVersionIn),
                windowWidth(p_windowWidth),
                windowHeight(p_windowHeight),
                glesVersion(p_glesVersion)
        {
            init(window, attribs, glesVersion);
        }


        void EGLAndroid::init(ANativeWindow *window, WindowFormat format, EGLint glesVersion)
        {
            EGLint renderableBit=0;

            switch (glesVersion)
            {
                case 1 :
                    renderableBit = EGL_OPENGL_ES_BIT;
                    break;
                case 2:
                    renderableBit = EGL_OPENGL_ES2_BIT;
                    break;
                case 3:
                    renderableBit = EGL_OPENGL_ES3_BIT_KHR;
                    break;
                default:
                    throw std::runtime_error("Unsupported OpenGL ES Version");
            }

            const EGLint lAttributes[] = {
                    EGL_BLUE_SIZE, format.bitsBlue,
                    EGL_GREEN_SIZE, format.bitsGreen,
                    EGL_RED_SIZE, format.bitsRed,
                    EGL_ALPHA_SIZE, format.bitsAlpha,
                    EGL_DEPTH_SIZE, format.bitsDepth,
                    EGL_STENCIL_SIZE, format.bitsStencil,
                    EGL_SAMPLE_BUFFERS, format.samples ? (EGLint) 1 : (EGLint) 0,
                    EGL_SAMPLES, format.samples,
                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT, ///\todo hack
                    EGL_RENDERABLE_TYPE, renderableBit,
                    EGL_NONE
            };

            init(window, lAttributes, glesVersion);
        }


        EGLAndroid::~EGLAndroid()
        {
            std::clog << "EGLAndroid destructor" << std::endl;
            if (display != EGL_NO_DISPLAY)
            {
                eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

                if (context != EGL_NO_CONTEXT)
                {
                    eglDestroyContext(display, context);
                    context = EGL_NO_CONTEXT;
                }


                destroySurface();

                eglTerminate(display);
                display = EGL_NO_DISPLAY;
            }
        }
    }
}


