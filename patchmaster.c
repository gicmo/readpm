#include "patchmaster.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int
pm_find_bundle_item(pm_bundle_header *header, const char *ext)
{
    int i;

    for (i = 0; i < header->nitems; i++) {
        pm_bundle_item *item = header->items + i;
        if (!strcmp(item->ext, ext))
            break;
    }

    return i < header->nitems ? i : -1;
}

int
pm_read_header(int fd, pm_bundle_header *header)
{
    int r;
    lseek(fd, 0, SEEK_SET);

    r = read(fd, header, sizeof(pm_bundle_header));
#ifdef DEBUG
    fprintf(stderr, "* r: %d\n", r);
#endif

    return r;
}

TreeNode *
pm_load_node(int fd, int nlevels, uint32_t *lvl_sizes, int level)
{
    TreeNode *self;
    uint32_t nchildren;
    int i;

    if (level > nlevels) {
        fprintf (stderr, "[E] level > nlevels!\n");
        return NULL;
    }

    self = malloc(sizeof(TreeNode));
    self->size = lvl_sizes[level];
    self->data = malloc (self->size);
    read(fd, self->data, self->size);


    read (fd, &nchildren, sizeof(nchildren));
    self->nchildren = nchildren;

#ifdef DEBUG
    fprintf (stderr, "* children %d\n", nchildren);
#endif

    if (nchildren > 0)
        self->children = malloc (sizeof (TreeNode *) * nchildren);
    else
        self->children = NULL;

    for (i = 0; i < nchildren; i++) {
        TreeNode *child;
        child = pm_load_node(fd, nlevels, lvl_sizes, level + 1);
        self->children[i] = child;
    }

    return self;
}

TreeNode *
pm_load_tree(int fd, int start)
{
    static uint32_t hr_magic = 0x54726565;
    uint32_t magic;
    uint32_t levels;
    uint32_t *lvl_sizes;
    uint32_t size;
    int i;
    TreeNode *root;

    lseek(fd, start, SEEK_SET);
    read(fd, &magic, sizeof(magic));

#ifdef DEBUG
    fprintf (stderr, "* %X\n", magic);
#endif

    if (magic != hr_magic) {
        fprintf (stderr, "Not a tree root\n");
        return NULL;
    }

    read(fd, &levels, sizeof(levels));
    printf("levels: %d\n", levels);

    //TODO: size check
    size = (uint32_t) sizeof(uint32_t) * levels;
    lvl_sizes = malloc(size);
    read(fd, lvl_sizes, size);

    for (i = 0; i < levels; i++) {
        printf("\t[%d] size: %d\n", i, lvl_sizes[i]);
    }

    root = pm_load_node(fd, levels, lvl_sizes, 0);
    return root;
}