/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2008 Carnegie Mellon University.  All rights
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

/**
 * @file ps_lattice_internal.h Word graph search implementation
 */

#ifndef __PS_LATTICE_INTERNAL_H__
#define __PS_LATTICE_INTERNAL_H__

/**
 * Linked list of DAG link pointers.
 *
 * Because the same link structure is used for forward and reverse
 * links, as well as for the agenda used in bestpath search, we can't
 * store the list pointer inside latlink_t.  We could use glist_t
 * here, but it wastes 4 bytes per entry on 32-bit machines.
 */
typedef struct latlink_list_s {
    ps_latlink_t *link;
    struct latlink_list_s *next;
} latlink_list_t;

/**
 * Word graph structure used in bestpath/nbest search.
 */
struct ps_lattice_s {
    int refcount;      /**< Reference count. */

    ps_search_t *search; /**< Search object which produced this DAG. */
    logmath_t *lmath;    /**< Log-math object. */

    ps_latnode_t *nodes;  /**< List of all nodes. */
    ps_latnode_t *start;  /**< Starting node. */
    ps_latnode_t *end;    /**< Ending node. */

    int32 n_frames;    /**< Number of frames for this utterance. */
    int32 final_node_ascr; /**< Acoustic score of implicit link exiting final node. */
    int32 norm;        /**< Normalizer for posterior probabilities. */
    char *hyp_str;     /**< Current hypothesis string. */

    listelem_alloc_t *latnode_alloc;     /**< Node allocator for this DAG. */
    listelem_alloc_t *latlink_alloc;     /**< Link allocator for this DAG. */
    listelem_alloc_t *latlink_list_alloc; /**< List element allocator for this DAG. */

    /* This will probably be replaced with a heap. */
    latlink_list_t *q_head; /**< Queue of links for traversal. */
    latlink_list_t *q_tail; /**< Queue of links for traversal. */
};

/**
 * Links between DAG nodes.
 *
 * A link corresponds to a single hypothesized instance of a word with
 * a given start and end point.

 */
struct ps_latlink_s {
    struct ps_latnode_s *from;	/**< From node */
    struct ps_latnode_s *to;	/**< To node */
    struct ps_latlink_s *best_prev;
    int32 ascr;			/**< Score for from->wid (from->sf to this->ef) */
    int32 path_scr;		/**< Best path score from root of DAG */
    int32 ef;			/**< Ending frame of this word  */
    int32 alpha;                /**< Forward probability of this link P(w,o_1^{ef}) */
    int32 beta;                 /**< Backward probability of this link P(w|o_{ef+1}^T) */
};

/**
 * DAG nodes.
 *
 * A node corresponds to a number of hypothesized instances of a word
 * which all share the same starting point.
 */
struct ps_latnode_s {
    int32 id;			/**< Unique id for this node */
    int32 wid;			/**< Dictionary word id */
    int32 basewid;		/**< Dictionary base word id */
    /* FIXME: These are (ab)used to store backpointer indices, therefore they MUST be 32 bits. */
    int32 fef;			/**< First end frame */
    int32 lef;			/**< Last end frame */
    int16 sf;			/**< Start frame */
    int16 reachable;		/**< From \verbatim </s> \endverbatim or \verbatim <s> \endverbatim */
    union {
	int32 fanin;		/**< Number nodes with links to this node */
	int32 rem_score;	/**< Estimated best score from node.sf to end */
	int32 best_exit;	/**< Best exit score (used for final nodes only) */
    } info;
    latlink_list_t *exits;      /**< Links out of this node */
    latlink_list_t *entries;    /**< Links into this node */

    struct ps_latnode_s *alt;   /**< Node with alternate pronunciation for this word */
    struct ps_latnode_s *next;	/**< Next node in DAG (no ordering implied) */
};

/**
 * Segmentation "iterator" for backpointer table results.
 */
typedef struct dag_seg_s {
    ps_seg_t base;       /**< Base structure. */
    ps_latlink_t **links;   /**< Array of lattice links. */
    int32 norm;     /**< Normalizer for posterior probabilities. */
    int16 n_links;  /**< Number of lattice links. */
    int16 cur;      /**< Current position in bpidx. */
} dag_seg_t;

