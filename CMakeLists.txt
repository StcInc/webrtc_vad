cmake_minimum_required(VERSION 3.22)
project(webrtc_vad_build)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


add_subdirectory(abseil-cpp)

set(HEADER_FILES
    src/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.h
    src/common_audio/vad/vad_core.h
    src/common_audio/vad/vad_sp.h
    src/common_audio/vad/vad_gmm.h
    src/common_audio/vad/vad_filterbank.h
    src/common_audio/vad/include/webrtc_vad.h
    src/common_audio/signal_processing/complex_fft_tables.h
    src/common_audio/signal_processing/resample_by_2_internal.h
    src/common_audio/signal_processing/dot_product_with_scale.h
    src/common_audio/signal_processing/include/real_fft.h
    src/common_audio/signal_processing/include/spl_inl.h
    src/common_audio/signal_processing/include/signal_processing_library.h
    src/rtc_base/type_traits.h
    src/rtc_base/compile_assert_c.h
    src/rtc_base/checks.h
    src/rtc_base/sanitizer.h
    src/rtc_base/system/arch.h
    src/rtc_base/system/inline.h
    src/rtc_base/numerics/safe_compare.h
)

set(SOURCE_FILES
    src/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.c
    src/common_audio/vad/vad_gmm.c
    src/common_audio/vad/webrtc_vad.c
    src/common_audio/vad/vad_core.c
    src/common_audio/vad/vad_sp.c
    src/common_audio/vad/vad_filterbank.c
    src/common_audio/signal_processing/resample_fractional.c
    src/common_audio/signal_processing/cross_correlation.c
    src/common_audio/signal_processing/get_scaling_square.c
    src/common_audio/signal_processing/complex_fft.c
    src/common_audio/signal_processing/min_max_operations.c
    src/common_audio/signal_processing/complex_bit_reverse.c
    src/common_audio/signal_processing/resample_by_2_internal.c
    src/common_audio/signal_processing/energy.c
    src/common_audio/signal_processing/downsample_fast.c
    src/common_audio/signal_processing/spl_init.c
    src/common_audio/signal_processing/vector_scaling_operations.c
    src/common_audio/signal_processing/resample_48khz.c
    src/common_audio/signal_processing/dot_product_with_scale.cc
    src/common_audio/signal_processing/spl_inl.c
    src/common_audio/signal_processing/spl_sqrt.c
    src/common_audio/signal_processing/division_operations.c
    src/rtc_base/checks.cc
)

add_library(webrtc_vad ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(webrtc_vad absl::meta)
set_target_properties(webrtc_vad PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

target_include_directories(webrtc_vad PUBLIC src/rtc_base/numerics)
target_include_directories(webrtc_vad PUBLIC src/common_audio/signal_processing/include)
target_include_directories(webrtc_vad PUBLIC src/rtc_base)
target_include_directories(webrtc_vad PUBLIC src/system_wrappers/include)
target_include_directories(webrtc_vad PUBLIC src/common_audio/vad)
target_include_directories(webrtc_vad PUBLIC src/common_audio/third_party/spl_sqrt_floor)
target_include_directories(webrtc_vad PUBLIC src/rtc_base/system)
target_include_directories(webrtc_vad PUBLIC src/common_audio/signal_processing)
target_include_directories(webrtc_vad PUBLIC src/common_audio/vad/include)
target_include_directories(webrtc_vad PUBLIC src)


add_executable(vad_test main.cc)

target_include_directories(vad_test PUBLIC webrtc_vad)
target_link_libraries(vad_test PUBLIC webrtc_vad)


add_compile_definitions(WEBRTC_POSIX)
