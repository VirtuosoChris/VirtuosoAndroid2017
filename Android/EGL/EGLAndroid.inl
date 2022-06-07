inline void EGLAndroid::destroySurface()
{
    std::clog << "eglDestroySurface " ;

    if (surface != EGL_NO_SURFACE)
    {
        std::clog << " : surface destroyed \n";
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
    else
    {
       std::clog << " : no surface to destroy \n";
    }

    std::clog << std::endl;
}

inline void EGLAndroid::makeCurrent() const
{
    std::clog << "eglMakeCurrent() " << std::endl;
    if (EGL_TRUE != eglMakeCurrent (display, surface, surface, context))
        throw std::runtime_error("unable to make context current");
}

inline EGLBoolean EGLAndroid::swapBuffers() const
{
    return eglSwapBuffers(display, surface);
}

inline EGLBoolean EGLAndroid::swapInterval(EGLint interval) const
{
    return eglSwapInterval(display, interval);
}

inline EGLAndroid::operator bool() const
{
    return (context != EGL_NO_CONTEXT) && (surface != EGL_NO_SURFACE);
}

inline void EGLAndroid::unbindSurface()
{
    eglMakeCurrent(display, NULL, NULL, context);
}

inline void EGLAndroid::updateDims()
{
    if (surface != EGL_NO_SURFACE)
    {
        eglQuerySurface(display,surface,EGL_WIDTH,&p_windowWidth);
        eglQuerySurface(display,surface,EGL_HEIGHT,&p_windowHeight);
    }
}
