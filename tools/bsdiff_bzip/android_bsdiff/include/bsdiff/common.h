// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _BSDIFF_COMMON_H_
#define _BSDIFF_COMMON_H_

// We defined this export macro to avoid depending on brillo (BRILLO_EXPORT in
// "brillo/brillo_export.h")
#ifndef BSDIFF_EXPORT
#ifdef WIN32
#define BSDIFF_EXPORT 
#else
#define BSDIFF_EXPORT __attribute__((__visibility__("default")))
#endif
#endif

#endif  // _BSDIFF_COMMON_H_
