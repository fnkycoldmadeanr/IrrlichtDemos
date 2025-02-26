/* $Id: tif_aux.c,v 1.23 2008/06/08 18:47:32 drolon Exp $ */

/*
 * Copyright (c) 1991-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * TIFF Library.
 *
 * Auxiliary Support Routines.
 */
#include "tiffiop.h"
#include "tif_predict.h"
#include <math.h>

tdata_t
_TIFFCheckRealloc(TIFF* tif, tdata_t buffer,
		  size_t nmemb, size_t elem_size, const char* what)
{
	tdata_t cp = NULL;
	tsize_t	bytes = nmemb * elem_size;

	/*
	 * XXX: Check for integer overflow.
	 */
	if (nmemb && elem_size && bytes / elem_size == nmemb)
		cp = _TIFFrealloc(buffer, bytes);

	if (cp == NULL)
		TIFFErrorExt(tif->tif_clientdata, tif->tif_name,
			     "No space %s", what);

	return cp;
}

tdata_t
_TIFFCheckMalloc(TIFF* tif, size_t nmemb, size_t elem_size, const char* what)
{
	return _TIFFCheckRealloc(tif, NULL, nmemb, elem_size, what);
}

static int
TIFFDefaultTransferFunction(TIFFDirectory* td)
{
	uint16 **tf = td->td_transferfunction;
	tsize_t i, n, nbytes;

	tf[0] = tf[1] = tf[2] = 0;
	if (td->td_bitspersample >= sizeof(tsize_t) * 8 - 2)
		return 0;

	n = 1<<td->td_bitspersample;
	nbytes = n * sizeof (uint16);
	if (!(tf[0] = (uint16 *)_TIFFmalloc(nbytes)))
		return 0;
	tf[0][0] = 0;
	for (i = 1; i < n; i++) {
		double t = (double)i/((double) n-1.);
		tf[0][i] = (uint16)floor(65535.*pow(t, 2.2) + .5);
	}

	if (td->td_samplesperpixel - td->td_extrasamples > 1) {
		if (!(tf[1] = (uint16 *)_TIFFmalloc(nbytes)))
			goto bad;
		_TIFFmemcpy(tf[1], tf[0], nbytes);
		if (!(tf[2] = (uint16 *)_TIFFmalloc(nbytes)))
			goto bad;
		_TIFFmemcpy(tf[2], tf[0], nbytes);
	}
	return 1;

bad:
	if (tf[0])
		_TIFFfree(tf[0]);
	if (tf[1])
		_TIFFfree(tf[1]);
	if (tf[2])
		_TIFFfree(tf[2]);
	tf[0] = tf[1] = tf[2] = 0;
	return 0;
}

/*
 * Like TIFFGetField, but return any default
 * value if the tag is not present in the directory.
 *
 * NB:	We use the value in the directory, rather than
 *	explcit values so that defaults exist only one
 *	place in the library -- in TIFFDefaultDirectory.
 */
