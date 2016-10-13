/*
* Copyright (C) 2007-2008 Gilles Gigan (gilles.gigan@gmail.com)
* eResearch Centre, James Cook University (eresearch.jcu.edu.au)
*
* This program was developed as part of the ARCHER project
* (Australian Research Enabling Environment) funded by a
* Systemic Infrastructure Initiative (SII) grant and supported by the Australian
* Department of Innovation, Industry, Science and Research
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public  License as published by the
* Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
#include <jni.h>
#include <stdint.h>

#include "libvideo.h"
#include "libvideo-palettes.h"
#include "common.h"
#include "debug.h"
#include "jniutils.c"

/**
 * Creates a ImageFormat object to wrap the given palette, and adds it to the list
 * @param e
 * 		JNI environment variable
 * @param list
 * 		List object to add format to
 * @param add_method
 * 		boolean List.add(Object) methodID for the given list object
 * @param format_class
 *		The class au.edu.jcu.v4l4j.ImageFormat
 * @param format_ctor
 * 		The constructor ImageFormat(String, int, long)
 * @param index
 * 		The index of the palette to add
 * @param dev
 * 		A pointer to the video device that the format is created for
 * @return 0 on success, else -1
 * @throws JNIException if there's a problem creating the given object
 */
static int add_format(JNIEnv *e, jobject list, jmethodID add_method, jclass format_class, jmethodID format_ctor, int index, struct v4l4j_device *dev){
	
	//Create an object to wrap the image format
	jstring name = (*e)->NewStringUTF(e, (const char*) libvideo_palettes[index].name);
	jobject obj = (*e)->NewObject(e, format_class, format_ctor, name, index, (jlong) (uintptr_t) dev);
	if(obj == NULL) {
		THROW_EXCEPTION(e, JNI_EXCP, "Error creating the ImageFormat object for palette %s (index %d)", libvideo_palettes[index].name, index);
		return -1;
	}
	
	//Add the ImageFormat object to the list
	(*e)->CallVoidMethod(e, list, add_method, obj);
	
	if ((*e)->ExceptionCheck(e))
		return -1;
	return 0;
}

static jobject lookupMember(JNIEnv* env, jobject self, jclass self_class, const char* name) {
	jfieldID member_fid = (*env)->GetFieldID(env, self_class, name, "Ljava/util/List");
	if(member_fid == NULL) {
		THROW_EXCEPTION(env, JNI_EXCP, "Error looking up the fieldID for %s", name);
		return NULL;
	}
	
	jobject member = (*env)->GetObjectField(env, self, memberfid);
	if(member == NULL)
		THROW_EXCEPTION(env, JNI_EXCP, "Error getting the value of member %s", name);
	return member;
}

/*
 * populate the formats, JPEGformats, RGBformats, YUVformats & YVUformats
 * members of the ImageFormat class with appropriate image formats
 */