/**
 * Partial path structure used in N-best (A*) search.
 *
 * Each partial path (latpath_t) is constructed by extending another
 * partial path--parent--by one node.
 */
typedef struct ps_latpath_s {
    ps_latnode_t *node;            /**< Node ending this path. */
    struct ps_latpath_s *parent;   /**< Previous element in this path. */
    struct ps_latpath_s *next;     /**< Pointer to next path in list of paths. */
    int32 score;                  /**< Exact score from start node up to node->sf. */
} ps_latpath_t;

/**
 * A* search structure.
 */
typedef struct ps_astar_s {
    ps_lattice_t *dag;
    ngram_model_t *lmset;
    float32 lwf;

    int16 sf;
    int16 ef;
    int32 w1;
    int32 w2;

    int32 n_hyp_tried;
    int32 n_hyp_insert;
    int32 n_hyp_reject;
    int32 insert_depth;
    int32 n_path;

    ps_latpath_t *path_list;
    ps_latpath_t *path_tail;
    ps_latpath_t *paths_done;

    glist_t hyps;	             /**< List of hypothesis strings. */
    listelem_alloc_t *latpath_alloc; /**< Path allocator for N-best search. */
} ps_astar_t;

/**
 * Segmentation "iterator" for A* search results.
 */
typedef struct astar_seg_s {
    ps_seg_t base;
    ps_latnode_t **nodes;
    int n_nodes;
    int cur;
} astar_seg_t;

/**
 * Construct an empty word graph with reference to a search structure.
 */
ps_lattice_t *ps_lattice_init_search(ps_search_t *search, int n_frame);

/**
 * Bypass filler words.
 */
void ps_lattice_bypass_fillers(ps_lattice_t *dag, int32 silpen, int32 fillpen);

/**
 * Remove nodes marked as unreachable.
 */
void ps_lattice_delete_unreachable(ps_lattice_t *dag);

/**
 * Add an edge to the traversal queue.
 */
void ps_lattice_pushq(ps_lattice_t *dag, ps_latlink_t *link);

/**
 * Remove an edge from the traversal queue.
 */
ps_latlink_t *ps_lattice_popq(ps_lattice_t *dag);

/**
 * Clear and reset the traversal queue.
 */
void ps_lattice_delq(ps_lattice_t *dag);

/**
 * Create a new lattice link element.
 */
latlink_list_t *latlink_list_new(ps_lattice_t *dag, ps_latlink_t *link,
                                 latlink_list_t *next);

/**
 * Get hypothesis string after bestpath search.
 */
char const *ps_lattice_hyp(ps_lattice_t *dag, ps_latlink_t *link);

/**
 * Get hypothesis segmentation iterator after bestpath search.
 */
ps_seg_t *ps_lattice_seg_iter(ps_lattice_t *dag, ps_latlink_t *link,
                              float32 lwf);

/**
 * Begin N-Gram based A* search on a word graph.
 *
 * @param sf Starting frame for N-best search.
 * @param ef Ending frame for N-best search, or -1 for last frame.
 * @param w1 First context word, or -1 for none.
 * @param w2 Second context word, or -1 for none.
 * @return 0 for success, <0 on error.
 */
ps_astar_t *ps_astar_start(ps_lattice_t *dag,
                           ngram_model_t *lmset,
                           float32 lwf,
                           int sf, int ef,
                           int w1, int w2);

/**
 * Find next best hypothesis of A* on a word graph.
 *
 * @return a complete path, or NULL if no more hypotheses exist.
 */
ps_latpath_t *ps_astar_next(ps_astar_t *nbest);

/**
 * Finish N-best search, releasing resources associated with it.
 */
void ps_astar_finish(ps_astar_t *nbest);

/**
 * Get hypothesis string from A* search.
 */
char const *ps_astar_hyp(ps_astar_t *nbest, ps_latpath_t *path);

/**
 * Get hypothesis segmentation from A* search.
 */
ps_seg_t *ps_astar_seg_iter(ps_astar_t *astar, ps_latpath_t *path, float32 lwf);


#endif /* __PS_LATTICE_INTERNAL_H__ */
