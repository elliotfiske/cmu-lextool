/* -*- c-file-style: "linux" -*- */
/* ====================================================================
 * Copyright (c) 2005 Carnegie Mellon University.  All rights 
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
/*********************************************************************
 *
 * File: bin_mdef.h
 * 
 * Description: 
 *	Binary format model definition files, with support for
 *	heterogeneous topologies and variable-size N-phones
 *
 * Author: 
 * 	David Huggins-Daines <dhuggins@cs.cmu.edu>
 *********************************************************************/
#ifndef __BIN_MDEF_H__
#define __BIN_MDEF_H__

#ifdef __cplusplus
extern "C" {
#if 0
}; /* Fool Emacs */
#endif
#endif /* __cplusplus */

/* SphinxBase headers. */
#include <mdef.h>
#include <mmio.h>
#include <cmd_ln.h>
#include <sphinx_types.h>

#define BIN_MDEF_FORMAT_VERSION 1
/* Little-endian machines will write "BMDF" to disk, big-endian ones "FDMB". */
#define BIN_MDEF_NATIVE_ENDIAN 0x46444d42 /* 'BMDF' in little-endian order */
#define BIN_MDEF_OTHER_ENDIAN 0x424d4446  /* 'BMDF' in big-endian order */
#ifdef __GNUC__
#define ATTRIBUTE_PACKED __attribute__((packed))
#else
#define ATTRIBUTE_PACKED
#endif

/**< Phone entry (on-disk, 12 bytes) */
typedef struct mdef_entry_s mdef_entry_t;
struct mdef_entry_s {
	int32 ssid; /**< Senone sequence ID */
	int32 tmat; /**< Transition matrix ID */
	/* FIXME: is any of this actually necessary? */
	union {
		/**< CI phone information - attributes (just "filler" for now) */
		struct {
			int8 filler;
			int8 reserved[3];
		} ci;
		/**< CD phone information - context info. */
		struct {
			int8 wpos;
			int8 ctx[3]; /**< quintphones will require hacking */
		} cd;
	} info;
} ATTRIBUTE_PACKED;

/**< Node in CD phone tree (on-disk, 8 bytes).  */
typedef struct cd_tree_s cd_tree_t;
struct cd_tree_s {
	int16 ctx; /**< Context (word position or CI phone) */
	int16 n_down; /**< Number of children (0 for leafnode) */
	union {
		s3pid_t pid; /**< Phone ID (leafnode) */
		int32 down; /**< Next level of the tree (offset from start of cd_trees) */ 
	} c;
};

/**< Model definition structure (in-memory). */
typedef struct bin_mdef_s bin_mdef_t;
struct bin_mdef_s {
	int32 n_ciphone;    /**< Number of base (CI) phones */
	int32 n_phone;	    /**< Number of base (CI) phones + (CD) triphones */
	int32 n_emit_state; /**< Number of emitting states per phone (0 for heterogeneous) */
	int32 n_ci_sen;	    /**< Number of CI senones; these are the first */
	int32 n_sen;	    /**< Number of senones (CI+CD) */
	int32 n_tmat;	    /**< Number of transition matrices */
	int32 n_sseq;       /**< Number of unique senone sequences */
	int32 n_ctx;	    /**< Number of phones of context */
	int32 n_cd_tree;    /**< Number of nodes in cd_tree (below) */
	s3cipid_t sil;	    /**< CI phone ID for silence */

	mmio_file_t *filemap; /**< File map for this file (if any) */
	char **ciname; /**< CI phone names */
	cd_tree_t *cd_tree; /**< Tree mapping CD phones to phone IDs */
	mdef_entry_t *phone; /**< All phone structures */
	s3senid_t **sseq;    /**< Unique senone sequences (2D array built at load time) */
	int8 *sseq_len;      /**< Number of states in each sseq (NULL for homogeneous) */

	/* These two are not stored on disk, but are generated at load time. */
	s3senid_t *cd2cisen;	/**< Parent CI-senone id for each senone */
	s3cipid_t *sen2cimap;	/**< Parent CI-phone for each senone (CI or CD) */

	/** Allocation mode for this object. */
	enum { BIN_MDEF_FROM_TEXT, BIN_MDEF_IN_MEMORY, BIN_MDEF_ON_DISK } alloc_mode;
};

#define bin_mdef_is_fillerphone(m,p)	(((p) < (m)->n_ciphone) \
		                         ? (m)->phone[p].info.ci.filler \
					 : (m)->phone[(m)->phone[p].info.cd.ctx[0]].info.ci.filler)
#define bin_mdef_is_ciphone(m,p)	((p) < (m)->n_ciphone)
#define bin_mdef_n_ciphone(m)		((m)->n_ciphone)
#define bin_mdef_n_phone(m)		((m)->n_phone)
#define bin_mdef_n_sseq(m)		((m)->n_sseq)
#define bin_mdef_n_emit_state(m)	((m)->n_emit_state)
#define bin_mdef_n_emit_state_phone(m,p) ((m)->n_emit_state ? (m)->n_emit_state \
					  : (m)->sseq_len[(m)->phone[p].ssid])
#define bin_mdef_n_sen(m)		((m)->n_sen)
#define bin_mdef_n_tmat(m)		((m)->n_tmat)
#define bin_mdef_pid2ssid(m,p)		((m)->phone[p].ssid)
#define bin_mdef_pid2tmatid(m,p)	((m)->phone[p].tmat)
#define bin_mdef_silphone(m)		((m)->sil)
#define bin_mdef_sseq2sen(m,ss,pos)	((m)->sseq[ss][pos])
#define bin_mdef_pid2ci(m,p)		(((p) < (m)->n_ciphone) ? (p) \
                                         : (m)->phone[p].info.cd.ctx[0])

/** Read a binary mdef from a file. */
bin_mdef_t *bin_mdef_read(cmd_ln_t *config, const char *filename);
/** Read a text mdef from a file (creating an in-memory binary mdef). */
bin_mdef_t *bin_mdef_read_text(cmd_ln_t *config, const char *filename);
/** Write a binary mdef to a file. */
int bin_mdef_write(bin_mdef_t *m, const char *filename);
/** Write a binary mdef to a text file. */
int bin_mdef_write_text(bin_mdef_t *m, const char *filename);

/** Free a binary mdef. */
void bin_mdef_free(bin_mdef_t *m);

/* Return value: ciphone id for the given ciphone string name */
s3cipid_t bin_mdef_ciphone_id (bin_mdef_t *m,	/* In: Model structure being queried */
			       const char *ciphone); /* In: ciphone for which id wanted */

/* Return value: READ-ONLY ciphone string name for the given ciphone id */
const char *bin_mdef_ciphone_str (bin_mdef_t *m,	/* In: Model structure being queried */
				  int32 ci);	/* In: ciphone id for which name wanted */

/* Return value: phone id for the given constituents if found, else BAD_S3PID */
s3pid_t bin_mdef_phone_id (bin_mdef_t *m,	/* In: Model structure being queried */
		       int32 b,		/* In: base ciphone id */
		       int32 l,		/* In: left context ciphone id */
		       int32 r,		/* In: right context ciphone id */
		       int32 pos);	/* In: Word position */

/*
 * Create a phone string for the given phone (base or triphone) id in the given buf.
 * Return value: 0 if successful, -1 if error.
 */
int32 bin_mdef_phone_str (bin_mdef_t *m,	/* In: Model structure being queried */
			  s3pid_t pid,	/* In: phone id being queried */
			  char *buf);	/* Out: On return, buf has the string */

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* __BIN_MDEF_H__ */
