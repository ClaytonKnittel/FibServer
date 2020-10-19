#include <rbtree.h>



#define set_parent rb_set_parent
#define get_left   rb_get_left
#define set_left   rb_set_left
#define get_right  rb_get_right
#define set_right  rb_set_right
#define get_root   rb_get_root

#define color_proxy parent

static int is_red(rb_node * n) {
    return (n != LEAF)
        && (((unsigned long) n->color_proxy) & COLOR_MASK) == RED;
}
static int is_black(rb_node * n) {
    return (n == LEAF)
        || (((unsigned long) n->color_proxy) & COLOR_MASK) == BLACK;
}

static void set_red(rb_node * n) {
    n->color_proxy = (rb_node *) (((unsigned long) n->color_proxy) | RED);
}
static void set_black(rb_node * n) {
    n->color_proxy = (rb_node *) (((unsigned long) n->color_proxy) & ~RED);
}

static void copy_color(rb_node * dst, rb_node * src) {
    dst->color_proxy =
        (rb_node *) ((((unsigned long) dst->color_proxy) & ~COLOR_MASK)
            | (((unsigned long) src->color_proxy) & COLOR_MASK));
}



/*
 * red-black tree op helper methods
 */

static int is_root(rb_node * n) {
    return (((unsigned long) n->left) & ROOT) == ROOT;
}

static void mark_as_root(rb_node * n) {
    n->left = (rb_node *) (((unsigned long) n->left) | ROOT);
}

static void set_root(rb_tree * t, rb_node * node) {
    mark_as_root(node);
    t->root      = node;
    node->parent = (rb_node *) t;
}

static rb_node * get_parent(rb_node * n) {
    return (rb_node *) (((unsigned long) n->parent) & ~PTR_MASK);
}

// sets left child to node known not to be a leaf
static void set_left_noleaf(rb_node * n, rb_node * l) {
    n->left = (rb_node *) ((((unsigned long) n->left) & PTR_MASK)
        | (unsigned long) l);
    set_parent(l, n);
}

// sets right child to node known not to be a leaf
static void set_right_noleaf(rb_node * n, rb_node * r) {
    n->right = r;
    set_parent(r, n);
}

/*
 * transfers root mark from old_root to new_root, which will mark the ROOT
 * bit of the left child of new_root and unmark old_root as the root if
 * old_root was the root, otherwise it will do nothing
 */
static void transfer_root_mark(rb_node * new_root, rb_node * old_root) {
    size_t root_mark = ((size_t) old_root->left) & ROOT;
    old_root->left = (rb_node *) (((unsigned long) old_root->left) ^ root_mark);
    new_root->left = (rb_node *) (((unsigned long) new_root->left) | root_mark);
}

// sets n to be the new child of the parent of par (gp)
static void set_parent_pointer(rb_node * n, rb_node * par) {
    rb_node * gp = get_parent(par);
    transfer_root_mark(n, par);
    if (get_left(gp) == par) {
        set_left(gp, n);
    }
    else {
        set_right(gp, n);
    }
}


// right rotate about p, with r = left(p)
static void rb_rotate_right(rb_node * p, rb_node * r) {
    set_parent_pointer(r, p);
    set_left(p, get_right(r));
    set_right_noleaf(r, p);
}

// left rotate about p, with l = right(p)
static void rb_rotate_left(rb_node * p, rb_node * l) {
    set_parent_pointer(l, p);
    set_right(p, get_left(l));
    set_left_noleaf(l, p);
}

// left-right rotate about p, with
// l = left(p), lr = right(l)
static void rb_rotate_left_right(rb_node * p, rb_node * l, rb_node * lr) {
    set_parent_pointer(lr, p);

    set_right(l, get_left(lr));
    set_left(p, get_right(lr));
    set_left_noleaf(lr, l);
    set_right_noleaf(lr, p);

    /*l->right = lr->left;
    p->left = lr->right;
    lr->left = l;
    l->parent = lr;
    lr->right = p;
    p->parent = lr;*/
}

// right-left rotate about p, with
// r = right(p), rl = left(r)
static void rb_rotate_right_left(rb_node * p, rb_node * r, rb_node * rl) {
    set_parent_pointer(rl, p);

    set_left(r, get_right(rl));
    set_right(p, get_left(rl));
    set_right_noleaf(rl, r);
    set_left_noleaf(rl, p);

    /*r->left = rl->right;
    p->right = rl->left;
    rl->right = r;
    r->parent = rl;
    rl->left = p;
    p->parent = rl;*/
}



/*
 * -------------------------------- rb_insert --------------------------------
 */

