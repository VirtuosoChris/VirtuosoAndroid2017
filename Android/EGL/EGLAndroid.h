//
// Created by Chris Pugh on 9/8/15.
//

#ifndef VIRTUOSO_EGLANDROID_H
#define VIRTUOSO_EGLANDROID_H

#include <EGL/egl.h>
#include <string>
#include <stdexcept>

#include <iostream>

#if !defined( EGL_OPENGL_ES3_BIT_KHR )
#define EGL_OPENGL_ES3_BIT_KHR 0x0040
#endif

namespace Virtuoso
{
    namespace Android
    {

        /// Describes the format of the OpenGL framebuffer to be created
        struct WindowFormat
        {
            EGLint bitsRed;
            EGLint bitsGreen;
            EGLint bitsBlue;
            EGLint bitsAlpha;
            EGLint bitsDepth;
            EGLint bitsStencil;
            EGLint samples;

            WindowFormat() : bitsRed(8), bitsGreen(8), bitsBlue(8),
                             bitsAlpha(8), bitsDepth(24), bitsStencil(0),
                             samples(0)
            {
            }
        };

        class EGLAndroid
        {
        private:

            EGLDisplay display;
            EGLSurface surface;
            EGLContext context;

            EGLint p_windowWidth;
            EGLint p_windowHeight;

            EGLConfig lConfig;

            EGLint p_glesVersion;

            void init(ANativeWindow *window, const EGLint *attributes, EGLint glesVersion = 3);

            void init(ANativeWindow *window, WindowFormat format, EGLint glesVersion = 3);

        public:

            const EGLint &windowWidth;
            const EGLint &windowHeight;
            const EGLint &glesVersion;

            const EGLDisplay& getDisplay() const {return display;}
            const EGLDisplay& getSurface() const {return surface;}
            const EGLDisplay& getContext() const {return context;}

            EGLAndroid &operator=(EGLAndroid &&in);

            EGLAndroid(); ///< empty ctor that initializes everything to "null" or equivalent values

            EGLAndroid(ANativeWindow *window, const EGLint *attributes, EGLint glesVersion = 3);

            EGLAndroid(ANativeWindow *window, WindowFormat format, EGLint glesVersion = 3);

            ~EGLAndroid();

            void updateDims();

            void unbindSurface();

            void createSurface(ANativeWindow *window);

            void destroySurface();

            inline void makeCurrent() const;

            inline EGLBoolean swapBuffers() const;

            inline EGLBoolean swapInterval(EGLint interval) const;

            inline operator bool() const;

            std::string availableConfigs() const; ///< Useful for logging.  Dumps all configs ///\todo pass in target parameter list to match instead of dumping all
        };

#include "EGLAndroid.inl"

    }
}

#endif //VIRTUOSO_EGLANDROID_H
