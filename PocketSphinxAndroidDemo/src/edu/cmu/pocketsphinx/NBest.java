/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package edu.cmu.pocketsphinx;

public class NBest {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected NBest(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(NBest obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        pocketsphinxJNI.delete_NBest(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setPtr(SWIGTYPE_p_ps_nbest_t value) {
    pocketsphinxJNI.NBest_ptr_set(swigCPtr, this, SWIGTYPE_p_ps_nbest_t.getCPtr(value));
  }

  public SWIGTYPE_p_ps_nbest_t getPtr() {
    long cPtr = pocketsphinxJNI.NBest_ptr_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_ps_nbest_t(cPtr, false);
  }

  public NBest(SWIGTYPE_p_ps_nbest_t ptr) {
    this(pocketsphinxJNI.new_NBest(SWIGTYPE_p_ps_nbest_t.getCPtr(ptr)), true);
  }

  public Hypothesis hyp() {
    long cPtr = pocketsphinxJNI.NBest_hyp(swigCPtr, this);
    return (cPtr == 0) ? null : new Hypothesis(cPtr, false);
  }

  public Segment seg() {
    long cPtr = pocketsphinxJNI.NBest_seg(swigCPtr, this);
    return (cPtr == 0) ? null : new Segment(cPtr, false);
  }

}