static void rb_insert_fix(rb_node * node) {
    // parent, grandparent, and uncle (parent's siblin)
    rb_node *p, *gp, *u;
    while (!is_root(node) && is_red(p = get_parent(node))) {
        if (p == get_left(gp = get_parent(p))) {
            u = get_right(gp);
            if (is_black(u)) {
                // rotate
                if (get_right(p) == node) {
                    rb_rotate_left_right(gp, p, node);
                    set_red(gp);
                    set_black(node);
                    node = p;   // so the loop terminates
                }
                else {
                    rb_rotate_right(gp, p);
                    set_red(gp);
                    set_black(p);
                }
            }
            else {
                // recolor
                set_red(gp);
                set_black(p);
                set_black(u);
                // we still haven't fixed the imbalance
                // in black height, so propogate upwards
                node = gp;
            }
        }
        else {
            u = get_left(gp);
            if (is_black(u)) {
                // rotate
                if (get_left(p) == node) {
                    rb_rotate_right_left(gp, p, node);
                    set_red(gp);
                    set_black(node);
                    node = p;   // so the loop terminates
                }
                else {
                    rb_rotate_left(gp, p);
                    set_red(gp);
                    set_black(p);
                }
            }
            else {
                // recolor
                set_red(gp);
                set_black(p);
                set_black(u);

                node = gp;
            }
        }
    }
    if (is_root(node)) {
        set_black(node);
    }
}

/*
 * Inserts an already mallocced rb_node into the tree.
 */
int _rb_insert_helper(rb_tree * tree, rb_node * node, rb_node * p) {
    set_left(node, LEAF);
    set_right(node, LEAF);

    if (p != LEAF) {
        set_red(node);
        rb_insert_fix(node);
    }
    else {
        set_root(tree, node);
        set_black(node);
    }

    return 1;   // success
}



/*
 * -------------------------------- rb_remove --------------------------------
 */


static void rb_remove_fix(rb_node * node) {
    // parent, sibling (of p), and a placeholder for rotations
    rb_node *p, *s, *slr;

    while (!is_root(node) && is_black(node)) {
        p = get_parent(node);
        if (node == get_left(p)) {
            s = get_right(p);
            if (is_red(s)) {
                rb_rotate_left(p, s);
                set_red(p);
                set_black(s);

                s = get_right(p);
            }
            // now s is guarenteed to be black
            if (is_black(get_left(s)) && is_black(get_right(s))) {
                set_red(s);
                // if p is already red, loop terminates and p is set
                // to black. Else, p was black and is now double black,
                // so we need to propogate the fix up through with p
                node = p;
            }
            else {
                if (is_red(get_left(s))) {
                    // right-left rotate about p, making
                    // left(s) (slr) the new root
                    slr = get_left(s);
                    rb_rotate_right_left(p, s, slr);

                    copy_color(slr, p);
                    set_black(p);
                    set_black(s);

                    break;
                    // node = get_root(m); // so the loop terminates
                }
                else {
                    // left rotate about p
                    rb_rotate_left(p, s);

                    copy_color(s, p);
                    set_black(p);
                    set_black(get_right(s));

                    break;
                    // node = get_root(m);
                }
            }
        }
        else {
            s = get_left(p);
            if (is_red(s)) {
                // right rotate about p
                rb_rotate_right(p, s);
                set_red(p);
                set_black(s);

                s = get_left(p);
            }
            // now s is guarenteed to be black
            if (is_black(get_right(s)) && is_black(get_left(s))) {
                set_red(s);
                node = p;
            }
            else {
                if (is_red(get_right(s))) {
                    // left-right rotate about p, making
                    // right(s) (slr) the new root
                    slr = get_right(s);
                    rb_rotate_left_right(p, s, slr);

                    copy_color(slr, p);
                    set_black(p);
                    set_black(s);

                    break;
                    // node = get_root(m);
                }
                else {
                    // right rotate about p
                    rb_rotate_right(p, s);

                    copy_color(s, p);
                    set_black(p);
                    set_black(get_left(s));

                    break;
                    // node = get_root(m);
                }
            }
        }
    }
    set_black(node);
}


/*
 * Remove rb_node from the tree. This does not free the
 * memory.
 */
