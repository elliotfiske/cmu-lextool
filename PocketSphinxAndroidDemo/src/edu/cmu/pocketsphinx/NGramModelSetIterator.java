/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package edu.cmu.pocketsphinx;

public class NGramModelSetIterator {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected NGramModelSetIterator(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(NGramModelSetIterator obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        sphinxbaseJNI.delete_NGramModelSetIterator(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setPtr(SWIGTYPE_p_ngram_model_set_iter_t value) {
    sphinxbaseJNI.NGramModelSetIterator_ptr_set(swigCPtr, this, SWIGTYPE_p_ngram_model_set_iter_t.getCPtr(value));
  }

  public SWIGTYPE_p_ngram_model_set_iter_t getPtr() {
    long cPtr = sphinxbaseJNI.NGramModelSetIterator_ptr_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_ngram_model_set_iter_t(cPtr, false);
  }

  public NGramModelSetIterator(SWIGTYPE_p_ngram_model_set_iter_t ptr) {
    this(sphinxbaseJNI.new_NGramModelSetIterator(SWIGTYPE_p_ngram_model_set_iter_t.getCPtr(ptr)), true);
  }

  public NGramModel next() {
    long cPtr = sphinxbaseJNI.NGramModelSetIterator_next(swigCPtr, this);
    return (cPtr == 0) ? null : new NGramModel(cPtr, false);
  }

}
