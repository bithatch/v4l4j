package au.edu.jcu.v4l4j.impl.jni;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import au.edu.jcu.v4l4j.V4L4JUtils;

public class MemoryUtils {

	static {
		V4L4JUtils.loadLibrary();
	}
	
	static native int getAlignment(int type);
	static native int getSize(int type);
	private static native ByteBuffer doWrap(long pointer, long length);
	
	public static native long unwrap(ByteBuffer buffer);
	public static ByteBuffer wrap(long pointer, long length) {
		ByteBuffer result = doWrap(pointer, length);
		result.order(ByteOrder.nativeOrder());
		return result;
	}
	
	public static long align(int alignment, long pointer) {
		return (pointer + alignment - 1) & ~(alignment - 1L);
	}
}