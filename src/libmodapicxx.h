/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#ifndef SHAREMIND_LIBMODAPICXX_LIBMODAPICXX_H
#define SHAREMIND_LIBMODAPICXX_LIBMODAPICXX_H

#include <cassert>
#include <exception>
#include <new>
#include <sharemind/ApplyTuples.h>
#include <sharemind/compiler-support/GccVersion.h>
#include <sharemind/compiler-support/GccPR54526.h>
#include <sharemind/compiler-support/GccPR55015.h>
#include <sharemind/DebugOnly.h>
#include <sharemind/libmodapi/libmodapi.h>
#include <sharemind/NoNullTuple.h>
#include <tuple>
#include <type_traits>
#include <utility>


namespace sharemind {

/*******************************************************************************
  Some type aliases
*******************************************************************************/

using SyscallCallable = ::SharemindSyscallCallable;
using SyscallWrapper = ::SharemindSyscallWrapper;
using Facility = ::SharemindFacility;
using ModuleApiError = ::SharemindModuleApiError;
using ModuleApiContext = ::SharemindModuleApiContext;

class ModuleApi;
class Module;
class Syscall;
class Pdk;
class Pd;
class Pdpi;


/*******************************************************************************
  Some helper macros
*******************************************************************************/

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS_(ClassName,fF,FF) \
    inline void set ## FF(const char * name, \
                          void * facility, \
                          void * context = nullptr) \
            SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2)))) \
    { \
        ::Sharemind ## ClassName ## _set ## FF(m_c, \
                                               (assert(name), name), \
                                               facility, \
                                               context); \
    } \
    inline bool unset ## FF(const char * name) noexcept \
            SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2)))) \
    { \
        return ::Sharemind ## ClassName ## _unset ## FF(m_c, \
                                                        (assert(name), name)); \
    } \
    inline const SharemindFacility * fF(const char * name) const noexcept \
            SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2)))) \
    { return ::Sharemind ## ClassName ## _ ## fF(m_c, (assert(name), name)); }

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS(ClassName,fN,FN) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS_(ClassName, \
                                                       fN ## Facility, \
                                                       FN ## Facility)

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_SELF_FACILITY_FUNCTIONS(ClassName) \
    SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS_(ClassName, \
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
            const noexcept SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2)))) \
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
            const noexcept SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2)))) \
    { \
        assert(signature); \
        return Detail::libmodapi::optChild( \
                    ::Sharemind ## ClassName ## _findSyscall(m_c, signature));\
    } \
    inline SyscallWrapper syscallWrapper(const char * const signature) \
            const noexcept SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2)))) \
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
            const noexcept SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2)))) \
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
        throw ::std::bad_alloc();
    return e;
}

template <typename CType> struct TypeInv;

#define SHAREMIND_LIBMODAPI_CXX_DEFINE_TYPEINV(t) \
    template <> struct TypeInv<t> { using type = ::Sharemind ## t; };\
    template <> struct TypeInv<SHAREMIND_GCCPR54526_WORKAROUND::Sharemind ## t>\
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

using Context = ModuleApiContext;
using XM = decltype(::std::declval<Context &>().moduleFacility);

template <typename OrigTpl, typename Tpl, size_t I, typename F>
struct XF {
  constexpr XM operator()() const noexcept {
    return [](Context * ctx, const char * signature) -> Facility const * {
        return noNullGet<I, OrigTpl>(*static_cast<Tpl *>(ctx->internal))(
                    signature);
    };
  }
};

template <typename OrigTpl, typename Tpl, size_t I>
struct XF<OrigTpl, Tpl, I, decltype(nullptr)> {
  constexpr XM operator()() const noexcept { return nullptr; }
};

template <size_t I, typename OrigTpl, typename Tpl, typename ... Fs>
struct GetGetters;
template <size_t I, typename OrigTpl, typename Tpl>
struct GetGetters<I, OrigTpl, Tpl> {
    static constexpr ::std::tuple<> get() { return ::std::tuple<>{}; }
};
#define SHAREMIND_LIBMODAPI_CXX_GETGETTERS \
    ::std::tuple_cat(::std::make_tuple(XF<OrigTpl, \
                                          Tpl, \
                                          I, \
                                          typename ::std::decay<F>::type>{}()),\
                     GetGetters<I + 1u, OrigTpl, Tpl, Fs...>::get())
template <size_t I, typename OrigTpl, typename Tpl, typename F, typename ... Fs>
struct GetGetters<I, OrigTpl, Tpl, F, Fs...> {
    static constexpr auto get() -> decltype(SHAREMIND_LIBMODAPI_CXX_GETGETTERS)
    { return SHAREMIND_LIBMODAPI_CXX_GETGETTERS; }
};
#undef SHAREMIND_LIBMODAPI_CXX_GETGETTERS

template <typename ... Args> Context * constructContext(Args && ... args)
{ return new Context{::std::forward<Args>(args)...}; }

template <typename> struct AlwaysXM { using type = XM; };

template <typename ... Args> Context * createContext(Args && ... args) {
    using OrigTpl = ::std::tuple<Args...>;
    using Tpl = NoNullTuple_t<typename std::decay<Args>::type...>;
    Tpl * const tpl = new Tpl(makeNoNullTuple(::std::forward<Args>(args)...));

    return applyTuples(
                 constructContext<
                     Tpl *,
                     decltype(::std::declval<Context &>().destructor),
                     typename AlwaysXM<Args>::type...>,
                 ::std::make_tuple(tpl,
                                   [](Context * x){
                                       delete static_cast<Tpl *>(x->internal);
                                       delete x;
                                   }),
                 GetGetters<0u, OrigTpl, Tpl, Args...>::get());
}

} /* namespace libmodapi { */
} /* namespace Detail { */