void _rb_remove_helper(rb_tree * tree, rb_node * node) {
    rb_node *replacer, *child, *par;

    // if this node is internal (has 2 children), then we want to
    // swap it with its successor and then delete the node, which
    // now has at most 1 non-leaf child
    if (get_left(node) == LEAF || get_right(node) == LEAF) {
        replacer = node;
    }
    else {
        // find successor of node
        replacer = get_right(node);
        while (get_left(replacer) != LEAF) {
            replacer = get_left(replacer);
        }
    }


    // if we removed a red node, the black height of the tree clearly
    // couldn't have changed, so we're good. Otherwise, we have to
    // fix any rb tree violations we may have incurred
    if (is_black(replacer)) {
        rb_remove_fix(replacer);
    }

    // proceed to remove replacer from the graph, which
    // is guarenteed to have at most 1 non-leaf child

    child = (get_right(replacer) == LEAF ? get_left(replacer)
                                         : get_right(replacer));
    if (child != LEAF && is_root(replacer)) {
        mark_as_root(child);
        set_black(child);
    }

    par = get_parent(replacer);
    if (get_left(par) == replacer) {
        set_left(par, child);
    }
    else {
        set_right(par, child);
    }

    // now put replacer where node is, if they are not the same
    if (node != replacer) {
        // we want to preserve the color of node when replacing
        // with replacer, so we can directly copy fields
        set_left(replacer, get_left(node));
        set_right(replacer, get_right(node));
        set_parent(replacer, get_parent(node));
        copy_color(replacer, node);
        transfer_root_mark(replacer, node);
        if (get_left(get_parent(node)) == node) {
            set_left(get_parent(node), replacer);
        }
        else {
            set_right(get_parent(node), replacer);
        }
    }
}



static rb_node * rb_find_leftmost_child(rb_node * node) {
    if (node != LEAF) {
        while (get_left(node) != LEAF) {
            node = get_left(node);
        }
    }
    return node;
}

rb_node * rb_find_leftmost(rb_tree * tree) {
    rb_node * l = rb_get_root(tree);
    return rb_find_leftmost_child(l);
}

rb_node * rb_find_successor(rb_node * node) {
    rb_node * p;

    if (get_right(node) != LEAF) {
        return rb_find_leftmost_child(get_right(node));
    }
    else {
        while (!is_root(node) && get_right((p = get_parent(node))) == node) {
            node = p;
        }
        if (is_root(node)) {
            return LEAF;
        }
        else {
            return p;
        }
    }
}



static void _rb_print(rb_node * node, int depth) {
    static const char * colstr[2] = { "black", "red" };
    if (!node)
        return;

    printf("%*.*s%p (%s)\n", depth, depth, "", node, colstr[is_red(node)]);
    _rb_print(get_left(node), depth + 1);
    _rb_print(get_right(node), depth + 1);
}


void rb_print(rb_tree * tree) {
    _rb_print(get_root(tree), 0);
}



/*
 * rb tree validation
 */

static size_t black_depth(rb_node * node) {
    if (node == LEAF)
        return 0;
    size_t l, r;
    l = black_depth(get_left(node));
    r = black_depth(get_right(node));
    if (l == -1LU || r == -1LU || l != r)
        return -1LU;
    return l + (is_black(node) ? 1 : 0);
}


static char no_red_red(rb_node * node) {
    if (node == LEAF)
        return 1;
    if (is_red(node)) {
        if (is_red(get_left(node)) || is_red(get_right(node))) {
            return 0;
        }
    }
    return no_red_red(get_left(node)) && no_red_red(get_right(node));
}


static char parents_valid(rb_node * node) {
    if (node == LEAF)
        return 1;
    if (get_left(node) != LEAF) {
        if (get_parent(get_left(node)) != node)
            return 0;
        if (!parents_valid(get_left(node)))
            return 0;
    }
    if (get_right(node) != LEAF) {
        if (get_parent(get_right(node)) != node)
            return 0;
        if (!parents_valid(get_right(node)))
            return 0;
    }
    return 1;
}

static char no_other_roots(rb_node * node) {
    if (node == LEAF)
        return 1;
    if (is_root(node))
        return 0;
    return no_other_roots(get_left(node)) && no_other_roots(get_right(node));
}


void _rb_validate_helper(rb_tree * tree) {
    rb_node * root;

    root = get_root(tree);

    RB_ASSERT(!is_red(root), tree);
    RB_ASSERT(black_depth(root) != -1LU, tree);
    RB_ASSERT(no_red_red(root), tree);
    RB_ASSERT(parents_valid(root), tree);

    if (root != LEAF) {
        RB_ASSERT(is_root(root), tree);
        RB_ASSERT(get_parent(root) == (rb_node *) tree, tree);
        RB_ASSERT(
            no_other_roots(get_left(root)) && no_other_roots(get_right(root)),
            tree);
    }
}
