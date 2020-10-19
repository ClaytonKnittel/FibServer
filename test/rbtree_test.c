
#include <rbtree.h>


int main(int argc, char * argv[]) {
    rb_tree tree;

    rb_init(&tree);

    rb_node el;
    rb_node el2;
    rb_insert_ptr(&tree, &el);
    rb_insert_ptr(&tree, &el2);

    rb_validate_ptr(&tree);
    rb_print(&tree);

    return 0;
}


