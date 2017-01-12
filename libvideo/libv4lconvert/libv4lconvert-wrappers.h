#ifndef __LIBV4LCONVERT_WRAPPERS_H
#define __LIBV4LCONVERT_WRAPPERS_H

ImageTransformer* ImageTransformer_imf_init(ImageTransformerPrototype* self, struct v4l2_format* src_fmt, struct v4l2_format* dst_fmt, char** errmsg, size_t options_len, ...);
ImageTransformer* ImageTransformer_jpeg_init(ImageTransformerPrototype* self, struct v4l2_format* src_fmt, struct v4l2_format* dst_fmt, char** errmsg, size_t options_len, ...);

#endif