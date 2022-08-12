package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "gtl",
    srcs = glob([
        # "src/gtl/net/*.cc",
        "src/gtl/thread/*.cc",
    ]),
    hdrs = glob(["src/**/*.h"]),
    includes = ["src"],
    visibility = ["//visibility:public"],
    copts = ["-Wall"],
    linkopts = [],
    deps = [
        "@fmtlib",
        "@spdlog",
    ],
)

cc_test(
    name = "gtl_test",
    srcs = glob([
        "src/**/*.cc",
    ]),
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gtl",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "udp_client",
    srcs = [
        "examples/net/udp_client.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "udp_server",
    srcs = [
        "examples/net/udp_server.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "tcp_client",
    srcs = [
        "examples/net/tcp_client.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "tcp_server",
    srcs = [
        "examples/net/tcp_server.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "thread",
    srcs = [
        "examples/thread/thread.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gtl",
    ],
)
