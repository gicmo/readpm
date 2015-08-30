#ifndef PATCHMASTER_H
#define PATCHMASTER_H

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
typedef bool _Bool;
#endif

typedef struct pm_bundle_item {
    uint32_t start;
    uint32_t end;
    char     ext[8];
} pm_bundle_item;

typedef struct pm_bundle_header {
    char   signature[8];
    char   version[32];
    double time;
    int32_t nitems;
    _Bool   little_endian;
    char    reserved[11];
    pm_bundle_item items[12];
} pm_bundle_header;

typedef struct TreeNode {

    uint32_t  nchildren;
    struct TreeNode **children;

    uint32_t  size;
    uint8_t  *data;

} TreeNode;

typedef struct AmAmplifierState {
    double StateVersion; //FIXMDE
    double RealCurrentGain;
    double RealF2Bandwidth;
    double F2Frequency;
    double RsValue;
    double RsFraction;
    double GLeak;
    double CFastAmp1;
    double CFastAmp2;
    double CFastTau;
    double CSlow;
    double GSeries;
    double StimDacScale;
    double CCStimScale;
    double VHold;
    double LastVHold;
    double VpOffset;
    double VLiquidJunction;

    double CCIHold;
    double CSlowStimVolts;
    double CCTrackVHold;
    double TimeoutLength;
    double SearchDelay;
    double MConductance;
    double MCapacitance;
    double SerialNumber; //FIXME

    int16_t E9Boards;
    int16_t CSlowCycles;
    int16_t IMonAdc;
    int16_t VMonAdc;
    int16_t MuxAdc;
    int16_t TstDac;
    int16_t StimDac;
    int16_t StimDacOffset;
    int16_t MaxDigitalBit;

    int16_t SpareInt1;
    int16_t SpareInt2;
    int16_t SpareInt3;

    uint8_t AmplKind;
    uint8_t IsEpc9N;
    uint8_t ADBoard;
    uint8_t BoardVersion;
    uint8_t ActiveE9Board;
    uint8_t Mode;
    uint8_t Range;
    uint8_t F2Response;

    uint8_t RsOn;
    uint8_t CSlowRange;
    uint8_t CCRange;
    uint8_t CCGain;
    char dummy2[36];
    uint8_t Gain;
    uint8_t Filter1;
    uint8_t StimFilterOn;
    uint8_t RsSlow;

} AmAmplifierState;

typedef struct AmplStateRecord {
    char dummy0[112];
    AmAmplifierState state;

} AmplStateRecord;

typedef struct PulseRoot {
    int32_t version;
    int32_t mark;
    char    VersionName[32];
    char    AuxFileName[80];
    char    RootText[400];
    double StartTime;
} PulseRoot;

typedef struct PulseGroup {
    int32_t mark;
    char    label[32];
    char    text[80];
    int32_t experiment_number;

} PulseGroup;

typedef struct PulseSeries {
    int32_t mark;
    char    label[32];
    char    comment[80];
    int32_t series_count;
    int32_t number_sweeps;
    int32_t ampl_state_offset;
    int32_t ampl_state_series;
    uint8_t series_type;
    _Bool   use_x_start;
    uint8_t Filler1;
    uint8_t Filler2;
    double  time;
    double  page_width;
    char    dummy[320];
    AmAmplifierState amp_state;

} PulseSeries;

TreeNode *pm_load_node(int fd, int nlevels, uint32_t *lvl_sizes, int level);
TreeNode *pm_load_tree(int fd, int start);

void pm_free(TreeNode **node);

int pm_read_header(int fd, pm_bundle_header *header);
int pm_find_bundle_item(pm_bundle_header *header, const char *ext);

#ifdef __cplusplus
}
#endif

#endif