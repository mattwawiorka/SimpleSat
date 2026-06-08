cmake_minimum_required(VERSION 3.22)

project(SimpleSat VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED JUCE_PATH)
    add_subdirectory(${JUCE_PATH} JUCE)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

file(GLOB_RECURSE SIMPLE_SAT_WEB_ASSETS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/web/dist/*")

if(NOT SIMPLE_SAT_WEB_ASSETS)
    message(FATAL_ERROR "No WebView UI assets found in web/dist. Run `npm install` and `npm run build` from the web folder before configuring the native build.")
endif()

juce_add_binary_data(SimpleSatWebAssets
    HEADER_NAME SimpleSatWebAssets.h
    NAMESPACE SimpleSatWebAssets
    SOURCES ${SIMPLE_SAT_WEB_ASSETS})

juce_add_plugin(SimpleSat
    COMPANY_NAME "mwiki"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    NEEDS_WEBVIEW2 TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Mwik
    PLUGIN_CODE Ssat
    FORMATS Standalone VST3
    PRODUCT_NAME "SimpleSat")

target_sources(SimpleSat
    PRIVATE
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp
        Source/SaturationDSP.cpp)

target_compile_definitions(SimpleSat
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_WIN_WEBVIEW2=1
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(SimpleSat
    PRIVATE
        SimpleSatWebAssets
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
