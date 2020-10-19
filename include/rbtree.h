#pragma once


#ifdef __cplusplus
extern "C" {
#endif


#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <util.h>


/*struct mstate;
extern void print_heap(struct mstate *m);
static struct mstate* find_mstate_for_ptr_2(void* ptr) {
    char *h;
    struct mstate *m;

    h = (char*) ALIGN_DOWN((uint64_t) ptr, MAX_HEAP_SIZE);
    //m = &h->state;
    m = (struct mstate*) (h + 64);
    return m;
}*/
// print_heap(find_mstate_for_ptr_2(tree));

#define RB_ASSERT(expr, tree)           \
    if (__builtin_expect(!(expr), 0)) { \
        printf("tree %p:\n", (tree));   \
        rb_print(tree);                 \
    }                                   \
    FIB_ASSERT(expr)


// declare red-black nodes early for usage in this struct
typedef struct rb_node {
    struct rb_node * left;
    struct rb_node * right;
    struct rb_node * parent;
} rb_node;



typedef struct rb_tree {
    struct rb_node * root;
} rb_tree;

/*
 * pointers as bitfields:
 *
 * the first bit of each pointer field in an rb_node struct is to be used
 * by the node to store information about its own state. The second bit stores
 * information about the node it points to
 *
 *
 *      - R/B says whether node it points to is red or black
 *      - root is set if this node is the root
 *      - col is the color (red or black) of this node
 *
 * left:
 *      +-------- ... -+-----+-----+-----+
 *      |         ...  |     | R/B |root?|
 *      +-------- ... -+-----+-----+-----+
 *
 * right:
 *      +-------- ... -+-----+-----+-----+
 *      |         ...  |     | R/B |     |
 *      +-------- ... -+-----+-----+-----+
 *
 * parent:
 *      +-------- ... -+-----+-----+-----+
 *      |         ...  |     | R/B | col |
 *      +-------- ... -+-----+-----+-----+
 *
 *
 *
 */



// to be put in the parent pointer field,
#define BLACK 0x0
#define RED   0x1

#define COLOR_MASK (BLACK | RED)

// to be put in left pointer field
#define ROOT 0x1

// mask covering all used bitfields in a pointer
#define PTR_MASK (RED | BLACK | ROOT)

#define LEAF ((rb_node *) BLACK)



static rb_node * rb_get_root(rb_tree * t) {
    return t->root;
}

static void rb_set_parent(rb_node * n, rb_node * p) {
    n->parent = (rb_node *) ((((unsigned long) n->parent) & PTR_MASK)
        | ((unsigned long) p));
}

static rb_node * rb_get_left(rb_node * n) {
    return (rb_node *) (((unsigned long) n->left) & ~PTR_MASK);
}

static void rb_set_left(rb_node * n, rb_node * l) {
    n->left = (rb_node *) ((((unsigned long) n->left) & PTR_MASK)
        | (unsigned long) l);
    if (l != LEAF) {
        rb_set_parent(l, n);
    }
}

static rb_node * rb_get_right(rb_node * n) {
    return n->right;
}

static void rb_set_right(rb_node * n, rb_node * r) {
    n->right = r;
    if (r != LEAF) {
        rb_set_parent(r, n);
    }
}


/*
 * Initialize the red black tree. Called only once during the initialization
 * of the state struct.
 *
 */
static void rb_init(rb_tree * tree) {
    tree->root = LEAF;
}


/*
 * macro for defining the find function for rb nodes, which essentially
 * determines the way in which the red-black tree sorts its elements.
 *
 * Find will return the leftmost node of tree which could be a parent of node
 * without violating the BST property of the tree
 */

#define RB_DEFINE_FIND_LOC(name, less_fn)                                   \
    static rb_node * rb_find_##name##_loc(rb_tree * tree, rb_node * node) { \
        rb_node * prev_node = LEAF;                                         \
        rb_node * root      = rb_get_root(tree);                            \
        while (root != LEAF) {                                              \
            prev_node = root;                                               \
            root      = (less_fn(root, node) ? rb_get_right(root)           \
                                        : rb_get_left(root));          \
        }                                                                   \
        return prev_node;                                                   \
    }



