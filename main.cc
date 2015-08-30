#include "patchmaster.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

static void
dump_time(double time)
{
    char *tstr;
    time_t t = (time_t) time;
    //t -= 4294967296.0;
    t += -3663799200LL; //-3663792000LL;
    tstr = ctime (&t);
    printf("Time: %s", tstr);
}

static void
dump_header(pm_bundle_header *header)
{
    int i;

    printf("sig: %s\n", header->signature);
    printf("ver: %s\n", header->version);
    printf("tim: %e\n", header->time);
    dump_time(header->time);
    printf("itm: %d\n", header->nitems);
    printf("le : %d\n", header->little_endian);
    printf("\n");

    for (i = 0; i < header->nitems; i++) {
        pm_bundle_item *item = header->items + i;

        printf ("item %d\n", i);
        printf ("\text  : %s\n", item->ext);
        printf ("\tstart: %d\n", item->start);
        printf ("\tend  : %d\n", item->end);
    }
}


static void
dump_amplifier_state(AmAmplifierState *state)
{
    printf("  Mode:  %hhd\n", state->Mode);
    printf("  Kind:  %hhd\n", state->AmplKind);
    printf("  RCG:   %e\n", state->RealCurrentGain);
    printf("  f2-fq: %e\n", state->F2Frequency);
    printf("  GLeak: %e\n", state->GLeak);
    printf("  RsOn:  %hhd\n", state->RsOn);

    printf("  RsVal: %e\n", state->RsValue);
    printf("  RsFrc: %e\n", state->RsFraction);
    printf("         %e\n", (1.0/state->RsFraction) *state->RsValue);
    printf("  CSlow: %e\n", state->CSlow);
    printf("  CFA1:  %e\n", state->CFastAmp1);
    printf("  CFA2:  %e\n", state->CFastAmp2);
    printf("  GSer:  %e\n", state->GSeries);
    printf("         %e\n", 1.0/state->GSeries);
    printf("  VHold: %e\n", state->VHold);
    printf("  VHo-1: %e\n", state->LastVHold);
    printf("  Gain:  %hhd\n", state->Gain);
    printf("  RsSl:  %hhd\n", state->RsSlow);

    printf("  MCap:  %e\n", state->MCapacitance);
    printf("  MCon:  %e\n", state->MConductance);
    printf("         %e\n", 1.0/state->MConductance);

}

static void
dump_pul_series (TreeNode *node)
{
    PulseSeries *series = (PulseSeries *) node->data;

    printf("\t\t\t mark:  %d\n", series->mark);
    printf("\t\t\t label: %s\n", series->label);
    printf("\t\t\t com:   %s\n", series->comment);
    printf("\t\t\t time:  %lf\n", series->time);
    dump_time(series->time);
    dump_amplifier_state (&(series->amp_state));
}

static void
dump_pul_group (TreeNode *node)
{
    PulseGroup *group;
    int i;

    group = (PulseGroup *) node->data;
    printf("\t\t mark:  %d\n", group->mark);
    printf("\t\t label: %s\n", group->label);
    printf("\t\t text:  %s\n", group->text);
    printf("\t\t exp-#: %d\n", group->experiment_number);

    for (i = 0; i < node->nchildren; i++) {
        printf("\t\t\t Series [%d]\n", i);
        dump_pul_series (node->children[i]);
    }
}

static void
dump_pul(TreeNode *node)
{
    int i;
    PulseRoot *root = (PulseRoot *) node->data;

    printf("Pulse\n");
    printf("\t version: %d\n", root->version);
    printf("\t mark:    %d\n", root->mark);
    printf("\t v-name:  %s\n", root->VersionName);
    printf("\t text:    %s\n", root->RootText);
    printf("\t t-start: %e\n", root->StartTime);

    for (i = 0; i < node->nchildren; i++) {
        printf("\t\t Group [%d]\n", i);
        dump_pul_group (node->children[i]);
    }
}


int main(int argc, char **argv) {
    int fd;
    int pos;
    int start;
    char buf[1024] = {0,};
    pm_bundle_header header;
    TreeNode *node;

    fd = open(argv[1], O_RDONLY);

    read (fd, buf, 4);
    printf("%s\n", buf);

    if (strcmp(buf, "DAT2")) {
        fprintf(stderr, "No int DAT2 file-format\n");
        return 1;
    }

    memset(&header, 0, sizeof(header));
    pm_read_header(fd, &header);
    dump_header(&header);

    pos = pm_find_bundle_item(&header, ".amp");
    if (pos == -1) {
        fprintf(stderr, "could not find .amp section\n");
        goto over_and_out;
    }

    start = header.items[pos].start;
    node = pm_load_tree(fd, start);

    pos = pm_find_bundle_item(&header, ".pul");
    start = header.items[pos].start;
    node = pm_load_tree(fd, start);
    dump_pul(node);

    over_and_out:
    if (fd > -1)
        close (fd);

    return 0;
}