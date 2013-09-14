%include <exception.i>

#if SWIGPYTHON
%include <file.i>
#endif

#if SWIGJAVA
%rename("%(lowercamelcase)s", notregexmatch$name="^[A-Z]") "";
%include <arrays_java.i>
#endif

// Define typemaps to wrap error codes returned by some functions,
// into runtime exceptions.
%typemap(in, numinputs=0, noblock=1) int *errcode {
  int errcode;
  $1 = &errcode;
}

%typemap(argout) int *errcode {
  if (*$1 < 0) {
    char buf[64];
    sprintf(buf, "$symname returned %d", *$1);
    SWIG_exception(SWIG_RuntimeError, buf);
  }
}

// Special typemap for arrays of audio.
#if SWIGJAVA

%typemap(in) \
  (const void *SDATA, size_t NSAMP) {
  $1 = (short const *) JCALL2(GetShortArrayElements, jenv, $input, NULL);
  $2 = JCALL1(GetArrayLength, jenv, $input);
};

%typemap(freearg) (short const *SDATA, size_t NSAMP) {
  JCALL3(ReleaseShortArrayElements, jenv, $input, $1, 0);
}

%typemap(jni) (const void *SDATA, size_t NSAMP) "jshortArray"
%typemap(jtype) (const void *SDATA, size_t NSAMP) "short[]"
%typemap(jstype) (const void *SDATA, size_t NSAMP) "short[]"
%typemap(javain) (const void *SDATA, size_t NSAMP) "$javainput"

#elif SWIGPYTHON

%typemap(in) \ 
  (const void *SDATA, size_t NSAMP) = (const char *STRING, size_t LENGTH);

%typemap(check) size_t NSAMP {
  char buf[64];
  if ($1 % sizeof(int16)) {
    sprintf(buf, "block size must be a multiple of %zd", sizeof(int16));
    SWIG_exception(SWIG_ValueError, buf);
  }
}
#endif


// Macro to construct iterable objects.
%define iterable(TYPE, PREFIX, VALUE_TYPE)

%inline %{
typedef struct {
  PREFIX##_iter_t *ptr;
} TYPE##Iterator;
%}

typedef struct {} TYPE;

#if SWIGPYTHON
%exception TYPE##Iterator##::next() {
  $action
  if (!arg1->ptr) {
    PyErr_SetString(PyExc_StopIteration, "");
    SWIG_fail;
  }
}
#endif

%extend TYPE##Iterator {
  TYPE##Iterator(PREFIX##_iter_t *ptr) {
    TYPE##Iterator *iter = ckd_malloc(sizeof *iter);
    iter->ptr = ptr;
    return iter;
  }

  ~TYPE##Iterator() {
    PREFIX##_iter_free($self->ptr);
    ckd_free($self);
  }

  VALUE_TYPE * next() {
    if ($self->ptr) {
      VALUE_TYPE *value = next_##TYPE##Iterator($self->ptr);
      $self->ptr = PREFIX##_iter_next($self->ptr);
      return value;
    } else {
      return NULL;
    }
  }
}

%extend TYPE {
  TYPE##Iterator * __iter__() {
    return new_##TYPE##Iterator(PREFIX##_iter($self));
  }
}
%enddef