#define RB_DEFINE_CONTAINS_HELPER(name, less_fn)                               \
    static int _rb_contains_##name##_helper(rb_node * root, rb_node * node) {  \
        while (root != LEAF) {                                                 \
            if (root == node)                                                  \
                return 1;                                                      \
            if (!less_fn(root, node) && !less_fn(node, root)) {                \
                return _rb_contains_##name##_helper(rb_get_left(root), node)   \
                    || _rb_contains_##name##_helper(rb_get_right(root), node); \
            }                                                                  \
            root = less_fn(root, node) ? rb_get_right(root)                    \
                                       : rb_get_left(root);                    \
        }                                                                      \
        return 0;                                                              \
    }


#define RB_DEFINE_CONTAINS(name, less_fn)                           \
    static int rb_contains_##name(rb_tree * tree, rb_node * node) { \
        rb_node * root;                                             \
        root = rb_get_root(tree);                                   \
        return _rb_contains_##name##_helper(root, node);            \
    }



int _rb_insert_helper(rb_tree * tree, rb_node * node, rb_node * p);

#define RB_DEFINE_INSERT(name, less_fn)                           \
    static int rb_insert_##name(rb_tree * tree, rb_node * node) { \
        rb_node * p;                                              \
        memset(node, 0, sizeof(rb_node));                         \
        p = rb_find_##name##_loc(tree, node);                     \
        if (p != LEAF) {                                          \
            if (less_fn(p, node)) {                               \
                rb_set_right(p, node);                            \
            }                                                     \
            else {                                                \
                rb_set_left(p, node);                             \
            }                                                     \
        }                                                         \
        return _rb_insert_helper(tree, node, p);                  \
    }


void _rb_remove_helper(rb_tree * tree, rb_node * node);

#define RB_DEFINE_REMOVE(name)                                     \
    static void rb_remove_##name(rb_tree * tree, rb_node * node) { \
        RB_ASSERT(rb_contains_##name(tree, node), tree);           \
        _rb_remove_helper(tree, node);                             \
    }


/*
 * does generic consistency checks (all besides BST property, which must
 * be generated with macros)
 */
void _rb_validate_helper(rb_tree * tree);


#define RB_DEFINE_BST_CHECK(name, less_fn)                     \
    static void _bst_check_##name(rb_node * node) {            \
        if (rb_get_left(node) != LEAF) {                       \
            FIB_ASSERT(!less_fn(node, rb_get_left(node)));  \
            _bst_check_##name(rb_get_left(node));              \
        }                                                      \
        if (rb_get_right(node) != LEAF) {                      \
            FIB_ASSERT(!less_fn(rb_get_right(node), node)); \
            _bst_check_##name(rb_get_right(node));             \
        }                                                      \
    }


#define RB_DEFINE_VALIDATE(name)                     \
    static void rb_validate_##name(rb_tree * tree) { \
        rb_node * root = rb_get_root(tree);          \
        _rb_validate_helper(tree);                   \
        if (root != LEAF) {                          \
            _bst_check_##name(root);                 \
        }                                            \
    }



#define RB_DEFINE_TYPE(name, less_fn)        \
    RB_DEFINE_FIND_LOC(name, less_fn)        \
    RB_DEFINE_CONTAINS_HELPER(name, less_fn) \
    RB_DEFINE_CONTAINS(name, less_fn)        \
    RB_DEFINE_INSERT(name, less_fn)          \
    RB_DEFINE_REMOVE(name)                   \
    RB_DEFINE_BST_CHECK(name, less_fn)       \
    RB_DEFINE_VALIDATE(name)

static int rb_is_empty(rb_tree * tree) {
    return rb_get_root(tree) == LEAF;
}



rb_node * rb_find_leftmost(rb_tree * tree);

rb_node * rb_find_successor(rb_node * node);


#define rb_for_each(tree, node)                             \
    for ((node) = rb_find_leftmost((tree)); (node) != LEAF; \
         (node) = rb_find_successor(node))



void rb_print(rb_tree * tree);



/*static int ptr_less(rb_node* a, rb_node* b) {
    return ((unsigned long) a) < ((unsigned long) b);
}*/

RB_DEFINE_TYPE(ptr, ptr_less)

#ifdef __cplusplus
}
#endif
