/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2013 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */


%extend Config {
  Config() {
    Config *c = cmd_ln_init(NULL, ps_args(), FALSE, NULL);
    return c;
  }

  Config(char const *file) {
    Config *c = cmd_ln_parse_file_r(NULL, ps_args(), file, FALSE);
    return c;
  }

  ~Config() {
    cmd_ln_free_r($self);
  }

  void setBoolean(char const *key, bool val) {
    cmd_ln_set_boolean_r($self, key, val);
  }

  void setInt(char const *key, int val) {
    cmd_ln_set_int_r($self, key, val);
  }

  void setFloat(char const *key, double val) {
    cmd_ln_set_float_r($self, key, val);
  }

  void setString(char const *key, char const *val) {
    cmd_ln_set_str_r($self, key, val);
  }

  bool exists(char const *key) {
    return cmd_ln_exists_r($self, key);
  }

  bool getBoolean(char const *key) {
    return cmd_ln_boolean_r($self, key);
  }

  int getInt(char const *key) {
    return cmd_ln_int_r($self, key);
  }

  double getFloat(char const *key) {
    return cmd_ln_float_r($self, key);
  }

  char const *getString(char const *key) {
    return cmd_ln_str_r($self, key);
  }
}
