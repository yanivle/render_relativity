cc_library(
    name = "perlin_noise",
    srcs = ["perlin_noise.cc"],
    hdrs = ["perlin_noise.h"],
)

cc_library(
    name = "object_registry",
    srcs = [
        "object_registry.cc",
    ],
    hdrs = ["object_registry.h"],
    deps = ["base_hdrs"],
)

cc_library(
    name = "material",
    srcs = ["material.cc"],
    hdrs = ["material.h"],
    deps = ["base_hdrs"],
)

cc_library(
    name = "counters",
    srcs = ["counters.cc"],
    hdrs = ["counters.h"],
    deps = ["base_hdrs"],
)

cc_library(
    name = "base_hdrs",
    hdrs = [
        "array2d.h",
        "array_view.h",
        "color.h",
        "colorizer.h",
        "counters.h",
        "fft.h",
        "filters.h",
        "image.h",
        "kdtree.h",
        "logging.h",
        "mat4.h",
        "material.h",
        "palette.h",
        "perlin_noise.h",
        "progress.h",
        "rand_utils.h",
        "range.h",
        "ray.h",
        "renderer.h",
        "rgb.h",
        "sdf.h",
        "singleton.h",
        "vec3.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "scene",
    hdrs = [
        "light.h",
        "point_mass.h",
        "rendering_params.h",
        "scene.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base_hdrs",
        ":object_registry",
    ],
)

cc_binary(
    name = "tests",
    srcs = [
        "tests/array2d_scalar_ops_test.cc",
        "tests/array2d_test.cc",
        "tests/array_view_test.cc",
        "tests/catch.hpp",
        "tests/counters_test.cc",
        "tests/fft_test.cc",
        "tests/spheres_kdtree_test.cc",
        "tests/tests_main.cc",
    ],
    deps = [
        ":base_hdrs",
        ":counters",
        ":material",
        ":object_registry",
    ],
)

cc_binary(
    name = "render_relativity",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":base_hdrs",
        ":counters",
        ":material",
        ":object_registry",
        ":perlin_noise",
        ":scene",
        "//scenes",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)

cc_binary(
    name = "sierpinski",
    srcs = [
        "sierpinski.cc",
    ],
    deps = [
        ":base_hdrs",
        ":counters",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)

cc_binary(
    name = "hough",
    srcs = [
        "hough.cc",
    ],
    deps = [
        ":base_hdrs",
        ":counters",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)

cc_binary(
    name = "unit_circles",
    srcs = [
        "unit_circles.cc",
    ],
    deps = [
        ":base_hdrs",
        ":counters",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)

cc_binary(
    name = "seam_carving",
    srcs = [
        "seam_carving.cc",
    ],
    deps = [
        ":base_hdrs",
        ":counters",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)

cc_binary(
    name = "language_evolution",
    srcs = [
        "language_evolution.cc",
    ],
    deps = [
        ":base_hdrs",
        ":counters",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)