JNIEXPORT void JNICALL Java_au_edu_jcu_v4l4j_ImageFormatList_listFormats(JNIEnv *e, jobject t, jlong o){
	dprint(LOG_CALLS, "[CALL] Entering %s\n",__PRETTY_FUNCTION__);
	jobject obj;
	struct v4l4j_device *d = (struct v4l4j_device *) (uintptr_t) o;
	
	/* Get handles on Java stuff */
	jclass this_class = (*e)->GetObjectClass(e, t);
	if(this_class == NULL) {
		THROW_EXCEPTION(e, JNI_EXCP, "Error looking up ImageFormatList class");
		return;
	}

	jclass format_class = (*e)->FindClass(e, "au/edu/jcu/v4l4j/ImageFormat");
	if(format_class == NULL) {
		THROW_EXCEPTION(e, JNI_EXCP, "Error looking up ImageFormat class");
		return;
	}

	jmethodID format_ctor = (*e)->GetMethodID(e, format_class, "<init>", "(Ljava/lang/String;IJ)V");
	if(format_ctor == NULL) {
		THROW_EXCEPTION(e, JNI_EXCP, "Error looking up the constructor of ImageFormat class");
		return;
	}

	jobject formats = lookupMember(e, t, this_class, "formats");
	if(formats == NULL)
		return;
	jmethodID formats_add_method = lookupAddMethod(e, formats);
	if (formats_add_method == NULL)
		return;
	
	//JPEG
	jobject jpeg_formats = lookupMember(e, t, this_class, "JPEGformats");
	if (jpeg_formats == NULL)
		return;
	jmethodID jpeg_formats_add = lookupAddMethod(e, jpeg_formats);
	if (jpeg_formats_add == NULL)
		return;
	
	//RGB
	jobject rgb_formats = lookupMember(e, t, this_class, "RGBformats");
	if (rgb_formats == NULL)
		return;
	jmethodID rgb_formats_add = lookupAddMethod(e, rgb_formats);
	if (rgb_formats_add == NULL)
		return;
	
	//BGR
	jobject bgr_formats = lookupMember(e, t, this_class, "BGRformats");
	if (bgr_formats == NULL)
		return;
	jmethodID bgr_formats_add = lookupAddMethod(e, bgr_formats);
	if (bgr_formats_add == NULL)
		return;
	
	jobject yuv420_formats = lookupMember(e, t, this_class, "YUV420formats");
	if (yuv420_formats == NULL)
		return;
	jmethodID yuv420_formats_add = lookupAddMethod(e, yuv420_formats);
	if (yuv420_formats_add == NULL)
		return;

	jobject yvu420_formats = lookupMember(e, t, this_class, "YVU420formats");
	if (yvu420_formats == NULL)
		return;
	jmethodID yvu420_formats_add = lookupAddMethod(e, yvu420_formats);
	if (yvu420_formats_add == NULL)
		return;
	
	int jpeg_conv_formats[] = JPEG_CONVERTIBLE_FORMATS;
	
	for (int i = 0; i < di->nb_palettes; i++) {
		struct palette_info palette = di->palettes[i];
		int palette_idx = palette.index;
		dprint(LOG_V4L4J, "[V4L4J] Checking format %s - index: %d - raw ? %s\n", libvideo_palettes[palette_idx].name, palette_idx, (!palette.raw_palettes?"Yes-adding it":"No-skipping it"));
		
		jobject format_list = NULL; //The list to add the format to
		jmethodID format_list_add = NULL; //The add method for format_list
		char* name = NULL; // For logging
		//Lookup which list to the image format to
		switch (palette_idx) {
			case YUV420:
				format_list = yuv420_formats;
				format_list_add = yuv420_formats_add;
				name = "YUV420";
				break;
			case YVU420:
				format_list = yvu420_formats;
				format_list_add = yvu420_formats_add;
				name = "YVU420";
				break;
			case RGB24:
				format_list = rgb_formats;
				format_list_add = rgb_formats_add;
				name = "RGB24";
				break;
			case BGR24:
				format_list = bgr_formats;
				format_list_add = bgr_formats_add;
				name = "RGB24";
				break;
			case JPEG:
				format_list = jpeg_formats;
				format_list_add = jpeg_formats_add;
				name = "RGB24";
				break;
			default:
				format_list = NULL;
				format_list_add = NULL;
				name = NULL;
				break;
		}
		
		//check if V4L4J can convert the format to JPEG
		//TODO optimize
		for(int j = 0; j < ARRAY_SIZE(jpeg_conv_formats); j++) {
			// V4L4J knows how to convert it to JPEG
			if(jpeg_conv_formats[j] == palette_idx) {
				dprint(LOG_V4L4J, "[V4L4J] Found v4l4j-convertible JPEG format from %s format - add it\n", libvideo_palettes[jpeg_conv_formats[j]].name);
				if(add_format(e, jpeg_formats, jpeg_formats_add, format_class, format_ctor, jpeg_conv_formats[j], d) == -1)
					return -1;
			}
		}
		
		if (palette.raw_palettes != NULL) {
			if (format_list == NULL)
				continue;
			for (int j = 0, raw_palette; (raw_palette = palette.raw_palettes[j]) != -1; j++) {
				dprint(LOG_V4L4J, "[V4L4J] Found libvideo-converted %s format from %s format - add it\n", name, libvideo_palettes[di->palettes[i].raw_palettes[j]].name);
				if (add_format(e, format_list, format_list_add, format_class, format_ctor, raw_palette, d) == -1)
					return -1;
			}
		} else {
			//Add to native format list
			if (add_format(e, formats, formats_add_method, format_class, format_ctor, palette_idx, d) == -1)
				return -1;
			//Add to other format list, if applicable
			if (format_list != NULL) {
				dprint(LOG_V4L4J, "[V4L4J] Found native %s format - adding it to list\n", name);
				if (add_format(e, format_list, format_list_add, format_class, format_ctor, palette_idx, d) == -1)
					return -1;
			}
		}	
	}
	return 0;
}

