/*
 * Copyright (C) 2019 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#pragma once

#include "gtkstyleprovider.h"

G_BEGIN_DECLS

/* The style provider priority to use for libhandy widgets custom styling. It is
 * higher than settings but lower than applications, so application developers
 * can nonetheless apply custom styling on top of it. */
#define GTK_HDY_STYLE_PROVIDER_PRIORITY (GTK_STYLE_PROVIDER_PRIORITY_SETTINGS + 1)

G_END_DECLS
