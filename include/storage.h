#pragma once
#include "pet.h"

bool storage_load(PetState *out);
bool storage_save(const PetState *p);
void storage_wipe();
