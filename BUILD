package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "gtl",
    srcs = glob([
        "src/gtl/net/*.cc",
        "src/gtl/thread/*.cc",
        "src/gtl/memory/*.cc",
    ]),
    hdrs = glob(["src/**/*.h"]),
    includes = ["src"],
    visibility = ["//visibility:public"],
    copts = ["-O2", "-Wall", "-Wreturn-type"],
    linkopts = ["-pthread"],
    deps = [
        "@fmtlib",
        "@spdlog",
        "@gflags//:gflags",
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
    copts = ["-Wall", "-Wreturn-type"],
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
    copts = ["-Wall", "-Wreturn-type"],
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
    copts = ["-Wall", "-Wreturn-type"],
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
    copts = ["-Wall", "-Wreturn-type"],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "self_connect",
    srcs = [
        "examples/net/self_connect.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    copts = ["-Wall", "-Wreturn-type"],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "reuse_addr_test",
    srcs = [
        "examples/net/reuse_addr_test.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    copts = ["-Wall", "-Wreturn-type"],
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
    copts = ["-Wall", "-Wreturn-type"],
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
    copts = ["-Wall", "-Wreturn-type"],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "thread_pool",
    srcs = [
        "examples/thread/thread_pool.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    copts = ["-Wall", "-Wreturn-type"],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "poller_server",
    srcs = [
        "examples/net/poller_server.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    copts = ["-Wall", "-Wreturn-type"],
    deps = [
        ":gtl",
    ],
)

cc_test(
    name = "reactor_server",
    srcs = [
        "examples/net/reactor_server.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    copts = ["-Wall", "-Wreturn-type"],
    deps = [
        ":gtl",
    ],
)