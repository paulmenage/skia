/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrYUVProvider_DEFINED
#define GrYUVProvider_DEFINED

#include "GrTypes.h"
#include "SkImageInfo.h"
#include "SkYUVSizeInfo.h"

class GrContext;
class GrTexture;
class GrTextureProxy;

/**
 *  There are at least 2 different ways to extract/retrieve YUV planar data...
 *  - SkPixelRef
 *  - SkImageGenerator
 *
 *  To share common functionality around using the planar data, we use this abstract base-class
 *  to represent accessing that data.
 */
class GrYUVProvider {
public:
    virtual ~GrYUVProvider() {}

    /**
     *  On success, this returns a texture proxy that has converted the YUV data from the provider
     *  into a form that is supported by the GPU (typically transformed into RGB). The texture will
     *  automatically have a key added, so it can be retrieved from the cache (assuming it is
     *  requested by a provider w/ the same genID). If srcColorSpace and dstColorSpace are
     *  specified, then a color conversion from src to dst will be applied to the pixels.
     *
     *  On failure (e.g. the provider had no data), this returns NULL.
     */
    sk_sp<GrTextureProxy> refAsTextureProxy(GrContext*, const GrSurfaceDesc&,
                                            const SkColorSpace* srcColorSpace,
                                            const SkColorSpace* dstColorSpace);

    virtual uint32_t onGetID() = 0;

    // These are not meant to be called by a client, only by the implementation

    /**
     *  If decoding to YUV is supported, this returns true.  Otherwise, this
     *  returns false and does not modify any of the parameters.
     *
     *  @param sizeInfo   Output parameter indicating the sizes and required
     *                    allocation widths of the Y, U, and V planes.
     *  @param colorSpace Output parameter.
     */
    virtual bool onQueryYUV8(SkYUVSizeInfo* sizeInfo, SkYUVColorSpace* colorSpace) const = 0;

    /**
     *  Returns true on success and false on failure.
     *  This always attempts to perform a full decode.  If the client only
     *  wants size, it should call onQueryYUV8().
     *
     *  @param sizeInfo   Needs to exactly match the values returned by the
     *                    query, except the WidthBytes may be larger than the
     *                    recommendation (but not smaller).
     *  @param planes     Memory for each of the Y, U, and V planes.
     */
    virtual bool onGetYUV8Planes(const SkYUVSizeInfo& sizeInfo, void* planes[3]) = 0;

private:
    // This is used as release callback for the YUV data that we capture in an SkImage when
    // uploading to a gpu. When the upload is complete and we release the SkImage this callback will
    // release the underlying data.
    static void YUVGen_DataReleaseProc(const void*, void* data);
};

#endif