/*******************************************************************************
  ModuleApiError
*******************************************************************************/

inline const char * ModuleApiError_toString(const ModuleApiError e) noexcept
{ return ::SharemindModuleApiError_toString(e); }



/*******************************************************************************
  *Exception
*******************************************************************************/

class ModuleApiExceptionBase: public ::std::exception {

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
        #define SHAREMIND_LIBMODAPI_CXX_SYSCALL_L1 \
            (void * s) { delete static_cast<Syscall *>(s); }
        #if SHAREMIND_GCCPR55015
        struct F { static void f SHAREMIND_LIBMODAPI_CXX_SYSCALL_L1 };
        #endif
        ::SharemindSyscall_setTagWithDestructor(
                    sc,
                    this,
                    #if SHAREMIND_GCCPR55015
                    &F::f
                    #else
                    []SHAREMIND_LIBMODAPI_CXX_SYSCALL_L1
                    #endif
                    );
        #undef SHAREMIND_LIBMODAPI_CXX_SYSCALL_L1
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
        SHAREMIND_NDEBUG_ONLY(__attribute__((nonnull(3))));

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
        #define SHAREMIND_LIBMODAPI_CXX_PDK_L1 \
            (void * p) noexcept { delete static_cast<Pdk *>(p); }
        #if SHAREMIND_GCCPR55015
        struct F { static void f SHAREMIND_LIBMODAPI_CXX_PDK_L1 };
        #endif
        ::SharemindPdk_setTagWithDestructor(
                    pdk,
                    this,
                    #if SHAREMIND_GCCPR55015
                    &F::f
                    #else
                    []SHAREMIND_LIBMODAPI_CXX_PDK_L1
                    #endif
                    );
        #undef SHAREMIND_LIBMODAPI_CXX_PDK_L1
    }

    inline ~Pdk() noexcept {}

    ::SharemindPd & newPd(const char * name, const char * configuration)
            SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(2))))
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

    friend class ModuleApi;

public: /* Types: */

    SHAREMIND_LIBMODAPI_CXX_DEFINE_EXCEPTION(Module);

public: /* Methods: */

    Module() = delete;
    Module(Module &&) = delete;
    Module(const Module &) = delete;
    Module & operator=(Module &&) = delete;
    Module & operator=(const Module &) = delete;

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

private: /* Methods: */

    Module(ModuleApi & moduleApi,
           const char * const filename,
           const char * const configuration)
            SHAREMIND_NDEBUG_ONLY(__attribute__ ((nonnull(3))));

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

    using Context = ModuleApiContext;

