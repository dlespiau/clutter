LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := clutter-test-actors
LOCAL_SRC_FILES := main.c
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz
LOCAL_STATIC_LIBRARIES := clutter json-glib atk cogl-pango pangocairo pangoft2 pango fontconfig expat cairo-gobject cairo pixman png freetype cogl android_native_app_glue gio gobject gmodule gthread glib-android glib iconv
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := 				\
	-Wall					\
	-DG_LOG_DOMAIN=\"TestActors\"		\
	-DCOGL_ENABLE_EXPERIMENTAL_API		\
	$(NULL)

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,base)
$(call import-module,glib)
$(call import-module,cairo)
$(call import-module,pango)
$(call import-module,cogl)
$(call import-module,clutter)
