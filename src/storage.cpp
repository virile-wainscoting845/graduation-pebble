#include "storage.h"
#include "config.h"
#include <Preferences.h>

static Preferences prefs;

bool storage_load(PetState *out) {
    if (!prefs.begin(cfg::NVS_NAMESPACE, /*readOnly*/ true)) return false;
    bool ok = false;
    PetState tmp{};
    size_t got = prefs.getBytes(cfg::NVS_STATE_KEY, &tmp, sizeof(tmp));
    if (got == sizeof(tmp) && tmp.magic == cfg::STATE_MAGIC) {
        uint32_t want = pet_compute_crc(&tmp);
        if (want == tmp.crc32) {
            *out = tmp;
            ok = true;
        }
    }
    prefs.end();
    return ok;
}

bool storage_save(const PetState *p) {
    PetState tmp = *p;
    tmp.magic = cfg::STATE_MAGIC;
    tmp.crc32 = pet_compute_crc(&tmp);
    if (!prefs.begin(cfg::NVS_NAMESPACE, /*readOnly*/ false)) return false;
    size_t put = prefs.putBytes(cfg::NVS_STATE_KEY, &tmp, sizeof(tmp));
    prefs.end();
    return put == sizeof(tmp);
}

void storage_wipe() {
    if (!prefs.begin(cfg::NVS_NAMESPACE, false)) return;
    prefs.clear();
    prefs.end();
}
