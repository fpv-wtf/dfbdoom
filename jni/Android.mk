LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS) 
LOCAL_MODULE := directfb
LOCAL_SRC_FILES := libdirectfb.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS) 
# give module name
LOCAL_MODULE    := hello_world  
# list your C files to compile
LOCAL_SRC_FILES := test.c
# this option will build executables instead of building library for android application.
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS) 
# give module name
LOCAL_MODULE    := df_matrix  
# list your C files to compile
LOCAL_SRC_FILES := df_matrix.c
LOCAL_SHARED_LIBRARIES := directfb
# this option will build executables instead of building library for android application.
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS) 
# give module name
LOCAL_MODULE    := fbdoom  
CFLAGS += -DNORMALUNIX -std=gnu99
LOCAL_C_INCLUDES := $(LOCAL_PATH)/fbdoom/src
LOCAL_CFLAGS += -DNORMALUNIX -std=gnu99 -DIPPORT_USERRESERVED=5000
# list your C files to compile
LOCAL_SRC_FILES := fbdoom/src/device/main.c fbdoom/src/device/i_fb_video.c fbdoom/src/device/i_no_sound.c fbdoom/src/device/i_no_music.c fbdoom/src/am_map.c fbdoom/src/d_items.c fbdoom/src/d_main.c fbdoom/src/d_net.c fbdoom/src/doomdef.c fbdoom/src/doomstat.c fbdoom/src/dstrings.c fbdoom/src/f_finale.c fbdoom/src/f_wipe.c fbdoom/src/g_game.c fbdoom/src/hu_lib.c fbdoom/src/hu_stuff.c fbdoom/src/i_net.c fbdoom/src/i_system.c fbdoom/src/info.c fbdoom/src/m_argv.c fbdoom/src/m_bbox.c fbdoom/src/m_cheat.c fbdoom/src/m_fixed.c fbdoom/src/m_menu.c fbdoom/src/m_misc.c fbdoom/src/m_random.c fbdoom/src/m_swap.c fbdoom/src/p_ceilng.c fbdoom/src/p_doors.c fbdoom/src/p_enemy.c fbdoom/src/p_floor.c fbdoom/src/p_inter.c fbdoom/src/p_lights.c fbdoom/src/p_map.c fbdoom/src/p_maputl.c fbdoom/src/p_mobj.c fbdoom/src/p_plats.c fbdoom/src/p_pspr.c fbdoom/src/p_saveg.c fbdoom/src/p_setup.c fbdoom/src/p_sight.c fbdoom/src/p_spec.c fbdoom/src/p_switch.c fbdoom/src/p_telept.c fbdoom/src/p_tick.c fbdoom/src/p_user.c fbdoom/src/r_bsp.c fbdoom/src/r_data.c fbdoom/src/r_draw.c fbdoom/src/r_main.c fbdoom/src/r_plane.c fbdoom/src/r_segs.c fbdoom/src/r_sky.c fbdoom/src/r_things.c fbdoom/src/s_sound.c fbdoom/src/sounds.c fbdoom/src/st_lib.c fbdoom/src/st_stuff.c fbdoom/src/tables.c fbdoom/src/v_video.c fbdoom/src/w_wad.c fbdoom/src/wi_stuff.c fbdoom/src/z_zone.c
LOCAL_SHARED_LIBRARIES := directfb
# this option will build executables instead of building library for android application.
include $(BUILD_EXECUTABLE)