int
TIFFVGetFieldDefaulted(TIFF* tif, ttag_t tag, va_list ap)
{
	TIFFDirectory *td = &tif->tif_dir;

	if (TIFFVGetField(tif, tag, ap))
		return (1);
	switch (tag) {
	case TIFFTAG_SUBFILETYPE:
		*va_arg(ap, uint32 *) = td->td_subfiletype;
		return (1);
	case TIFFTAG_BITSPERSAMPLE:
		*va_arg(ap, uint16 *) = td->td_bitspersample;
		return (1);
	case TIFFTAG_THRESHHOLDING:
		*va_arg(ap, uint16 *) = td->td_threshholding;
		return (1);
	case TIFFTAG_FILLORDER:
		*va_arg(ap, uint16 *) = td->td_fillorder;
		return (1);
	case TIFFTAG_ORIENTATION:
		*va_arg(ap, uint16 *) = td->td_orientation;
		return (1);
	case TIFFTAG_SAMPLESPERPIXEL:
		*va_arg(ap, uint16 *) = td->td_samplesperpixel;
		return (1);
	case TIFFTAG_ROWSPERSTRIP:
		*va_arg(ap, uint32 *) = td->td_rowsperstrip;
		return (1);
	case TIFFTAG_MINSAMPLEVALUE:
		*va_arg(ap, uint16 *) = td->td_minsamplevalue;
		return (1);
	case TIFFTAG_MAXSAMPLEVALUE:
		*va_arg(ap, uint16 *) = td->td_maxsamplevalue;
		return (1);
	case TIFFTAG_PLANARCONFIG:
		*va_arg(ap, uint16 *) = td->td_planarconfig;
		return (1);
	case TIFFTAG_RESOLUTIONUNIT:
		*va_arg(ap, uint16 *) = td->td_resolutionunit;
		return (1);
	case TIFFTAG_PREDICTOR:
                {
			TIFFPredictorState* sp = (TIFFPredictorState*) tif->tif_data;
			*va_arg(ap, uint16*) = (uint16) sp->predictor;
			return 1;
                }
	case TIFFTAG_DOTRANGE:
		*va_arg(ap, uint16 *) = 0;
		*va_arg(ap, uint16 *) = (1<<td->td_bitspersample)-1;
		return (1);
	case TIFFTAG_INKSET:
		*va_arg(ap, uint16 *) = INKSET_CMYK;
		return 1;
	case TIFFTAG_NUMBEROFINKS:
		*va_arg(ap, uint16 *) = 4;
		return (1);
	case TIFFTAG_EXTRASAMPLES:
		*va_arg(ap, uint16 *) = td->td_extrasamples;
		*va_arg(ap, uint16 **) = td->td_sampleinfo;
		return (1);
	case TIFFTAG_MATTEING:
		*va_arg(ap, uint16 *) =
		    (td->td_extrasamples == 1 &&
		     td->td_sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA);
		return (1);
	case TIFFTAG_TILEDEPTH:
		*va_arg(ap, uint32 *) = td->td_tiledepth;
		return (1);
	case TIFFTAG_DATATYPE:
		*va_arg(ap, uint16 *) = td->td_sampleformat-1;
		return (1);
	case TIFFTAG_SAMPLEFORMAT:
		*va_arg(ap, uint16 *) = td->td_sampleformat;
                return(1);
	case TIFFTAG_IMAGEDEPTH:
		*va_arg(ap, uint32 *) = td->td_imagedepth;
		return (1);
	case TIFFTAG_YCBCRCOEFFICIENTS:
		{
			/* defaults are from CCIR Recommendation 601-1 */
			static float ycbcrcoeffs[] = { 0.299f, 0.587f, 0.114f };
			*va_arg(ap, float **) = ycbcrcoeffs;
			return 1;
		}
	case TIFFTAG_YCBCRSUBSAMPLING:
		*va_arg(ap, uint16 *) = td->td_ycbcrsubsampling[0];
		*va_arg(ap, uint16 *) = td->td_ycbcrsubsampling[1];
		return (1);
	case TIFFTAG_YCBCRPOSITIONING:
		*va_arg(ap, uint16 *) = td->td_ycbcrpositioning;
		return (1);
	case TIFFTAG_WHITEPOINT:
		{
			static float whitepoint[2];

			/* TIFF 6.0 specification tells that it is no default
			   value for the WhitePoint, but AdobePhotoshop TIFF
			   Technical Note tells that it should be CIE D50. */
			whitepoint[0] =	D50_X0 / (D50_X0 + D50_Y0 + D50_Z0);
			whitepoint[1] =	D50_Y0 / (D50_X0 + D50_Y0 + D50_Z0);
			*va_arg(ap, float **) = whitepoint;
			return 1;
		}
	case TIFFTAG_TRANSFERFUNCTION:
		if (!td->td_transferfunction[0] &&
		    !TIFFDefaultTransferFunction(td)) {
			TIFFErrorExt(tif->tif_clientdata, tif->tif_name, "No space for \"TransferFunction\" tag");
			return (0);
		}
		*va_arg(ap, uint16 **) = td->td_transferfunction[0];
		if (td->td_samplesperpixel - td->td_extrasamples > 1) {
			*va_arg(ap, uint16 **) = td->td_transferfunction[1];
			*va_arg(ap, uint16 **) = td->td_transferfunction[2];
		}
		return (1);
	case TIFFTAG_REFERENCEBLACKWHITE:
		{
			int i;
			static float ycbcr_refblackwhite[] = 
			{ 0.0F, 255.0F, 128.0F, 255.0F, 128.0F, 255.0F };
			static float rgb_refblackwhite[6];

			for (i = 0; i < 3; i++) {
				rgb_refblackwhite[2 * i + 0] = 0.0F;
				rgb_refblackwhite[2 * i + 1] =
					(float)((1L<<td->td_bitspersample)-1L);
			}
			
			if (td->td_photometric == PHOTOMETRIC_YCBCR) {
				/*
				 * YCbCr (Class Y) images must have the
				 * ReferenceBlackWhite tag set. Fix the
				 * broken images, which lacks that tag.
				 */
				*va_arg(ap, float **) = ycbcr_refblackwhite;
			} else {
				/*
				 * Assume RGB (Class R)
				 */
				*va_arg(ap, float **) = rgb_refblackwhite;
			}
			return 1;
		}
	}
	return 0;
}

/*
 * Like TIFFGetField, but return any default
 * value if the tag is not present in the directory.
 */
int
TIFFGetFieldDefaulted(TIFF* tif, ttag_t tag, ...)
{
	int ok;
	va_list ap;

	va_start(ap, tag);
	ok =  TIFFVGetFieldDefaulted(tif, tag, ap);
	va_end(ap);
	return (ok);
}

/* vim: set ts=8 sts=8 sw=8 noet: */