public: /* Methods: */

    ModuleApi(ModuleApi &&) = delete;
    ModuleApi(const ModuleApi &) = delete;
    ModuleApi & operator=(ModuleApi &&) = delete;
    ModuleApi & operator=(const ModuleApi &) = delete;

    /** \todo Limit this to specific versions as GCC devs make progress at
              https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70808 */
    #if defined(SHAREMIND_GCC_VERSION)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
    #endif
    inline ModuleApi() : ModuleApi(nullptr) {}
    inline ModuleApi(decltype(nullptr), decltype(nullptr), decltype(nullptr))
        : ModuleApi() {}

    template <typename FindModuleFacility = decltype(nullptr),
              typename FindPdFacility = decltype(nullptr),
              typename FindPdpiFacility = decltype(nullptr)>
    inline ModuleApi(FindModuleFacility && findModuleFacility
                            = FindModuleFacility(),
                     FindPdFacility && findPdFacility = FindPdFacility(),
                     FindPdpiFacility && findPdpiFacility = FindPdpiFacility())
        : ModuleApi(Detail::libmodapi::createContext(
                        ::std::forward<FindModuleFacility>(findModuleFacility),
                        ::std::forward<FindPdFacility>(findPdFacility),
                        ::std::forward<FindPdpiFacility>(findPdpiFacility)))
    {}
    /** \todo Limit this to specific versions as GCC devs make progress at
              https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70808 */
    #if defined(SHAREMIND_GCC_VERSION)
    #pragma GCC diagnostic pop
    #endif

    inline ModuleApi(Context * const ctx)
        : m_c([ctx]{
                  ModuleApiError error;
                  const char * errorStr;
                  try {
                      ::SharemindModuleApi * const modapi =
                              ::SharemindModuleApi_new(ctx, &error, &errorStr);
                      if (modapi)
                          return modapi;
                      throw Exception(Detail::libmodapi::allocThrow(error),
                                      errorStr);
                  } catch (...) {
                      delete ctx;
                      throw;
                  }
              }())
    {
        #define SHAREMIND_LIBMODAPI_CXX_MODULEAPI_L1 \
            (void * m) noexcept { \
                ModuleApi * const moduleApi = \
                        static_cast<ModuleApi *>(m); \
                moduleApi->m_c = nullptr; \
                delete moduleApi; \
            }
        #if SHAREMIND_GCCPR55015
        struct F { static void f SHAREMIND_LIBMODAPI_CXX_MODULEAPI_L1 };
        #endif
        ::SharemindModuleApi_setTagWithDestructor(
                    m_c,
                    this,
                    #if SHAREMIND_GCCPR55015
                    &F::f
                    #else
                    []SHAREMIND_LIBMODAPI_CXX_MODULEAPI_L1
                    #endif
                    );
        #undef SHAREMIND_LIBMODAPI_CXX_MODULEAPI_L1
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

    template <typename ... Args>
    inline Module & loadModule(Args && ... args)
    { return *(new Module(*this, std::forward<Args>(args)...)); }

private: /* Methods: */

    ::SharemindModule & newModule_(const char * const filename,
                                   const char * const configuration)
            SHAREMIND_NDEBUG_ONLY(__attribute__((nonnull(2))))
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
    #define SHAREMIND_LIBMODAPI_CXX_PDPI_L1 \
        (void * p) noexcept { \
            Pdpi * const pdpi = static_cast<Pdpi *>(p); \
            pdpi->m_c = nullptr; \
            delete pdpi; \
        }
    #if SHAREMIND_GCCPR55015
    struct F { static void f SHAREMIND_LIBMODAPI_CXX_PDPI_L1 };
    #endif
    ::SharemindPdpi_setTagWithDestructor(
                m_c,
                this,
                #if SHAREMIND_GCCPR55015
                &F::f
                #else
                []SHAREMIND_LIBMODAPI_CXX_PDPI_L1
                #endif
                );
    #undef SHAREMIND_LIBMODAPI_CXX_PDPI_L1
}


/*******************************************************************************
  Pd methods
*******************************************************************************/

inline Pd::Pd(Pdk & pdk, const char * name, const char * configuration)
    : m_c(&pdk.newPd(name, configuration))
{
    #define SHAREMIND_LIBMODAPI_CXX_PD_L1 \
        (void * p) noexcept { \
            Pd * const pd = static_cast<Pd *>(p); \
            pd->m_c = nullptr; \
            delete pd; \
        }
    #if SHAREMIND_GCCPR55015
    struct F { static void f SHAREMIND_LIBMODAPI_CXX_PD_L1 };
    #endif
    ::SharemindPd_setTagWithDestructor(
                m_c,
                this,
                #if SHAREMIND_GCCPR55015
                &F::f
                #else
                []SHAREMIND_LIBMODAPI_CXX_PD_L1
                #endif
                );
    #undef SHAREMIND_LIBMODAPI_CXX_PD_L1
}


/*******************************************************************************
  Module methods
*******************************************************************************/

inline Module::Module(ModuleApi & moduleApi,
                      const char * const filename,
                      const char * const configuration)
    : m_c(&moduleApi.newModule_(filename, configuration))
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
        #define SHAREMIND_LIBMODAPI_CXX_MODULE_L1 \
            (void * m) noexcept { \
                Module * const module = static_cast<Module *>(m); \
                module->m_c = nullptr; \
                delete module; \
            }
        #if SHAREMIND_GCCPR55015
        struct F { static void f SHAREMIND_LIBMODAPI_CXX_MODULE_L1 };
        #endif
        ::SharemindModule_setTagWithDestructor(
                    m_c,
                    this,
                    #if SHAREMIND_GCCPR55015
                    &F::f
                    #else
                    []SHAREMIND_LIBMODAPI_CXX_MODULE_L1
                    #endif
                    );
        #undef SHAREMIND_LIBMODAPI_CXX_MODULE_L1
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
#undef SHAREMIND_LIBMODAPI_CXX_DEFINE_FACILITY_FUNCTIONS_

#endif /* SHAREMIND_LIBMODAPICXX_LIBMODAPICXX_H */
