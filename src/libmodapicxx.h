/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPICXX_LIBMODAPICXX_H
#define SHAREMIND_LIBMODAPICXX_LIBMODAPICXX_H

#include <cassert>
#include <exception>
#include <new>
#include <sharemind/compiler-support/GccPR54526.h>
#include <type_traits>
#include "libmodapi.h"


namespace sharemind {

/*******************************************************************************
  Some type aliases
*******************************************************************************/

using SyscallCallable = ::SharemindSyscallCallable;
using SyscallWrapper = ::SharemindSyscallWrapper;
using Facility = ::SharemindFacility;
using ModuleApiError = ::SharemindModuleApiError;

class ModuleApi;
class Module;
class Syscall;
class Pdk;
class Pd;
class Pdpi;


/*******************************************************************************
  Some helper macros
*******************************************************************************/

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS__(ClassName,fF,FF) \
    inline void set ## FF(const char * name, \
                          void * facility, \
                          void * context = nullptr) \
            __attribute__ ((nonnull(2))) \
    { \
        ::Sharemind ## ClassName ## _set ## FF(m_c, \
                                               (assert(name), name), \
                                               facility, \
                                               context); \
    } \
    inline bool unset ## FF(const char * name) noexcept \
            __attribute__ ((nonnull(2))) \
    { \
        return ::Sharemind ## ClassName ## _unset ## FF(m_c, \
                                                        (assert(name), name)); \
    } \
    inline const SharemindFacility * fF(const char * name) const noexcept \
            __attribute__ ((nonnull(2))) \
    { return ::Sharemind ## ClassName ## _ ## fF(m_c, (assert(name), name)); }

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(ClassName,fN,FN) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS__(ClassName, \
                                                        fN ## Facility, \
                                                        FN ## Facility)

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_SELF_FACILITY_FUNCTIONS(ClassName) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS__(ClassName, \
                                                        facility, \
                                                        Facility)

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_PDCHILD_STUFF(ClassName) \
    inline size_t numPds() const noexcept \
{ return ::Sharemind ## ClassName ## _numPds(m_c); } \
    inline Pd * pd(size_t index) const noexcept { \
        return Detail::libmodapi::optChild( \
                    ::Sharemind ## ClassName ## _pd(m_c, index)); \
    } \
    inline Pd * findPd(const char * name) \
            const noexcept __attribute__ ((nonnull(2))) \
    { \
        assert(name); \
        return Detail::libmodapi::optChild( \
                    ::Sharemind ## ClassName ## _findPd(m_c, name)); \
    }

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_PDK_AND_SYSCALL_CHILD_STUFF(ClassName) \
    inline size_t numSyscalls() const noexcept \
    { return ::Sharemind ## ClassName ## _numSyscalls(m_c); } \
    inline Syscall * syscall(size_t index) const noexcept { \
        return Detail::libmodapi::optChild( \
                    ::Sharemind ## ClassName ## _syscall(m_c, index)); \
    } \
    inline Syscall * findSyscall(const char * const signature) \
            const noexcept __attribute__ ((nonnull(2))) \
    { \
        assert(signature); \
        return Detail::libmodapi::optChild( \
                    ::Sharemind ## ClassName ## _findSyscall(m_c, signature));\
    } \
    inline SyscallWrapper syscallWrapper(const char * const signature) \
            const noexcept __attribute__ ((nonnull(2))) \
    { \
        assert(signature); \
        return ::Sharemind ## ClassName ## _syscallWrapper(m_c, signature); \
    } \
    inline size_t numPdks() const noexcept \
    { return ::Sharemind ## ClassName ## _numPdks(m_c); } \
    inline Pdk * pdk(size_t index) const noexcept { \
        return Detail::libmodapi::optChild( \
                    ::Sharemind ## ClassName ## _pdk(m_c, index)); \
    } \
    inline Pdk * findPdk(const char * name) \
            const noexcept __attribute__ ((nonnull(2))) \
    { \
        assert(name); \
        return Detail::libmodapi::optChild( \
                    ::Sharemind ## ClassName ## _findPdk(m_c, name)); \
    }

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(ClassName,Parent,parent) \
    inline Parent * parent() const noexcept { \
        return Detail::libmodapi::mustTag( \
                ::Sharemind ## ClassName ## _ ## parent(m_c)); \
    }

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS(ClassName) \
    inline ::Sharemind ## ClassName * cPtr() noexcept { return m_c; } \
    inline const ::Sharemind ## ClassName * cPtr() const noexcept { return m_c;}


/*******************************************************************************
  Details
*******************************************************************************/

namespace Detail {
namespace libmodapi {

inline ModuleApiError allocThrow(const ModuleApiError e) {
    if (e == ::SHAREMIND_MODULE_API_OUT_OF_MEMORY)
        throw std::bad_alloc();
    return e;
}

template <typename CType> struct TypeInv;

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPEINV(t) \
    template <> struct TypeInv<t> { using type = ::Sharemind ## t; };\
    template <> struct TypeInv<SHAREMIND_GCCPR54526::Sharemind ## t> \
    { using type = t; };

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_TAGGETTER(type) \
    inline type * modapiGetTag(const ::Sharemind ## type * const o) noexcept \
    { return static_cast<type *>(::Sharemind ## type ## _tag(o)); }

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF(type) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPEINV(type) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPEINV(type const) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPEINV(type volatile) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPEINV(type const volatile) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_TAGGETTER(type)

SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF(ModuleApi)
SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF(Module)
SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF(Syscall)
SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF(Pdk)
SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF(Pd)
SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF(Pdpi)

#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPE_STUFF
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_TAGGETTER
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPEINV

template <typename CType>
inline typename TypeInv<CType>::type * mustTag(CType * const ssc) noexcept {
    assert(ssc);
    typename TypeInv<CType>::type * const sc = modapiGetTag(ssc);
    return (assert(sc), sc);
}

template <typename CType>
inline typename TypeInv<CType>::type * optChild(CType * const ssc) noexcept
{ return ssc ? mustTag(ssc) : nullptr; }

} /* namespace libmodapi { */
} /* namespace Detail { */


/*******************************************************************************
  ModuleApiError
*******************************************************************************/

using ModuleApiError = ::SharemindModuleApiError;

inline const char * ModuleApiError_toString(const ModuleApiError e) noexcept
{ return ::SharemindModuleApiError_toString(e); }



/*******************************************************************************
  *Exception
*******************************************************************************/

class ModuleApiExceptionBase: public std::exception {

public: /* Methods: */

    inline ModuleApiExceptionBase(const ModuleApiError errorCode,
                                  const char * const errorStr)
        : m_errorCode((assert(errorCode != ::SHAREMIND_MODULE_API_OK),
                       errorCode))
        , m_errorStr(errorStr ? errorStr : ModuleApiError_toString(errorCode))
    {}

    inline ModuleApiError code() const noexcept { return m_errorCode; }
    inline const char * what() const noexcept override { return m_errorStr; }

private: /* Fields: */

    const ModuleApiError m_errorCode;
    const char * const m_errorStr;

}; /* class ModuleApiExceptionBase { */

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION(ClassName) \
    class Exception: public ModuleApiExceptionBase { \
    public: /* Methods: */ \
        inline Exception(const ::Sharemind ## ClassName & c) \
            : ModuleApiExceptionBase( \
                      Detail::libmodapi::allocThrow( \
                              ::Sharemind ## ClassName ## _lastError(&c)), \
                      ::Sharemind ## ClassName ## _lastErrorString(&c)) \
        {} \
        inline Exception(const ClassName & c) \
            : Exception(*(c.cPtr())) \
        {} \
        inline Exception(const ModuleApiError error, \
                         const char * const errorStr = nullptr) \
            : ModuleApiExceptionBase(error, errorStr) \
        {} \
        inline Exception(const ModuleApiError error, \
                         const ::Sharemind ## ClassName & c) \
            : ModuleApiExceptionBase( \
                    error, \
                    ::Sharemind ## ClassName ## _lastErrorString(&c)) \
        {} \
        inline Exception(const ModuleApiError error, const ClassName & c) \
            : Exception(error, *(c.cPtr())) \
        {} \
    }


/*******************************************************************************
  Syscall
*******************************************************************************/

class Syscall final {

    friend class Module;

public: /* Methods: */

    Syscall() = delete;
    Syscall(Syscall &&) = delete;
    Syscall(const Syscall &) = delete;
    Syscall & operator=(Syscall &&) = delete;
    Syscall & operator=(const Syscall &) = delete;

    SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS(Syscall)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Syscall,Module,module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Syscall,ModuleApi,moduleApi)

    inline SyscallWrapper wrapper() const noexcept
    { return ::SharemindSyscall_wrapper(m_c); }

    inline const char * signature() const noexcept
    { return ::SharemindSyscall_signature(m_c); }

private: /* Methods: */

    inline Syscall(::SharemindSyscall * const sc) noexcept
        : m_c((assert(sc), sc))
    {
        ::SharemindSyscall_setTagWithDestructor(
                    sc,
                    this,
                    [](void * s) { delete static_cast<Syscall *>(s); });
    }

    inline ~Syscall() noexcept {}

private: /* Fields: */

    ::SharemindSyscall * m_c;

}; /* class Syscall { */


/*******************************************************************************
  Pdpi
*******************************************************************************/

class Pdpi {

public: /* Types: */

    SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION(Pdpi);

public: /* Methods: */

    Pdpi() = delete;
    Pdpi(Pdpi &&) = delete;
    Pdpi(const Pdpi &) = delete;
    Pdpi & operator=(Pdpi &&) = delete;
    Pdpi & operator=(const Pdpi &) = delete;


    inline Pdpi(Pd & pd);

    inline ~Pdpi() noexcept {
        if (m_c) {
            if (::SharemindPdpi_tag(m_c) == this)
                ::SharemindPdpi_releaseTag(m_c);
            ::SharemindPdpi_free(m_c);
        }
    }

    SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS(Pdpi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pdpi,Pd,pd)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pdpi,Pdk,pdk)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pdpi,Module,module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pdpi,ModuleApi,moduleApi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_SELF_FACILITY_FUNCTIONS(Pdpi)

    inline bool isStarted() const noexcept
    { return ::SharemindPdpi_isStarted(m_c); }

    inline void start() {
        const ModuleApiError r = ::SharemindPdpi_start(m_c);
        if (r != ::SHAREMIND_MODULE_API_OK)
            throw Exception(r, *this);
    }

    inline void stop() noexcept { ::SharemindPdpi_stop(m_c); }

    inline void * handle() const noexcept
    { return ::SharemindPdpi_handle(m_c); }

private: /* Fields: */

    ::SharemindPdpi * m_c;

}; /* class Pd { */


/*******************************************************************************
  Pd
*******************************************************************************/

class Pd {

    friend Pdpi::Pdpi(Pd &);

public: /* Types: */

    SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION(Pd);

public: /* Methods: */

    Pd() = delete;
    Pd(Pd &&) = delete;
    Pd(const Pd &) = delete;
    Pd & operator=(Pd &&) = delete;
    Pd & operator=(const Pd &) = delete;

    inline Pd(Pdk & pdk, const char * name, const char * configuration)
        __attribute__((nonnull(3)));

    virtual inline ~Pd() noexcept {
        if (m_c) {
            if (::SharemindPd_tag(m_c) == this)
                ::SharemindPd_releaseTag(m_c);
            ::SharemindPd_free(m_c);
        }
    }

    SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS(Pd)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pd,Pdk,pdk)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pd,Module,module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pd,ModuleApi,moduleApi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_SELF_FACILITY_FUNCTIONS(Pd)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(Pd, pdpi, Pdpi)

    inline bool isStarted() const noexcept
    { return ::SharemindPd_isStarted(m_c); }

    inline void start() {
        const ModuleApiError r = ::SharemindPd_start(m_c);
        if (r != ::SHAREMIND_MODULE_API_OK)
            throw Exception(r, *m_c);
    }

    inline void stop() noexcept { ::SharemindPd_stop(m_c); }

    inline const char * name() const noexcept
    { return ::SharemindPd_name(m_c); }

    inline const char * conf() const noexcept
    { return ::SharemindPd_conf(m_c); }

    inline void * handle() const noexcept
    { return ::SharemindPd_handle(m_c); }

private: /* Methods: */

    inline ::SharemindPdpi & newPdpi() {
        ::SharemindPdpi * const pdpi = ::SharemindPd_newPdpi(m_c);
        if (pdpi)
            return *pdpi;
        throw Exception(*m_c);
    }

private: /* Fields: */

    ::SharemindPd * m_c;

}; /* class Pd { */


/*******************************************************************************
  Pdk
*******************************************************************************/

class Pdk final {

    friend Pd::Pd(Pdk & pdk, const char * name, const char * configuration);
    friend class Module;

public: /* Types: */

    SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION(Pdk);

public: /* Methods: */

    Pdk() = delete;
    Pdk(Pdk &&) = delete;
    Pdk(const Pdk &) = delete;
    Pdk & operator=(Pdk &&) = delete;
    Pdk & operator=(const Pdk &) = delete;

    SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS(Pdk)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pdk,Module,module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Pdk,ModuleApi,moduleApi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(Pdk, pd, Pd)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(Pdk, pdpi, Pdpi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PDCHILD_STUFF(Pdk)

    inline const char * name() const noexcept
    { return ::SharemindPdk_name(m_c); }

    inline size_t index() const noexcept
    { return ::SharemindPdk_index(m_c); }

private: /* Methods: */

    Pdk(::SharemindPdk * const pdk) noexcept
        : m_c((assert(pdk), pdk))
    {
        ::SharemindPdk_setTagWithDestructor(
                    pdk,
                    this,
                    [](void * p) noexcept { delete static_cast<Pdk *>(p); });
    }

    inline ~Pdk() noexcept {}

    ::SharemindPd & newPd(const char * name, const char * configuration)
            __attribute__ ((nonnull(2)))
    {
        assert(name);
        ::SharemindPd * const pd =
                ::SharemindPdk_newPd(m_c, name, configuration);
        if (pd)
            return *pd;
        throw Exception(*this);
    }

private: /* Fields: */

    ::SharemindPdk * const m_c;

}; /* class Pdk { */


/*******************************************************************************
  Module
*******************************************************************************/

class Module {

public: /* Types: */

    SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION(Module);

public: /* Methods: */

    Module() = delete;
    Module(Module &&) = delete;
    Module(const Module &) = delete;
    Module & operator=(Module &&) = delete;
    Module & operator=(const Module &) = delete;

    Module(ModuleApi & moduleApi,
           const char * const filename,
           const char * const configuration) __attribute__ ((nonnull(3)));

    virtual inline ~Module() noexcept {
        if (m_c) {
            if (::SharemindModule_tag(m_c) == this)
                ::SharemindModule_releaseTag(m_c);
            ::SharemindModule_free(m_c);
        }
    }

    SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS(Module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER(Module,ModuleApi,moduleApi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PDK_AND_SYSCALL_CHILD_STUFF(Module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PDCHILD_STUFF(Module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_SELF_FACILITY_FUNCTIONS(Module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(Module, pd, Pd)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(Module, pdpi, Pdpi)

    inline void init() {
        const ModuleApiError r = ::SharemindModule_init(m_c);
        if (r != ::SHAREMIND_MODULE_API_OK)
            throw Exception(r, *this);
    }

    inline void deinit() noexcept { ::SharemindModule_deinit(m_c); }

    inline bool isInitialized() const noexcept
    { return ::SharemindModule_isInitialized(m_c); }

    inline const char * filename() const noexcept
    { return ::SharemindModule_filename(m_c); }

    inline const char * name() const noexcept
    { return ::SharemindModule_name(m_c); }

    inline const char * conf() const noexcept
    { return ::SharemindModule_conf(m_c); }

    inline uint32_t apiVersionInUse() const noexcept
    { return ::SharemindModule_apiVersionInUse(m_c); }

    inline void * handle() const noexcept
    { return ::SharemindModule_handle(m_c); }

private: /* Fields: */

    ::SharemindModule * m_c;

}; /* class Module { */

/*******************************************************************************
  ModuleApi
*******************************************************************************/

class ModuleApi {

    friend Module::Module(ModuleApi &, const char * const, const char * const);

public: /* Types: */

    SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION(ModuleApi);

public: /* Methods: */

    ModuleApi(ModuleApi &&) = delete;
    ModuleApi(const ModuleApi &) = delete;
    ModuleApi & operator=(ModuleApi &&) = delete;
    ModuleApi & operator=(const ModuleApi &) = delete;

    inline ModuleApi()
        : m_c([]{
                  ModuleApiError error;
                  const char * errorStr;
                  ::SharemindModuleApi * const modapi =
                          ::SharemindModuleApi_new(&error, &errorStr);
                  if (modapi)
                      return modapi;
                  throw Exception(Detail::libmodapi::allocThrow(error),
                                  errorStr);
              }())
    {
        ::SharemindModuleApi_setTagWithDestructor(
                    m_c,
                    this,
                    [](void * m) noexcept {
                        ModuleApi * const moduleApi =
                                static_cast<ModuleApi *>(m);
                        moduleApi->m_c = nullptr;
                        delete moduleApi;
                    });
    }

    virtual inline ~ModuleApi() noexcept {
        if (m_c) {
            if (::SharemindModuleApi_tag(m_c) == this)
                ::SharemindModuleApi_releaseTag(m_c);
            ::SharemindModuleApi_free(m_c);
        }
    }

    SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS(ModuleApi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PDK_AND_SYSCALL_CHILD_STUFF(ModuleApi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_PDCHILD_STUFF(ModuleApi)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(ModuleApi, module, Module)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(ModuleApi, pd, Pd)
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(ModuleApi, pdpi, Pdpi)

private: /* Methods: */

    ::SharemindModule & newModule(const char * const filename,
                                  const char * const configuration)
            __attribute__((nonnull(2)))
    {
        assert(filename);
        ::SharemindModule * const m =
                ::SharemindModuleApi_newModule(m_c, filename, configuration);
        if (m)
            return *m;
        throw Exception(*m_c);
    }

private: /* Fields: */

    ::SharemindModuleApi * m_c;

}; /* class ModuleApi { */


/*******************************************************************************
  Pdpi methods
*******************************************************************************/

inline Pdpi::Pdpi(Pd & pd)
    : m_c(&pd.newPdpi())
{
    ::SharemindPdpi_setTagWithDestructor(
                m_c,
                this,
                [](void * p) noexcept {
                    Pdpi * const pdpi = static_cast<Pdpi *>(p);
                    pdpi->m_c = nullptr;
                    delete pdpi;
                });
}


/*******************************************************************************
  Pd methods
*******************************************************************************/

inline Pd::Pd(Pdk & pdk, const char * name, const char * configuration)
    : m_c(&pdk.newPd(name, configuration))
{
    ::SharemindPd_setTagWithDestructor(
                m_c,
                this,
                [](void * p) noexcept {
                    Pd * const pd = static_cast<Pd *>(p);
                    pd->m_c = nullptr;
                    delete pd;
                });
}


/*******************************************************************************
  Module methods
*******************************************************************************/

inline Module::Module(ModuleApi & moduleApi,
                      const char * const filename,
                      const char * const configuration)
    : m_c(&moduleApi.newModule(filename, configuration))
{
    try {
        {
            const size_t nscs = numSyscalls();
            for (size_t i = 0u; i < nscs; i++)
                new Syscall(::SharemindModule_syscall(m_c, i));
        }{
            const size_t npdks = numPdks();
            for (size_t i = 0u; i < npdks; i++)
                new Pdk(::SharemindModule_pdk(m_c, i));
        }
        ::SharemindModule_setTagWithDestructor(
                    m_c,
                    this,
                    [](void * m) noexcept {
                        Module * const module = static_cast<Module *>(m);
                        module->m_c = nullptr;
                        delete module;
                    });
    } catch (...) {
        ::SharemindModule_free(m_c);
        throw;
    }
}

} /* namespace sharemind { */


/*******************************************************************************
  Undefine helper macros
*******************************************************************************/

#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_CPTR_GETTERS
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_PARENT_GETTER
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_PDK_AND_SYSCALL_CHILD_STUFF
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_PDCHILD_STUFF
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_SELF_FACILITY_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS__

#endif /* SHAREMIND_LIBMODAPICXX_LIBMODAPICXX_H */
