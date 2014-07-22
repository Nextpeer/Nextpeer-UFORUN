LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared
LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp

CPP_FILES := $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)
CPP_FILES += $(wildcard $(LOCAL_PATH)/../../Classes/LevelHelper/*.cpp)
CPP_FILES += $(wildcard $(LOCAL_PATH)/../../Classes/LevelHelper/Utilities/*.cpp)
CPP_FILES += $(wildcard $(LOCAL_PATH)/../../Classes/LevelHelper/CustomClasses/*.cpp)
CPP_FILES += $(wildcard $(LOCAL_PATH)/../../Classes/LevelHelper/Nodes/*.cpp)
LOCAL_SRC_FILES += $(CPP_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := \
					$(LOCAL_PATH)/../../Classes \
                    $(LOCAL_PATH)/../../../cocos2d-x-2.2/external/Box2D \
                    $(LOCAL_PATH)/../../../cocos2d-x-2.2/extensions/physics_nodes \
                    $(LOCAL_PATH)/../../Classes/LevelHelper \
                    $(LOCAL_PATH)/../../Classes/LevelHelper/Utilities \
                    $(LOCAL_PATH)/../../Classes/LevelHelper/CustomClasses \
                    $(LOCAL_PATH)/../../Classes/LevelHelper/Nodes

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static


include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)